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
#define RAINMAN_TEST

template <typename T>
void padding_input_data(const T *orig_input, T *new_input, int height,
                        int width, int num_channels) {
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
                  bool use_relu = false, bool use_maxpool_2x2 = false) {
    LOG(INFO) << "Conv2DRainmanAccel REAL start";
    // check type
    if (!std::is_same<T, int32_t>::value)
      LOG(FATAL) << "Specialised type T should be an int32_t";

    // prepare parameters
    auto dev = FpgaDevice::Global();
    // LOG(INFO) << dev->base_addr();
    bool use_bias = bias_data != nullptr;
    int input_addr = reinterpret_cast<size_t>(input_data);
    int coeff_addr = reinterpret_cast<size_t>(weights_data);
    int output_addr = reinterpret_cast<size_t>(output_data);
    bool use_bn = bn_a_data != nullptr;
    size_t bias_addr, a_addr, b_addr;
    if (use_bias) bias_addr = reinterpret_cast<size_t>(bias_data);
    if (use_bn) {
      a_addr = reinterpret_cast<size_t>(bn_a_data);
      b_addr = reinterpret_cast<size_t>(bn_b_data);
    }
    int block_size = 0;
    height += 2 * pad;
    width += 2 * pad;

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

    bool use_padding = (pad != 0);
    auto p = dev->GetMemDev();
    auto alct = LinearAllocator::Global((size_t)p, DDR_MEM_SIZE);
    auto new_input = alct->Alloc<T>(height * width * num_channels);
    // LOG(INFO) << (size_t)new_input << "\n" << (size_t)p;
    if (use_padding) {
      padding_input_data<T>(input_data, new_input, height, width, num_channels);
      input_addr = reinterpret_cast<size_t>(new_input);
    }
// for(int i = 0; i < height * width * num_channels; i++)
//  std::cout << "NEW" << new_input[i] << std::endl;
// for(int j = 0; j < (height - 2 * pad) * (width - 2 * pad) * num_channels;
// j ++)
//  std::cout << "ORIG" << input_data[j] << std::endl;
/*
std::ofstream new_data("/root/new_data.bin", std::ios::out |
std::ios::binary); for(int i = 0; i < height * width * num_channels; i++)
  new_data.write((char *)&new_input[i], sizeof(unsigned int));
  //new_data << static_cast<float>(new_input[i]) / (1 << 20) << std::endl;
new_data.close();

std::ofstream orig_data("/root/orig_data.bin", std::ios::out |
std::ios::binary); for(int j = 0; j < (height - 2 * pad) * (width - 2 * pad)
* num_channels; j ++) orig_data.write((char *)&input_data[j],
sizeof(unsigned int));
  //orig_data << static_cast<float>( input_data[j])/(1 << 20) << std::endl;
orig_data.close();
*/
#ifdef TEST_RAINMAN
    int weights_num_elems =
        num_filters * num_channels * kernel_size * kernel_size;
    auto new_coeff = alct->Alloc<T>(weights_num_elems);
    LOG(INFO) << (size_t)new_coeff;
    for (int i = 0; i < weights_num_elems; i++) new_coeff[i] = weights_data[i];
    ConvertCoeff<T>(new_coeff, weights_num_elems, num_filters, num_channels);
    coeff_addr = reinterpret_cast<size_t>(new_coeff);
#endif
    // for(int i = 0; i < kernel_size * num_channels * kernel_size *
    // num_filters; i++)
    //  std::cout << "NEW" << new_coeff[i] << "ORIG" << weights_data[i] <<
    //  std::endl;
    /*
    std::ofstream coeff_data("/root/orig_coeff_data.bin", std::ios::out |
    std::ios::binary); for(int i = 0; i < kernel_size * num_channels *
    kernel_size * num_filters; i++) coeff_data.write((char *)&weights_data[i],
    sizeof(unsigned int));
      //coeff_data << "New " << static_cast<float>(new_coeff[i]) / (1 << 20) <<
    " Orig " << static_cast<float>(weights_data[i]) / (1 << 20) << std::endl;
    coeff_data.close();

    std::ofstream new_coeff_data("/root/new_coeff_data.bin", std::ios::out |
    std::ios::binary); for(int i = 0; i < kernel_size * num_channels *
    kernel_size * num_filters; i++) new_coeff_data.write((char *)&new_coeff[i],
    sizeof(unsigned int)); new_coeff_data.close();
    */
    Conv2DAccel(dev, height, width, num_channels, num_filters, kernel_size,
                block_size, use_padding, use_maxpool_2x2, use_bias, use_relu,
                use_bn, input_addr, coeff_addr, output_addr, bias_addr, a_addr,
                b_addr);
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
    LOG(INFO) << "Conv2DRainmanAccel SIM start";

#ifdef DATA_TYPE16
    // check type
    if (!std::is_same<T, int16_t>::value)
      LOG(FATAL) << "Specialised type T should be an int16_t";
    // we assume that the simulated compuation is in FixedPoint<23, 8, true>
    using FpT = FixedPoint<FIXED_NUM_FB, FIXED_NUM_TT - 1 - FIXED_NUM_FB, true>;
    static_assert(std::is_same<FpT::ST, int16_t>::value,
                  "Storaged type FpT::ST should be int16_t");
#else
    if (!std::is_same<T, int32_t>::value)
      LOG(FATAL) << "Specialised type T should be an int32_t";
    // we assume that the simulated compuation is in FixedPoint<23, 8, true>
    using FpT = FixedPoint<FIXED_NUM_FB, FIXED_NUM_TT - 1 - FIXED_NUM_FB, true>;
    static_assert(std::is_same<FpT::ST, int32_t>::value,
                  "Storaged type FpT::ST should be int32_t");

#endif

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
        output_data[i] = output_data_pool_sim[i].val();

      delete[] output_data_pool_sim;
    } else {
      for (int i = 0; i < output_data_num_elems; i++)
        output_data[i] = output_data_sim[i].val();
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
}  // namespace rainman
}  // namespace accel
}  // namespace nn
}  // namespace raintime

#undef TEST_RAINMAN
#endif