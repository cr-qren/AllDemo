/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_FIXED_POINT_HH__
#define RAINTIME_FIXED_POINT_HH__

#include <glog/logging.h>
#include <stdint.h>
#include <unistd.h>
#include <cmath>
#include <limits>
#include <third_party/Eigen/Core>

namespace raintime {

inline constexpr float PowerOfTwoByShift(const int &exp) {
  return (exp >= 0)
             ? (static_cast<float>(static_cast<int64_t>(1) << exp))
             : (1.0 / static_cast<float>(static_cast<int64_t>(1) << (-exp)));
}

/*!< Return the scaling factor of a fixed-point value */
inline constexpr float GetFixedPointScale(const int &num_fb) {
  return PowerOfTwoByShift(num_fb);
}

/*! \brief Get the maximum value in float that can be represented by
 * the FixedPoint<num_fb, num_ib, sign>.
 */
inline constexpr float GetMaxFixedPointValue(const int &num_fb,
                                             const int &num_bits,
                                             const bool &sign) {
  return sign
             ? ((PowerOfTwoByShift(num_bits - 1) - 1) /
                GetFixedPointScale(num_fb))
             : ((PowerOfTwoByShift(num_bits) - 1) / GetFixedPointScale(num_fb));
}

/*! \brief Get the minimum value in float that can be represented by
 * the FixedPoint<num_fb, num_ib, sign>.
 */
inline constexpr float GetMinFixedPointValue(const int &num_fb,
                                             const int &num_bits,
                                             const bool &sign) {
  return sign ? (-(PowerOfTwoByShift(num_bits - 1) - 1) /
                 GetFixedPointScale(num_fb))
              : 0.0f;
}

/*!< Clip the input value by the given range. */
inline float ClipByValue(const float &val, float min, float max) {
  float new_val = val;
  new_val = new_val < min ? min : new_val;
  new_val = new_val > max ? max : new_val;
  return new_val;
}

/*!< Round the given floating point to fixed-point through
 * the round-to-nearest method.
 */
inline float RoundToNearest(const float &x, const float &scale, float min_val,
                            float max_val, const bool &min_max_scaled = false,
                            const bool &scale_result = true) {
  float rounded_and_scaled = std::floor(x * scale + 0.5f);
  if (!min_max_scaled) {
    min_val *= scale;
    max_val *= scale;
  }
  float result = ClipByValue(rounded_and_scaled, min_val, max_val);
  return scale_result ? result / scale : result;
}

/*!< Round to nearest with values computed online */
inline float RoundToNearest(const float &x, const int &num_fb,
                            const int &num_bits, const bool &sign,
                            const bool &scale_result = true) {
  CHECK(!(!sign && x < 0))
      << "We cannot represent minus value with unsigned integer";

  float scale = GetFixedPointScale(num_fb);
  float min_val = GetMinFixedPointValue(num_fb, num_bits, sign);
  float max_val = GetMaxFixedPointValue(num_fb, num_bits, sign);

  return RoundToNearest(x, scale, min_val, max_val, false, scale_result);
}

template <int NF, int NI, bool S>
struct FixedPointTraits {
  static const bool IsValid = false;
};

#define DOUBLE(x) DOUBLE_##x
#define DOUBLE_8 16
#define DOUBLE_16 32
#define DOUBLE_32 64
#define CAT2(x, y, z) x##y##z
#define CAT(x, y, z) CAT2(x, y, z)

#define INIT_FIXED_POINT_TRAITS(nf, bw)              \
  template <>                                        \
  struct FixedPointTraits<nf, bw - nf - 1, true> {   \
    static const bool IsValid = true;                \
    typedef int##bw##_t StorageType;                 \
    typedef CAT(int, DOUBLE(bw), _t) TemporaryType;  \
  };                                                 \
  template <>                                        \
  struct FixedPointTraits<nf, bw - nf, false> {      \
    static const bool IsValid = true;                \
    typedef uint##bw##_t StorageType;                \
    typedef CAT(uint, DOUBLE(bw), _t) TemporaryType; \
  };

#define INIT_FIXED_POINT_TRAITS_8(nf) INIT_FIXED_POINT_TRAITS(nf, 8)
#define INIT_FIXED_POINT_TRAITS_16(nf) INIT_FIXED_POINT_TRAITS(nf, 16)
#define INIT_FIXED_POINT_TRAITS_32(nf) INIT_FIXED_POINT_TRAITS(nf, 32)

template <int NF, int NI, bool S>
class FixedPoint {
 public:
  // ST stands for storage type, TT stands for temporary type
  typedef typename FixedPointTraits<NF, NI, S>::StorageType ST;
  typedef typename FixedPointTraits<NF, NI, S>::TemporaryType TT;

  static constexpr int NB = S ? NF + NI + 1 : NF + NI;
  static constexpr float SCALE = GetFixedPointScale(NF);
  // min and max in float
  static constexpr float MIN_FLOAT = GetMinFixedPointValue(NF, NB, S);
  static constexpr float MAX_FLOAT = GetMaxFixedPointValue(NF, NB, S);
  // min and max in ST
  static constexpr ST MAX_FIXED = std::numeric_limits<ST>::max();
  static constexpr ST MIN_FIXED = std::numeric_limits<ST>::min();

  static constexpr TT ROUND_VALUE = (1 << (NF - 1));

  FixedPoint() {}

  FixedPoint(ST val) { val_ = val; }

  FixedPoint(const float &val) {
    val_ = static_cast<ST>(RoundToNearest(val, false));
  }

  static float RoundToNearest(const float val, bool scale_result = true) {
    // wrap the function external to this class
    // we round with scaled min-max values and scale the returned value by
    // default.
    return ::raintime::RoundToNearest(val, scale(), min_in_float_scaled(),
                                      max_in_float_scaled(), true,
                                      scale_result);
  }

  static ST Saturate(TT x) {
    if (x > max())
      return max();
    else if (x < min())
      return min();
    else
      return static_cast<ST>(x);
  }

  FixedPoint<NF, NI, S> &operator=(const float val) {
    val_ = static_cast<ST>(RoundToNearest(val, false));
    return *this;
  }

  FixedPoint<NF, NI, S> &operator=(ST val) {
    val_ = val;
    return *this;
  }

  explicit operator float() const { return static_cast<float>(val_) / SCALE; }

  ST raw_value() const { return val_; }

  template <int NF2, int NI2, bool S2>
  FixedPoint<NF, NI, S> &operator=(const FixedPoint<NF2, NI2, S2> &fp2) {
    *this = static_cast<float>(fp2);
    return *this;
  }

  FixedPoint<NF, NI, S> operator*(const FixedPoint<NF, NI, S> &fp) const {
    TT tmp = static_cast<TT>(val()) * static_cast<TT>(fp.val());
    tmp += ROUND_VALUE;

    return FixedPoint<NF, NI, S>(Saturate(tmp >> NF));
  }
  raintime::FixedPoint<NF, NI, S> operator*=(const FixedPoint<NF, NI, S> &fp) {
    TT tmp = static_cast<TT>(val()) * static_cast<TT>(fp.val());
    tmp += ROUND_VALUE;
    val_ = FixedPoint<NF, NI, S>(Saturate(tmp >> NF));
    return *this;
  }

  FixedPoint<NF, NI, S> operator+(const FixedPoint<NF, NI, S> &fp) const {
    return FixedPoint<NF, NI, S>(
        Saturate(static_cast<TT>(val()) + static_cast<TT>(fp.val())));
  }

  FixedPoint<NF, NI, S> operator+=(const FixedPoint<NF, NI, S> &fp) {
    val_ = Saturate(static_cast<TT>(val()) + static_cast<TT>(fp.val()));
    return *this;
  }

  FixedPoint<NF, NI, S> operator-(const FixedPoint<NF, NI, S> &fp) const {
    return FixedPoint<NF, NI, S>(
        Saturate(static_cast<TT>(val()) - static_cast<TT>(fp.val())));
  }

  FixedPoint<NF, NI, S> operator-=(const FixedPoint<NF, NI, S> &fp) {
    val_ = Saturate(static_cast<TT>(val()) - static_cast<TT>(fp.val()));
    return *this;
  }

  FixedPoint<NF, NI, S> operator/(const FixedPoint<NF, NI, S> &fp) const {
    TT temp = static_cast<TT>(val()) << NF;
    if ((temp >= static_cast<TT>(0.0f) &&
         static_cast<TT>(fp.val()) >= static_cast<TT>(0.0f)) ||
        (temp < static_cast<TT>(0.0f) && static_cast<TT>(fp.val()) < 0)) {
      temp += static_cast<TT>(fp.val()) / static_cast<TT>(2.0f);
    } else {
      temp -= static_cast<TT>(fp.val()) / static_cast<TT>(2.0f);
    }
    return Saturate(static_cast<TT>(temp) / static_cast<TT>(fp.val()));
  }
  FixedPoint<NF, NI, S> operator/=(const FixedPoint<NF, NI, S> &fp) const {
    TT temp = static_cast<TT>(val()) << NF;
    if ((temp >= static_cast<TT>(0.0f) &&
         static_cast<TT>(fp.val()) >= static_cast<TT>(0.0f)) ||
        (temp < static_cast<TT>(0.0f) && static_cast<TT>(fp.val()) < 0)) {
      temp += static_cast<TT>(fp.val()) / static_cast<TT>(2.0f);
    } else {
      temp -= static_cast<TT>(fp.val()) / static_cast<TT>(2.0f);
    }
    val_ = Saturate(static_cast<TT>(temp) / static_cast<TT>(fp.val()));
    return *this;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const FixedPoint<NF, NI, S> &fp) {
    os << static_cast<float>(fp.val());
    return os;
  }

  bool operator<(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) < static_cast<float>(fp.val());
  }
  bool operator>(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) > static_cast<float>(fp.val());
  }
  bool operator==(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) == static_cast<float>(fp.val());
  }
  bool operator!=(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) != static_cast<float>(fp.val());
  }
  bool operator<=(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) <= static_cast<float>(fp.val());
  }
  bool operator>=(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) >= static_cast<float>(fp.val());
  }
  bool operator&&(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) && static_cast<float>(fp.val());
  }
  bool operator||(const FixedPoint<NF, NI, S> &fp) const {
    return static_cast<float>(this->val_) || static_cast<float>(fp.val());
  }
  bool operator&(const FixedPoint<NF, NI, S> &fp) const {
    return this->val_ & fp.val();
  }
  bool operator|(const FixedPoint<NF, NI, S> &fp) const {
    return this->val_ | fp.val();
  }

  ST val() const { return val_; }

  static int num_bits() { return NB; }
  static float scale() { return SCALE; }
  static ST max() { return MAX_FIXED; }
  static ST min() { return MIN_FIXED; }
  static float max_in_float() { return MAX_FLOAT; }
  static float min_in_float() { return MIN_FLOAT; }
  static float max_in_float_scaled() { return max_in_float() * scale(); }
  static float min_in_float_scaled() { return min_in_float() * scale(); }

 private:
  ST val_;
};

INIT_FIXED_POINT_TRAITS_8(1);
INIT_FIXED_POINT_TRAITS_8(2);
INIT_FIXED_POINT_TRAITS_8(3);
INIT_FIXED_POINT_TRAITS_8(4);
INIT_FIXED_POINT_TRAITS_8(5);
INIT_FIXED_POINT_TRAITS_8(6);
INIT_FIXED_POINT_TRAITS_8(7);

INIT_FIXED_POINT_TRAITS_16(1);
INIT_FIXED_POINT_TRAITS_16(2);
INIT_FIXED_POINT_TRAITS_16(3);
INIT_FIXED_POINT_TRAITS_16(4);
INIT_FIXED_POINT_TRAITS_16(5);
INIT_FIXED_POINT_TRAITS_16(6);
INIT_FIXED_POINT_TRAITS_16(7);
INIT_FIXED_POINT_TRAITS_16(8);
INIT_FIXED_POINT_TRAITS_16(9);
INIT_FIXED_POINT_TRAITS_16(10);
INIT_FIXED_POINT_TRAITS_16(11);
INIT_FIXED_POINT_TRAITS_16(12);
INIT_FIXED_POINT_TRAITS_16(13);
INIT_FIXED_POINT_TRAITS_16(14);
INIT_FIXED_POINT_TRAITS_16(15);

INIT_FIXED_POINT_TRAITS_32(1);
INIT_FIXED_POINT_TRAITS_32(2);
INIT_FIXED_POINT_TRAITS_32(3);
INIT_FIXED_POINT_TRAITS_32(4);
INIT_FIXED_POINT_TRAITS_32(5);
INIT_FIXED_POINT_TRAITS_32(6);
INIT_FIXED_POINT_TRAITS_32(7);
INIT_FIXED_POINT_TRAITS_32(8);
INIT_FIXED_POINT_TRAITS_32(9);
INIT_FIXED_POINT_TRAITS_32(10);
INIT_FIXED_POINT_TRAITS_32(11);
INIT_FIXED_POINT_TRAITS_32(12);
INIT_FIXED_POINT_TRAITS_32(13);
INIT_FIXED_POINT_TRAITS_32(14);
INIT_FIXED_POINT_TRAITS_32(15);
INIT_FIXED_POINT_TRAITS_32(16);
INIT_FIXED_POINT_TRAITS_32(17);
INIT_FIXED_POINT_TRAITS_32(18);
INIT_FIXED_POINT_TRAITS_32(19);
INIT_FIXED_POINT_TRAITS_32(20);
INIT_FIXED_POINT_TRAITS_32(21);
INIT_FIXED_POINT_TRAITS_32(22);
INIT_FIXED_POINT_TRAITS_32(23);
INIT_FIXED_POINT_TRAITS_32(24);
INIT_FIXED_POINT_TRAITS_32(25);
INIT_FIXED_POINT_TRAITS_32(26);
INIT_FIXED_POINT_TRAITS_32(27);
INIT_FIXED_POINT_TRAITS_32(28);
INIT_FIXED_POINT_TRAITS_32(29);
INIT_FIXED_POINT_TRAITS_32(30);
INIT_FIXED_POINT_TRAITS_32(31);
}  // namespace raintime

namespace Eigen {
template <int NF, int NI, bool S>
struct NumTraits<raintime::FixedPoint<NF, NI, S>> : NumTraits<float> {
  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsReal = 1,
    IsSigned = S,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 1,
    MulCost = 3,
    DivCost = 3
  };
  static inline raintime::FixedPoint<NF, NI, S> highest() {
    return raintime::FixedPoint<NF, NI, S>::max();
  }

  static inline raintime::FixedPoint<NF, NI, S> lowest() {
    return raintime::FixedPoint<NF, NI, S>::min();
  }
};
}  // namespace Eigen
namespace raintime {
template <int NF, int NI, bool S>
inline const FixedPoint<NF, NI, S> conj(const FixedPoint<NF, NI, S> &x) {
  return x;
}
template <int NF, int NI, bool S>
inline const FixedPoint<NF, NI, S> real(const FixedPoint<NF, NI, S> &x) {
  return x;
}
template <int NF, int NI, bool S>
inline FixedPoint<NF, NI, S> imag(const FixedPoint<NF, NI, S> &) {
  return 0.;
}
template <int NF, int NI, bool S>
inline FixedPoint<NF, NI, S> abs(const FixedPoint<NF, NI, S> &x) {
  return fabs(static_cast<float>(x));
}
template <int NF, int NI, bool S>
inline FixedPoint<NF, NI, S> abs2(const FixedPoint<NF, NI, S> &x) {
  return x * x;
}

template <int NF, int NI, bool S>
inline float abs2(const FixedPoint<NF, NI, S> &x) {
  return static_cast<float>(x * x);
}
}  // namespace raintime

namespace Eigen {
namespace internal {
template <int NF, int NI, bool S>
struct abs2_impl<raintime::FixedPoint<NF, NI, S>> {
  EIGEN_DEVICE_FUNC
  static inline float run(const raintime::FixedPoint<NF, NI, S> &x) {
    return static_cast<float>(x * x);
  }
};
}  // namespace internal
}  // namespace Eigen

#endif
