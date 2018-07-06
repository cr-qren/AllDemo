/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_DECONV_HH_
#define RAINTIME_NN_OPS_DECONV_HH_

#include <memory>
#include "raintime/core/op_impl.hh"

namespace raintime {
namespace nn {
namespace ops {

using namespace raintime::op_impl;

template <typename Device, typename T>
class Conv2DTransposeOpImpl : public OpImpl {
 public:
  explicit Conv2DTransposeOpImpl(OpImplContext* ctx);

  void Run(OpImplEnv* env);
  TensorShape GetOutputTensorShape(OpImplEnv* env);
};
}  // namespace ops
}  // namespace nn
}  // namespace raintime

#endif
