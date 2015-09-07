#ifndef STORM_LOGIC_LONGRUNAVERAGEOPERATORFORMULA_H_
#define STORM_LOGIC_LONGRUNAVERAGEOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class LongRunAverageOperatorFormula : public OperatorFormula {
        public:
            LongRunAverageOperatorFormula(std::shared_ptr<Formula const> const& subformula);
            LongRunAverageOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            LongRunAverageOperatorFormula(OptimizationDirection optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            LongRunAverageOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula);
            LongRunAverageOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~LongRunAverageOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isLongRunAverageOperatorFormula() const override;
            
            virtual bool isPctlStateFormula() const override;
            virtual bool containsProbabilityOperator() const override;
            virtual bool containsNestedProbabilityOperators() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_LONGRUNAVERAGEOPERATORFORMULA_H_ */