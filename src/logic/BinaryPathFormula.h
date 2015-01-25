#ifndef STORM_LOGIC_BINARYPATHFORMULA_H_
#define STORM_LOGIC_BINARYPATHFORMULA_H_

#include <memory>

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class BinaryPathFormula : public PathFormula {
        public:
            BinaryPathFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula);
            
            virtual ~BinaryPathFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isBinaryPathFormula() const override;
            
            virtual bool isPctlPathFormula() const override;
            virtual bool isLtlFormula() const override;
            virtual bool hasProbabilityOperator() const override;
            virtual bool hasNestedProbabilityOperators() const override;
            
            Formula const& getLeftSubformula() const;
            Formula const& getRightSubformula() const;
            
        private:
            std::shared_ptr<Formula const> leftSubformula;
            std::shared_ptr<Formula const> rightSubformula;
        };
    }
}

#endif /* STORM_LOGIC_BINARYPATHFORMULA_H_ */