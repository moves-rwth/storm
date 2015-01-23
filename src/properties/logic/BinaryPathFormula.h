#ifndef STORM_LOGIC_BINARYPATHFORMULA_H_
#define STORM_LOGIC_BINARYPATHFORMULA_H_

#include <memory>

#include "src/properties/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class BinaryPathFormula : public PathFormula {
        public:
            BinaryPathFormula(std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula);
            
            virtual ~BinaryPathFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isBinaryPathFormula() const override;
            
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

#endif /* STORM_LOGIC_BINARYPATHFORMULA_H_ */