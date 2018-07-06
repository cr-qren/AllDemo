/*!\copyright
 * Copyright (c) 2018 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_FLATTEN_HH
#define RAINTIME_NN_OPS_FLATTEN_HH

#include "raintime/core.h"

#include <iostream>
#include <memory>

using namespace raintime::tensor;
using namespace raintime::op_impl;

namespace raintime {
namespace nn {
namespace ops {

/** An operator implementation of the Flatten operation.
 *
 * The input tensor should have a 4-D shape and the output
 * will be flattened to 2-D.
 *
 * (batch_size, C, H, W) -> (batch_size, C * H * W)
 *
 * We simply ignore the Device parameter of this tensor and
 * assume that it should be run on CPU.
 */
template <typename Device, typename T>
class FlattenOpImpl : public OpImpl {
 public:
  explicit FlattenOpImpl(OpImplContext *ctx);

  void Run(OpImplEnv *env);
  TensorShape GetOutputTensorShape(OpImplEnv *env);
};

}  // namespace ops
}  // namespace nn
}  // namespace raintime

#endif
