#ifndef STORM_LOGIC_CONDITIONALPATHFORMULA_H_
#define STORM_LOGIC_CONDITIONALPATHFORMULA_H_

#include "src/logic/BinaryPathFormula.h"

namespace storm {
    namespace logic {
        class ConditionalPathFormula : public BinaryPathFormula {
        public:
            ConditionalPathFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, bool isRewardFormula = false);
            
            virtual ~ConditionalPathFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isPctlWithConditionalPathFormula() const override;
            virtual bool isRewardPathFormula() const override;
            virtual bool isConditionalPathFormula() const override;
            virtual bool isValidProbabilityPathFormula() const override;
            virtual bool isValidRewardPathFormula() const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
        private:
            bool isRewardFormula;
        };
    }
}

#endif /* STORM_LOGIC_CONDITIONALPATHFORMULA_H_ */