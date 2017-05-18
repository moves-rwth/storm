#include "storm/logic/VariableSubstitutionVisitor.h"

#include "storm/logic/Formulas.h"

namespace storm {
    namespace logic {
        
        VariableSubstitutionVisitor::VariableSubstitutionVisitor(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) : substitution(substitution) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> VariableSubstitutionVisitor::substitute(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }
               
        boost::any VariableSubstitutionVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<TimeOperatorFormula>(subformula, substituteOperatorInformation(f.getOperatorInformation())));
        }
        
        boost::any VariableSubstitutionVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<LongRunAverageOperatorFormula>(subformula, substituteOperatorInformation(f.getOperatorInformation())));
        }
        
        boost::any VariableSubstitutionVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<ProbabilityOperatorFormula>(subformula, substituteOperatorInformation(f.getOperatorInformation())));
        }
        
        boost::any VariableSubstitutionVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<RewardOperatorFormula>(subformula, f.getOptionalRewardModelName(), substituteOperatorInformation(f.getOperatorInformation())));
        }
        
        boost::any VariableSubstitutionVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            auto left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            auto right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            
            boost::optional<TimeBound> lowerBound;
            if (f.hasLowerBound()) {
                lowerBound = TimeBound(f.isLowerBoundStrict(), f.getLowerBound().substitute(substitution));
            }
            boost::optional<TimeBound> upperBound;
            if (f.hasUpperBound()) {
                upperBound = TimeBound(f.isUpperBoundStrict(), f.getUpperBound().substitute(substitution));
            }

            return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(left, right, lowerBound, upperBound, f.getTimeBoundType()));
        }
        
        boost::any VariableSubstitutionVisitor::visit(CumulativeRewardFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<CumulativeRewardFormula>(storm::logic::TimeBound(f.isBoundStrict(), f.getBound().substitute(substitution)), f.getTimeBoundType()));
        }
        
        boost::any VariableSubstitutionVisitor::visit(InstantaneousRewardFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<InstantaneousRewardFormula>(f.getBound().substitute(substitution), f.getTimeBoundType()));
        }
        
        boost::any VariableSubstitutionVisitor::visit(AtomicExpressionFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<AtomicExpressionFormula>(f.getExpression().substitute(substitution)));
        }
        
        OperatorInformation VariableSubstitutionVisitor::substituteOperatorInformation(OperatorInformation const& operatorInformation) const {
            boost::optional<Bound> bound;
            if(operatorInformation.bound) {
               bound = Bound(operatorInformation.bound->comparisonType, operatorInformation.bound->threshold.substitute(substitution));
            }
            return OperatorInformation(operatorInformation.optimalityType, bound);
        }

    }
}
