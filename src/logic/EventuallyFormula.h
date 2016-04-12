#ifndef STORM_LOGIC_EVENTUALLYFORMULA_H_
#define STORM_LOGIC_EVENTUALLYFORMULA_H_

#include "src/logic/UnaryPathFormula.h"
#include "src/logic/FormulaContext.h"

namespace storm {
    namespace logic {
        class EventuallyFormula : public UnaryPathFormula {
        public:            
            EventuallyFormula(std::shared_ptr<Formula const> const& subformula, FormulaContext context = FormulaContext::Probability);
            
            virtual ~EventuallyFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isEventuallyFormula() const override;
            virtual bool isReachabilityProbabilityFormula() const override;
            virtual bool isReachabilityRewardFormula() const override;
            virtual bool isReachabilityTimeFormula() const override;
            virtual bool isProbabilityPathFormula() const override;
            virtual bool isRewardPathFormula() const override;
            virtual bool isTimePathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;

        private:
            FormulaContext context;
        };
    }
}

#endif /* STORM_LOGIC_EVENTUALLYFORMULA_H_ */