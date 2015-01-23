#include "src/properties/logic/OptimalityType.h"

namespace storm {
    namespace logic {
        std::ostream& operator<<(std::ostream& out, OptimalityType const& optimalityType) {
            switch (optimalityType) {
                case Maximize: out << "max"; break;
                case Minimize: out << "min"; break;
            }
            return out;
        }
    }
}