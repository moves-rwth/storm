#ifndef STORM_UTILITY_CONSTANTSCOMPARATOR_H_
#define STORM_UTILITY_CONSTANTSCOMPARATOR_H_

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

#include <limits>
#include <cstdint>

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace utility {
        
        template<typename ValueType>
        ValueType one() {
            return ValueType(1);
        }
        
        template<typename ValueType>
        ValueType zero() {
            return ValueType(0);
        }
        
        template<typename ValueType>
        ValueType infinity() {
            return std::numeric_limits<ValueType>::infinity();
        }
        
        // A class that can be used for comparing constants.
        template<typename ValueType>
        class ConstantsComparator {
        public:
            bool isOne(ValueType const& value) const {
                return value == one<ValueType>();
            }
            
            bool isZero(ValueType const& value) const {
                return value == zero<ValueType>();
            }
            
            bool isEqual(ValueType const& value1, ValueType const& value2) const {
                return value1 == value2;
            }
        };
        
        // For doubles we specialize this class and consider the comparison modulo some predefined precision.
        template<>
        class ConstantsComparator<double> {
        public:
            ConstantsComparator() : precision(storm::settings::generalSettings().getPrecision()) {
                // Intentionally left empty.
            }

            ConstantsComparator(double precision) : precision(precision) {
                // Intentionally left empty.
            }
            
            bool isOne(double const& value) const {
                return std::abs(value - one<double>()) <= precision;
            }
            
            bool isZero(double const& value) const {
                return std::abs(value) <= precision;
            }
            
            bool isEqual(double const& value1, double const& value2) const {
                return std::abs(value1 - value2) <= precision;
            }
            
            bool isConstant(double const& value) const {
                return true;
            }
            
        private:
            // The precision used for comparisons.
            double precision;
        };
        
    }
}

#endif /* STORM_UTILITY_CONSTANTSCOMPARATOR_H_ */