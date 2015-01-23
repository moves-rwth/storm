#ifndef STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_
#define STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_

#include "src/properties/logic/OperatorFormula.h"

namespace storm {
    namespace logic {
        class ProbabilityOperatorFormula : public OperatorFormula {
        public:
            ProbabilityOperatorFormula(std::shared_ptr<Formula> const& subformula);
            ProbabilityOperatorFormula(ComparisonType comparisonType, double bound, std::shared_ptr<Formula> const& subformula);
            ProbabilityOperatorFormula(ComparisonType comparisonType, double bound, OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            ProbabilityOperatorFormula(OptimalityType optimalityType, std::shared_ptr<Formula> const& subformula);
            
            virtual ~ProbabilityOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isProbabilityOperator() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            ProbabilityOperatorFormula(boost::optional<ComparisonType> comparisonType, boost::optional<double> bound, boost::optional<OptimalityType> optimalityType, std::shared_ptr<Formula> const& subformula);
        };
    }
}

#endif /* STORM_LOGIC_PROBABILITYOPERATORFORMULA_H_ */