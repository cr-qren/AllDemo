/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DFG_DEF_FACTORY_HH
#define RAINTIME_DFG_DEF_FACTORY_HH

#include "plumber_ir/dfg_def.pb.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include <iostream>

namespace raintime {
namespace dfg {

using namespace plumber_ir;
using namespace google::protobuf;

/*! Factor class to create DFGDef */
class DFGDefFactory {
 public:
  /*! Load DFGDef from a File
   * \param file_name the file name of the DFGDef file.
   * \return a pointer to the DFGDef
   */
  static DFGDef* LoadFile(const char* file_name) {
    auto dfg_def = new DFGDef();

    int fd = open(file_name, O_RDONLY);

    CHECK(fd >= 0) << "Cannot open file " << file_name << std::endl;

    io::FileInputStream in(fd);
    in.SetCloseOnDelete(true);

    CHECK(TextFormat::Parse(&in, dfg_def))
        << "Failed to parse file " << file_name << std::endl;

    return dfg_def;
  }
};

}  // namespace dfg
}  // namespace raintime

#endif