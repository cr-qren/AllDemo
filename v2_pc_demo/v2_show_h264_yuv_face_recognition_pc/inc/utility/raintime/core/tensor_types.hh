/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_CORE_TENSOR_TYPES_H_
#define RAINTIME_CORE_TENSOR_TYPES_H_

#include "Eigen/unsupported/CXX11/Tensor"

namespace raintime {
namespace tensor {

/*!
 * Types that characterise tensors of different dimensions.
 *
 * Types under this scope are simply wrappers of
 * `TensorMap` in Eigen.
 * To use these types, you just need to make sure you
 * prepare the tensor data in a pre-allocated buffer,
 * and create tensors by following the `TensorMap`
 * documentation in Eigen.
 *
 * An example is in `test/tensor_test.cc` (`TensorInit` test).
 *
 * \tparam T scalar type
 * \tparam N number of dimensions
 * \note We must assign `Eigen::RowMajor` or something unexpected
 * might happen.
 */
template <typename T, size_t N = 1, typename IndexType = Eigen::DenseIndex>
struct TensorTypes {
  typedef Eigen::TensorMap<Eigen::Tensor<T, N, Eigen::RowMajor, IndexType>,
                           Eigen::Aligned>
      Tensor; /*!< Multi-dimensional array type */
  typedef Eigen::TensorMap<Eigen::Tensor<T, 2, Eigen::RowMajor, IndexType>,
                           Eigen::Aligned>
      Matrix; /*!< Matrix type */
  typedef Eigen::TensorMap<Eigen::Tensor<T, 1, Eigen::RowMajor, IndexType>,
                           Eigen::Aligned>
      Vector; /*!< Vector type */
};
}
}

#endif
