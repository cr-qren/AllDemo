/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_MAX_POOL2D_FUNCTOR_HH
#define RAINTIME_MAX_POOL2D_FUNCTOR_HH

#include "raintime/core.h"
#include "raintime/nn/accel.h"

#include <glog/logging.h>

namespace raintime {
namespace nn {
namespace functor {

using namespace accel;
using namespace tensor;

template <typename Device, typename T>
struct MaxPool2DFunctor {
  void operator()(const Tensor *input, Tensor *output, int kernel_size, int pad,
                  int stride) {
    LOG(FATAL) << "This Conv2DFunctor is not specialised properly";
  }
};

template <typename T>
struct MaxPool2DFunctor<CpuDevice, T> {
  void operator()(const Tensor *input, Tensor *output, int kernel_size, int pad,
                  int stride) {
    const auto input_data = input->tensor<T, 4>();
    auto output_data = output->tensor<T, 4>();

    auto input_height = static_cast<int>(input->dim_size(2));
    auto input_width = static_cast<int>(input->dim_size(3));
    auto num_channels = static_cast<int>(input->dim_size(1));
    auto output_height = static_cast<int>(output->dim_size(2));
    auto output_width = static_cast<int>(output->dim_size(3));

    CHECK_EQ(output_height,
             (input_height - kernel_size + 2 * pad) / stride + 1);
    CHECK_EQ(output_width, (input_width - kernel_size + 2 * pad) / stride + 1);

    // extract pointers
    auto input_ptr = input_data.data();
    auto output_ptr = output_data.data();

    cpu::MaxPool2DCpuAccel<T>()(input_ptr, output_ptr, input_height,
                                input_width, num_channels, kernel_size, pad,
                                stride);
  }
};
}
}
}

#endif
