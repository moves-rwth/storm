#include "src/utility/ConstantsComparator.h"

#include "src/utility/constants.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace utility {
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isOne(ValueType const& value) const {
            return value == one<ValueType>();
        }
        
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isZero(ValueType const& value) const {
            return value == zero<ValueType>();
        }
        
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isEqual(ValueType const& value1, ValueType const& value2) const {
            return value1 == value2;
        }

        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isConstant(ValueType const& value) const {
            return true;
        }

        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isInfinity(ValueType const& value) const {
            return false;
        }
        
        ConstantsComparator<float>::ConstantsComparator() : precision(static_cast<float>(storm::settings::generalSettings().getPrecision())) {
            // Intentionally left empty.
        }
        
        ConstantsComparator<float>::ConstantsComparator(float precision) : precision(precision) {
            // Intentionally left empty.
        }
        
        bool ConstantsComparator<float>::isOne(float const& value) const {
            return std::abs(value - one<float>()) <= precision;
        }
        
        bool ConstantsComparator<float>::isZero(float const& value) const {
            return std::abs(value) <= precision;
        }
        
        bool ConstantsComparator<float>::isEqual(float const& value1, float const& value2) const {
            return std::abs(value1 - value2) <= precision;
        }
        
        bool ConstantsComparator<float>::isConstant(float const& value) const {
            return true;
        }
        
        bool ConstantsComparator<float>::isInfinity(float const& value) const {
            return value == storm::utility::infinity<float>();
        }
        
        ConstantsComparator<double>::ConstantsComparator() : precision(storm::settings::generalSettings().getPrecision()) {
            // Intentionally left empty.
        }
        
        ConstantsComparator<double>::ConstantsComparator(double precision) : precision(precision) {
            // Intentionally left empty.
        }
        
        bool ConstantsComparator<double>::isOne(double const& value) const {
            return std::abs(value - one<double>()) <= precision;
        }
        
        bool ConstantsComparator<double>::isZero(double const& value) const {
            return std::abs(value) <= precision;
        }
        
        bool ConstantsComparator<double>::isInfinity(double const& value) const {
            return value == infinity<double>();
        }
        
        bool ConstantsComparator<double>::isEqual(double const& value1, double const& value2) const {
            return std::abs(value1 - value2) <= precision;
        }
        
        bool ConstantsComparator<double>::isConstant(double const& value) const {
            return true;
        }
        
#ifdef STORM_HAVE_CARL
        ConstantsComparator<storm::RationalFunction>::ConstantsComparator() {
            // Intentionally left empty.
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isOne(storm::RationalFunction const& value) const {
            return value.isOne();
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isZero(storm::RationalFunction const& value) const {
            return value.isZero();
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isEqual(storm::RationalFunction const& value1, storm::RationalFunction const& value2) const {
            return value1 == value2;
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isConstant(storm::RationalFunction const& value) const {
            return value.isConstant();
        }
        
        ConstantsComparator<storm::Polynomial>::ConstantsComparator() {
            // Intentionally left empty.
        }
        
        bool ConstantsComparator<storm::Polynomial>::isOne(storm::Polynomial const& value) const {
            return value.isOne();
        }
        
        bool ConstantsComparator<storm::Polynomial>::isZero(storm::Polynomial const& value) const {
            return value.isZero();
        }
        
        bool ConstantsComparator<storm::Polynomial>::isEqual(storm::Polynomial const& value1, storm::Polynomial const& value2) const {
            return value1 == value2;
        }
        
        bool ConstantsComparator<storm::Polynomial>::isConstant(storm::Polynomial const& value) const {
            return value.isConstant();
        }
#endif
        
        // Explicit instantiations.
        template class ConstantsComparator<double>;
        template class ConstantsComparator<float>;
        template class ConstantsComparator<int>;
        
#ifdef STORM_HAVE_CARL
        template class ConstantsComparator<RationalFunction>;
        template class ConstantsComparator<Polynomial>;
#endif
    }
}