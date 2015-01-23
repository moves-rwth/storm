#ifndef STORM_LOGIC_REACHABILITYREWARDFORMULA_H_
#define STORM_LOGIC_REACHABILITYREWARDFORMULA_H_

#include <memory>

#include "src/properties/logic/PathRewardFormula.h"
#include "src/properties/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class ReachabilityRewardFormula : public PathRewardFormula {
        public:
            ReachabilityRewardFormula(std::shared_ptr<StateFormula> const& subformula);
            
            virtual ~ReachabilityRewardFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isReachabilityRewardFormula() const override;
            
            StateFormula& getSubformula();
            StateFormula const& getSubformula() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            std::shared_ptr<StateFormula> const& subformula;
        };
    }
}

#endif /* STORM_LOGIC_REACHABILITYREWARDFORMULA_H_ */