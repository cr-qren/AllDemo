/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */
#ifndef RAINTIME_DFG_RUNNER_HH
#define RAINTIME_DFG_RUNNER_HH

#include "plumber_ir/dfg_def.pb.h"
#include "raintime/core/allocator.hh"
#include "raintime/core/dfg.hh"
#include "raintime/core/dfg_data_map.hh"
#include "raintime/core/op_impl.hh"
#include "raintime/core/tensor.hh"

#include <map>
#include <set>

using namespace plumber_ir;
using namespace raintime::tensor;
using namespace raintime::op_impl;

namespace raintime {
namespace dfg {
class DFGRunner {
 public:
  using TensorMap = std::map<std::string, Tensor *>;
  using TensorMapKeyT = std::string;
  using TensorMapPair = std::pair<std::string, Tensor *>;
  using NodeList = std::vector<DFGNode *>;
  using DeviceEnum = DFGNodeDef_Device;

  /*! The default constructor for the DFGRunner.
   *
   * \param is_sim should the runner execute software simulated
   *  functions for those ops appointed to FpgaDevice.
   */
  DFGRunner(bool is_sim = true);
  ~DFGRunner();

  DFGDataMap *Run(DFG *dfg, DFGDataMap *const_data_map,
                  DFGDataMap *input_data_map,
                  bool collect_intermediate_output = false);

  /*!< Look for whether the tensor exists. */
  bool HasTensor(TensorMapKeyT key);

  /*! Create tensor and load data into it.
   *
   * The behavior of this function depends on tensor_map_ and
   * reuse_tensor_.
   * It will create a new tensor in the tensor_map if node->name()
   * is not found in it,
   * and will not do it if reuse_tensor_ is false.
   * It will load data into the tensor in the tensor map unless
   * explicitly specified.
   */
  Tensor *LoadTensor(DFGNode *node, DFGDataMap::PtrT data, TensorShape shape,
                     TensorMapKeyT tensor_key = "");
  Tensor *LoadTensorFromDataMap(DFGNode *node, DFGDataMap::KeyT data_key,
                                DFGDataMap *data_map,
                                TensorMapKeyT tensor_key = "");

  /*! Load constants into tensors from data map.
   *
   * We check update_const_ if the tensor requested exists
   * to see whether the constant should be updated.
   * You should provide the tensor key since constant values are
   * commonly not equal to the name of the node.
   */
  Tensor *LoadConstTensorFromDataMap(DFGNode *node, DFGDataMap::KeyT data_key,
                                     DFGDataMap *data_map,
                                     TensorMapKeyT tensor_key);

  Allocator *GetAllocator(DeviceEnum device);
  TensorShape GetInputTensorShape(DFGNodeDef node_def);

  /*! Create tensor in the tensor_map_ */
  Tensor *CreateTensor(TensorMapKeyT name, DeviceEnum device, DataType type,
                       TensorShape shape);
  Tensor *CreateTensor(TensorMapKeyT name, Allocator *alct, DataType type,
                       TensorShape shape);

  Tensor *GetTensor(std::string key);
  Tensor *GetTensor(std::string key, DataType type);
  Tensor *GetTensor(std::string key, TensorMap *tensor_map);
  Tensor *GetTensor(std::string key, TensorMap *tensor_map, DataType type);

  /*! Insert a tensor into the inner tensor_map.
   * If the key is in the map, and we allow overwrite,
   * we simply replace the old tensor by the new one.
   * If the overwrite behavior is not allow, we throw an exception.
   *
   * \param key key of the tensor map entry
   * \param tensor the tensor to be inserted
   */
  void InsertTensor(TensorMapKeyT key, Tensor *tensor);
  void InsertTensor(TensorMapKeyT key, Tensor *tensor, bool overwrite);

  /*! Remove tensor from the inner tensor map.
   * The tensor will be deleted and the corresponding entry will
   * be deleted.
   * If the key is not found, we will throw an exception.
   */
  void RemoveTensor(TensorMapKeyT key);

  /*! Create tensor in the tensor_map_ */
  Tensor *GetOrCreateTensor(TensorMapKeyT name, DeviceEnum device,
                            DataType type, TensorShape shape);
  Tensor *GetOrCreateTensor(TensorMapKeyT name, Allocator *alct, DataType type,
                            TensorShape shape);

  /*! Initialise the input nodes of DFG.
   *
   * This method will insert input nodes into nodes_to_process,
   * as the runner will process input nodes in the first place.
   * tensor_map will hold the tensor that stores input data in the current run.
   * input_data_map contains all the data for every input nodes.
   *
   * We expect all the input nodes will be properly initialised.
   *
   * \param dfg The DFG to be executed
   * \param nodes_to_process A list of nodes that will be processed in sequence
   * \param input_data_map A data map that contains data for all input nodes
   */
  void InitInputNodes(DFG *dfg, NodeList *nodes_to_process,
                      DFGDataMap *input_data_map);

  /*! Create tensors for constants found by checking each node in the DFG.
   *
   * This function should insert constant tensors into tensor_map.
   * Reusing tensor is also supported.
   *
   * \param dfg a pointer to the DFG to be executed
   * \param data_map a pointer to the data map that holds all constant data
   */
  void InitConstTensors(DFG *dfg, DFGDataMap *data_map);

  DFGNode *UpdateNodesToProcess(NodeList *nodes_to_process);

  OpImpl *CreateOpImpl(DFGNode *node);
  OpImplEnv *CreateOpImplEnv(DFGNode *node);
  DFGDataMap *CreateOutputDataMap(DFG *dfg,
                                  bool collect_intermediate_output = false);

  size_t base_addr() const { return base_addr_; }
  size_t num_bytes() const { return num_bytes_; }
  const TensorMap *tensor_map() const { return &tensor_map_; }

  void set_reuse_tensor(bool reuse_tensor) { reuse_tensor_ = reuse_tensor; }
  void set_update_const(bool update_const) { update_const_ = update_const; }
  void set_allow_tensor_overwrite(bool allow_tensor_overwrite) {
    allow_tensor_overwrite_ = allow_tensor_overwrite;
  }

  bool IsTensorCreated(TensorMapKeyT key) const {
    return created_tensors_.find(key) != created_tensors_.end();
  }

  /*!< Recycle all allocated op_impl_env during a single run. */
  void RecycleAllOpImplEnvs();

  /*!< Recycle all allocated tensors for type casting. */
  void RecycleTypeCastTensors();

 private:
  bool reuse_tensor_ = true;  /*!< shall we reuse tensor by name */
  bool update_const_ = false; /*!< shall we update the content in constants */
  bool allow_tensor_overwrite_ = false; /*!< tensor entry can be overwriten */
  /*!< automatically recycle type case tensors in the end */
  bool recycle_type_cast_tensors_ = true;

  bool is_sim_;
  size_t base_addr_;
  size_t num_bytes_;

  DefaultCpuAllocator *cpu_alct_;
  LinearAllocator *fpga_alct_;

  TensorMap tensor_map_;
  std::set<TensorMapKeyT> created_tensors_;
  std::vector<OpImplEnv *> op_impl_envs_;
};
}  // namespace dfg
}  // namespace raintime

#endif
