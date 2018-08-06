/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_MAX_POOL2D_HH
#define RAINTIME_NN_OPS_MAX_POOL2D_HH

#include "raintime/core.h"

#include <Eigen/Dense>
#include <iostream>
#include <memory>

using namespace raintime::tensor;
using namespace raintime::op_impl;

namespace raintime {
namespace nn {
namespace ops {

template <typename Device, typename T>
class MaxPool2DOpImpl : public OpImpl {
 public:
  explicit MaxPool2DOpImpl(OpImplContext *ctx);

  void Run(OpImplEnv *env);
};
}
}
}

#endif