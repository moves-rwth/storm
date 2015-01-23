#ifndef STORM_LOGIC_OPTIMALITYTYPE_H_
#define STORM_LOGIC_OPTIMALITYTYPE_H_

#include <iostream>

namespace storm {
	namespace logic {
		enum OptimalityType { Minimize, Maximize };
        
        std::ostream& operator<<(std::ostream& out, OptimalityType const& optimalityType);
	}
}

#endif /* STORM_LOGIC_OPTIMALITYTYPE_H_ */
