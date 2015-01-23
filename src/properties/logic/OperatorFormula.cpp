#include "src/properties/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        OperatorFormula::OperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula) : UnaryStateFormula(subformula), comparisonType(comparisonType), bound(bound), optimalityType(optimalityType) {
            // Intentionally left empty.
        }
        
        bool OperatorFormula::hasBound() const {
            return static_cast<bool>(bound);
        }
        
        ComparisonType const& OperatorFormula::getComparisonType() const {
            return comparisonType.get();
        }
        
        double OperatorFormula::getBound() const {
            return bound.get();
        }
        
        bool OperatorFormula::hasOptimalityType() const {
            return static_cast<bool>(optimalityType);
        }
        
        OptimalityType const& OperatorFormula::getOptimalityType() const {
            return optimalityType.get();
        }
        
        std::ostream& OperatorFormula::writeToStream(std::ostream& out) const {
            if (hasOptimalityType()) {
                out << getOptimalityType();
            }
            if (hasBound()) {
                out << getComparisonType() << getBound();
            } else {
                out << "=?";
            }
            out << " [";
            this->getSubformula().writeToStream(out);
            out << "]";
            return out;
        }
    }
}