#ifndef STORM_LOGIC_LONGRUNAVERAGEREWARDFORMULA_H_
#define STORM_LOGIC_LONGRUNAVERAGEREWARDFORMULA_H_

#include <memory>

#include "src/logic/RewardPathFormula.h"


namespace storm {
    namespace logic {
        class LongRunAverageRewardFormula : public RewardPathFormula {
        public:
            LongRunAverageRewardFormula();
            
            virtual ~LongRunAverageRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isLongRunAverageRewardFormula() const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        };
    }
}

#endif /* STORM_LOGIC_LONGRUNAVERAGEREWARDFORMULA_H_ */