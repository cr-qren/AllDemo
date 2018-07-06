/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_ARGMAX_FUNCTOR_HH
#define RAINTIME_ARGMAX_FUNCTOR_HH

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
struct ArgmaxFunctor {
  void operator()(Tensor *input, Tensor *output, const int &input_size,
                  const int &batch_size, const int &dimension) {
    LOG(FATAL) << "The Argmax functor is not specialised properly";
  }

  void operator()(Tensor *input, Tensor *output, const int &height,
                  const int &width, const int &batch_size,
                  const int &dimension) {
    LOG(FATAL) << "The Argmax functor is not specialised properly";
  }
};

template <typename T>
struct ArgmaxFunctor<CpuDevice, T> {
  void operator()(Tensor *input, Tensor *output, const int &input_size,
                  const int &batch_size, const int &dimension) {
    auto input_num_elems = static_cast<int>(input->dim_size(1));
    auto input_batch_size = static_cast<int>(input->dim_size(0));

    CHECK_EQ(input_num_elems, input_size);
    CHECK_EQ(input_batch_size, batch_size);

    auto input_ptr = input->data<T>();
    auto output_ptr = output->data<T>();

    cpu::ArgmaxCpuAccel<T>()(input_ptr, output_ptr, input_size, batch_size,
                             dimension);
  }
  void operator()(Tensor *input, Tensor *output, const int &height,
                  const int &width, const int &batch_size,
                  const int &dimension) {
    auto input_height = static_cast<int>(input->dim_size(1));
    auto input_width = static_cast<int>(input->dim_size(2));
    auto input_batch_size = static_cast<int>(input->dim_size(0));

    CHECK_EQ(input_height, height);
    CHECK_EQ(input_width, width);
    CHECK_EQ(input_batch_size, batch_size);

    auto input_ptr = input->data<T>();
    auto output_ptr = output->data<T>();

    cpu::ArgmaxCpuAccel<T>()(input_ptr, output_ptr, height, width, batch_size,
                             dimension);
  }
};

}  // namespace functor
}  // namespace nn
}  // namespace raintime
#endif
