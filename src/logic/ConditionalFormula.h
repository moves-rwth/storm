#ifndef STORM_LOGIC_CONDITIONALFORMULA_H_
#define STORM_LOGIC_CONDITIONALFORMULA_H_

#include "src/logic/BinaryPathFormula.h"
#include "src/logic/FormulaContext.h"

namespace storm {
    namespace logic {
        class ConditionalFormula : public Formula {
        public:
            enum class Context { Probability, Reward };
            
            ConditionalFormula(std::shared_ptr<Formula const> const& subformula, std::shared_ptr<Formula const> const& conditionFormula, FormulaContext context = FormulaContext::Probability);
            
            virtual ~ConditionalFormula() {
                // Intentionally left empty.
            }
            
            Formula const& getSubformula() const;
            Formula const& getConditionFormula() const;

            virtual bool isConditionalProbabilityFormula() const override;
            virtual bool isConditionalRewardFormula() const override;

            virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual std::shared_ptr<Formula> substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const override;
            
            virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
            virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
            virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;
            
        private:
            std::shared_ptr<Formula const> subformula;
            std::shared_ptr<Formula const> conditionFormula;
            FormulaContext context;
        };
    }
}

#endif /* STORM_LOGIC_CONDITIONALFORMULA_H_ */