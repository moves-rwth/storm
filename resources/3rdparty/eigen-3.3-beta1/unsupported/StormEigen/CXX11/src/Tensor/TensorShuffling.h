// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2014 Benoit Steiner <benoit.steiner.goog@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_CXX11_TENSOR_TENSOR_SHUFFLING_H
#define STORMEIGEN_CXX11_TENSOR_TENSOR_SHUFFLING_H

namespace StormEigen {

/** \class TensorShuffling
  * \ingroup CXX11_Tensor_Module
  *
  * \brief Tensor shuffling class.
  *
  *
  */
namespace internal {
template<typename Shuffle, typename XprType>
struct traits<TensorShufflingOp<Shuffle, XprType> > : public traits<XprType>
{
  typedef typename XprType::Scalar Scalar;
  typedef traits<XprType> XprTraits;
  typedef typename packet_traits<Scalar>::type Packet;
  typedef typename XprTraits::StorageKind StorageKind;
  typedef typename XprTraits::Index Index;
  typedef typename XprType::Nested Nested;
  typedef typename remove_reference<Nested>::type _Nested;
  static const int NumDimensions = XprTraits::NumDimensions;
  static const int Layout = XprTraits::Layout;
};

template<typename Shuffle, typename XprType>
struct eval<TensorShufflingOp<Shuffle, XprType>, StormEigen::Dense>
{
  typedef const TensorShufflingOp<Shuffle, XprType>& type;
};

template<typename Shuffle, typename XprType>
struct nested<TensorShufflingOp<Shuffle, XprType>, 1, typename eval<TensorShufflingOp<Shuffle, XprType> >::type>
{
  typedef TensorShufflingOp<Shuffle, XprType> type;
};

}  // end namespace internal



template<typename Shuffle, typename XprType>
class TensorShufflingOp : public TensorBase<TensorShufflingOp<Shuffle, XprType> >
{
  public:
  typedef typename StormEigen::internal::traits<TensorShufflingOp>::Scalar Scalar;
  typedef typename StormEigen::internal::traits<TensorShufflingOp>::Packet Packet;
  typedef typename StormEigen::NumTraits<Scalar>::Real RealScalar;
  typedef typename XprType::CoeffReturnType CoeffReturnType;
  typedef typename XprType::PacketReturnType PacketReturnType;
  typedef typename StormEigen::internal::nested<TensorShufflingOp>::type Nested;
  typedef typename StormEigen::internal::traits<TensorShufflingOp>::StorageKind StorageKind;
  typedef typename StormEigen::internal::traits<TensorShufflingOp>::Index Index;

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE TensorShufflingOp(const XprType& expr, const Shuffle& shuffle)
      : m_xpr(expr), m_shuffle(shuffle) {}

    STORMEIGEN_DEVICE_FUNC
    const Shuffle& shufflePermutation() const { return m_shuffle; }

    STORMEIGEN_DEVICE_FUNC
    const typename internal::remove_all<typename XprType::Nested>::type&
    expression() const { return m_xpr; }

    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorShufflingOp& operator = (const TensorShufflingOp& other)
    {
      typedef TensorAssignOp<TensorShufflingOp, const TensorShufflingOp> Assign;
      Assign assign(*this, other);
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
      return *this;
    }

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    STORMEIGEN_STRONG_INLINE TensorShufflingOp& operator = (const OtherDerived& other)
    {
      typedef TensorAssignOp<TensorShufflingOp, const OtherDerived> Assign;
      Assign assign(*this, other);
      internal::TensorExecutor<const Assign, DefaultDevice>::run(assign, DefaultDevice());
      return *this;
    }

  protected:
    typename XprType::Nested m_xpr;
    const Shuffle m_shuffle;
};


// Eval as rvalue
template<typename Shuffle, typename ArgType, typename Device>
struct TensorEvaluator<const TensorShufflingOp<Shuffle, ArgType>, Device>
{
  typedef TensorShufflingOp<Shuffle, ArgType> XprType;
  typedef typename XprType::Index Index;
  static const int NumDims = internal::array_size<typename TensorEvaluator<ArgType, Device>::Dimensions>::value;
  typedef DSizes<Index, NumDims> Dimensions;
  typedef typename XprType::Scalar Scalar;

  enum {
    IsAligned = false,
    PacketAccess = (internal::packet_traits<Scalar>::size > 1),
    Layout = TensorEvaluator<ArgType, Device>::Layout,
    CoordAccess = false,  // to be implemented
  };

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE TensorEvaluator(const XprType& op, const Device& device)
      : m_impl(op.expression(), device)
  {
    const typename TensorEvaluator<ArgType, Device>::Dimensions& input_dims = m_impl.dimensions();
    const Shuffle& shuffle = op.shufflePermutation();
    for (int i = 0; i < NumDims; ++i) {
      m_dimensions[i] = input_dims[shuffle[i]];
    }

    array<Index, NumDims> inputStrides;

    if (static_cast<int>(Layout) == static_cast<int>(ColMajor)) {
      inputStrides[0] = 1;
      m_outputStrides[0] = 1;
      for (int i = 1; i < NumDims; ++i) {
        inputStrides[i] = inputStrides[i - 1] * input_dims[i - 1];
        m_outputStrides[i] = m_outputStrides[i - 1] * m_dimensions[i - 1];
      }
    } else {
      inputStrides[NumDims - 1] = 1;
      m_outputStrides[NumDims - 1] = 1;
      for (int i = NumDims - 2; i >= 0; --i) {
        inputStrides[i] = inputStrides[i + 1] * input_dims[i + 1];
        m_outputStrides[i] = m_outputStrides[i + 1] * m_dimensions[i + 1];
      }
    }

    for (int i = 0; i < NumDims; ++i) {
      m_inputStrides[i] = inputStrides[shuffle[i]];
    }
  }

  typedef typename XprType::CoeffReturnType CoeffReturnType;
  typedef typename XprType::PacketReturnType PacketReturnType;

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const Dimensions& dimensions() const { return m_dimensions; }

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE bool evalSubExprsIfNeeded(Scalar* /*data*/) {
    m_impl.evalSubExprsIfNeeded(NULL);
    return true;
  }
  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE void cleanup() {
    m_impl.cleanup();
  }

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE CoeffReturnType coeff(Index index) const
  {
    return m_impl.coeff(srcCoeff(index));
  }

  template<int LoadMode>
  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE PacketReturnType packet(Index index) const
  {
    const int packetSize = internal::unpacket_traits<PacketReturnType>::size;
    STORMEIGEN_STATIC_ASSERT(packetSize > 1, YOU_MADE_A_PROGRAMMING_MISTAKE)
    eigen_assert(index+packetSize-1 < dimensions().TotalSize());

    STORMEIGEN_ALIGN_MAX typename internal::remove_const<CoeffReturnType>::type values[packetSize];
    for (int i = 0; i < packetSize; ++i) {
      values[i] = coeff(index+i);
    }
    PacketReturnType rslt = internal::pload<PacketReturnType>(values);
    return rslt;
  }

  STORMEIGEN_DEVICE_FUNC Scalar* data() const { return NULL; }

 protected:
  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE Index srcCoeff(Index index) const {
    Index inputIndex = 0;
    if (static_cast<int>(Layout) == static_cast<int>(ColMajor)) {
      for (int i = NumDims - 1; i > 0; --i) {
        const Index idx = index / m_outputStrides[i];
        inputIndex += idx * m_inputStrides[i];
        index -= idx * m_outputStrides[i];
      }
      return inputIndex + index * m_inputStrides[0];
    } else {
      for (int i = 0; i < NumDims - 1; ++i) {
        const Index idx = index / m_outputStrides[i];
        inputIndex += idx * m_inputStrides[i];
        index -= idx * m_outputStrides[i];
      }
      return inputIndex + index * m_inputStrides[NumDims - 1];
    }
  }

  Dimensions m_dimensions;
  array<Index, NumDims> m_outputStrides;
  array<Index, NumDims> m_inputStrides;
  TensorEvaluator<ArgType, Device> m_impl;
};


// Eval as lvalue
template<typename Shuffle, typename ArgType, typename Device>
struct TensorEvaluator<TensorShufflingOp<Shuffle, ArgType>, Device>
    : public TensorEvaluator<const TensorShufflingOp<Shuffle, ArgType>, Device>
{
  typedef TensorEvaluator<const TensorShufflingOp<Shuffle, ArgType>, Device> Base;

  typedef TensorShufflingOp<Shuffle, ArgType> XprType;
  typedef typename XprType::Index Index;
  static const int NumDims = internal::array_size<typename TensorEvaluator<ArgType, Device>::Dimensions>::value;
  typedef DSizes<Index, NumDims> Dimensions;
  typedef typename XprType::Scalar Scalar;

  enum {
    IsAligned = false,
    PacketAccess = (internal::packet_traits<Scalar>::size > 1),
  };

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE TensorEvaluator(const XprType& op, const Device& device)
      : Base(op, device)
  { }

  typedef typename XprType::CoeffReturnType CoeffReturnType;
  typedef typename XprType::PacketReturnType PacketReturnType;

  STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE CoeffReturnType& coeffRef(Index index)
  {
    return this->m_impl.coeffRef(this->srcCoeff(index));
  }

  template <int StoreMode> STORMEIGEN_STRONG_INLINE
  void writePacket(Index index, const PacketReturnType& x)
  {
    static const int packetSize = internal::unpacket_traits<PacketReturnType>::size;
    STORMEIGEN_STATIC_ASSERT(packetSize > 1, YOU_MADE_A_PROGRAMMING_MISTAKE)

    STORMEIGEN_ALIGN_MAX typename internal::remove_const<CoeffReturnType>::type values[packetSize];
    internal::pstore<CoeffReturnType, PacketReturnType>(values, x);
    for (int i = 0; i < packetSize; ++i) {
      this->coeffRef(index+i) = values[i];
    }
  }
};


} // end namespace StormEigen

#endif // STORMEIGEN_CXX11_TENSOR_TENSOR_SHUFFLING_H
