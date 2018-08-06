/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_LRN_HH
#define RAINTIME_NN_OPS_LRN_HH

#include "raintime/core.h"

#include <iostream>
#include <memory>

using namespace raintime::tensor;
using namespace raintime::op_impl;

namespace raintime {
namespace nn {
namespace ops {

template <typename Device, typename T>
class LRNOpImpl : public OpImpl {
 public:
  explicit LRNOpImpl(OpImplContext *ctx);

  void Run(OpImplEnv *env);
};
}  // namespace ops
}  // namespace nn
}  // namespace raintime

#endif
