#ifndef STORM_LOGIC_BOUND_H_
#define	STORM_LOGIC_BOUND_H_

#include "src/logic/ComparisonType.h"

namespace storm {
    namespace logic {
        template<typename ValueType>
        struct Bound {
            Bound(ComparisonType comparisonType, ValueType const& threshold) : comparisonType(comparisonType), threshold(threshold) {
                // Intentionally left empty.
            }
            
            ComparisonType comparisonType;
            ValueType threshold;

            friend std::ostream& operator<<(std::ostream& out, Bound<ValueType> const& bound);
        };

        template<typename ValueType>
        std::ostream& operator<<(std::ostream& out, Bound<ValueType> const& bound) {
            out << bound.comparisonType << bound.threshold;
            return out;
        }
    }
    
    template<typename ValueType>
    using Bound = typename logic::Bound<ValueType>;
}

#endif	/* STORM_LOGIC_BOUND_H_ */

