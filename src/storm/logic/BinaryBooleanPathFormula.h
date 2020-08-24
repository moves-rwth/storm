#ifndef STORM_LOGIC_BINARYBOOLEANPATHFORMULA_H_
#define STORM_LOGIC_BINARYBOOLEANPATHFORMULA_H_

#include <map>

#include "storm/logic/BinaryPathFormula.h"
#include "storm/logic/BinaryBooleanOperatorType.h"

namespace storm {
    namespace logic {
        class BinaryBooleanPathFormula : public BinaryPathFormula {
        public:
            typedef storm::logic::BinaryBooleanOperatorType OperatorType;

            BinaryBooleanPathFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula);
            
            virtual ~BinaryBooleanPathFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isBinaryBooleanPathFormula() const override;
            
            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
            
            OperatorType getOperator() const;
            
            virtual bool isAnd() const;
            virtual bool isOr() const;
                        
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_LOGIC_BINARYBOOLEANPATHFORMULA_H_ */
