#include "SparseParameterLiftingModelChecker.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            template <typename SparseModelType, typename ConstantType>
            SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseParameterLiftingModelChecker(SparseModelType const& parametricModel) : parametricModel(parametricModel) {
                //Intentionally left empty
            }
            
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                reset();
                currentFormula = checkTask.getFormula().asSharedPointer();
                currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, ConstantType>>(checkTask.substituteFormula(*currentFormula).template convertValueType<ConstantType>());
                
                if(currentCheckTask->getFormula().isProbabilityOperatorFormula()) {
                    auto const& probOpFormula = currentCheckTask->getFormula().asProbabilityOperatorFormula();
                    if(probOpFormula.getSubformula().isBoundedUntilFormula()) {
                        specifyBoundedUntilFormula(currentCheckTask->substituteFormula(probOpFormula.getSubformula().asBoundedUntilFormula()));
                    } else if(probOpFormula.getSubformula().isUntilFormula()) {
                        specifyUntilFormula(currentCheckTask->substituteFormula(probOpFormula.getSubformula().asUntilFormula()));
                    } else if (probOpFormula.getSubformula().isEventuallyFormula()) {
                        specifyReachabilityProbabilityFormula(currentCheckTask->substituteFormula(probOpFormula.getSubformula().asEventuallyFormula()));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
                    }
                } else if (currentCheckTask->getFormula().isRewardOperatorFormula()) {
                    auto const& rewOpFormula = currentCheckTask->getFormula().asRewardOperatorFormula();
                    if(rewOpFormula.getSubformula().isEventuallyFormula()) {
                        specifyReachabilityRewardFormula(currentCheckTask->substituteFormula(rewOpFormula.getSubformula().asEventuallyFormula()));
                    } else if (rewOpFormula.getSubformula().isCumulativeRewardFormula()) {
                        specifyCumulativeRewardFormula(currentCheckTask->substituteFormula(rewOpFormula.getSubformula().asCumulativeRewardFormula()));
                    }
                }
            }
            
            template <typename SparseModelType, typename ConstantType>
            std::unique_ptr<CheckResult> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::check(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
                auto quantitativeResult = computeQuantitativeValues(region, dirForParameters);
                if(currentCheckTask->getFormula().hasQuantitativeResult()) {
                    return quantitativeResult;
                } else {
                    return quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>().compareAgainstBound(this->currentCheckTask->getFormula().asOperatorFormula().getComparisonType(), this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
                }
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyBoundedUntilFormula(CheckTask<logic::BoundedUntilFormula, ConstantType> const& checkTask) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyUntilFormula(CheckTask<logic::UntilFormula, ConstantType> const& checkTask) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityProbabilityFormula(CheckTask<logic::EventuallyFormula, ConstantType> const& checkTask) {
                // transform to until formula
                auto untilFormula = std::make_shared<storm::logic::UntilFormula const>(storm::logic::Formula::getTrueFormula(), checkTask.getFormula().getSubformula().asSharedPointer());
                specifyUntilFormula(currentCheckTask->substituteFormula(*untilFormula));
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityRewardFormula(CheckTask<logic::EventuallyFormula, ConstantType> const& checkTask) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyCumulativeRewardFormula(CheckTask<logic::CumulativeRewardFormula, ConstantType> const& checkTask) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
            }
    
            template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
            template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
            template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
            template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;

        }
    }
}