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

namespace raintime {
namespace nn {
namespace functor {

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
    LOG(INFO) << "Conv2DFunctor for FpgaDevice start";

#ifdef DATA_TYPE16
    if (!std::is_same<T, int16_t>::value)
      LOG(FATAL)
          << "Only int16_t is supported as the scalar type for running on FPGA";
    CHECK_EQ(input->type(), DataType::T_FIXED16);
    CHECK_EQ(weights->type(), DataType::T_FIXED16);
    if (bias != nullptr) CHECK_EQ(bias->type(), DataType::T_FIXED16);
    if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
      CHECK_EQ(batch_norm_a->type(), DataType::T_FIXED16);
      CHECK_EQ(batch_norm_b->type(), DataType::T_FIXED16);
    }
    CHECK_EQ(output->type(), DataType::T_FIXED16);
#else
    if (!std::is_same<T, int32_t>::value)
      LOG(FATAL)
          << "Only int32_t is supported as the scalar type for running on FPGA";
    CHECK_EQ(input->type(), DataType::T_FIXED32);
    CHECK_EQ(weights->type(), DataType::T_FIXED32);
    if (bias != nullptr) CHECK_EQ(bias->type(), DataType::T_FIXED32);
    if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
      CHECK_EQ(batch_norm_a->type(), DataType::T_FIXED32);
      CHECK_EQ(batch_norm_b->type(), DataType::T_FIXED32);
    }
    CHECK_EQ(output->type(), DataType::T_FIXED32);
#endif

    // address check
    if (impl == rainman::Conv2DAccelImpl::REAL) {
      auto dev = FpgaDevice::Global();

      CHECK(dev->IsInAddressRange(reinterpret_cast<size_t>(input->ptr())));
      CHECK(dev->IsInAddressRange(reinterpret_cast<size_t>(weights->ptr())));
      if (bias != nullptr)
        CHECK(dev->IsInAddressRange(reinterpret_cast<size_t>(bias->ptr())));
      if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
        CHECK(dev->IsInAddressRange(
            reinterpret_cast<size_t>(batch_norm_a->ptr())));
        CHECK(dev->IsInAddressRange(
            reinterpret_cast<size_t>(batch_norm_b->ptr())));
      }
      CHECK(dev->IsInAddressRange(reinterpret_cast<size_t>(output->ptr())));
    }

    // extract Eigen::TensorMap from the Tensors for further computation
    const auto input_data = input->tensor<T, 4>();
    const auto weights_data = weights->tensor<T, 4>();
    auto output_data = output->tensor<T, 4>();

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

    CHECK_EQ(kernel_height, kernel_width);
    CHECK_EQ(output_height * output_dim_scale,
             (input_height - kernel_height + 2 * pad) / stride + 1);
    CHECK_EQ(output_width * output_dim_scale,
             (input_width - kernel_width + 2 * pad) / stride + 1);

    // extract pointers
    auto input_ptr = input_data.data();
    auto weights_ptr = weights_data.data();
    auto output_ptr = output_data.data();

    const T *bias_ptr = nullptr;
    const T *bn_a_ptr = nullptr;
    const T *bn_b_ptr = nullptr;
    if (bias != nullptr) bias_ptr = bias->data<T>();
    if (batch_norm_a != nullptr && batch_norm_b != nullptr) {
      bn_a_ptr = batch_norm_a->data<T>();
      bn_b_ptr = batch_norm_b->data<T>();
    }
    rainman::Conv2DRainmanAccel<T, impl>()(
        input_ptr, weights_ptr, bias_ptr, bn_a_ptr, bn_b_ptr, output_ptr,
        input_height, input_width, num_channels, num_filters, kernel_height,
        pad, stride, use_relu, use_maxpool_2x2);
  }
};
}  // namespace functor
}  // namespace nn
}  // namespace raintime

#endif
