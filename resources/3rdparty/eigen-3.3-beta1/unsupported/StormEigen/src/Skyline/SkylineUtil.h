// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2009 Guillaume Saupin <guillaume.saupin@cea.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_SKYLINEUTIL_H
#define STORMEIGEN_SKYLINEUTIL_H

namespace StormEigen { 

#ifdef NDEBUG
#define STORMEIGEN_DBG_SKYLINE(X)
#else
#define STORMEIGEN_DBG_SKYLINE(X) X
#endif

const unsigned int SkylineBit = 0x1200;
template<typename Lhs, typename Rhs, int ProductMode> class SkylineProduct;
enum AdditionalProductEvaluationMode {SkylineTimeDenseProduct, SkylineTimeSkylineProduct, DenseTimeSkylineProduct};
enum {IsSkyline = SkylineBit};


#define STORMEIGEN_SKYLINE_INHERIT_ASSIGNMENT_OPERATOR(Derived, Op) \
template<typename OtherDerived> \
STORMEIGEN_STRONG_INLINE Derived& operator Op(const StormEigen::SkylineMatrixBase<OtherDerived>& other) \
{ \
  return Base::operator Op(other.derived()); \
} \
STORMEIGEN_STRONG_INLINE Derived& operator Op(const Derived& other) \
{ \
  return Base::operator Op(other); \
}

#define STORMEIGEN_SKYLINE_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(Derived, Op) \
template<typename Other> \
STORMEIGEN_STRONG_INLINE Derived& operator Op(const Other& scalar) \
{ \
  return Base::operator Op(scalar); \
}

#define STORMEIGEN_SKYLINE_INHERIT_ASSIGNMENT_OPERATORS(Derived) \
  STORMEIGEN_SKYLINE_INHERIT_ASSIGNMENT_OPERATOR(Derived, =) \
  STORMEIGEN_SKYLINE_INHERIT_ASSIGNMENT_OPERATOR(Derived, +=) \
  STORMEIGEN_SKYLINE_INHERIT_ASSIGNMENT_OPERATOR(Derived, -=) \
  STORMEIGEN_SKYLINE_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(Derived, *=) \
  STORMEIGEN_SKYLINE_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(Derived, /=)

#define _STORMEIGEN_SKYLINE_GENERIC_PUBLIC_INTERFACE(Derived, BaseClass) \
  typedef BaseClass Base; \
  typedef typename StormEigen::internal::traits<Derived>::Scalar Scalar; \
  typedef typename StormEigen::NumTraits<Scalar>::Real RealScalar; \
  typedef typename StormEigen::internal::traits<Derived>::StorageKind StorageKind; \
  typedef typename StormEigen::internal::index<StorageKind>::type Index; \
  enum {  Flags = StormEigen::internal::traits<Derived>::Flags, };

#define STORMEIGEN_SKYLINE_GENERIC_PUBLIC_INTERFACE(Derived) \
  _STORMEIGEN_SKYLINE_GENERIC_PUBLIC_INTERFACE(Derived, StormEigen::SkylineMatrixBase<Derived>)

template<typename Derived> class SkylineMatrixBase;
template<typename _Scalar, int _Flags = 0> class SkylineMatrix;
template<typename _Scalar, int _Flags = 0> class DynamicSkylineMatrix;
template<typename _Scalar, int _Flags = 0> class SkylineVector;
template<typename _Scalar, int _Flags = 0> class MappedSkylineMatrix;

namespace internal {

template<typename Lhs, typename Rhs> struct skyline_product_mode;
template<typename Lhs, typename Rhs, int ProductMode = skyline_product_mode<Lhs,Rhs>::value> struct SkylineProductReturnType;

template<typename T> class eval<T,IsSkyline>
{
    typedef typename traits<T>::Scalar _Scalar;
    enum {
          _Flags = traits<T>::Flags
    };

  public:
    typedef SkylineMatrix<_Scalar, _Flags> type;
};

} // end namespace internal

} // end namespace StormEigen

#endif // STORMEIGEN_SKYLINEUTIL_H
