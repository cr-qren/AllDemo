/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_OP_HH_
#define RAINTIME_OP_HH_

#include <memory>
#include <unordered_map>

#include "plumber_ir/ops.pb.h"
#include "status.hh"

namespace raintime {
/*! This namespace contains essential classes for using operations in raintime.
 */
namespace op {

/*! A registry for operations that can be used in raintime.
 *
 * The registry should be instantiated as a singleton and all the operations
 * must be registered statically.
 */
class OpRegistry {
 public:
  /*!< return a singleton of the global op registry */
  static OpRegistry *Global();

  /*!
   * Register an operator by its definition (OpDef)
   *
   * @param op_def
   * @return a boolean that indicates whether the registration is successful
   * or not.
   */
  bool Register(const plumber_ir::ops::OpDef* op_def);

  /*!
   * Register from a file of OpDefList.
   *
   * @param file_name the name of the input file
   * @return a boolean that indicates whether the registration is successful
   * or not.
   */
  bool Register(const std::string& file_name);

  /*! Look up a OpDef by its name.
   *
   * @param op_name
   * @return if the op exists, return the pointer; or return nullptr.
   */
  const plumber_ir::ops::OpDef* LookUp(std::string op_name) const;

  void CleanUp() { registry_.clear(); }

  int64_t num_ops() const { return registry_.size(); }

 private:
  std::unordered_map<std::string, const plumber_ir::ops::OpDef *> registry_;
};

}
}

#endif
