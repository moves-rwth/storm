#include "src/logic/FragmentChecker.h"

#include "src/logic/Formulas.h"

namespace storm {
    namespace logic {
        class InheritedInformation {
        public:
            InheritedInformation(FragmentSpecification const& fragmentSpecification) : fragmentSpecification(fragmentSpecification) {
                // Intentionally left empty.
            }
            
            FragmentSpecification const& getSpecification() const {
                return fragmentSpecification;
            }

        private:
            FragmentSpecification const& fragmentSpecification;
        };
        
        bool FragmentChecker::conformsToSpecification(Formula const& f, FragmentSpecification const& specification) const {
            boost::any result = f.accept(*this, InheritedInformation(specification));
            return boost::any_cast<bool>(result);
        }
        
        boost::any FragmentChecker::visit(AtomicExpressionFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areAtomicExpressionFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(AtomicLabelFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areAtomicLabelFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areBinaryBooleanStateFormulasAllowed();
            result = result && boost::any_cast<bool>(f.getLeftSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getRightSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(BooleanLiteralFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areBooleanLiteralFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areBoundedUntilFormulasAllowed();
            if (!inherited.getSpecification().areNestedPathFormulasAllowed()) {
                result = result && !f.getLeftSubformula().isPathFormula();
                result = result && !f.getRightSubformula().isPathFormula();
            }
            if (f.hasDiscreteTimeBound()) {
                result = result && inherited.getSpecification().areStepBoundedUntilFormulasAllowed();
            } else {
                result = result && inherited.getSpecification().areTimeBoundedUntilFormulasAllowed();
            }
            result = result && boost::any_cast<bool>(f.getLeftSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getRightSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(ConditionalFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = true;
            if (f.isConditionalProbabilityFormula()) {
                result = result && inherited.getSpecification().areConditionalProbabilityFormulasAllowed();
            } else if (f.isConditionalRewardFormula()) {
                result = result && inherited.getSpecification().areConditionalRewardFormulasFormulasAllowed();
            }
            if (inherited.getSpecification().areOnlyEventuallyFormuluasInConditionalFormulasAllowed()) {
                if (f.isConditionalProbabilityFormula()) {
                    result = result && f.getSubformula().isReachabilityProbabilityFormula() && f.getConditionFormula().isReachabilityProbabilityFormula();
                } else if (f.isConditionalRewardFormula()) {
                    result = result && f.getSubformula().isReachabilityRewardFormula() && f.getConditionFormula().isEventuallyFormula();
                }
            }
            result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getConditionFormula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(CumulativeRewardFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areCumulativeRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(EventuallyFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = true;
            if (f.isReachabilityProbabilityFormula()) {
                result = inherited.getSpecification().areReachabilityProbabilityFormulasAllowed();
                if (!inherited.getSpecification().areNestedPathFormulasAllowed()) {
                    result = result && !f.getSubformula().isPathFormula();
                }
            } else if (f.isReachabilityRewardFormula()) {
                result = result && inherited.getSpecification().areReachabilityRewardFormulasAllowed();
                result = result && f.getSubformula().isStateFormula();
            } else if (f.isReachabilityTimeFormula()) {
                result = result && inherited.getSpecification().areReachbilityTimeFormulasAllowed();
                result = result && f.getSubformula().isStateFormula();
            }
            result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areTimeOperatorsAllowed();
            result = result && (!f.hasQualitativeResult() || inherited.getSpecification().areQualitativeOperatorResultsAllowed());
            result = result && (!f.hasQuantitativeResult() || inherited.getSpecification().areQuantitativeOperatorResultsAllowed());
            result = result && f.getSubformula().isTimePathFormula();
            result = result && (inherited.getSpecification().isVarianceMeasureTypeAllowed() || f.getMeasureType() == RewardMeasureType::Expectation);
            if (!inherited.getSpecification().areNestedOperatorsAllowed()) {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, InheritedInformation(inherited.getSpecification().copy().setOperatorsAllowed(false))));
            } else {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            }
            return result;
        }
        
        boost::any FragmentChecker::visit(GloballyFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areGloballyFormulasAllowed();
            if (!inherited.getSpecification().areNestedPathFormulasAllowed()) {
                result = result && !f.getSubformula().isPathFormula();
            }
            result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(InstantaneousRewardFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areInstantaneousRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areLongRunAverageOperatorsAllowed();
            result = result && f.getSubformula().isStateFormula();
            if (!inherited.getSpecification().areNestedOperatorsAllowed()) {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, InheritedInformation(inherited.getSpecification().copy().setOperatorsAllowed(false))));
            } else {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            }
            return result;
        }
        
        boost::any FragmentChecker::visit(LongRunAverageRewardFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            return inherited.getSpecification().areLongRunAverageRewardFormulasAllowed();
        }
        
        boost::any FragmentChecker::visit(NextFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areNextFormulasAllowed();
            if (!inherited.getSpecification().areNestedPathFormulasAllowed()) {
                result = result && !f.getSubformula().isPathFormula();
            }
            result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areProbabilityOperatorsAllowed();
            result = result && (!f.hasQualitativeResult() || inherited.getSpecification().areQualitativeOperatorResultsAllowed());
            result = result && (!f.hasQuantitativeResult() || inherited.getSpecification().areQuantitativeOperatorResultsAllowed());
            result = result && (f.getSubformula().isProbabilityPathFormula() || f.getSubformula().isConditionalProbabilityFormula());
            if (!inherited.getSpecification().areNestedOperatorsAllowed()) {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, InheritedInformation(inherited.getSpecification().copy().setOperatorsAllowed(false))));
            } else {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            }
            return result;
        }
        
        boost::any FragmentChecker::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areRewardOperatorsAllowed();
            result = result && (!f.hasQualitativeResult() || inherited.getSpecification().areQualitativeOperatorResultsAllowed());
            result = result && (!f.hasQuantitativeResult() || inherited.getSpecification().areQuantitativeOperatorResultsAllowed());
            result = result && (f.getSubformula().isRewardPathFormula() || f.getSubformula().isConditionalRewardFormula());
            result = result && (inherited.getSpecification().isVarianceMeasureTypeAllowed() || f.getMeasureType() == RewardMeasureType::Expectation);
            if (!inherited.getSpecification().areNestedOperatorsAllowed()) {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, InheritedInformation(inherited.getSpecification().copy().setOperatorsAllowed(false))));
            } else {
                result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            }
            return result;
        }
        
        boost::any FragmentChecker::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areUnaryBooleanStateFormulasAllowed();
            result = result && boost::any_cast<bool>(f.getSubformula().accept(*this, data));
            return result;
        }
        
        boost::any FragmentChecker::visit(UntilFormula const& f, boost::any const& data) const {
            InheritedInformation const& inherited = boost::any_cast<InheritedInformation const&>(data);
            bool result = inherited.getSpecification().areUntilFormulasAllowed();
            if (!inherited.getSpecification().areNestedPathFormulasAllowed()) {
                result = result && !f.getLeftSubformula().isPathFormula();
                result = result && !f.getRightSubformula().isPathFormula();
            }
            result = result && boost::any_cast<bool>(f.getLeftSubformula().accept(*this, data));
            result = result && boost::any_cast<bool>(f.getRightSubformula().accept(*this, data));
            return result;
        }
    }
}