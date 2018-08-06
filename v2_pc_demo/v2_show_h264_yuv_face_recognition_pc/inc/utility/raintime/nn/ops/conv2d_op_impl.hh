/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_OPS_CONV2D_HH_
#define RAINTIME_NN_OPS_CONV2D_HH_

#include "raintime/core/op_impl.hh"

#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <memory>

#include <glog/logging.h>
#include "raintime/core/tensor.hh"
#include "raintime/nn/accel.h"
#include "raintime/nn/functor.h"

using namespace raintime::tensor;
using namespace raintime::nn::functor;
using namespace raintime::nn::accel;

namespace raintime {
namespace nn {
namespace ops {

using namespace raintime::op_impl;

template <typename Device, typename T,
          cpu::Conv2DAccelImpl cpu_impl = cpu::Conv2DAccelImpl::CONV2D_IM2COL,
          rainman::Conv2DAccelImpl rainman_accel =
              rainman::Conv2DAccelImpl::SIM>
class Conv2DOpImpl : public OpImpl {
 public:
  explicit Conv2DOpImpl(OpImplContext *ctx);

  void Run(OpImplEnv *env);
};
}
}
}

#endif
