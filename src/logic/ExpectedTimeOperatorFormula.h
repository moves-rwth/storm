#ifndef STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_
#define STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class ExpectedTimeOperatorFormula : public OperatorFormula {
        public:
            ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(OptimalityType optimalityType, ComparisonType comparisonType, double bound, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula const> const& subformula);
            ExpectedTimeOperatorFormula(boost::optional<OptimalityType> optimalityType, boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~ExpectedTimeOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isExpectedTimeOperatorFormula() const override;
            
            virtual bool isPctlStateFormula() const override;
            virtual bool containsProbabilityOperator() const override;
            virtual bool containsNestedProbabilityOperators() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_ */