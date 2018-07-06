/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_SOFTMAX_FUNCTOR_HH
#define RAINTIME_SOFTMAX_FUNCTOR_HH

#include "raintime/core.h"
#include "raintime/nn/accel.h"

#include <glog/logging.h>

namespace raintime {
namespace nn {
namespace functor {

using namespace accel;
using namespace tensor;

template <typename Device, typename T>
struct SoftmaxFunctor {
  void operator()(const Tensor *input, Tensor *output, int num_classes,
                  int batch_size) {
    LOG(FATAL) << "This SoftmaxFunctor is not specialised properly";
  }
};

template <typename T>
struct SoftmaxFunctor<CpuDevice, T> {
  void operator()(const Tensor *input, Tensor *output, int num_classes,
                  int batch_size) {
    if (input->dims() != 2) {
      const auto input_data = input->tensor<T, 4>();
      auto output_data = output->tensor<T, 2>();

      auto input_height = static_cast<int>(input->dim_size(2));
      auto input_width = static_cast<int>(input->dim_size(3));
      auto num_channels = static_cast<int>(input->dim_size(1));
      auto shape_1 = static_cast<int>(output->dim_size(0));
      auto shape_2 = static_cast<int>(output->dim_size(1));

      CHECK_EQ(shape_1, batch_size);
      CHECK_EQ(shape_2, num_classes);

      // extract pointers
      auto input_ptr = input_data.data();
      auto output_ptr = output_data.data();
      auto new_input_ptr = new T[shape_1 * shape_2];

      cpu::ChannelToLastCpuAccel<T>()(input_ptr, new_input_ptr, input_height,
                                      input_width, num_channels);

      cpu::SoftmaxCpuAccel<T>()(new_input_ptr, output_ptr, shape_2, shape_1);

      delete[] new_input_ptr;
    } else {
      const auto input_data = input->tensor<T, 2>();
      auto output_data = output->tensor<T, 2>();

      auto batch = static_cast<int>(output->dim_size(0));
      auto classes = static_cast<int>(output->dim_size(1));

      CHECK_EQ(batch_size, batch);
      CHECK_EQ(num_classes, classes);

      // extract pointers
      auto input_ptr = input_data.data();
      auto output_ptr = output_data.data();

      cpu::SoftmaxCpuAccel<T>()(input_ptr, output_ptr, num_classes, batch_size);
    }
  }
};
}  // namespace functor
}  // namespace nn
}  // namespace raintime
#endif
