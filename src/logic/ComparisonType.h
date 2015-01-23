#ifndef STORM_LOGIC_COMPARISONTYPE_H_
#define STORM_LOGIC_COMPARISONTYPE_H_

#include <iostream>

namespace storm {
	namespace logic {
		enum ComparisonType { Less, LessEqual, Greater, GreaterEqual };
        
        std::ostream& operator<<(std::ostream& out, ComparisonType const& comparisonType);
	}
}

#endif /* STORM_LOGIC_COMPARISONTYPE_H_ */
