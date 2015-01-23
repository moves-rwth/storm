#include "src/properties/logic/ComparisonType.h"

namespace storm {
    namespace logic {
        std::ostream& operator<<(std::ostream& out, ComparisonType const& comparisonType) {
            switch (comparisonType) {
                case Less: out << "<"; break;
                case LessEqual: out << "<="; break;
                case Greater: out << ">"; break;
                case GreaterEqual: out << ">="; break;
            }
            return out;
        }
    }
}