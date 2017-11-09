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
            std::vector<boost::optional<TimeBound>> lowerBounds, upperBounds;
            std::vector<TimeBoundReference> timeBoundReferences;
            for (uint64_t i = 0; i < f.getDimension(); ++i) {
                if (f.hasLowerBound(i)) {
                    lowerBounds.emplace_back(TimeBound(f.isLowerBoundStrict(i), f.getLowerBound(i).substitute(substitution)));
                } else {
                    lowerBounds.emplace_back();
                }
                if (f.hasUpperBound(i)) {
                    upperBounds.emplace_back(TimeBound(f.isUpperBoundStrict(i), f.getUpperBound(i).substitute(substitution)));
                } else {
                    upperBounds.emplace_back();
                }
                timeBoundReferences.push_back(f.getTimeBoundReference(i));
            }
            if (f.hasMultiDimensionalSubformulas()) {
                std::vector<std::shared_ptr<Formula const>> leftSubformulas, rightSubformulas;
                for (uint64_t i = 0; i < f.getDimension(); ++i) {
                    leftSubformulas.push_back(boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula(i).accept(*this, data)));
                    rightSubformulas.push_back(boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula(i).accept(*this, data)));
                }
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences));
            } else {
                std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
                std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(left, right, lowerBounds, upperBounds, timeBoundReferences));
            }
        }
        
        boost::any VariableSubstitutionVisitor::visit(CumulativeRewardFormula const& f, boost::any const&) const {
            std::vector<TimeBound> bounds;
            std::vector<TimeBoundReference> timeBoundReferences;
            for (uint64_t i = 0; i < f.getDimension(); ++i) {
                bounds.emplace_back(TimeBound(f.isBoundStrict(i), f.getBound(i).substitute(substitution)));
                timeBoundReferences.push_back(f.getTimeBoundReference(i));
            }
            return std::static_pointer_cast<Formula>(std::make_shared<CumulativeRewardFormula>(bounds, timeBoundReferences));
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
