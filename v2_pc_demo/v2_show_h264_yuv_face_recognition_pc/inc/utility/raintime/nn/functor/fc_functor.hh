/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_FC_FUNCTOR_HH
#define RAINTIME_FC_FUNCTOR_HH

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
struct FCFunctor {
  void operator()(Tensor *input, const Tensor *weights, const Tensor *bias,
                  Tensor *output, const int &rows, const int &columns,
                  const int &batch_size, const bool &use_relu = false) {
    LOG(FATAL) << "The Fully Connected functor is not specialised properly";
  }
};

template <typename T>
struct FCFunctor<CpuDevice, T> {
  void operator()(Tensor *input, const Tensor *weights, const Tensor *bias,
                  Tensor *output, const int &rows, const int &columns,
                  const int &batch_size, const bool &use_relu = false) {
    auto input_columns = static_cast<int>(weights->dim_size(1));
    auto output_rows = static_cast<int>(weights->dim_size(0));
    auto input_batch_size = static_cast<int>(input->dim_size(0));

    CHECK_EQ(input_columns, columns);
    CHECK_EQ(output_rows, rows);
    CHECK_EQ(input_batch_size, batch_size);

    auto input_ptr = input->data<T>();
    auto weights_ptr = weights->data<T>();
    auto bias_ptr = bias->data<T>();
    auto output_ptr = output->data<T>();

    cpu::FCCpuAccel<T>()(input_ptr, weights_ptr, bias_ptr, output_ptr, rows,
                         columns, batch_size, use_relu);
  }
};
}  // namespace functor
}  // namespace nn
}  // namespace raintime
#endif
