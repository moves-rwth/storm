// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2014 Benoit Steiner <benoit.steiner.goog@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_CXX11_TENSOR_TENSOR_FIXED_SIZE_H
#define STORMEIGEN_CXX11_TENSOR_TENSOR_FIXED_SIZE_H

namespace StormEigen {

/** \class TensorFixedSize
  * \ingroup CXX11_Tensor_Module
  *
  * \brief The fixed sized version of the tensor class.
  *
  * The fixed sized equivalent of
  * StormEigen::Tensor<float, 3> t(3, 5, 7);
  * is
  * StormEigen::TensorFixedSize<float, Size<3,5,7>> t;
  */

template<typename Scalar_, typename Dimensions_, int Options_, typename IndexType>
class TensorFixedSize : public TensorBase<TensorFixedSize<Scalar_, Dimensions_, Options_, IndexType> >
{
  public:
    typedef TensorFixedSize<Scalar_, Dimensions_, Options_, IndexType> Self;
    typedef TensorBase<TensorFixedSize<Scalar_, Dimensions_, Options_, IndexType> > Base;
    typedef typename StormEigen::internal::nested<Self>::type Nested;
    typedef typename internal::traits<Self>::StorageKind StorageKind;
    typedef typename internal::traits<Self>::Index Index;
    typedef Scalar_ Scalar;
    typedef typename internal::packet_traits<Scalar>::type Packet;
    typedef typename NumTraits<Scalar>::Real RealScalar;
    typedef typename Base::CoeffReturnType CoeffReturnType;

    static const int Options = Options_;

    enum {
      IsAligned = bool(STORMEIGEN_MAX_ALIGN_BYTES>0),
      PacketAccess = (internal::packet_traits<Scalar>::size > 1),
      Layout = Options_ & RowMajor ? RowMajor : ColMajor,
      CoordAccess = true,
   };

  typedef Dimensions_ Dimensions;
  static const std::size_t NumIndices = Dimensions::count;

  protected:
  TensorStorage<Scalar, Dimensions, Options> m_storage;

  public:
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Index                      rank()                   const { return NumIndices; }
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Index                    dimension(std::size_t n) const { return m_storage.dimensions()[n]; }
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const Dimensions&        dimensions()             const { return m_storage.dimensions(); }
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Index                    size()                   const { return m_storage.size(); }
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Scalar                   *data()                        { return m_storage.data(); }
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const Scalar             *data()                  const { return m_storage.data(); }

    // This makes STORMEIGEN_INITIALIZE_COEFFS_IF_THAT_OPTION_IS_ENABLED
    // work, because that uses base().coeffRef() - and we don't yet
    // implement a similar class hierarchy
    inline Self& base()             { return *this; }
    inline const Self& base() const { return *this; }

#ifdef STORMEIGEN_HAS_VARIADIC_TEMPLATES
    template<typename... IndexTypes>
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const Scalar& coeff(Index firstIndex, IndexTypes... otherIndices) const
    {
      // The number of indices used to access a tensor coefficient must be equal to the rank of the tensor.
      STORMEIGEN_STATIC_ASSERT(sizeof...(otherIndices) + 1 == NumIndices, YOU_MADE_A_PROGRAMMING_MISTAKE)
      return coeff(array<Index, NumIndices>{{firstIndex, otherIndices...}});
    }
#endif

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& coeff(const array<Index, NumIndices>& indices) const
    {
      eigen_internal_assert(checkIndexRange(indices));
      return m_storage.data()[linearizedIndex(indices)];
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& coeff(Index index) const
    {
      eigen_internal_assert(index >= 0 && index < size());
      return m_storage.data()[index];
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& coeff() const
    {
      STORMEIGEN_STATIC_ASSERT(NumIndices == 0, YOU_MADE_A_PROGRAMMING_MISTAKE);
      return m_storage.data()[0];
    }


#ifdef STORMEIGEN_HAS_VARIADIC_TEMPLATES
    template<typename... IndexTypes>
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Scalar& coeffRef(Index firstIndex, IndexTypes... otherIndices)
    {
      // The number of indices used to access a tensor coefficient must be equal to the rank of the tensor.
      STORMEIGEN_STATIC_ASSERT(sizeof...(otherIndices) + 1 == NumIndices, YOU_MADE_A_PROGRAMMING_MISTAKE)
      return coeffRef(array<Index, NumIndices>{{firstIndex, otherIndices...}});
    }
#endif

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& coeffRef(const array<Index, NumIndices>& indices)
    {
      eigen_internal_assert(checkIndexRange(indices));
      return m_storage.data()[linearizedIndex(indices)];
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& coeffRef(Index index)
    {
      eigen_internal_assert(index >= 0 && index < size());
      return m_storage.data()[index];
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& coeffRef()
    {
      STORMEIGEN_STATIC_ASSERT(NumIndices == 0, YOU_MADE_A_PROGRAMMING_MISTAKE);
      return m_storage.data()[0];
    }


#ifdef STORMEIGEN_HAS_VARIADIC_TEMPLATES
    template<typename... IndexTypes>
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const Scalar& operator()(Index firstIndex, IndexTypes... otherIndices) const
    {
      // The number of indices used to access a tensor coefficient must be equal to the rank of the tensor.
      STORMEIGEN_STATIC_ASSERT(sizeof...(otherIndices) + 1 == NumIndices, YOU_MADE_A_PROGRAMMING_MISTAKE)
      return this->operator()(array<Index, NumIndices>{{firstIndex, otherIndices...}});
    }
#endif

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& operator()(const array<Index, NumIndices>& indices) const
    {
      eigen_assert(checkIndexRange(indices));
      return coeff(indices);
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& operator()(Index index) const
    {
      eigen_internal_assert(index >= 0 && index < size());
      return coeff(index);
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& operator()() const
    {
      STORMEIGEN_STATIC_ASSERT(NumIndices == 0, YOU_MADE_A_PROGRAMMING_MISTAKE);
      return coeff();
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE const Scalar& operator[](Index index) const
    {
      // The bracket operator is only for vectors, use the parenthesis operator instead.
      STORMEIGEN_STATIC_ASSERT(NumIndices == 1, YOU_MADE_A_PROGRAMMING_MISTAKE);
      return coeff(index);
    }

#ifdef STORMEIGEN_HAS_VARIADIC_TEMPLATES
    template<typename... IndexTypes>
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Scalar& operator()(Index firstIndex, IndexTypes... otherIndices)
    {
      // The number of indices used to access a tensor coefficient must be equal to the rank of the tensor.
      STORMEIGEN_STATIC_ASSERT(sizeof...(otherIndices) + 1 == NumIndices, YOU_MADE_A_PROGRAMMING_MISTAKE)
      return operator()(array<Index, NumIndices>{{firstIndex, otherIndices...}});
    }
#endif

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& operator()(const array<Index, NumIndices>& indices)
    {
      eigen_assert(checkIndexRange(indices));
      return coeffRef(indices);
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& operator()(Index index)
    {
      eigen_assert(index >= 0 && index < size());
      return coeffRef(index);
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& operator()()
    {
      STORMEIGEN_STATIC_ASSERT(NumIndices == 0, YOU_MADE_A_PROGRAMMING_MISTAKE);
      return coeffRef();
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Scalar& operator[](Index index)
    {
      // The bracket operator is only for vectors, use the parenthesis operator instead
      STORMEIGEN_STATIC_ASSERT(NumIndices == 1, YOU_MADE_A_PROGRAMMING_MISTAKE)
      return coeffRef(index);
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize()
      : m_storage()
    {
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize(const Self& other)
      : m_storage(other.m_storage)
    {
    }

#ifdef STORMEIGEN_HAVE_RVALUE_REFERENCES
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE TensorFixedSize(Self&& other)
      : m_storage(other.m_storage)
    {
    }
#endif

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize(const TensorBase<OtherDerived, ReadOnlyAccessors>& other)
    {
      typedef TensorAssignOp<TensorFixedSize, const OtherDerived> Assign;
      Assign assign(*this, other.derived());
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
    }
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize(const TensorBase<OtherDerived, WriteAccessors>& other)
    {
      typedef TensorAssignOp<TensorFixedSize, const OtherDerived> Assign;
      Assign assign(*this, other.derived());
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize& operator=(const TensorFixedSize& other)
    {
      // FIXME: check that the dimensions of other match the dimensions of *this.
      // Unfortunately this isn't possible yet when the rhs is an expression.
      typedef TensorAssignOp<Self, const TensorFixedSize> Assign;
      Assign assign(*this, other);
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
      return *this;
    }
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorFixedSize& operator=(const OtherDerived& other)
    {
      // FIXME: check that the dimensions of other match the dimensions of *this.
      // Unfortunately this isn't possible yet when the rhs is an expression.
      typedef TensorAssignOp<Self, const OtherDerived> Assign;
      Assign assign(*this, other);
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
      return *this;
    }

  protected:
    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE bool checkIndexRange(const array<Index, NumIndices>& /*indices*/) const
    {
      using internal::array_apply_and_reduce;
      using internal::array_zip_and_reduce;
      using internal::greater_equal_zero_op;
      using internal::logical_and_op;
      using internal::lesser_op;

      return true;
        // check whether the indices are all >= 0
          /*       array_apply_and_reduce<logical_and_op, greater_equal_zero_op>(indices) &&
        // check whether the indices fit in the dimensions
        array_zip_and_reduce<logical_and_op, lesser_op>(indices, m_storage.dimensions());*/
    }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE Index linearizedIndex(const array<Index, NumIndices>& indices) const
    {
      if (Options&RowMajor) {
        return m_storage.dimensions().IndexOfRowMajor(indices);
      } else {
        return m_storage.dimensions().IndexOfColMajor(indices);
      }
    }
};


} // end namespace StormEigen

#endif // STORMEIGEN_CXX11_TENSOR_TENSOR_FIXED_SIZE_H
