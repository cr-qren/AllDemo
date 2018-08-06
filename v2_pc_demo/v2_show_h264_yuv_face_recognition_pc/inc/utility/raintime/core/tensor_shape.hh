/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_CORE_TENSOR_SHAPE_HH_
#define RAINTIME_CORE_TENSOR_SHAPE_HH_

#include <stdint.h>
#include <stdlib.h>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Eigen/unsupported/CXX11/Tensor"

namespace raintime {
namespace tensor {

/*!
 * Represents the shape of a Tensor.
 *
 * TensorFlow provides a powerful and complicated implementation
 * of this class,
 * we don't need that.
 * The key idea of designing this class is to provide very
 * basic representation of the shape of the Tensor.
 *
 * We use std::vector as the basic data structure.
 *
 * \todo Provide an implementation based on a new tensor_shape.proto
 * file.
 */
class TensorShape {
 public:
  static constexpr int kMaxNumDims =
      4; /*!< maximum number of dimensions in a Tensor */

  TensorShape() {}

  /*! create a tensor shape with num_dims.
   * \param num_dims Number of dimensions of this tensor shape.
   */
  TensorShape(size_t num_dims) : dims_(num_dims) {}

  TensorShape(std::vector<int> dims) {
    for (auto dim : dims) dims_.push_back(dim);
  }

  /*! create a tensor shape with a initialiser list.
   * \param l the initialiser list
   */
  TensorShape(std::initializer_list<int64_t> l) {
    if (l.size() > kMaxNumDims) {
      throw std::runtime_error(
          "The initializer list should be shorter than maximal number of "
          "dimensions");
    }

    // copy
    dims_ = l;
  }

  /*! copy ctor */
  TensorShape(const TensorShape &rhs) {
    // copy the dims vector from rhs to the current object
    this->dims_ = rhs.GetDims();
  }

  friend bool operator==(const TensorShape &lhs, const TensorShape &rhs) {
    if (rhs.GetNumDims() != lhs.GetNumDims()) return false;

    for (int i = 0; i < lhs.GetNumDims(); i++)
      if (rhs.GetDim(i) != lhs.GetDim(i)) return false;

    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const TensorShape &shape) {
    os << "TensorShape(";
    for (int i = 0; i < shape.GetNumDims(); i++) {
      os << shape.GetDim(i);
      if (i != shape.GetNumDims() - 1) os << ", ";
    }
    os << ")";

    return os;
  }

  /*! Get the dimensions in the tensor shape
   *
   * \return The dimension vector
   * \todo May use a iterator based approach.
   */
  std::vector<int64_t> GetDims() const { return dims_; }

  /*! Get number of dimensions.
   * \return number of dimensions
   */
  size_t GetNumDims() const { return dims_.size(); }

  /*! Get number of elements at the idx dimension.
   *
   * \note We don't handle the exception related to
   * `idx > kMaxNumDims` here. You can try to handle
   * it in your upper applications.
   *
   * \param idx id of the dimension
   * \return number of elements in that dimension
   */
  int64_t GetDim(int idx) const { return dims_[idx]; }

  /*! Get total number of elements
   * \return Total number of elements
   */
  int64_t GetNumElems() const {
    if (GetNumDims() == 0) {
      return 0;
    }

    int64_t ans = 1;
    for (auto num_elems : dims_) {
      ans *= num_elems;
    }

    return ans;
  }

  /*! Change the number of elements in dim.
   *
   * If you pass dim >= total dimensions, you will receive
   * exception.
   *
   * \param dim index for dimension
   * \param num_elems number of elements to be set at this dimension
   */
  void InsertDim(size_t dim, int64_t num_elems) {
    // will throw exception if dim is out of range
    if (dim >= GetNumDims())
      throw std::out_of_range("dimension should within the range");

    dims_[dim] = num_elems;
  }

  /*! Directly append a new dimension.
   * \param num_elems number of elements in the new dimension.
   */
  void AddDim(int64_t num_elems) {
    if (GetNumDims() >= kMaxNumDims)
      throw std::runtime_error(
          "current number of dimensions already reached maximal number of "
          "dimensions");

    dims_.push_back(num_elems);
  }

  /*! Convert tensor shape to DSizes in Eigen.
   *
   * \return Eigen DSizes
   */
  template <size_t NDIMS>
  Eigen::DSizes<Eigen::DenseIndex, NDIMS> AsEigenDSizes() const {
    static_assert(NDIMS <= TensorShape::kMaxNumDims,
                  "NDIMS should not exceed kMaxNumDims");

    if (NDIMS < GetNumDims())
      throw std::runtime_error(
          "NDIMS should not be less than the current dimensions");

    Eigen::DSizes<Eigen::DenseIndex, NDIMS> dsizes;
    for (int d = 0; d < GetNumDims(); d++) dsizes[d] = dims_[d];
    for (int d = GetNumDims(); d < NDIMS; d++) dsizes[d] = 1;
    // for (int d = 0; d < GetNumDims(); d++) dsizes[NDIMS - d - 1] = dims_[d];
    // for (int d = GetNumDims(); d < NDIMS; d++) dsizes[NDIMS - d - 1] = 1;

    return dsizes;
  }

 private:
  std::vector<int64_t> dims_;
};
}  // namespace tensor
}  // namespace raintime
#endif
