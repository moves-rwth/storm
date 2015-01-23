#ifndef STORM_LOGIC_BINARYSTATEFORMULA_H_
#define STORM_LOGIC_BINARYSTATEFORMULA_H_

#include "src/properties/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class BinaryStateFormula : public StateFormula {
        public:
            BinaryStateFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula);
            
            virtual ~BinaryStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isBinaryStateFormula() const override;

            Formula& getLeftSubformula();
            Formula const& getLeftSubformula() const;
            
            Formula& getRightSubformula();
            Formula const& getRightSubformula() const;
            
        private:
            std::shared_ptr<Formula> leftSubformula;
            std::shared_ptr<Formula> rightSubformula;
        };
    }
}

#endif /* STORM_LOGIC_BINARYSTATEFORMULA_H_ */