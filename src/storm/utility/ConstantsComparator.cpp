#include "storm/utility/ConstantsComparator.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/sparse/StateType.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/constants.h"

namespace storm {
namespace utility {

template<typename ValueType, typename Enable>
bool ConstantsComparator<ValueType, Enable>::isOne(ValueType const& value) const {
    return isEqual(value, storm::utility::one<ValueType>());
}

template<typename ValueType, typename Enable>
bool ConstantsComparator<ValueType, Enable>::isZero(ValueType const& value) const {
    return isEqual(value, storm::utility::zero<ValueType>());
}

template<typename ValueType, typename Enable>
bool ConstantsComparator<ValueType, Enable>::isEqual(ValueType const& value1, ValueType const& value2) const {
    return value1 == value2;
}

template<typename ValueType, typename Enable>
bool ConstantsComparator<ValueType, Enable>::isConstant(ValueType const& value) const {
    return storm::utility::isConstant(value);
}

template<typename ValueType, typename Enable>
bool ConstantsComparator<ValueType, Enable>::isLess(ValueType const& value1, ValueType const& value2) const {
    return value1 < value2;
}

template<typename ValueType>
ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::ConstantsComparator()
    : ConstantsComparator(
          storm::NumberTraits<ValueType>::IsExact
              ? storm::utility::zero<ValueType>()
              : storm::utility::convertNumber<ValueType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision())) {
    // Intentionally left empty
}

template<typename ValueType>
ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::ConstantsComparator(ValueType const& precision, bool const& relative)
    : precision(precision), relative(relative) {
    // Intentionally left empty
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isOne(ValueType const& value) const {
    return isEqual(value, storm::utility::one<ValueType>());
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isZero(ValueType const& value) const {
    return isEqual(value, storm::utility::zero<ValueType>());
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isEqual(ValueType const& value1, ValueType const& value2) const {
    if (value1 == value2) {
        return true;
    } else if (storm::utility::isZero(precision)) {
        return false;
    } else {
        ValueType absDiff = storm::utility::abs<ValueType>(value1 - value2);
        if (relative) {
            return absDiff <= precision * (storm::utility::abs(value1) + storm::utility::abs(value2));
        } else {
            return absDiff <= precision;
        }
    }
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isConstant(ValueType const& value) const {
    return storm::utility::isConstant(value);
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isInfinity(ValueType const& value) const {
    return storm::utility::isInfinity(value);
}

template<typename ValueType>
bool ConstantsComparator<ValueType, ConstantsComparatorEnablePrecision<ValueType>>::isLess(ValueType const& value1, ValueType const& value2) const {
    return value1 < value2 - precision;
}

// Explicit instantiations.
template class ConstantsComparator<double>;
template class ConstantsComparator<int>;
template class ConstantsComparator<storm::storage::sparse::state_type>;

#ifdef STORM_HAVE_CARL
#if defined(STORM_HAVE_CLN)
template class ConstantsComparator<ClnRationalNumber>;
#endif

#if defined(STORM_HAVE_GMP)
template class ConstantsComparator<GmpRationalNumber>;
#endif

template class ConstantsComparator<RationalFunction>;
template class ConstantsComparator<Polynomial>;
template class ConstantsComparator<Interval>;
#endif
}  // namespace utility
}  // namespace storm
