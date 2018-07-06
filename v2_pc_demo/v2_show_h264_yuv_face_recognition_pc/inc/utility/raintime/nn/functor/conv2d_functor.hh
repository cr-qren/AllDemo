/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_CONV2D_FUNCTOR_HH
#define RAINTIME_CONV2D_FUNCTOR_HH

#include <iostream>
#include "raintime/core.h"
#include "raintime/nn/accel.h"

#include <glog/logging.h>
#include <type_traits>

#define FPGA_DTYPE(BW) FPGA_DTYPE_HELPER(BW)
#define FPGA_DTYPE_HELPER(BW) int##BW##_t

namespace raintime {
namespace nn {
namespace functor {

namespace {
template <typename T>
inline typename T::ST *GetRawData(const Tensor *tensor) {
  return (tensor != nullptr) ? const_cast<Tensor *>(tensor)->AllocRawData<T>()
                             : nullptr;
}

inline bool IsTensorPadded(const Tensor *tensor) {
  return tensor->pad_size() != 0;
}

template <typename T>
inline void CheckRawPtrInFpgaAddrRange(typename T::ST *ptr,
                                       FpgaDevice *dev = nullptr) {
  if (ptr == nullptr) return;  // skip
  if (dev == nullptr) dev = FpgaDevice::Global();

  CHECK(dev->IsInAddressRange(reinterpret_cast<size_t>(ptr)));
}

void CheckConv2DTensorShapes(const Tensor *input, const Tensor *weights,
                             Tensor *output, int pad, int stride,
                             bool use_maxpool_2x2 = false) {
  auto input_height = static_cast<int>(input->dim_size(2));
  auto input_width = static_cast<int>(input->dim_size(3));
  auto kernel_height = static_cast<int>(weights->dim_size(2));
  auto kernel_width = static_cast<int>(weights->dim_size(3));
  auto num_channels = static_cast<int>(input->dim_size(1));
  auto num_filters = static_cast<int>(weights->dim_size(0));
  auto output_height = static_cast<int>(output->dim_size(2));
  auto output_width = static_cast<int>(output->dim_size(3));
  auto output_dim_scale = (use_maxpool_2x2) ? 2 : 1;

  if (IsTensorPadded(input)) {
    input_height -= 2 * input->pad_size();
    input_width -= 2 * input->pad_size();
  }

  if (IsTensorPadded(output)) {
    output_height -= 2 * output->pad_size();
    output_width -= 2 * output->pad_size();
  }

  CHECK_EQ(kernel_height, kernel_width);
  CHECK_EQ(output_height * output_dim_scale,
           (input_height - kernel_height + 2 * pad) / stride + 1);
  CHECK_EQ(output_width * output_dim_scale,
           (input_width - kernel_width + 2 * pad) / stride + 1);
}

}  // namespace

using namespace accel;
using namespace tensor;

template <typename Device, typename T,
          cpu::Conv2DAccelImpl cpu_impl = cpu::CONV2D_IM2COL,
          rainman::Conv2DAccelImpl rainman_impl = rainman::Conv2DAccelImpl::SIM>
struct Conv2DFunctor {
  void operator()(const Tensor *input, const Tensor *weights,
                  const Tensor *bias, Tensor *output, int pad, int stride,
                  bool use_relu = false, bool use_maxpool_2x2 = false) {
    LOG(FATAL) << "This Conv2DFunctor is not specialised properly";
  }
};

template <typename T, cpu::Conv2DAccelImpl cpu_impl,
          rainman::Conv2DAccelImpl impl>
struct Conv2DFunctor<CpuDevice, T, cpu_impl, impl> {
  void operator()(const Tensor *input, const Tensor *weights,
                  const Tensor *bias, const Tensor *batch_norm_a,
                  const Tensor *batch_norm_b, Tensor *output, int pad,
                  int stride, bool use_relu = false,
                  bool use_maxpool_2x2 = false) {
    CHECK(use_maxpool_2x2 == false)
        << "You should not use max_pool 2x2 in CPU functor.";
    CHECK_EQ(input->pad_size(), 0) << "We don't accept padded input tensor in "
                                      "the software version of Conv2D";

    // LOG(INFO) << "Conv2DFunctor for CpuDevice start";
    // extract Eigen::TensorMap from the Tensors for further computation

    // get dimensions
    auto input_height = static_cast<int>(input->dim_size(2));
    auto input_width = static_cast<int>(input->dim_size(3));
    auto kernel_height = static_cast<int>(weights->dim_size(2));
    auto kernel_width = static_cast<int>(weights->dim_size(3));
    auto num_channels = static_cast<int>(input->dim_size(1));
    auto num_filters = static_cast<int>(weights->dim_size(0));
    auto output_height = static_cast<int>(output->dim_size(2));
    auto output_width = static_cast<int>(output->dim_size(3));

    CHECK_EQ(kernel_height, kernel_width);

    CHECK_EQ(output_height,
             (input_height - kernel_height + 2 * pad) / stride + 1);

    CHECK_EQ(output_width, (input_width - kernel_width + 2 * pad) / stride + 1);
    if (bias != nullptr) {
      CHECK_EQ(num_filters, static_cast<int>(bias->dim_size(0)));
    }
    if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
      CHECK_EQ(num_filters, static_cast<int>(batch_norm_a->dim_size(0)));
      CHECK_EQ(num_filters, static_cast<int>(batch_norm_b->dim_size(0)));
    }
    // extract pointers
    auto input_ptr = input->data<T>();
    auto weights_ptr = weights->data<T>();
    auto output_ptr = output->data<T>();

    const T *bias_ptr = nullptr;
    const T *bn_a_ptr = nullptr;
    const T *bn_b_ptr = nullptr;
    if (bias != nullptr) bias_ptr = bias->data<T>();
    if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
      bn_a_ptr = batch_norm_a->data<T>();
      bn_b_ptr = batch_norm_b->data<T>();
    }
    cpu::Conv2DCpuAccel<T, cpu_impl>()(
        input_ptr, weights_ptr, bias_ptr, bn_a_ptr, bn_b_ptr, output_ptr,
        input_height, input_width, num_channels, num_filters, kernel_height,
        pad, stride, use_relu);
  }
};

template <typename T, cpu::Conv2DAccelImpl cpu_impl,
          rainman::Conv2DAccelImpl impl>
struct Conv2DFunctor<FpgaDevice, T, cpu_impl, impl> {
  void operator()(const Tensor *input, const Tensor *weights,
                  const Tensor *bias, const Tensor *batch_norm_a,
                  const Tensor *batch_norm_b, Tensor *output, int pad,
                  int stride, bool use_relu = false,
                  bool use_maxpool_2x2 = false) {
    CHECK_NOTNULL(input);
    CHECK_NOTNULL(weights);
    CHECK_NOTNULL(output);

    LOG(INFO) << "Conv2DFunctor for FpgaDevice start in mode "
              << ((impl == rainman::Conv2DAccelImpl::REAL) ? "REAL" : "SIM");

    // SIM mode doesn't support in-tensor padding
    if (impl == rainman::Conv2DAccelImpl::SIM) {
      if (input->pad_size() != 0 || output->pad_size() != 0)
        LOG(FATAL) << "SIM mode of Conv2D doesn't support padded tensors: "
                      "input tensor pad_size ("
                   << input->pad_size() << ") output (" << output->pad_size()
                   << ")";
    }

#ifndef FPGA_DTYPE_BW
#error \
    "You need to provide -DFPGA_DTYPE_BW=<bit-width> to specify the bit-width of data-type on FPGA."
#endif

    LOG(INFO) << "Input tensor padded: " << IsTensorPadded(input)
              << " output: " << IsTensorPadded(output);
    if (IsTensorPadded(input)) CHECK_EQ(input->pad_size(), pad);

    // get dimensions
    auto input_height = static_cast<int>(input->dim_size(2));
    auto input_width = static_cast<int>(input->dim_size(3));
    auto kernel_height = static_cast<int>(weights->dim_size(2));
    auto kernel_width = static_cast<int>(weights->dim_size(3));
    auto num_channels = static_cast<int>(input->dim_size(1));
    auto num_filters = static_cast<int>(weights->dim_size(0));
    auto output_height = static_cast<int>(output->dim_size(2));
    auto output_width = static_cast<int>(output->dim_size(3));
    auto output_dim_scale = (use_maxpool_2x2) ? 2 : 1;

    CheckConv2DTensorShapes(input, weights, output, pad, stride,
                            use_maxpool_2x2);

    // pointers to raw data allocated
    auto raw_input_ptr = GetRawData<T>(input);
    auto raw_weights_ptr = GetRawData<T>(weights);
    auto raw_bias_ptr = GetRawData<T>(bias);
    auto raw_bn_a_ptr = GetRawData<T>(batch_norm_a);
    auto raw_bn_b_ptr = GetRawData<T>(batch_norm_b);
    auto raw_output_ptr = GetRawData<T>(output);

    if (impl == rainman::Conv2DAccelImpl::REAL) {
      LOG(INFO) << "Running Conv2DAccel REAL ...";
      CHECK(pad == 1 || pad == 0)
          << "FPGA only supports either no padding or padding with width 1";

      auto dev = FpgaDevice::Global();

      // check address range
      CheckRawPtrInFpgaAddrRange<T>(raw_input_ptr);
      CheckRawPtrInFpgaAddrRange<T>(raw_weights_ptr);
      CheckRawPtrInFpgaAddrRange<T>(raw_bias_ptr);
      CheckRawPtrInFpgaAddrRange<T>(raw_bn_a_ptr);
      CheckRawPtrInFpgaAddrRange<T>(raw_bn_b_ptr);
      CheckRawPtrInFpgaAddrRange<T>(raw_output_ptr);

      if (IsTensorPadded(input)) {
        // remove the padding size required for the hardware processing.
        pad = 0;
      }

      if (IsTensorPadded(output)) {
        // set output_data with 0 for FPGA output pad.
        LOG(INFO) << "output pad: " << output->pad_size();
        auto n_size = output->dims();
        CHECK_EQ(n_size, 4) << "n_size should equal 4";
        output->tensor<T, 4>().setZero();
      }

      rainman::Conv2DRainmanAccel<typename T::ST,
                                  rainman::Conv2DAccelImpl::REAL>()(
          raw_input_ptr, raw_weights_ptr, raw_bias_ptr, raw_bn_a_ptr,
          raw_bn_b_ptr, raw_output_ptr, input_height, input_width, num_channels,
          num_filters, kernel_height, pad, stride, use_relu, use_maxpool_2x2,
          IsTensorPadded(input), IsTensorPadded(output));

      // synchronise results from the raw_data
      if (output->sync_from_raw()) output->SyncDataFromRawData();

    } else if (impl == rainman::Conv2DAccelImpl::SIM) {
      LOG(INFO) << "Running Conv2DAccel SIM ...";

      auto bias_data = bias == nullptr ? nullptr : bias->data<T>();
      auto bn_a_data =
          batch_norm_a == nullptr ? nullptr : batch_norm_a->data<T>();
      auto bn_b_data =
          batch_norm_b == nullptr ? nullptr : batch_norm_b->data<T>();

      rainman::Conv2DRainmanAccel<T, rainman::Conv2DAccelImpl::SIM>()(
          input->data<T>(), weights->data<T>(), bias_data, bn_a_data, bn_b_data,
          output->data<T>(), input_height, input_width, num_channels,
          num_filters, kernel_height, pad, stride, use_relu, use_maxpool_2x2);
    }
  }
};
}  // namespace functor
}  // namespace nn
}  // namespace raintime

#endif
