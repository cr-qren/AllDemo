/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef CORE_TYPES_HH_
#define CORE_TYPES_HH_
#include <iostream>
#include "plumber_ir/type_def.pb.h"

#include "raintime/core/fixed_point.hh"

// NOTE(vince): I think it would be a good idea to use 32 by default.
// #ifdef DATA_TYPE32
#define FIXED_NUM_TT 32
#define FIXED_NUM_FB 21
// #endif

#ifdef DATA_TYPE16
#undef FIXED_NUM_TT
#undef FIXED_NUM_FB
#define FIXED_NUM_TT 16
#define FIXED_NUM_FB 6
#endif

// Implementation notes:
// We put all classes, functions, etc. in under core/ into
// the top namespace.
// We credits design ideas in this file to TensorFlow.

namespace raintime {

using namespace plumber_ir;

/*!
 * This enum class identifies types of memory that a
 * data block should reside in.
 *
 * Regarding the memory architectre in our system,
 * although physically the address space is continuous,
 * there are two implicitly separated parts inside:
 * one is shared between FPGA and CPU, the other one can
 * only be accessed by the host CPU.
 * To clearly define data objects, we need to explicitly
 * assign a memory type to each of them.
 *
 * Considering **extension**,
 * you need to directly revise this enum class and
 * add new enum value.
 */
enum MemoryType {
  HOST_MEMORY = 0,        /*!< Memory for host-only */
  FPGA_SHARED_MEMORY = 1, /*!< Memory shared by FPGA and CPU */
};

/*!
 * This namespace includes all numeric types and their utilities.
 *
 * See @ref Types for the complete documentation.
 */
namespace type {

/*!
 * Specify whether a numeric type is valid in the library
 * during compilation.
 *
 * You need to use the macro \p DATA_TYPE_AND_BUILT_IN macro
 * to specify a type that should be valid.
 */
template <DataType T, typename TP = FixedParamDef>
struct IsDataTypeValid {
  static constexpr bool value = false; /*!< specify whether T is valid */
};

/*!< Convert DataType to built-in types */
template <DataType T, typename TP = FixedParamDef>
struct DataTypeToBuiltIn {
  static_assert(IsDataTypeValid<T>::value, "Numeric type is not supported");
};

#define SINGLE_ARG(...) __VA_ARGS__
#define DATA_TYPE_AND_BUILT_IN(TYPE, BUILT_IN) \
  template <>                                  \
  struct IsDataTypeValid<TYPE> {               \
    static constexpr bool value = true;        \
  };                                           \
  template <>                                  \
  struct DataTypeToBuiltIn<TYPE> {             \
    typedef BUILT_IN T;                        \
  };

DATA_TYPE_AND_BUILT_IN(T_FLOAT, float)
DATA_TYPE_AND_BUILT_IN(T_FIXED32, int32_t)
DATA_TYPE_AND_BUILT_IN(T_FIXED16, int16_t)
DATA_TYPE_AND_BUILT_IN(T_FIXED8, int8_t)
DATA_TYPE_AND_BUILT_IN(FLOAT, float)
DATA_TYPE_AND_BUILT_IN(FIXED, int32_t)
DATA_TYPE_AND_BUILT_IN(INT, int32_t)
DATA_TYPE_AND_BUILT_IN(T_FIXED8_1_U, SINGLE_ARG(FixedPoint<1, 7, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_2_U, SINGLE_ARG(FixedPoint<2, 6, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_3_U, SINGLE_ARG(FixedPoint<3, 5, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_4_U, SINGLE_ARG(FixedPoint<4, 4, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_5_U, SINGLE_ARG(FixedPoint<5, 3, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_6_U, SINGLE_ARG(FixedPoint<6, 2, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_7_U, SINGLE_ARG(FixedPoint<7, 1, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_1_S, SINGLE_ARG(FixedPoint<1, 6, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_2_S, SINGLE_ARG(FixedPoint<2, 5, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_3_S, SINGLE_ARG(FixedPoint<3, 4, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_4_S, SINGLE_ARG(FixedPoint<4, 3, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_5_S, SINGLE_ARG(FixedPoint<5, 2, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_6_S, SINGLE_ARG(FixedPoint<6, 1, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED8_7_S, SINGLE_ARG(FixedPoint<7, 0, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_1_U, SINGLE_ARG(FixedPoint<1, 15, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_2_U, SINGLE_ARG(FixedPoint<2, 14, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_3_U, SINGLE_ARG(FixedPoint<3, 13, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_4_U, SINGLE_ARG(FixedPoint<4, 12, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_5_U, SINGLE_ARG(FixedPoint<5, 11, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_6_U, SINGLE_ARG(FixedPoint<6, 10, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_7_U, SINGLE_ARG(FixedPoint<7, 9, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_8_U, SINGLE_ARG(FixedPoint<8, 8, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_9_U, SINGLE_ARG(FixedPoint<9, 7, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_10_U, SINGLE_ARG(FixedPoint<10, 6, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_11_U, SINGLE_ARG(FixedPoint<11, 5, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_12_U, SINGLE_ARG(FixedPoint<12, 4, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_13_U, SINGLE_ARG(FixedPoint<13, 3, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_14_U, SINGLE_ARG(FixedPoint<14, 2, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_15_U, SINGLE_ARG(FixedPoint<15, 1, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_1_S, SINGLE_ARG(FixedPoint<1, 14, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_2_S, SINGLE_ARG(FixedPoint<2, 13, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_3_S, SINGLE_ARG(FixedPoint<3, 12, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_4_S, SINGLE_ARG(FixedPoint<4, 11, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_5_S, SINGLE_ARG(FixedPoint<5, 10, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_6_S, SINGLE_ARG(FixedPoint<6, 9, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_7_S, SINGLE_ARG(FixedPoint<7, 8, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_8_S, SINGLE_ARG(FixedPoint<8, 7, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_9_S, SINGLE_ARG(FixedPoint<9, 6, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_10_S, SINGLE_ARG(FixedPoint<10, 5, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_11_S, SINGLE_ARG(FixedPoint<11, 4, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_12_S, SINGLE_ARG(FixedPoint<12, 3, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_13_S, SINGLE_ARG(FixedPoint<13, 2, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_14_S, SINGLE_ARG(FixedPoint<14, 1, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED16_15_S, SINGLE_ARG(FixedPoint<15, 0, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_1_U, SINGLE_ARG(FixedPoint<1, 31, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_2_U, SINGLE_ARG(FixedPoint<2, 30, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_3_U, SINGLE_ARG(FixedPoint<3, 29, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_4_U, SINGLE_ARG(FixedPoint<4, 28, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_5_U, SINGLE_ARG(FixedPoint<5, 27, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_6_U, SINGLE_ARG(FixedPoint<6, 26, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_7_U, SINGLE_ARG(FixedPoint<7, 25, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_8_U, SINGLE_ARG(FixedPoint<8, 24, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_9_U, SINGLE_ARG(FixedPoint<9, 23, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_10_U, SINGLE_ARG(FixedPoint<10, 22, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_11_U, SINGLE_ARG(FixedPoint<11, 21, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_12_U, SINGLE_ARG(FixedPoint<12, 20, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_13_U, SINGLE_ARG(FixedPoint<13, 19, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_14_U, SINGLE_ARG(FixedPoint<14, 18, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_15_U, SINGLE_ARG(FixedPoint<15, 17, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_16_U, SINGLE_ARG(FixedPoint<16, 16, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_17_U, SINGLE_ARG(FixedPoint<17, 15, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_18_U, SINGLE_ARG(FixedPoint<18, 14, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_19_U, SINGLE_ARG(FixedPoint<19, 13, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_20_U, SINGLE_ARG(FixedPoint<20, 12, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_21_U, SINGLE_ARG(FixedPoint<21, 11, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_22_U, SINGLE_ARG(FixedPoint<22, 10, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_23_U, SINGLE_ARG(FixedPoint<23, 9, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_24_U, SINGLE_ARG(FixedPoint<24, 8, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_25_U, SINGLE_ARG(FixedPoint<25, 7, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_26_U, SINGLE_ARG(FixedPoint<26, 6, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_27_U, SINGLE_ARG(FixedPoint<27, 5, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_28_U, SINGLE_ARG(FixedPoint<28, 4, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_29_U, SINGLE_ARG(FixedPoint<29, 3, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_30_U, SINGLE_ARG(FixedPoint<30, 2, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_31_U, SINGLE_ARG(FixedPoint<31, 1, false>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_1_S, SINGLE_ARG(FixedPoint<1, 30, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_2_S, SINGLE_ARG(FixedPoint<2, 29, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_3_S, SINGLE_ARG(FixedPoint<3, 28, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_4_S, SINGLE_ARG(FixedPoint<4, 27, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_5_S, SINGLE_ARG(FixedPoint<5, 26, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_6_S, SINGLE_ARG(FixedPoint<6, 25, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_7_S, SINGLE_ARG(FixedPoint<7, 24, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_8_S, SINGLE_ARG(FixedPoint<8, 23, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_9_S, SINGLE_ARG(FixedPoint<9, 22, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_10_S, SINGLE_ARG(FixedPoint<10, 21, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_11_S, SINGLE_ARG(FixedPoint<11, 20, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_12_S, SINGLE_ARG(FixedPoint<12, 19, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_13_S, SINGLE_ARG(FixedPoint<13, 18, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_14_S, SINGLE_ARG(FixedPoint<14, 17, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_15_S, SINGLE_ARG(FixedPoint<15, 16, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_16_S, SINGLE_ARG(FixedPoint<16, 15, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_17_S, SINGLE_ARG(FixedPoint<17, 14, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_18_S, SINGLE_ARG(FixedPoint<18, 13, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_19_S, SINGLE_ARG(FixedPoint<19, 12, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_20_S, SINGLE_ARG(FixedPoint<20, 11, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_21_S, SINGLE_ARG(FixedPoint<21, 10, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_22_S, SINGLE_ARG(FixedPoint<22, 9, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_23_S, SINGLE_ARG(FixedPoint<23, 8, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_24_S, SINGLE_ARG(FixedPoint<24, 7, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_25_S, SINGLE_ARG(FixedPoint<25, 6, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_26_S, SINGLE_ARG(FixedPoint<26, 5, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_27_S, SINGLE_ARG(FixedPoint<27, 4, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_28_S, SINGLE_ARG(FixedPoint<28, 3, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_29_S, SINGLE_ARG(FixedPoint<29, 2, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_30_S, SINGLE_ARG(FixedPoint<30, 1, true>))
DATA_TYPE_AND_BUILT_IN(T_FIXED32_31_S, SINGLE_ARG(FixedPoint<31, 0, true>))
#undef DATA_TYPE_AND_BUILT_IN
#undef BUILT_IN
}  // namespace type
}  // namespace raintime

#endif
