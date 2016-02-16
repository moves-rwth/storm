#ifndef STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_
#define STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class ExpectedTimeOperatorFormula : public OperatorFormula {
        public:
            ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, Bound<double> const& bound, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(OptimizationDirection optimalityType, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(boost::optional<OptimizationDirection> optimalityType, boost::optional<Bound<double>> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~ExpectedTimeOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isExpectedTimeOperatorFormula() const override;
            
            virtual bool isPctlStateFormula() const override;
            virtual bool isPctlWithConditionalStateFormula() const override;
            virtual bool containsProbabilityOperator() const override;
            virtual bool containsNestedProbabilityOperators() const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_ */