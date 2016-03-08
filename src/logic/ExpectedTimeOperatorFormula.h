#ifndef STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_
#define STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_

#include "src/logic/OperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        class ExpectedTimeOperatorFormula : public OperatorFormula {
        public:
            ExpectedTimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation = OperatorInformation());
            
            virtual ~ExpectedTimeOperatorFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isExpectedTimeOperatorFormula() const override;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
        };
    }
}

#endif /* STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_ */