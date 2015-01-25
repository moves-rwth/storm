#ifndef STORM_LOGIC_BINARYBOOLEANSTATEFORMULA_H_
#define STORM_LOGIC_BINARYBOOLEANSTATEFORMULA_H_

#include "src/logic/BinaryStateFormula.h"

namespace storm {
    namespace logic {
        class BinaryBooleanStateFormula : public BinaryStateFormula {
        public:
            enum class OperatorType {And, Or};

            BinaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula);
            
            virtual ~BinaryBooleanStateFormula() {
                // Intentionally left empty.
            };
            
            virtual bool isBinaryBooleanStateFormula() const override;
            
            virtual bool isPropositionalFormula() const override;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            OperatorType operatorType;
        };
    }
}

#endif /* STORM_LOGIC_BINARYBOOLEANSTATEFORMULA_H_ */