/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_CORE_TENSOR_H_
#define RAINTIME_CORE_TENSOR_H_

#include <memory>
#include <vector>

#include "third_party/Eigen/unsupported/CXX11/Tensor"

#include "plumber_ir/type_def.pb.h"

#include "raintime/core/allocator.hh"
#include "raintime/core/device.hh"
#include "raintime/core/dfg_data_map.hh"
#include "raintime/core/tensor_shape.hh"
#include "raintime/core/tensor_types.hh"
#include "raintime/core/types.hh"

using namespace raintime::dfg;

namespace raintime {

/*!
 * This namespace covers all necessary elements for using tensor
 * arithmetic.
 *
 * See @ref Tensor for more information.
 */
namespace tensor {
/*!
 * Representation for a Tensor in raintime.
 */
class Tensor {
 public:
  /*! Full constructor of the Tensor class.
   *
   * This ctor requires inputs of allocator and
   * tensor shape.
   *
   * \param alct the allocator of the system
   * \param type the type of the tensor
   * \param shape the shape of the tensor
   */
  Tensor(Allocator *alct, DataType type, TensorShape shape);
  Tensor(std::string name, Allocator *alct, DataType type, TensorShape shape);
  Tensor(Allocator *alct, TensorShape shape);
  Tensor(std::string name, Allocator *alct, TensorShape shape);
  Tensor(TensorShape shape);
  Tensor(std::string name, TensorShape shape);
  Tensor(DataType type, TensorShape shape, DFGDataMap::PtrT data);
  Tensor(std::string name, DataType type, TensorShape shape,
         DFGDataMap::PtrT data);
  Tensor(Allocator *alct, DataType type, TensorShape shape,
         DFGDataMap::PtrT data);
  Tensor(std::string name, Allocator *alct, DataType type, TensorShape shape,
         DFGDataMap::PtrT data);

  ~Tensor();

  Allocator *allocator() const { return alct_; }
  size_t dims() const { return shape_.GetNumDims(); }
  int64_t dim_size(int index) const { return shape_.GetDim(index); }
  TensorShape shape() const { return shape_; }
  DataType type() const { return type_; }
  void *ptr() const { return data_; }
  DeviceEnum device() const { return device_; }
  std::string name() const { return name_; }
  size_t pad_size() const { return shape_.pad_size(); }

  template <typename T>
  T *data() const {
    return reinterpret_cast<T *>(data_);
  }
  template <typename T>
  typename T::ST *raw_data() const {
    return reinterpret_cast<typename T::ST *>(raw_data_);
  }

  /*! Flatten the inner data array into another array with
   * a different data type.
   *
   * The type of the data stored is T1 and the target type is T2.
   */
  template <typename T1, typename T2>
  std::vector<T2> flatten() const {
    auto result = std::vector<T2>(num_elems());

    // implicitly copy-and-type-convert contents from the original type to the
    // result
    std::copy(data<T1>(), data<T1>() + num_elems(), result.begin());

    return result;
  }
  int num_elems() const { return shape_.GetNumElems(); }
  size_t num_bytes_per_elem() const { return num_bytes_per_elem_; }
  void *raw_data_ptr() const { return raw_data_; }

  /** Copy data from the given input tensor.
   *
   * This function is mainly used for developing reshape functors.
   * The input tensor should have the same number of elements as the current
   * tensor.
   * Also, both tensors should have the same type.
   *
   * \param src A pointer to the input tensor.
   */
  void CopyDataFrom(Tensor *src);

  /*! Load data from a pointer to an entry in the DFGDataMap.
   * We use raw pointer to enhance code reusability.
   * This whole function should be used in the constructor as well.
   * This function can only loads floating point data.
   *
   * We define raw data as floating-point data.
   *
   * \param data data to be loaded
   * \param size size of the data
   */
  void LoadRawData(float *data, size_t size);

  /*! Allocate raw data.
   * Allocate the memory and sync data from the data memory
   *
   * \return pointer to the raw data.
   */
  template <typename T>
  typename T::ST *AllocRawData() {
    using RT = typename T::ST;

    // whether the raw_data has been allocated.
    bool is_alloc = raw_data_ != nullptr;

    if (!is_alloc) {
      // only initialise cases for fixed-point data.
      using RT = typename T::ST;  // raw data type
      raw_data_ = reinterpret_cast<void *>(alct_->Alloc<RT>(num_elems()));
    }

    // TODO(vince): we might be able to optimise this part later.
    // One optimisation point is, we need to use the CPU memory to allocate the
    // FixedPoint* pointer and only use the FPGA memory for raw_data_.
    // If it is the first time to allocate raw data, we must synchronise. If
    // not, it depends on the flags const_ and sync_to_raw_:
    // 1. Constant tensors should not be synchronised;
    // 2. sync_to_raw_ is true
    if (!is_alloc || (!const_ && sync_to_raw_)) {
      LOG(INFO) << "Synchronizing data of " << name_
                << " from typed to raw ...";
      SyncDataToRawData();
    }

    return raw_data<T>();
  }

  /*! Synchronise inner data from the raw data.
   *
   * Just assign the content in data_ by the corresponding content in raw_data_.
   */
  void SyncDataFromRawData();
  void SyncDataToRawData();

  void Flatten() const;

  void Reshape2D(const int &x, const int &y) const;

  void ReshapeIntoNHWC(const int height, const int width,
                       const int num_channels);

  void ReshapeIntoNCHW(const int height, const int width,
                       const int num_channels);

  /*! Wrap the tensor object into Eigen tensor map.
   *
   * \return A tensor wrapper in Eigen.
   *
   * \tparam T type of the tensor
   * \tparam NDIMS number of dimensions of the expected Tensor.
   */
  template <typename T, size_t NDIMS = 4>
  typename TensorTypes<T, NDIMS>::Tensor tensor() const {
    return typename TensorTypes<T, NDIMS>::Tensor(
        static_cast<T *>(data_), shape_.AsEigenDSizes<NDIMS>());
  }

  DFGDataMap::PtrT FlattenToFloat();

  std::vector<int> GetShapeDimList() {
    auto output_shape = shape().GetDims();
    std::vector<int> shape(output_shape.begin(), output_shape.end());

    return shape;
  }

  void set_pad_size(size_t pad_size) { shape_.set_pad_size(pad_size); }

  void set_const(bool constant) { const_ = constant; }
  void set_sync_from_raw(bool sync_from_raw) { sync_from_raw_ = sync_from_raw; }
  void set_sync_to_raw(bool sync_to_raw) { sync_to_raw_ = sync_to_raw; }
  bool sync_from_raw() const { return sync_from_raw_; }
  bool sync_to_raw() const { return sync_to_raw_; }

  bool is_on_fpga() const {
    // TODO(vince): we should use a direct indicator of whether the tensor is
    // placed on FPGA or not.
    return dynamic_cast<LinearAllocator *>(alct_) != nullptr;
  }

 private:
  Allocator *alct_;
  DataType type_;
  DeviceEnum device_;
  std::string name_;

  mutable TensorShape shape_;
  size_t num_bytes_per_elem_;

  // is this tensor a constant?
  bool const_ = false;

  // flags to enable synchronisation between typed and raw data.
  bool sync_from_raw_ = false;
  bool sync_to_raw_ = false;

  void *data_;
  void *raw_data_ = nullptr; /*!< pointer to raw data */
};
}  // namespace tensor
}  // namespace raintime

#endif
