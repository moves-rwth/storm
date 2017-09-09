#include "storm/solver/MultiplicationStyle.h"

namespace storm {
    namespace solver {
        
        std::ostream& operator<<(std::ostream& out, MultiplicationStyle const& style) {
            switch (style) {
                case MultiplicationStyle::AllowGaussSeidel: out << "Allow-Gauss-Seidel"; break;
                case MultiplicationStyle::Regular: out << "Regular"; break;
            }
            return out;
        }
        
    }
}
