#pragma once

#include <type_traits>
#include "storm/adapters/RationalNumberForward.h"
#include "storm/utility/ConstantsComparatorForward.h"

namespace storm {
namespace utility {

// A class that can be used for comparing constants.
template<typename ValueType, typename Enable>
class ConstantsComparator {
   public:
    ConstantsComparator() = default;
    bool isOne(ValueType const& value) const;
    bool isZero(ValueType const& value) const;
    bool isEqual(ValueType const& value1, ValueType const& value2) const;
    bool isConstant(ValueType const& value) const;
    bool isLess(ValueType const& value1, ValueType const& value2) const;
};

// Specialization for numbers where there can be a precision
template<typename ValueType>
using ConstantsComparatorEnablePrecision =
    typename std::enable_if_t<std::is_same<ValueType, double>::value || std::is_same<ValueType, storm::RationalNumber>::value>;

template<typename ValueType>
class ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>> {
   public:
    ConstantsComparator();
    ConstantsComparator(ValueType const& precision, bool const& relative = false);
    bool isOne(ValueType const& value) const;
    bool isZero(ValueType const& value) const;
    bool isEqual(ValueType const& value1, ValueType const& value2) const;
    bool isConstant(ValueType const& value) const;
    bool isInfinity(ValueType const& value) const;
    bool isLess(ValueType const& value1, ValueType const& value2) const;

   private:
    ValueType precision;
    bool relative;
};
}  // namespace utility
}  // namespace storm
