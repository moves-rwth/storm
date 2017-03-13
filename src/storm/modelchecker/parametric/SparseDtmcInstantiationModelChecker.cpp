#include "SparseDtmcInstantiationModelChecker.h"

#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"
namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            template <typename SparseModelType, typename ConstantType>
            SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::SparseDtmcInstantiationModelChecker(SparseModelType const& parametricModel) : SparseInstantiationModelChecker<SparseModelType, ConstantType>(parametricModel), modelInstantiator(parametricModel) {
                //Intentionally left empty
            }
    
            template <typename SparseModelType, typename ConstantType>
            std::unique_ptr<CheckResult> SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) {
                STORM_LOG_THROW(this->currentCheckTask, storm::exceptions::InvalidStateException, "Checking has been invoked but no property has been specified before.");
                this->swInstantiation.start();
                auto const& instantiatedModel = modelInstantiator.instantiate(valuation);
                this->swInstantiation.stop();
                STORM_LOG_ASSERT(instantiatedModel.getTransitionMatrix().isProbabilistic(), "Instantiated matrix is not probabilistic!");
                storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>> modelChecker(instantiatedModel);

                // Check if there are some optimizations implemented for the specified property
                this->swCheck.start();
                if(!this->currentCheckTask->isQualitativeSet() && this->currentCheckTask->getFormula().isInFragment(storm::logic::reachability().setRewardOperatorsAllowed(true).setReachabilityRewardFormulasAllowed(true))) {
                    auto result = checkWithHint(modelChecker);
                    this->swCheck.stop();
                    return result;
                    //return checkWithHint(modelChecker);
                } else {
                    STORM_LOG_WARN("Checking without hint"); // todo:remove this warning
                    auto result = modelChecker.check(*this->currentCheckTask);
                    this->swCheck.stop();
                    return result;
                    //return modelChecker.check(*this->currentCheckTask);
                }
            }
                
            template <typename SparseModelType, typename ConstantType>
            std::unique_ptr<CheckResult> SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>::checkWithHint(storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>>& modelchecker) {
                // Check the formula and store the result as a hint for the next call.
                // For qualitative properties, we still want a quantitative result hint. Hence we perform the check on the subformula
                if(this->currentCheckTask->getFormula().asOperatorFormula().hasQuantitativeResult()) {
                    std::unique_ptr<storm::modelchecker::CheckResult> result = modelchecker.check(*this->currentCheckTask);
                    this->currentCheckTask->setHint(ExplicitModelCheckerHint<ConstantType>(result->template asExplicitQuantitativeCheckResult<ConstantType>().getValueVector()));
                    return result;
                } else {
                    std::unique_ptr<storm::modelchecker::CheckResult> quantitativeResult;
                    auto newCheckTask = this->currentCheckTask->substituteFormula(this->currentCheckTask->getFormula().asOperatorFormula().getSubformula()).setOnlyInitialStatesRelevant(false);
                    if(this->currentCheckTask->getFormula().isProbabilityOperatorFormula()) {
                        quantitativeResult = modelchecker.computeProbabilities(newCheckTask);
                    } else if (this->currentCheckTask->getFormula().isRewardOperatorFormula()) {
                        quantitativeResult = modelchecker.computeRewards(this->currentCheckTask->getFormula().asRewardOperatorFormula().getMeasureType(), newCheckTask);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Checking with result hint is only implemented for probability or reward operator formulas");
                    }
                    std::unique_ptr<storm::modelchecker::CheckResult> qualitativeResult = quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>().compareAgainstBound(this->currentCheckTask->getFormula().asOperatorFormula().getComparisonType(), this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
                    this->currentCheckTask->setHint(ExplicitModelCheckerHint<ConstantType>(std::move(quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>().getValueVector())));
                    return qualitativeResult;
                }
            }
            
            
            template class SparseDtmcInstantiationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;

        }
    }
}