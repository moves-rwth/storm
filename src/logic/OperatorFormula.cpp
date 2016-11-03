#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        OperatorInformation::OperatorInformation(boost::optional<storm::solver::OptimizationDirection> const& optimizationDirection, boost::optional<Bound<RationalNumber>> const& bound) : optimalityType(optimizationDirection), bound(bound) {
            // Intentionally left empty.
        }
        
        OperatorFormula::OperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation) : UnaryStateFormula(subformula), operatorInformation(operatorInformation) {
            // Intentionally left empty.
        }
        
        bool OperatorFormula::hasBound() const {
            return static_cast<bool>(operatorInformation.bound);
        }
        
        ComparisonType OperatorFormula::getComparisonType() const {
            return operatorInformation.bound.get().comparisonType;
        }
        
        void OperatorFormula::setComparisonType(ComparisonType newComparisonType) {
            operatorInformation.bound.get().comparisonType = newComparisonType;
        }
        
        RationalNumber const& OperatorFormula::getThreshold() const {
            return operatorInformation.bound.get().threshold;
        }
        
        void OperatorFormula::setThreshold(RationalNumber const& newThreshold) {
            operatorInformation.bound.get().threshold = newThreshold;
        }
        
        Bound<RationalNumber> const& OperatorFormula::getBound() const {
            return operatorInformation.bound.get();
        }
        
        void OperatorFormula::setBound(Bound<RationalNumber> const& newBound) {
            operatorInformation.bound = newBound;
        }
        
        bool OperatorFormula::hasOptimalityType() const {
            return static_cast<bool>(operatorInformation.optimalityType);
        }
        
        OptimizationDirection const& OperatorFormula::getOptimalityType() const {
            return operatorInformation.optimalityType.get();
        }
        
        bool OperatorFormula::isOperatorFormula() const {
            return true;
        }
        
        OperatorInformation const& OperatorFormula::getOperatorInformation() const {
            return operatorInformation;
        }
        
        bool OperatorFormula::hasQualitativeResult() const {
            return this->hasBound();
        }
        
        bool OperatorFormula::hasQuantitativeResult() const {
            return !this->hasBound();
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