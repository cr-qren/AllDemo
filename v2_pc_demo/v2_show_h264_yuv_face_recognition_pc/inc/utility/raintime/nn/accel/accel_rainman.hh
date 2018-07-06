/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_ACCEL_RAINMAN_HH
#define RAINTIME_NN_ACCEL_RAINMAN_HH

#include "raintime/core.h"
#include "raintime/nn/accel/rainman/conv2d_rainman_accel.hh"

#include <glog/logging.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <type_traits>

#define IN_CACHE_SIZE 128 * 1024
// #define RAINMAN_TEST

template <typename T>
void PaddingInputData(const T *orig_input, T *new_input, int height, int width,
                      int num_channels) {
  for (int c = 0; c < num_channels; c++) {
    for (int h = 0; h < height; h++) {
      for (int w = 0; w < width; w++) {
        if (h == 0 || h == height - 1) {
          new_input[c * height * width + h * width + w] = static_cast<T>(0.0f);
          continue;
        }
        if (w == 0 || w == width - 1) {
          new_input[c * height * width + h * width + w] = static_cast<T>(0.0f);
          continue;
        }
        new_input[c * height * width + h * width + w] =
            orig_input[c * (height - 2) * (width - 2) + (h - 1) * (width - 2) +
                       w - 1];
      }
    }
  }
}

// prepare coeff for rainman board use
template <typename T>
void ConvertCoeff(T *weights, int length, int NF, int NC) {
  auto temp = new T[length];
  int o = 0;
  for (int i = 0; i < NF / 4; i++) {
    for (int m = 0; m < NC; m++) {
      for (int n = 0; n < 4; n++) {
        for (int j = 0; j < 9; j++) {
          auto q = n * NC * 9 + m * 9 + j + i * NC * 4 * 9;
          // std::cout << q << std::endl;
          temp[o++] = weights[q];
        }
      }
    }
  }
  for (int l = 0; l < length; l++) weights[l] = temp[l];
  delete[] temp;
}

namespace raintime {
namespace nn {
namespace accel {
namespace rainman {

enum class Conv2DAccelImpl {
  REAL, /*!< implementation with real hardware functions */
  SIM   /*!< simulated */
};

// paritial template specialisation
template <typename T = int32_t, Conv2DAccelImpl impl = Conv2DAccelImpl::REAL>
struct Conv2DRainmanAccel {};

template <typename T>
struct Conv2DRainmanAccel<T, Conv2DAccelImpl::REAL> {
  void operator()(const T *input_data, const T *weights_data,
                  const T *bias_data, const T *bn_a_data, const T *bn_b_data,
                  T *output_data, int height, int width, int num_channels,
                  int num_filters, int kernel_size, int pad, int stride,
                  bool use_relu = false, bool use_maxpool_2x2 = false,
                  bool is_input_padded = false, bool is_output_padded = false) {
    LOG(INFO) << "Conv2DRainmanAccel REAL start";
    // check type

    if (!(std::is_same<T, int32_t>::value || std::is_same<T, int16_t>::value ||
          std::is_same<T, int8_t>::value))
      LOG(FATAL) << "Data-type of input pointers is not supported on FPGA, "
                    "should be one of (int32_t, int16_t, int8_t).";

    // prepare parameters
    auto dev = FpgaDevice::Global();
    auto p = dev->GetMemDev();
    auto alct = LinearAllocator::Global((size_t)p, DDR_MEM_SIZE);

    bool use_bias = bias_data != nullptr;
    bool use_bn = bn_a_data != nullptr;

    auto input_addr = reinterpret_cast<size_t>(input_data);
    auto coeff_addr = reinterpret_cast<size_t>(weights_data);
    auto output_addr = reinterpret_cast<size_t>(output_data);

    size_t bias_addr, a_addr, b_addr;
    if (use_bias) bias_addr = reinterpret_cast<size_t>(bias_data);
    if (use_bn) {
      a_addr = reinterpret_cast<size_t>(bn_a_data);
      b_addr = reinterpret_cast<size_t>(bn_b_data);
    }

    int block_size = 0;
    // if the input tensor has been padded, then height and width are already
    // padded values.
    if (!is_input_padded) {
      height += 2 * pad;
      width += 2 * pad;
    }

    if (height >= 34) {
      if (34 * 34 * num_channels <= IN_CACHE_SIZE) {
        block_size = 34;
      } else if (18 * 18 * num_channels <= IN_CACHE_SIZE) {
        block_size = 18;
      } else if (10 * 10 * num_channels <= IN_CACHE_SIZE) {
        block_size = 10;
      } else
        LOG(FATAL) << "Cannot support!";
    } else if (height > 10) {
      if (18 * 18 * num_channels <= IN_CACHE_SIZE) {
        block_size = 18;
      } else if (10 * 10 * num_channels <= IN_CACHE_SIZE) {
        block_size = 10;
      } else
        LOG(FATAL) << "Cannot support!";
    } else if (10 * 10 * num_channels <= IN_CACHE_SIZE) {
      block_size = 10;
    } else
      LOG(FATAL) << "Cannot support!";
    // LOG(INFO) << "block_size is: " << block_size;

    T *new_input = nullptr;

    bool use_padding = (pad != 0) && !is_input_padded;

    if (use_padding) {
      LOG(WARNING) << "should not do pad!";
      new_input = alct->Alloc<T>(height * width * num_channels);
      CHECK_NOTNULL(new_input);
      // LOG(INFO) << (size_t)new_input << "\n" << (size_t)p;

      PaddingInputData<T>(input_data, new_input, height, width, num_channels);

      input_addr = reinterpret_cast<size_t>(new_input);
    }

/* ifdef RAINMAN_TEST, do weight convert for hardware PF;*/
#ifdef RAINMAN_TEST
    int weights_num_elems =
        num_filters * num_channels * kernel_size * kernel_size;
    auto new_coeff = alct->Alloc<T>(weights_num_elems);
    LOG(INFO) << (size_t)new_coeff;
    for (int i = 0; i < weights_num_elems; i++) new_coeff[i] = weights_data[i];
    ConvertCoeff<T>(new_coeff, weights_num_elems, num_filters, num_channels);
    coeff_addr = reinterpret_cast<size_t>(new_coeff);
#endif
    //TODO:(jiajun) find a better way to decide whether it is the first conv2d
    if (num_channels != 3) input_addr -= sizeof(T);
    if (is_output_padded) {
      output_addr += ((use_maxpool_2x2 ? height / 2 : height) + 1) * sizeof(T);
    }
    Conv2DAccel(dev, height, width, num_channels, num_filters, kernel_size,
                block_size, is_output_padded, use_maxpool_2x2, use_bias,
                use_relu, use_bn, input_addr, coeff_addr, output_addr,
                bias_addr, a_addr, b_addr);

    if (use_padding) alct->Free<T>(new_input);

#ifdef RAINMAN_TEST
    alct->Free<T>(new_coeff);
#endif
  }
};

/**
 * In this specialisation we use a convolution accelerate function implemented
 * on CPU.
 */
template <typename T>
struct Conv2DRainmanAccel<T, Conv2DAccelImpl::SIM> {
  void operator()(const T *input_data, const T *weights_data,
                  const T *bias_data, const T *bn_a_data, const T *bn_b_data,
                  T *output_data, int height, int width, int num_channels,
                  int num_filters, int kernel_size, int pad, int stride,
                  bool use_relu = false, bool use_maxpool_2x2 = false) {
    using FpT = T;

    int output_height = (height - kernel_size + 2 * pad) / stride + 1;
    int output_width = (width - kernel_size + 2 * pad) / stride + 1;

    int input_data_num_elems = height * width * num_channels;
    int weights_data_num_elems =
        num_filters * num_channels * kernel_size * kernel_size;
    int bias_data_num_elems = num_filters;
    int bn_a_data_num_elems = num_filters;
    int bn_b_data_num_elems = num_filters;
    int output_data_num_elems = num_filters * output_height * output_width;

    // do type conversion
    FpT *input_data_sim = new FpT[input_data_num_elems];
    FpT *weights_data_sim = new FpT[weights_data_num_elems];
    FpT *output_data_sim = new FpT[output_data_num_elems];
    FpT *bias_data_sim = nullptr;
    FpT *bn_a_data_sim = nullptr;
    FpT *bn_b_data_sim = nullptr;

    for (int i = 0; i < input_data_num_elems; i++)
      input_data_sim[i] = input_data[i];
    for (int i = 0; i < weights_data_num_elems; i++)
      weights_data_sim[i] = weights_data[i];
    if (bias_data != nullptr) {
      bias_data_sim = new FpT[bias_data_num_elems];
      for (int i = 0; i < bias_data_num_elems; i++)
        bias_data_sim[i] = bias_data[i];
    }
    if (bn_a_data != nullptr && bn_b_data != nullptr) {
      bn_a_data_sim = new FpT[bn_a_data_num_elems];
      bn_b_data_sim = new FpT[bn_b_data_num_elems];
      for (int i = 0; i < bn_a_data_num_elems; i++) {
        bn_a_data_sim[i] = bn_a_data[i];
        bn_b_data_sim[i] = bn_b_data[i];
      }
    }

    cpu::Conv2DCpuAccel<FpT, cpu::Conv2DAccelImpl::CONV2D_REFER>()(
        input_data_sim, weights_data_sim, bias_data_sim, bn_a_data_sim,
        bn_b_data_sim, output_data_sim, height, width, num_channels,
        num_filters, kernel_size, pad, stride, use_relu);

    if (use_maxpool_2x2) {
      FpT *output_data_pool_sim = new FpT[output_data_num_elems / 4];
      cpu::MaxPool2DCpuAccel<FpT>()(output_data_sim, output_data_pool_sim,
                                    output_height, output_width, num_filters, 2,
                                    0, 2);

      for (int i = 0; i < output_data_num_elems / 4; i++)
        output_data[i] = output_data_pool_sim[i];

      delete[] output_data_pool_sim;
    } else {
      for (int i = 0; i < output_data_num_elems; i++)
        output_data[i] = output_data_sim[i];
    }

    delete[] input_data_sim;
    delete[] weights_data_sim;
    if (bias_data != nullptr) delete[] bias_data_sim;
    if (bn_a_data != nullptr && bn_b_data != nullptr) {
      delete[] bn_a_data_sim;
      delete[] bn_b_data_sim;
    }
    delete[] output_data_sim;
  }
};

template <>
struct Conv2DRainmanAccel<float, Conv2DAccelImpl::SIM> {
  void operator()(const float *input_data, const float *weights_data,
                  const float *bias_data, const float *bn_a_data,
                  const float *bn_b_data, float *output_data, int height,
                  int width, int num_channels, int num_filters, int kernel_size,
                  int pad, int stride, bool use_relu = false,
                  bool use_maxpool_2x2 = false) {}
};

}  // namespace rainman
}  // namespace accel
}  // namespace nn
}  // namespace raintime

#undef RAINMAN_TEST
#endif
