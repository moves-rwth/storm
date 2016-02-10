#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        OperatorFormula::OperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : UnaryStateFormula(subformula), comparisonType(comparisonType), bound(bound), optimalityType(optimalityType) {
            // Intentionally left empty.
        }
        
        bool OperatorFormula::hasBound() const {
            return static_cast<bool>(bound);
        }
        
        ComparisonType OperatorFormula::getComparisonType() const {
            return comparisonType.get();
        }
        
        void OperatorFormula::setComparisonType(ComparisonType t) {
            comparisonType = boost::optional<ComparisonType>(t);
        }
        
        double OperatorFormula::getBound() const {
            return bound.get();
        }
        
        void OperatorFormula::setBound(double b) {
            bound = boost::optional<double>(b);
        }
        
        bool OperatorFormula::hasOptimalityType() const {
            return static_cast<bool>(optimalityType);
        }
        
        OptimizationDirection const& OperatorFormula::getOptimalityType() const {
            return optimalityType.get();
        }
        
        bool OperatorFormula::isOperatorFormula() const {
            return true;
        }
        
        std::ostream& OperatorFormula::writeToStream(std::ostream& out) const {
            if (hasOptimalityType()) {
                out << (getOptimalityType() == OptimizationDirection::Minimize ? "min" : "max");
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