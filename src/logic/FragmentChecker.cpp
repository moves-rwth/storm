#include "src/logic/FragmentChecker.h"

#include "src/logic/Formulas.h"

namespace storm {
    namespace logic {
        bool FragmentChecker::conformsToSpecification(Formula const& f, FragmentSpecification const& specification) const {
            boost::any result = f.accept(*this, specification);
            return boost::any_cast<bool>(result);
        }
        
        boost::any FragmentChecker::visit(AtomicExpressionFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areAtomicExpressionFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(AtomicLabelFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areAtomicLabelFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            bool result = specification.areBinaryBooleanStateFormulasAllowed();
            result = result && boost::any_cast<bool>(f.getLeftSubformula().accept(*this, specification));
            result = result && boost::any_cast<bool>(f.getRightSubformula().accept(*this, specification));
            return result;
        }
        
        boost::any FragmentChecker::visit(BooleanLiteralFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areBooleanLiteralFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            bool result = specification.areBoundedUntilFormulasAllowed();
            result = result && boost::any_cast<bool>(f.getLeftSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getRightSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(ConditionalFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            bool result = true;
            if (f.isConditionalProbabilityFormula()) {
                result &= specification.areConditionalProbabilityFormulasAllowed();
            } else if (f.Formula::isConditionalRewardFormula()) {
                result &= specification.areConditionalRewardFormulasFormulasAllowed();
            }
            result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getConditionFormula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(CumulativeRewardFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areCumulativeRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(EventuallyFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            bool result = specification.areEventuallyFormulasAllowed();
            result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(ExpectedTimeOperatorFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areExpectedTimeOperatorsAllowed();
        }
        
        boost::any FragmentChecker::visit(GloballyFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areGloballyFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(InstantaneousRewardFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areInstantaneousRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areLongRunAverageOperatorsAllowed();
        }
        
        boost::any FragmentChecker::visit(LongRunAverageRewardFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areLongRunAverageRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(NextFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areNextFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areProbabilityOperatorsAllowed();
        }
        
        boost::any FragmentChecker::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areRewardOperatorsAllowed();
        }
        
        boost::any FragmentChecker::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areUnaryBooleanStateFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(UntilFormula const& f, boost::any const& data) const {
            FragmentSpecification const& specification = boost::any_cast<FragmentSpecification const&>(data);
            return specification.areUntilFormulasAllowed();
        }
    }
}