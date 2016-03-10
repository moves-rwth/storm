#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        std::ostream& operator<<(std::ostream& out, MeasureType const& type) {
            switch (type) {
                case MeasureType::Value:
                    out << "val";
                    break;
                case MeasureType::Expectation:
                    out << "exp";
                    break;
                case MeasureType::Variance:
                    out << "var";
                    break;
            }
            return out;
        }
        
        OperatorInformation::OperatorInformation(MeasureType const& measureType, boost::optional<storm::solver::OptimizationDirection> const& optimizationDirection, boost::optional<Bound<double>> const& bound) : measureType(measureType), optimalityType(optimizationDirection), bound(bound) {
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
        
        double OperatorFormula::getThreshold() const {
            return operatorInformation.bound.get().threshold;
        }
        
        void OperatorFormula::setThreshold(double newThreshold) {
            operatorInformation.bound.get().threshold = newThreshold;
        }
        
        Bound<double> const& OperatorFormula::getBound() const {
            return operatorInformation.bound.get();
        }
        
        void OperatorFormula::setBound(Bound<double> const& newBound) {
            operatorInformation.bound = newBound;
        }
        
        bool OperatorFormula::hasOptimalityType() const {
            return static_cast<bool>(operatorInformation.optimalityType);
        }
        
        OptimizationDirection const& OperatorFormula::getOptimalityType() const {
            return operatorInformation.optimalityType.get();
        }
        
        MeasureType OperatorFormula::getMeasureType() const {
            return operatorInformation.measureType;
        }
        
        bool OperatorFormula::isOperatorFormula() const {
            return true;
        }
        
        bool OperatorFormula::hasQualitativeResult() const {
            return this->hasBound();
        }
        
        bool OperatorFormula::hasQuantitativeResult() const {
            return !this->hasBound();
        }
        
        std::ostream& OperatorFormula::writeToStream(std::ostream& out) const {
            out << "[" << this->operatorInformation.measureType << "]";
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