#ifndef STORM_LOGIC_UNARYPATHFORMULA_H_
#define STORM_LOGIC_UNARYPATHFORMULA_H_

#include <memory>

#include "src/logic/PathFormula.h"

namespace storm {
    namespace logic {
        class UnaryPathFormula : public PathFormula {
        public:
            UnaryPathFormula(std::shared_ptr<Formula const> const& subformula);
            
            virtual ~UnaryPathFormula() {
                // Intentionally left empty.
            }

            virtual bool isUnaryPathFormula() const override;
            
            virtual bool isPctlPathFormula() const override;
            virtual bool isPctlWithConditionalPathFormula() const override;
            virtual bool isLtlFormula() const override;
            virtual bool containsBoundedUntilFormula() const override;
            virtual bool containsNextFormula() const override;
            virtual bool containsProbabilityOperator() const override;
            virtual bool containsNestedProbabilityOperators() const override;
            virtual bool containsRewardOperator() const override;
            virtual bool containsNestedRewardOperators() const override;
            
            Formula const& getSubformula() const;
            
            virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
            virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
            virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;
            
        private:
            std::shared_ptr<Formula const> subformula;
        };
    }
}

#endif /* STORM_LOGIC_UNARYPATHFORMULA_H_ */