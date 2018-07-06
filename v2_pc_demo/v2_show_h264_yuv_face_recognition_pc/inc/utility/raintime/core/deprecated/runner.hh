/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_RUNNER_HH
#define RAINTIME_RUNNER_HH

#include "raintime/core/dfg.hh"

namespace raintime {
namespace runner {

using namespace raintime::dfg;

class Runner {
 public:
  Runner(DFG *dfg);

  /*! The function that run throw all nodes in the DFG.
   */
  void Run();

 private:
  DFG *dfg_;
};

}
}

#endif //RAINTIME_RUNNER_HH
