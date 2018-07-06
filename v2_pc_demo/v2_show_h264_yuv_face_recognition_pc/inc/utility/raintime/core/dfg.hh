/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */
#ifndef RAINTIME_DFG_HH
#define RAINTIME_DFG_HH

#include <map>
#include <vector>

#include "plumber_ir/dfg_def.pb.h"
#include "plumber_ir/type_def.pb.h"
#include "raintime/core/device.hh"

namespace raintime {
namespace dfg {

using namespace plumber_ir;

class DFGNode;
class DFGEdge;
class DFG;

class DFGNode {
 public:
  DFGNode(DFGNodeDef node_def, DFG* dfg);

  std::string name() const { return node_def_.name(); }
  std::string op() const { return node_def_.op(); }
  DataType type() const { return node_def_.type(); }

  DFGNodeDef node_def() const { return node_def_; }

  std::vector<std::string> inputs() const { return inputs_; }
  std::vector<DFGEdge*> in_edges() const { return in_edges_; }
  std::vector<DFGEdge*> out_edges() const { return out_edges_; }
  int num_inputs() const { return in_edges_.size(); }
  int num_outputs() const { return out_edges_.size(); }
  std::vector<DFGNode*> in_nodes() const;
  std::vector<DFGNode*> out_nodes() const;

  void AddInEdge(DFGEdge* edge);
  void AddOutEdge(DFGEdge* edge);

  bool AllInNodesOnFpga() const;
  bool AllOutNodesOnFpga() const;
  bool is_on_fpga() const { return node_def_.device() == DEVICE_FPGA; }

 private:
  DFG* dfg_;
  DFGNodeDef node_def_;
  std::vector<std::string> inputs_;
  std::vector<DFGEdge*> in_edges_;
  std::vector<DFGEdge*> out_edges_;
};

class DFGEdge {
 public:
  DFGEdge(DFGNode* src, DFGNode* dst);

  DFGNode* src() const { return src_; }
  DFGNode* dst() const { return dst_; }

 private:
  DFGNode *src_, *dst_;
};

class DFG {
 public:
  DFG() : dfg_def_(nullptr) {}
  DFG(DFGDef* dfg_def);

  int num_edges() const { return edges_.size(); }
  int num_nodes() const { return nodes_.size(); }

  std::vector<DFGNode*> nodes() const { return nodes_; }
  std::vector<DFGEdge*> edges() const { return edges_; }
  std::vector<DFGNode*> input_nodes() const;
  std::vector<DFGNode*> output_nodes() const;

  DFGNode* AddNode(DFGNodeDef node_def);
  DFGEdge* AddEdge(DFGNode* src, DFGNode* dst);

  DFGNode* FindNode(std::string name);

 private:
  DFGDef* dfg_def_;

  std::vector<DFGNode*> nodes_;
  std::vector<DFGEdge*> edges_;
  std::map<std::string, DFGNode*> node_map_;
};
}  // namespace dfg
}  // namespace raintime

#endif
