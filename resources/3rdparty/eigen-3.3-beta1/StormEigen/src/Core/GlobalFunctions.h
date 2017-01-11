// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2010-2012 Gael Guennebaud <gael.guennebaud@inria.fr>
// Copyright (C) 2010 Benoit Jacob <jacob.benoit.1@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_GLOBAL_FUNCTIONS_H
#define STORMEIGEN_GLOBAL_FUNCTIONS_H

#define STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(NAME,FUNCTOR) \
  template<typename Derived> \
  inline const StormEigen::CwiseUnaryOp<StormEigen::internal::FUNCTOR<typename Derived::Scalar>, const Derived> \
  (NAME)(const StormEigen::ArrayBase<Derived>& x) { \
    return StormEigen::CwiseUnaryOp<StormEigen::internal::FUNCTOR<typename Derived::Scalar>, const Derived>(x.derived()); \
  }

#define STORMEIGEN_ARRAY_DECLARE_GLOBAL_STORMEIGEN_UNARY(NAME,FUNCTOR) \
  \
  template<typename Derived> \
  struct NAME##_retval<ArrayBase<Derived> > \
  { \
    typedef const StormEigen::CwiseUnaryOp<StormEigen::internal::FUNCTOR<typename Derived::Scalar>, const Derived> type; \
  }; \
  template<typename Derived> \
  struct NAME##_impl<ArrayBase<Derived> > \
  { \
    static inline typename NAME##_retval<ArrayBase<Derived> >::type run(const StormEigen::ArrayBase<Derived>& x) \
    { \
      return typename NAME##_retval<ArrayBase<Derived> >::type(x.derived()); \
    } \
  };

namespace StormEigen
{
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(real,scalar_real_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(imag,scalar_imag_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(conj,scalar_conjugate_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(inverse,scalar_inverse_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(sin,scalar_sin_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(cos,scalar_cos_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(tan,scalar_tan_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(atan,scalar_atan_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(asin,scalar_asin_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(acos,scalar_acos_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(sinh,scalar_sinh_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(cosh,scalar_cosh_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(tanh,scalar_tanh_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(lgamma,scalar_lgamma_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(erf,scalar_erf_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(erfc,scalar_erfc_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(exp,scalar_exp_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(log,scalar_log_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(log10,scalar_log10_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(abs,scalar_abs_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(abs2,scalar_abs2_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(arg,scalar_arg_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(sqrt,scalar_sqrt_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(square,scalar_square_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(cube,scalar_cube_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(round,scalar_round_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(floor,scalar_floor_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(ceil,scalar_ceil_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(isnan,scalar_isnan_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(isinf,scalar_isinf_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(isfinite,scalar_isfinite_op)
  STORMEIGEN_ARRAY_DECLARE_GLOBAL_UNARY(sign,scalar_sign_op)
  
  template<typename Derived>
  inline const StormEigen::CwiseUnaryOp<StormEigen::internal::scalar_pow_op<typename Derived::Scalar>, const Derived>
  pow(const StormEigen::ArrayBase<Derived>& x, const typename Derived::Scalar& exponent) {
    return x.derived().pow(exponent);
  }

  /** \returns an expression of the coefficient-wise power of \a x to the given array of \a exponents.
    *
    * This function computes the coefficient-wise power.
    *
    * Example: \include Cwise_array_power_array.cpp
    * Output: \verbinclude Cwise_array_power_array.out
    * 
    * \sa ArrayBase::pow()
    */
  template<typename Derived,typename ExponentDerived>
  inline const StormEigen::CwiseBinaryOp<StormEigen::internal::scalar_binary_pow_op<typename Derived::Scalar, typename ExponentDerived::Scalar>, const Derived, const ExponentDerived>
  pow(const StormEigen::ArrayBase<Derived>& x, const StormEigen::ArrayBase<ExponentDerived>& exponents) 
  {
    return StormEigen::CwiseBinaryOp<StormEigen::internal::scalar_binary_pow_op<typename Derived::Scalar, typename ExponentDerived::Scalar>, const Derived, const ExponentDerived>(
      x.derived(),
      exponents.derived()
    );
  }
  
  /** \returns an expression of the coefficient-wise power of the scalar \a x to the given array of \a exponents.
    *
    * This function computes the coefficient-wise power between a scalar and an array of exponents.
    * Beaware that the scalar type of the input scalar \a x and the exponents \a exponents must be the same.
    *
    * Example: \include Cwise_scalar_power_array.cpp
    * Output: \verbinclude Cwise_scalar_power_array.out
    * 
    * \sa ArrayBase::pow()
    */
  template<typename Derived>
  inline const StormEigen::CwiseBinaryOp<StormEigen::internal::scalar_binary_pow_op<typename Derived::Scalar, typename Derived::Scalar>, const typename Derived::ConstantReturnType, const Derived>
  pow(const typename Derived::Scalar& x, const StormEigen::ArrayBase<Derived>& exponents) 
  {
    typename Derived::ConstantReturnType constant_x(exponents.rows(), exponents.cols(), x);
    return StormEigen::CwiseBinaryOp<StormEigen::internal::scalar_binary_pow_op<typename Derived::Scalar, typename Derived::Scalar>, const typename Derived::ConstantReturnType, const Derived>(
      constant_x,
      exponents.derived()
    );
  }
  
  /**
  * \brief Component-wise division of a scalar by array elements.
  **/
  template <typename Derived>
  inline const StormEigen::CwiseUnaryOp<StormEigen::internal::scalar_inverse_mult_op<typename Derived::Scalar>, const Derived>
    operator/(const typename Derived::Scalar& s, const StormEigen::ArrayBase<Derived>& a)
  {
    return StormEigen::CwiseUnaryOp<StormEigen::internal::scalar_inverse_mult_op<typename Derived::Scalar>, const Derived>(
      a.derived(),
      StormEigen::internal::scalar_inverse_mult_op<typename Derived::Scalar>(s)  
    );
  }

  namespace internal
  {
    STORMEIGEN_ARRAY_DECLARE_GLOBAL_STORMEIGEN_UNARY(real,scalar_real_op)
    STORMEIGEN_ARRAY_DECLARE_GLOBAL_STORMEIGEN_UNARY(imag,scalar_imag_op)
    STORMEIGEN_ARRAY_DECLARE_GLOBAL_STORMEIGEN_UNARY(abs2,scalar_abs2_op)
  }
}

// TODO: cleanly disable those functions that are not supported on Array (numext::real_ref, internal::random, internal::isApprox...)

#endif // STORMEIGEN_GLOBAL_FUNCTIONS_H
