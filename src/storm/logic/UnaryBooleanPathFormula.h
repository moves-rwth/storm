#ifndef STORM_LOGIC_UNARYBOOLEANPATHFORMULA_H_
#define STORM_LOGIC_UNARYBOOLEANPATHFORMULA_H_

#include "storm/logic/UnaryPathFormula.h"
#include "storm/logic/UnaryBooleanOperatorType.h"

namespace storm {
    namespace logic {
        class UnaryBooleanPathFormula : public UnaryPathFormula {
        public:
            typedef storm::logic::UnaryBooleanOperatorType OperatorType;

            UnaryBooleanPathFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& subformula);
            
            virtual ~UnaryBooleanPathFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isUnaryBooleanPathFormula() const override;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            OperatorType getOperator() const;
            
            virtual bool isNot() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;

        private:
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_LOGIC_UNARYBOOLEANPATHFORMULA_H_ */
