#ifndef STORM_LOGIC_BOUND_H_
#define	STORM_LOGIC_BOUND_H_

#include "storm/logic/ComparisonType.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/utility/constants.h"


namespace storm {
    namespace logic {
        struct Bound {
            Bound(ComparisonType comparisonType, storm::expressions::Expression const& threshold) : comparisonType(comparisonType), threshold(threshold) {
                // Intentionally left empty.
            }

            ComparisonType comparisonType;
            storm::expressions::Expression threshold;

            friend std::ostream& operator<<(std::ostream& out, Bound const& bound);
        };

        inline std::ostream& operator<<(std::ostream& out, Bound const& bound) {
            out << bound.comparisonType << bound.threshold;
            return out;
        }
    }
    
}

#endif	/* STORM_LOGIC_BOUND_H_ */

