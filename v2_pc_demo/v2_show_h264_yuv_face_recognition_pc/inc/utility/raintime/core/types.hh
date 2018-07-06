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

template <DataType T>
struct IsDataTypeFixedPoint {
  static constexpr bool value = false;
};

/*!< Convert DataType to built-in types */
template <DataType T, typename TP = FixedParamDef>
struct DataTypeToBuiltIn {
  static_assert(IsDataTypeValid<T>::value, "Numeric type is not supported");
};

// a type alias
using TypeEnum = DataType;

inline const char* GetTypeName(TypeEnum type_enum) {
  return DataType_Name(type_enum).c_str();
}

#define SINGLE_ARG(...) __VA_ARGS__
#define DATA_TYPE_AND_BUILT_IN(TYPE, BUILT_IN, IS_FIXED_POINT) \
  template <>                                                  \
  struct IsDataTypeValid<TYPE> {                               \
    static constexpr bool value = true;                        \
  };                                                           \
  template <>                                                  \
  struct IsDataTypeFixedPoint<TYPE> {                          \
    static constexpr bool value = IS_FIXED_POINT;              \
  };                                                           \
  template <>                                                  \
  struct DataTypeToBuiltIn<TYPE> {                             \
    typedef BUILT_IN T;                                        \
  };
#define DATA_TYPE_AND_BUILT_IN_FIXED_POINT(NB, FB)                         \
  DATA_TYPE_AND_BUILT_IN(T_FIXED##NB##_##FB##_U,                           \
                         SINGLE_ARG(FixedPoint<FB, NB - FB, false>), true) \
  DATA_TYPE_AND_BUILT_IN(T_FIXED##NB##_##FB##_S,                           \
                         SINGLE_ARG(FixedPoint<FB, NB - FB - 1, true>), true)

// Floating-point data type
DATA_TYPE_AND_BUILT_IN(T_FLOAT, float, false)

// Default configuration of fixed-point data types
DATA_TYPE_AND_BUILT_IN(
    T_FIXED32,
    SINGLE_ARG(FixedPoint<FIXED_NUM_FB_32, 31 - FIXED_NUM_FB_32, true>), true)
DATA_TYPE_AND_BUILT_IN(
    T_FIXED16,
    SINGLE_ARG(FixedPoint<FIXED_NUM_FB_16, 15 - FIXED_NUM_FB_16, true>), true)
DATA_TYPE_AND_BUILT_IN(
    T_FIXED8, SINGLE_ARG(FixedPoint<FIXED_NUM_FB_8, 7 - FIXED_NUM_FB_8, true>),
    true)

// construct all cases of fixed-point data types
#define ITERATE_ALL_7(F, ...) \
  F(1, ##__VA_ARGS__)         \
  F(2, ##__VA_ARGS__)         \
  F(3, ##__VA_ARGS__)         \
  F(4, ##__VA_ARGS__)         \
  F(5, ##__VA_ARGS__) F(6, ##__VA_ARGS__) F(7, ##__VA_ARGS__)
#define ITERATE_ALL_7_15(F, ...) \
  F(9, ##__VA_ARGS__)            \
  F(10, ##__VA_ARGS__)           \
  F(11, ##__VA_ARGS__)           \
  F(12, ##__VA_ARGS__)           \
  F(13, ##__VA_ARGS__) F(14, ##__VA_ARGS__) F(15, ##__VA_ARGS__)
#define ITERATE_ALL_7_23(F, ...) \
  F(17, ##__VA_ARGS__)           \
  F(18, ##__VA_ARGS__)           \
  F(19, ##__VA_ARGS__)           \
  F(20, ##__VA_ARGS__)           \
  F(21, ##__VA_ARGS__) F(22, ##__VA_ARGS__) F(23, ##__VA_ARGS__)
#define ITERATE_ALL_7_31(F, ...) \
  F(25, ##__VA_ARGS__)           \
  F(26, ##__VA_ARGS__)           \
  F(27, ##__VA_ARGS__)           \
  F(28, ##__VA_ARGS__)           \
  F(29, ##__VA_ARGS__) F(30, ##__VA_ARGS__) F(31, ##__VA_ARGS__)
#define ITERATE_ALL_8(F, ...) \
  ITERATE_ALL_7(F, ##__VA_ARGS__) F(8, ##__VA_ARGS__)
#define ITERATE_ALL_15(F, ...) \
  ITERATE_ALL_8(F, ##__VA_ARGS__) ITERATE_ALL_7_15(F, ##__VA_ARGS__)
#define ITERATE_ALL_16(F, ...) \
  ITERATE_ALL_15(F, ##__VA_ARGS__) F(16, ##__VA_ARGS__)
#define ITERATE_ALL_23(F, ...) \
  ITERATE_ALL_16(F, ##__VA_ARGS__) ITERATE_ALL_7_23(F, ##__VA_ARGS__)
#define ITERATE_ALL_24(F, ...) \
  ITERATE_ALL_23(F, ##__VA_ARGS__) F(24, ##__VA_ARGS__)
#define ITERATE_ALL_31(F, ...) \
  ITERATE_ALL_24(F, ##__VA_ARGS__) ITERATE_ALL_7_31(F, ##__VA_ARGS__)
#define ITERATE_ALL_32(F, ...) \
  ITERATE_ALL_31(F, ##__VA_ARGS__) F(32, ##__VA_ARGS__)

// 8-bit fixed-point data types
#define DATA_TYPE_AND_BUILT_IN_FIXED8(i) \
  DATA_TYPE_AND_BUILT_IN_FIXED_POINT(8, i)
// 16-bit fixed-point data types
#define DATA_TYPE_AND_BUILT_IN_FIXED16(i) \
  DATA_TYPE_AND_BUILT_IN_FIXED_POINT(16, i)
// 32-bit fixed-point data types
#define DATA_TYPE_AND_BUILT_IN_FIXED32(i) \
  DATA_TYPE_AND_BUILT_IN_FIXED_POINT(32, i)

ITERATE_ALL_7(DATA_TYPE_AND_BUILT_IN_FIXED8)
ITERATE_ALL_15(DATA_TYPE_AND_BUILT_IN_FIXED16)
ITERATE_ALL_31(DATA_TYPE_AND_BUILT_IN_FIXED32)

#undef DATA_TYPE_AND_BUILT_IN_FIXED_POINT
#undef DATA_TYPE_AND_BUILT_IN

// Call types under a specific category
#define BUILT_IN(TYPE) DataTypeToBuiltIn<TYPE>::T
// Actual place that calls the FUNC
#define WRAP_BUILT_IN(TYPE, FUNC, ...) \
  FUNC(SINGLE_ARG(BUILT_IN(TYPE)), ##__VA_ARGS__)

// initialize function with floating-point data types
#define INIT_BY_FLOAT_TYPE(FUNC, ...) FUNC(T_FLOAT, ##__VA_ARGS__)

// initialize function with fixed-point data types
#define INIT_BY_FIXED_NB_FB_TYPE(NB, FB, FUNC, ...) \
  FUNC(T_FIXED##NB##_##FB##_S, ##__VA_ARGS__)       \
  FUNC(T_FIXED##NB##_##FB##_U, ##__VA_ARGS__)

#define INIT_BY_FIXED32_FB_TYPE(FB, FUNC, ...) \
  INIT_BY_FIXED_NB_FB_TYPE(32, FB, FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED16_FB_TYPE(FB, FUNC, ...) \
  INIT_BY_FIXED_NB_FB_TYPE(16, FB, FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED8_FB_TYPE(FB, FUNC, ...) \
  INIT_BY_FIXED_NB_FB_TYPE(8, FB, FUNC, ##__VA_ARGS__)

#define INIT_BY_FIXED32_TYPE_W(FUNC, ...) \
  ITERATE_ALL_31(INIT_BY_FIXED32_FB_TYPE, FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED16_TYPE_W(FUNC, ...) \
  ITERATE_ALL_15(INIT_BY_FIXED16_FB_TYPE, FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED8_TYPE_W(FUNC, ...) \
  ITERATE_ALL_7(INIT_BY_FIXED8_FB_TYPE, FUNC, ##__VA_ARGS__)

#define INIT_BY_FIXED32_TYPE_O(FUNC, ...) FUNC(T_FIXED32, ##__VA_ARGS__)
#define INIT_BY_FIXED16_TYPE_O(FUNC, ...) FUNC(T_FIXED16, ##__VA_ARGS__)
#define INIT_BY_FIXED8_TYPE_O(FUNC, ...) FUNC(T_FIXED8, ##__VA_ARGS__)

// in the LITE mode, we only use T_FIXED32, T_FIXED16, and T_FIXED8
#ifdef LITE
#define INIT_BY_FIXED32_TYPE(FUNC, ...) \
  INIT_BY_FIXED32_TYPE_O(FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED16_TYPE(FUNC, ...) \
  INIT_BY_FIXED16_TYPE_O(FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED8_TYPE(FUNC, ...) \
  INIT_BY_FIXED8_TYPE_O(FUNC, ##__VA_ARGS__)

#define INIT_BY_FIXED_TYPE_W(FUNC, ...)     \
  INIT_BY_FIXED32_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED16_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED8_TYPE(FUNC, ##__VA_ARGS__)
#else
#define INIT_BY_FIXED32_TYPE(FUNC, ...)       \
  INIT_BY_FIXED32_TYPE_O(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED32_TYPE_W(FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED16_TYPE(FUNC, ...)       \
  INIT_BY_FIXED16_TYPE_O(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED16_TYPE_W(FUNC, ##__VA_ARGS__)
#define INIT_BY_FIXED8_TYPE(FUNC, ...)       \
  INIT_BY_FIXED8_TYPE_O(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED8_TYPE_W(FUNC, ##__VA_ARGS__)

#define INIT_BY_FIXED_TYPE_W(FUNC, ...)       \
  INIT_BY_FIXED32_TYPE_W(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED16_TYPE_W(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED8_TYPE_W(FUNC, ##__VA_ARGS__)
#endif

#define INIT_BY_FIXED_TYPE(FUNC, ...)       \
  INIT_BY_FIXED32_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED16_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED8_TYPE(FUNC, ##__VA_ARGS__)

#define INIT_BY_ALL_TYPE(FUNC, ...)       \
  INIT_BY_FLOAT_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED_TYPE(FUNC, ##__VA_ARGS__)

#define INIT_BY_ALL_TYPE_W(FUNC, ...)     \
  INIT_BY_FLOAT_TYPE(FUNC, ##__VA_ARGS__) \
  INIT_BY_FIXED_TYPE_W(FUNC, ##__VA_ARGS__)

#undef SINGLE_ARG

}  // namespace type
}  // namespace raintime

#endif
