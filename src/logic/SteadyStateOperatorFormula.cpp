#include "src/logic/SteadyStateOperatorFormula.h"

namespace storm {
    namespace logic {
        SteadyStateOperatorFormula::SteadyStateOperatorFormula(std::shared_ptr<Formula const> const& subformula) : SteadyStateOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateOperatorFormula::SteadyStateOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : SteadyStateOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateOperatorFormula::SteadyStateOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : SteadyStateOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        SteadyStateOperatorFormula::SteadyStateOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : SteadyStateOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
                
        bool SteadyStateOperatorFormula::isSteadyStateOperatorFormula() const {
            return true;
        }
        
        bool SteadyStateOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool SteadyStateOperatorFormula::hasProbabilityOperator() const {
            return this->getSubformula().hasProbabilityOperator();
        }
        
        bool SteadyStateOperatorFormula::hasNestedProbabilityOperators() const {
            return this->getSubformula().hasNestedProbabilityOperators();
        }
        
        SteadyStateOperatorFormula::SteadyStateOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& SteadyStateOperatorFormula::writeToStream(std::ostream& out) const {
            out << "S";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}