#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"

#include "storm/logic/Formulas.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace logic {

        ExtractMaximalStateFormulasVisitor::ExtractMaximalStateFormulasVisitor(std::vector<LabelFormulaPair>& extractedFormulas) : extractedFormulas(extractedFormulas), nestingLevel(0) {
        }

        std::shared_ptr<Formula> ExtractMaximalStateFormulasVisitor::extract(PathFormula const& f, std::vector<LabelFormulaPair>& extractedFormulas) {
            ExtractMaximalStateFormulasVisitor visitor(extractedFormulas);
            boost::any result = f.accept(visitor, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(BinaryBooleanPathFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            if (left->hasQualitativeResult()) {
                left = extract(left);
            }

            std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            if (right->hasQualitativeResult()) {
                right = extract(right);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<BinaryBooleanPathFormula>(f.getOperator(), left, right));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            STORM_LOG_THROW(true, storm::exceptions::InvalidOperationException, "Can not extract maximal state formulas for bounded until");
            // never reached
            return boost::any();
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> sub = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            if (sub->hasQualitativeResult()) {
                sub = extract(sub);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<EventuallyFormula>(sub));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> sub = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            if (sub->hasQualitativeResult()) {
                sub = extract(sub);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<GloballyFormula>(sub));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(NextFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> sub = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            if (sub->hasQualitativeResult()) {
                sub = extract(sub);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<NextFormula>(sub));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(UnaryBooleanPathFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> sub = boost::any_cast<std::shared_ptr<Formula>>(f.getSubformula().accept(*this, data));
            if (sub->hasQualitativeResult()) {
                sub = extract(sub);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<UnaryBooleanPathFormula>(f.getOperator(), sub));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(UntilFormula const& f, boost::any const& data) const {
            if (nestingLevel > 0) {
                return CloneVisitor::visit(f, data);
            }

            std::shared_ptr<Formula> left = boost::any_cast<std::shared_ptr<Formula>>(f.getLeftSubformula().accept(*this, data));
            if (left->hasQualitativeResult()) {
                left = extract(left);
            }

            std::shared_ptr<Formula> right = boost::any_cast<std::shared_ptr<Formula>>(f.getRightSubformula().accept(*this, data));
            if (right->hasQualitativeResult()) {
                right = extract(right);
            }

            return std::static_pointer_cast<Formula>(std::make_shared<UntilFormula>(left, right));
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            incrementNestingLevel();
            boost::any result = CloneVisitor::visit(f, data);
            decrementNestingLevel();
            return result;
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            incrementNestingLevel();
            boost::any result = CloneVisitor::visit(f, data);
            decrementNestingLevel();
            return result;
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(MultiObjectiveFormula const& f, boost::any const& data) const {
            incrementNestingLevel();
            boost::any result = CloneVisitor::visit(f, data);
            decrementNestingLevel();
            return result;
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            incrementNestingLevel();
            boost::any result = CloneVisitor::visit(f, data);
            decrementNestingLevel();
            return result;
        }

        boost::any ExtractMaximalStateFormulasVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            incrementNestingLevel();
            boost::any result = CloneVisitor::visit(f, data);
            decrementNestingLevel();
            return result;
        }

        std::shared_ptr<Formula> ExtractMaximalStateFormulasVisitor::extract(std::shared_ptr<Formula> f) const {
            std::string label = "p" + std::to_string(extractedFormulas.size());

            const_cast<std::vector<LabelFormulaPair>&>(extractedFormulas).emplace_back(label, f);

            return std::make_shared<storm::logic::AtomicLabelFormula>(label);
        }

        void ExtractMaximalStateFormulasVisitor::incrementNestingLevel() const {
            const_cast<std::size_t&>(nestingLevel)++;
        }
        void ExtractMaximalStateFormulasVisitor::decrementNestingLevel() const {
            STORM_LOG_ASSERT(nestingLevel > 0, "Illegal nesting level decrement");
            const_cast<std::size_t&>(nestingLevel)--;
        }


    }
}
