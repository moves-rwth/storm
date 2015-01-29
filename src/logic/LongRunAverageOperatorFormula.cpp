#include "src/logic/LongRunAverageOperatorFormula.h"

namespace storm {
    namespace logic {
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimalityType>(), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(comparisonType), boost::optional<double>(bound), subformula) {
            // Intentionally left empty.
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula) : LongRunAverageOperatorFormula(boost::optional<OptimalityType>(optimalityType), boost::optional<ComparisonType>(), boost::optional<double>(), subformula) {
            // Intentionally left empty.
        }
                
        bool LongRunAverageOperatorFormula::isLongRunAverageOperatorFormula() const {
            return true;
        }
        
        bool LongRunAverageOperatorFormula::isPctlStateFormula() const {
            return this->getSubformula().isPctlStateFormula();
        }
        
        bool LongRunAverageOperatorFormula::containsProbabilityOperator() const {
            return this->getSubformula().containsProbabilityOperator();
        }
        
        bool LongRunAverageOperatorFormula::containsNestedProbabilityOperators() const {
            return this->getSubformula().containsNestedProbabilityOperators();
        }
        
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula) : OperatorFormula(optimalityType, comparisonType, bound, subformula) {
            // Intentionally left empty.
        }
        
        std::ostream& LongRunAverageOperatorFormula::writeToStream(std::ostream& out) const {
            out << "LRA";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}