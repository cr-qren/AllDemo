/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_OP_IMPL_HH
#define RAINTIME_OP_IMPL_HH

#include "plumber_ir/type_def.pb.h"

#include "dfg.hh"
#include "raintime/core/tensor.hh"

namespace raintime {
namespace op_impl {

using namespace plumber_ir;
using namespace raintime::tensor;

class OpImpl;
class OpImplEnv;
class OpImplContext;

class OpImpl {
 public:
  explicit OpImpl(OpImplContext *ctx) : ctx_(ctx) {}

  virtual void Run(OpImplEnv *env) = 0;

 protected:
  OpImplContext *ctx_;
};

/*!
 * Essential information required to run a single operator.
 */
class OpImplEnv {
 public:
  explicit OpImplEnv(Allocator *alct) : alct_(alct) {}

  ~OpImplEnv();

  int num_inputs() const { return static_cast<int>(inputs_.size()); }

  Tensor *input(int index) const;

  Tensor *output(int index) const;

  void AddInput(Tensor *tensor) { inputs_.push_back(tensor); }

  void AllocOutput(DataType type, TensorShape shape, Tensor **tensor);

 private:
  std::vector<Tensor *> inputs_;

  std::vector<Tensor *> outputs_;

  Allocator *alct_;
};

/*!
 * This context contains the properties that can be decided
 * before actually initialising the computation.
 */
class OpImplContext {
 public:
  OpImplContext(dfg::DFGNode *node) : node_(node){};

  dfg::DFGNode *node() const { return node_; }

 private:
  dfg::DFGNode *node_;
};
}  // namespace op_impl
}  // namespace raintime

#endif  // RAINTIME_OP_IMPL_HH
