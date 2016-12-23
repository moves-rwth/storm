#include "storm/logic/CloneVisitor.h"

#include "storm/logic/Formulas.h"

namespace storm {
    namespace logic {
        
        std::shared_ptr<Formula> CloneVisitor::clone(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }
        
        boost::any CloneVisitor::visit(AtomicExpressionFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<AtomicExpressionFormula>(f));
        }
        
        boost::any CloneVisitor::visit(AtomicLabelFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<AtomicLabelFormula>(f));
        }
        
        boost::any CloneVisitor::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<BinaryBooleanStateFormula>(f.getOperator(), left, right));
        }
        
        boost::any CloneVisitor::visit(BooleanLiteralFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<BooleanLiteralFormula>(f));
        }
        
        boost::any CloneVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            if (f.hasDiscreteTimeBound()) {
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(left, right, f.getDiscreteTimeBound()));
            } else {
                return std::static_pointer_cast<Formula>(std::make_shared<BoundedUntilFormula>(left, right, f.getIntervalBounds()));
            }
        }
        
        boost::any CloneVisitor::visit(ConditionalFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            std::shared_ptr<Formula> conditionFormula = boost::any_cast<std::shared_ptr<Formula>>(f.getConditionFormula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<ConditionalFormula>(subformula, conditionFormula, f.getContext()));
        }
        
        boost::any CloneVisitor::visit(CumulativeRewardFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<CumulativeRewardFormula>(f));
        }
        
        boost::any CloneVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(subformula, f.getContext()));
        }
        
        boost::any CloneVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<TimeOperatorFormula>(subformula, f.getOperatorInformation()));
        }
        
        boost::any CloneVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<GloballyFormula>(subformula));
        }
        
        boost::any CloneVisitor::visit(InstantaneousRewardFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<InstantaneousRewardFormula>(f));
        }
        
        boost::any CloneVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<LongRunAverageOperatorFormula>(subformula, f.getOperatorInformation()));
        }
        
        boost::any CloneVisitor::visit(LongRunAverageRewardFormula const& f, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<LongRunAverageRewardFormula>(f));
        }
        
        boost::any CloneVisitor::visit(MultiObjectiveFormula const& f, boost::any const& data) const {
            std::vector<std::shared_ptr<Formula const>> subformulas;
            for(auto const& subF : f.getSubformulas()){
                subformulas.push_back(boost::any_cast<std::shared_ptr<Formula>>(subF->accept(*this, data)));
            }
            return std::static_pointer_cast<Formula>(std::make_shared<MultiObjectiveFormula>(subformulas));
        }
        
        boost::any CloneVisitor::visit(NextFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<NextFormula>(subformula));
        }
        
        boost::any CloneVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<ProbabilityOperatorFormula>(subformula, f.getOperatorInformation()));
        }
        
        boost::any CloneVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<RewardOperatorFormula>(subformula, f.getOptionalRewardModelName(), f.getOperatorInformation()));
        }
        
        boost::any CloneVisitor::visit(TotalRewardFormula const&, boost::any const&) const {
            return std::static_pointer_cast<Formula>(std::make_shared<TotalRewardFormula>());
        }
        
        boost::any CloneVisitor::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> subformula = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<UnaryBooleanStateFormula>(f.getOperator(), subformula));
        }
        
        boost::any CloneVisitor::visit(UntilFormula const& f, boost::any const& data) const {
            std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            return std::static_pointer_cast<Formula>(std::make_shared<UntilFormula>(left, right));
        }
        
    }
}
