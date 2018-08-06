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
  Tensor(Allocator *alct, TensorShape shape);
  Tensor(TensorShape shape);
  Tensor(DataType type, TensorShape shape, DFGDataMap::PtrT data);
  Tensor(Allocator *alct, DataType type, TensorShape shape,
         DFGDataMap::PtrT data, const FixedParamDef *fixed_param_def = nullptr);

  ~Tensor();

  Allocator *allocator() const { return alct_; }
  size_t dims() const { return shape_.GetNumDims(); }
  int64_t dim_size(int index) const { return shape_.GetDim(index); }
  TensorShape shape() const { return shape_; }
  DataType type() const { return type_; }
  void *ptr() const { return data_; }

  template <typename T>
  T *data() const {
    return reinterpret_cast<T *>(data_);
  }

  int num_elems() const { return shape_.GetNumElems(); }

  /*! Get the number of bytes for each element in the stored tensor data */
  size_t num_bytes_per_elem() const { return num_bytes_per_elem_; }

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

 private:
  Allocator *alct_;
  void *data_;
  mutable TensorShape shape_;
  size_t num_bytes_per_elem_;
  DataType type_;
};
}  // namespace tensor
}  // namespace raintime

#endif
