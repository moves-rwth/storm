#ifndef STORM_LOGIC_COMPARISONTYPE_H_
#define STORM_LOGIC_COMPARISONTYPE_H_

#include <iostream>

namespace storm {
	namespace logic {
		enum class ComparisonType { Less, LessEqual, Greater, GreaterEqual };
         
                inline bool isStrict(ComparisonType t) {
                    return (t == ComparisonType::Less || t == ComparisonType::Greater);
                }
                
                inline bool isLowerBound(ComparisonType t) {
                    return (t == ComparisonType::Greater || t == ComparisonType::GreaterEqual);
                }
        std::ostream& operator<<(std::ostream& out, ComparisonType const& comparisonType);
	}
}

#endif /* STORM_LOGIC_COMPARISONTYPE_H_ */
