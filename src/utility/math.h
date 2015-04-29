#ifndef STORM_UTILITY_MATH_H_
#define STORM_UTILITY_MATH_H_

#include <cmath>

#include "src/utility/OsDetection.h"

namespace storm {
    namespace utility {
        namespace math {
            // We provide this method explicitly, because MSVC does not offer it (non-C99 compliant).
            template<typename ValueType>
            static inline double log2(ValueType number) {
#		ifndef WINDOWS
                return std::log2(number);
#		else
                return std::log(number) / std::log(2);
#		endif
            }
        }
    }
}

#endif /* STORM_UTILITY_MATH_H_ */