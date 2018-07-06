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

#include "raintime/core/device.hh"
#include "raintime/core/dfg.hh"
#include "raintime/core/tensor.hh"
#include "raintime/core/types.hh"

namespace raintime {
namespace op_impl {

using namespace plumber_ir;
using namespace raintime::dfg;
using namespace raintime::tensor;
using namespace raintime::type;

class OpImpl;
class OpImplEnv;
class OpImplContext;

/*! The base class to implement operation in raintime
 */
class OpImpl {
 public:
  explicit OpImpl(OpImplContext *ctx) : ctx_(ctx) {}

  virtual void Run(OpImplEnv *env) = 0;
  virtual TensorShape GetOutputTensorShape(OpImplEnv *env) = 0;

 protected:
  OpImplContext *ctx_;
};

/*!
 * Essential information required to run a single operator.
 */
class OpImplEnv {
 public:
  explicit OpImplEnv(Allocator *alct) : alct_(alct), node_(nullptr) {}
  explicit OpImplEnv(DFGNode *node, Allocator *alct)
      : alct_(alct), node_(node) {}

  ~OpImplEnv();

  int num_inputs() const { return static_cast<int>(inputs_.size()); }

  Tensor *input(int index) const;

  Tensor *output(int index) const;

  /*! Add an input tensor to the environment.
   *
   * Note that this method might tweak the content of the input tensor.
   */
  void AddInput(Tensor *tensor);

  void AllocOutput(DataType type, TensorShape shape, Tensor **tensor);

  void set_force_sync_raw_data(bool force_sync_raw_data) {
    force_sync_raw_data_ = force_sync_raw_data;
  }

 private:
  std::vector<Tensor *> inputs_;

  std::vector<Tensor *> outputs_;

  Allocator *alct_;
  DFGNode *node_;

  /*!< force to synchronise data between typed and raw */
  bool force_sync_raw_data_ = false;
};

/*!
 * This context contains the properties that can be decided
 * before actually initialising the computation.
 */
class OpImplContext {
 public:
  OpImplContext(dfg::DFGNode *node) : node_(node){};
  OpImplContext(dfg::DFGNode *node, bool is_sim)
      : node_(node), is_sim_(is_sim) {}

  dfg::DFGNode *node() const { return node_; }
  bool is_sim() const { return is_sim_; }

 private:
  dfg::DFGNode *node_;
  bool is_sim_ = false;
};

/*! Register pointers to constructor functions.
 *
 * We follow the OpKernelRegistrar implementation in TensorFlow.
 */
class OpImplRegistrar {
 public:
  // Type of the function pointer to OpImpl constructor.
  // Any OpImpl children classes' constructors belong to this type.
  using Factory = OpImpl *(*)(OpImplContext *);

  /*! Initialise an entry in the global registry.
   *
   * \param op_name name of the OP binded
   * \param device_enum enum value of the device
   * \param type_enum enum value of the type
   * \param factory OpImpl constructor
   */
  OpImplRegistrar(const char *op_name, DeviceEnum device_enum,
                  TypeEnum type_enum, const char *label, Factory factory);
};

// an entry that contains essential information for a registration
struct OpImplRegistration {
  OpImplRegistration(DeviceEnum device_enum, TypeEnum type_enum,
                     OpImplRegistrar::Factory factory)
      : device_enum(device_enum), type_enum(type_enum), factory(factory) {}

  const DeviceEnum device_enum;
  const TypeEnum type_enum;
  const OpImplRegistrar::Factory factory;
};

/*! Look up an OpImplRegistrar from the global registry.
 *
 * This method will return nullptr if no registrar can be found.
 */
OpImplRegistration *FindOpImplRegistration(const char *op_name,
                                           DeviceEnum device_enum,
                                           TypeEnum type_enum,
                                           const char *label = nullptr);

void LogAllOpImplRegistration();

#define REGISTER_OP_IMPL(OP_NAME, CLS, DEVICE_ENUM, TYPE_ENUM, ...)          \
  REGISTER_OP_IMPL_WITH_LABEL(OP_NAME, CLS, DEVICE_ENUM, TYPE_ENUM, nullptr, \
                              ##__VA_ARGS__)

#define REGISTER_OP_IMPL_WITH_LABEL(OP_NAME, CLS, DEVICE_ENUM, TYPE_ENUM, \
                                    LABEL, ...)                           \
  REGISTER_OP_IMPL_UNIQ_HELPER(__COUNTER__, OP_NAME, CLS, DEVICE_ENUM,    \
                               TYPE_ENUM, LABEL, ##__VA_ARGS__)

#define REGISTER_OP_IMPL_UNIQ_HELPER(ctr, OP_NAME, CLS, DEVICE_ENUM,      \
                                     TYPE_ENUM, LABEL, ...)               \
  REGISTER_OP_IMPL_UNIQ(ctr, OP_NAME, CLS, DEVICE_ENUM, TYPE_ENUM, LABEL, \
                        ##__VA_ARGS__)

#define REGISTER_OP_IMPL_UNIQ(ctr, OP_NAME, CLS, DEVICE_ENUM, TYPE_ENUM,    \
                              LABEL, ...)                                   \
  template class CLS<DeviceTypeTraits<DEVICE_ENUM>::DeviceType,             \
                     type::DataTypeToBuiltIn<TYPE_ENUM>::T, ##__VA_ARGS__>; \
  static OpImplRegistrar registrar__body__##ctr##__object(                  \
      OP_NAME, DEVICE_ENUM, TYPE_ENUM, LABEL,                               \
      [](OpImplContext *ctx) -> OpImpl * {                                  \
        using CLS_T =                                                       \
            CLS<DeviceTypeTraits<DEVICE_ENUM>::DeviceType,                  \
                type::DataTypeToBuiltIn<TYPE_ENUM>::T, ##__VA_ARGS__>;      \
        return new CLS_T(ctx);                                              \
      });

#define REGISTER_OP_IMPL_ON_CPU(TYPE, OP_NAME, CLS, ...) \
  REGISTER_OP_IMPL(OP_NAME, CLS, DEVICE_CPU, TYPE, ##__VA_ARGS__)
#define REGISTER_OP_IMPL_ON_FPGA(TYPE, OP_NAME, CLS, ...) \
  REGISTER_OP_IMPL(OP_NAME, CLS, DEVICE_FPGA, TYPE, ##__VA_ARGS__)

#define REGISTER_OP_IMPL_ON_CPU_WITH_LABEL(TYPE, OP_NAME, CLS, LABEL, ...) \
  REGISTER_OP_IMPL_WITH_LABEL(OP_NAME, CLS, DEVICE_CPU, TYPE, LABEL,       \
                              ##__VA_ARGS__)
#define REGISTER_OP_IMPL_ON_FPGA_WITH_LABEL(TYPE, OP_NAME, CLS, LABEL, ...) \
  REGISTER_OP_IMPL_WITH_LABEL(OP_NAME, CLS, DEVICE_FPGA, TYPE, LABEL,       \
                              ##__VA_ARGS__)

#define REGISTER_OP_IMPL_ON_ALL_DEVICES(TYPE, OP_NAME, CLS, ...) \
  REGISTER_OP_IMPL_ON_CPU(TYPE, OP_NAME, CLS, ##__VA_ARGS__)     \
  REGISTER_OP_IMPL_ON_FPGA(TYPE, OP_NAME, CLS, ##__VA_ARGS__)
#define REGISTER_OP_IMPL_ON_ALL_DEVICES_WITH_LABEL(TYPE, OP_NAME, CLS, LABEL,  \
                                                   ...)                        \
  REGISTER_OP_IMPL_ON_CPU_WITH_LABEL(TYPE, OP_NAME, CLS, LABEL, ##__VA_ARGS__) \
  REGISTER_OP_IMPL_ON_FPGA_WITH_LABEL(TYPE, OP_NAME, CLS, LABEL, ##__VA_ARGS__)
// #define REGISTER_OP_IMPL_NO_DEVICE(TYPE, OP_NAME, CLS, ...)

}  // namespace op_impl
}  // namespace raintime

#endif  // RAINTIME_OP_IMPL_HH
