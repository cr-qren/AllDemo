/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DFG_BUILDER_HH
#define RAINTIME_DFG_BUILDER_HH

#include "dfg.hh"
#include "plumber_ir/dfg_def.pb.h"

namespace raintime {
namespace dfg {

using namespace plumber_ir;

/*! Construct a DFG object from different sources.
 */
class DFGBuilder {
public:
  DFGBuilder(DFGDef *dfg_def);

  ~DFGBuilder();

  /*! Construct a DFG from a DFGDef.
   *
   * @return A DFG pointer.
   */
  DFG *Build();

private:
  DFGDef *dfg_def_;
};
}
}

#endif // RAINTIME_DFG_BUILDER_HH
