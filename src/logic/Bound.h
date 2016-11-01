#ifndef STORM_LOGIC_BOUND_H_
#define	STORM_LOGIC_BOUND_H_

#include "src/logic/ComparisonType.h"
#include "src/utility/constants.h"

namespace storm {
    namespace logic {
        template<typename ValueType>
        struct Bound {
            Bound(ComparisonType comparisonType, ValueType const& threshold) : comparisonType(comparisonType), threshold(threshold) {
                // Intentionally left empty.
            }

            template<typename OtherValueType>
            Bound<OtherValueType> convertToOtherValueType() const {
                return Bound<OtherValueType>(comparisonType, storm::utility::convertNumber<OtherValueType>(threshold));
            }
            
            ComparisonType comparisonType;
            ValueType threshold;

            template<typename ValueTypePrime>
            friend std::ostream& operator<<(std::ostream& out, Bound<ValueTypePrime> const& bound);
        };

        template<typename ValueType>
        std::ostream& operator<<(std::ostream& out, Bound<ValueType> const& bound) {
            out << bound.comparisonType << storm::utility::convertNumber<double>(bound.threshold);
            return out;
        }
    }
    
    template<typename ValueType>
    using Bound = typename logic::Bound<ValueType>;
}

#endif	/* STORM_LOGIC_BOUND_H_ */

