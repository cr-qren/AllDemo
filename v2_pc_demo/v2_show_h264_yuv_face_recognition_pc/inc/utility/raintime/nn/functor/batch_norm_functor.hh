/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_BATCH_NORM_FUNCTOR_HH
#define RAINTIME_BATCH_NORM_FUNCTOR_HH

#include "raintime/core.h"
#include "raintime/nn/accel.h"

#include <glog/logging.h>

namespace raintime {
namespace nn {
namespace functor {

using namespace accel;
using namespace tensor;

template <typename Device, typename T>
struct BatchNormFunctor {
  void operator()(const Tensor *input, const Tensor *BN_A, const Tensor *BN_B,
                  Tensor *output, bool use_relu, bool use_maxpool_2x2) {
    LOG(FATAL) << "This BatchNormFunctor is not specialised properly";
  }
};

template <typename T>
struct BatchNormFunctor<CpuDevice, T> {
  void operator()(const Tensor *input, const Tensor *BN_A, const Tensor *BN_B,
                  Tensor *output, bool use_relu, bool use_maxpool_2x2) {
    const auto input_data = input->tensor<T, 4>();
    const auto bn_a_data = BN_A->tensor<T, 1>();
    const auto bn_b_data = BN_B->tensor<T, 1>();
    auto output_data = output->tensor<T, 4>();

    auto input_height = static_cast<int>(input->dim_size(2));
    auto input_width = static_cast<int>(input->dim_size(3));
    auto num_channels = static_cast<int>(input->dim_size(1));
    auto batch = static_cast<int>(input->dim_size(0));
    auto bn_a_shape = static_cast<int>(BN_A->dim_size(0));
    auto bn_b_shape = static_cast<int>(BN_B->dim_size(0));
    auto output_height = static_cast<int>(output->dim_size(2));
    auto output_width = static_cast<int>(output->dim_size(3));
    auto output_nc = static_cast<int>(output->dim_size(1));
    auto output_batch = static_cast<int>(output->dim_size(0));
    auto input_num_elems = input_height * input_width * num_channels;
    auto dim_scale = (use_maxpool_2x2) ? 2 : 1;

    CHECK_EQ(input_height, output_height * dim_scale);
    CHECK_EQ(input_width, output_width * dim_scale);
    CHECK_EQ(num_channels, output_nc);
    CHECK_EQ(batch, output_batch);
    CHECK_EQ(num_channels, bn_a_shape);
    CHECK_EQ(num_channels, bn_b_shape);

    // extract pointers
    auto input_ptr = input_data.data();
    auto bn_a_ptr = bn_a_data.data();
    auto bn_b_ptr = bn_b_data.data();
    auto output_ptr = output_data.data();
    T *output_tmp = new T[input_num_elems];

    cpu::BatchNormCpuAccel<T>()(input_ptr, bn_a_ptr, bn_b_ptr, output_tmp,
                                input_height, input_width, num_channels,
                                use_relu);
    if (use_maxpool_2x2) {
      T *output_data_pool = new T[input_num_elems / 4];
      cpu::MaxPool2DCpuAccel<T>()(output_tmp, output_data_pool, input_height,
                                  input_width, num_channels, 2, 0, 2);
      for (int i = 0; i < input_num_elems / 4; i++)
        output_ptr[i] = output_data_pool[i];
      delete[] output_data_pool;
    } else {
      for (int i = 0; i < input_num_elems; i++) output_ptr[i] = output_tmp[i];
    }
    delete[] output_tmp;
  }
};
}
}
}
#endif
