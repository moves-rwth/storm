#ifndef STORM_LOGIC_BINARYSTATEFORMULA_H_
#define STORM_LOGIC_BINARYSTATEFORMULA_H_

#include "src/logic/StateFormula.h"

namespace storm {
    namespace logic {
        class BinaryStateFormula : public StateFormula {
        public:
            BinaryStateFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula);
            
            virtual ~BinaryStateFormula() {
                // Intentionally left empty.
            }
            
            virtual bool isBinaryStateFormula() const override;

            virtual bool isPctlStateFormula() const override;
            virtual bool isLtlFormula() const override;
            virtual bool hasProbabilityOperator() const override;
            virtual bool hasNestedProbabilityOperators() const override;
            
            Formula const& getLeftSubformula() const;
            Formula const& getRightSubformula() const;
            
            virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
            virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
            
        private:
            std::shared_ptr<Formula const> leftSubformula;
            std::shared_ptr<Formula const> rightSubformula;
        };
    }
}

#endif /* STORM_LOGIC_BINARYSTATEFORMULA_H_ */