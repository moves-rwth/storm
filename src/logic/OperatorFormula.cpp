#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        OperatorFormula::OperatorFormula(boost::optional<storm::solver::OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula) : UnaryStateFormula(subformula), bound(bound), optimalityType(optimalityType) {
            // Intentionally left empty.
        }
        
        bool OperatorFormula::hasBound() const {
            return static_cast<bool>(bound);
        }
        
        ComparisonType OperatorFormula::getComparisonType() const {
            return bound.get().comparisonType;
        }
        
        void OperatorFormula::setComparisonType(ComparisonType newComparisonType) {
            bound.get().comparisonType = newComparisonType;
        }
        
        double OperatorFormula::getThreshold() const {
            return bound.get().threshold;
        }
        
        void OperatorFormula::setThreshold(double newThreshold) {
            bound.get().threshold = newThreshold;
        }
        
        Bound<double> const& OperatorFormula::getBound() const {
            return bound.get();
        }
        
        void OperatorFormula::setBound(Bound<double> const& newBound) {
            bound = newBound;
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
                out << getBound();
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