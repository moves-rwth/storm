#ifndef STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_
#define STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_

#include "src/logic/UnaryStateFormula.h"

namespace storm {
    namespace logic {
        class UnaryBooleanStateFormula : public UnaryStateFormula {
        public:
            enum class OperatorType { Not };

            UnaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~UnaryBooleanStateFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isUnaryBooleanStateFormula() const override;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            OperatorType getOperator() const;
            
            virtual bool isNot() const;

            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;

        private:
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_LOGIC_UNARYBOOLEANSTATEFORMULA_H_ */