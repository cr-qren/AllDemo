/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_LRN_FUNCTOR_HH
#define RAINTIME_LRN_FUNCTOR_HH

#include <glog/logging.h>
#include <iostream>
#include "raintime/core.h"
#include "raintime/nn/accel.h"

namespace raintime {
namespace nn {
namespace functor {

using namespace accel;
using namespace tensor;

template <typename Device, typename T>
struct LRNFunctor {
  void operator()(const Tensor *input, Tensor *output, const int &dim_size_1,
                  const int &dim_size_2, const int &dim_size_3,
                  const int &dim_size_4, const int &depth_radius,
                  const float &bias, const float &beta, const float &alpha) {
    LOG(FATAL) << "The Local response normalization functor is not specialised "
                  "properly";
  }
};

template <typename T>
struct LRNFunctor<CpuDevice, T> {
  void operator()(const Tensor *input, Tensor *output, const int &dim_size_1,
                  const int &dim_size_2, const int &dim_size_3,
                  const int &dim_size_4, const int &depth_radius,
                  const float &bias, const float &beta, const float &alpha) {
    auto dim1 = static_cast<int>(input->dim_size(0));
    auto dim2 = static_cast<int>(input->dim_size(1));
    auto dim3 = static_cast<int>(input->dim_size(2));
    auto dim4 = static_cast<int>(input->dim_size(3));

    CHECK_EQ(dim1, dim_size_1);
    CHECK_EQ(dim2, dim_size_2);
    CHECK_EQ(dim3, dim_size_3);
    CHECK_EQ(dim4, dim_size_4);

    auto temp_data_1 = new T[dim1 * dim2 * dim3 * dim4];
    auto temp_data_2 = new T[dim1 * dim2 * dim3 * dim4];

    auto input_ptr = input->data<T>();
    auto output_ptr = output->data<T>();

    // Convert NCHW -> NHWC
    cpu::ChannelToLastCpuAccel<T>()(input_ptr, temp_data_1, dim3, dim4, dim2);

    cpu::LRNCpuAccel<T>()(temp_data_1, temp_data_2, dim1, dim4, dim3, dim2,
                          depth_radius, bias, beta, alpha);
    // Convert NHWC -> NCHW
    cpu::ChannelToSecondCpuAccel<T>()(temp_data_2, output_ptr, dim3, dim4,
                                      dim2);

    delete[] temp_data_1;
    delete[] temp_data_2;
  }
};
}  // namespace functor
}  // namespace nn
}  // namespace raintime
#endif
