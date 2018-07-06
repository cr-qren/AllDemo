/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DFG_H_
#define RAINTIME_DFG_H_

#include "plumber_ir/dfg.pb.h"
#include "status.hh"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

namespace raintime {
/*!
 * This namespace contains modules related to DFG graph/nodes etc.
 *
 * See @ref Graph for more information.
 */
namespace dfg {

// forward declaration
class DFGNode;
class DFGNodeProps;
class DFGNodeIterable;
class DFGEdge;
class DFG;

/*! Class for defining node in the graph.
 */
class DFGNode {
 public:
  /*! The default constructor.
   * \note TensorFlow forbids default constructor of a node.
   */
  DFGNode() : id_(-1), props_(nullptr) {}

  /*!
   * Initialise a DFGNode from its id and properties.
   *
   * @param id The unique id of the node
   * @param props The properties of the node
   */
  DFGNode(int id, std::shared_ptr<DFGNodeProps> props) : id_(id) {
    // NOTE: you don't move the original props, you just
    // moved the copy of the props;
    // The reference to the local copy will be reduced to 0,
    // but the one of the original is still unchanged.
    props_ = std::move(props);
  }

  /*! Get the id of the node
   *
   * \return Node ID
   */
  int id() const { return id_; }

  /*! Get the name of the node
   * \return Node name
   */
  const std::string &name() const;

  const int64_t num_in_edges() const { return in_edges_.size(); }
  const int64_t num_out_edges() const { return out_edges_.size(); }
  void add_in_edge(DFGEdge* e) { in_edges_.insert(e); }
  void add_out_edge(DFGEdge* e) { out_edges_.insert(e); }

  const plumber_ir::type::DataType& type() const;

  /*! Get the vector of input names of the node.
   *
   * @return a vector of strings
   */
  const std::vector<std::string> inputs() const;

 private:
  DFGNodeProps *props() const { return props_.get(); }

  int id_;                                  /*!< node unique id */
  std::shared_ptr<DFGNodeProps> props_;     /*!< node properties */
  std::unordered_set<DFGEdge *> in_edges_;  /*!< input edges */
  std::unordered_set<DFGEdge *> out_edges_; /*!< output edges */
};

/*! Class that contains essential node properties.
 */
class DFGNodeProps {
 public:
  explicit DFGNodeProps(const plumber_ir::dfg::DFGNodeDef &node_def) : node_def(node_def) {}

  plumber_ir::dfg::DFGNodeDef node_def; /*!< copied DFG node definition */
};

class DFGNodeIterable {
 public:
  explicit DFGNodeIterable(const std::vector<DFGNode *> &nodes)
      : nodes_(nodes) {
  }

  typedef DFGNode *value_type;

  class const_iterator {
   public:
    const_iterator(std::vector<value_type>::const_iterator iter,
                   std::vector<value_type>::const_iterator end)
        : iter_(iter), end_(end) {
      apply_filter();
    }

    bool operator!=(const const_iterator &rhs) {
      return (iter_ != rhs.iter_);
    }

    const_iterator &operator++() {
      ++iter_;
      apply_filter();
      return *this;
    }

    value_type operator*() { return *iter_; }

   private:
    std::vector<value_type>::const_iterator iter_, end_;

    void apply_filter() {
      while (iter_ != end_ && *iter_ == nullptr) {
        ++iter_;
      }
    }
  };

  const_iterator begin() {
    return const_iterator(nodes_.begin(), nodes_.end());
  }

  const_iterator end() {
    return const_iterator(nodes_.end(), nodes_.end());
  }

 private:
  const std::vector<DFGNode *> &nodes_;
};

/*! Class for defining edge in the graph.
 *
 * Each edge contains information about which node is the source node and
 * which one is the destination.
 * Also, since each node has multiple inputs and outputs, each edge should
 * contain indices for corresponding input and output "ports" of each node.
 */
class DFGEdge {
 public:
  /*!
   * The default initializer.
   *
   * @param id id of this edge
   * @param src source node
   * @param src_output index of the port of the source node
   * @param dst destination node
   * @param dst_input index of the connected port of the destination node
   */
  explicit DFGEdge(int id,
                   DFGNode *src,
                   int src_output,
                   DFGNode *dst,
                   int dst_input)
      : id_(id),
        src_(src),
        src_output_(src_output),
        dst_(dst),
        dst_input_(dst_input) {}

  DFGEdge() = delete;

  DFGNode *src() const { return src_; }

  DFGNode *dst() const { return dst_; }

  int id() const { return id_; }

  int src_output() const { return src_output_; }

  int dst_input() const { return dst_input_; }

 private:
  int id_;
  DFGNode *src_;
  DFGNode *dst_;
  int src_output_;
  int dst_input_;
};

/*! Class for defining the graph.
 */
class DFG {
 public:
  DFG();

  ~DFG();

  /*!< The index for a control edge (no data flow) */
  static const int kControlSlot = -1;
  static const int kSourceNodeId = 0;
  static const int kSinkNodeId = 1;
  static constexpr char kSourceNodeName[] = "_SOURCE";
  static constexpr char kSinkNodeName[] = "_SINK";

  DFGNode *AddNode(const plumber_ir::dfg::DFGNodeDef &node_def);

  /*!
   * Add a new edge to the DFG.
   *
   * When adding a new edge, source and destination nodes and their connected
   * slot indices should be specified.
   *
   * @param src
   * @param src_output
   * @param dst
   * @param dst_input
   * @return the created new edge
   */
  DFGEdge *AddEdge(DFGNode *src,
                   int src_output,
                   DFGNode *dst,
                   int dst_input);

  Status IsValidNode(DFGNode *node) const;

  DFGNode *FindNodeId(int id) const { return nodes_[id]; }

  /*! Find node by its name
   *
   * @param name name of the target node
   * @return a pointer to the node
   */
  DFGNode *FindNodeName(std::string name) const;

  int64_t num_nodes() const { return num_nodes_; }
  int64_t num_node_ids() const { return nodes_.size(); }
  int64_t num_edges() const { return num_edges_; }
  int64_t num_edge_ids() const { return edges_.size(); }
  DFGNode *source_node() const { return FindNodeId(kSourceNodeId); }
  DFGNode *sink_node() const { return FindNodeId(kSinkNodeId); }
  DFGNodeIterable nodes() const {
    return DFGNodeIterable(nodes_);
  }
  /*! Search for nodes that haven't connected their outputs to any node yet.
   *
   * @return A list of node pointers
   */
  std::vector<DFGNode *> FindOutputNodes() const;

 private:
  DFGNode *AllocNode(std::shared_ptr<DFGNodeProps> props);

  int64_t num_nodes_ = 0;
  int64_t num_edges_ = 0;

  std::vector<DFGNode *> nodes_;
  std::vector<DFGEdge *> edges_;
};

// Implementation Details
}
}

#endif
