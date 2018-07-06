/*!\copyright
 * Copyright (c) 2018 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_RESHAPE_HH
#define RAINTIME_NN_OPS_RESHAPE_HH

#include "raintime/core.h"

#include <iostream>
#include <memory>

using namespace raintime::tensor;
using namespace raintime::op_impl;

namespace raintime {
namespace nn {
namespace ops {

/** An operator implementation of the Reshape operation.
 *
 * The input tensor should have a 4-D shape and the output
 * will be reshaped to any given shape.
 *
 * During reshape, the underlying data sequence will not be
 * altered. We simply create a new output tensor and copy
 * the content from the input tensor.
 *
 * We simply ignore the Device parameter of this tensor and
 * assume that it should be run on CPU.
 */
template <typename Device, typename T>
class ReshapeOpImpl : public OpImpl {
 public:
  explicit ReshapeOpImpl(OpImplContext *ctx);

  void Run(OpImplEnv *env);
  TensorShape GetOutputTensorShape(OpImplEnv *env);
};

}  // namespace ops
}  // namespace nn
}  // namespace raintime

#endif
