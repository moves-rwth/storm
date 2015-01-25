#ifndef STORM_LOGIC_STEADYSTATEFORMULA_H_
#define STORM_LOGIC_STEADYSTATEFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class SteadyStateOperatorFormula : public OperatorFormula {
        public:
            SteadyStateOperatorFormula(std::shared_ptr<Formula const> const& subformula);
            SteadyStateOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            SteadyStateOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            SteadyStateOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula);
            SteadyStateOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~SteadyStateOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isSteadyStateOperatorFormula() const override;
            
            virtual bool isPctlStateFormula() const override;
            virtual bool hasProbabilityOperator() const override;
            virtual bool hasNestedProbabilityOperators() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_STEADYSTATEFORMULA_H_ */