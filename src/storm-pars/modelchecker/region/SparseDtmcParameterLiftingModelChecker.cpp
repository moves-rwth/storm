#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/NumberTraits.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ConstantType>
        SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseDtmcParameterLiftingModelChecker() : SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>(std::make_unique<storm::solver::GeneralMinMaxLinearEquationSolverFactory<ConstantType>>()) {
            // Intentionally left empty
        }
        
        template <typename SparseModelType, typename ConstantType>
        SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseDtmcParameterLiftingModelChecker(std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ConstantType>>&& solverFactory) : solverFactory(std::move(solverFactory)) {
            // Intentionally left empty
        }
        
        template <typename SparseModelType, typename ConstantType>
        bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) const {
            bool result = parametricModel->isOfType(storm::models::ModelType::Dtmc);
            result &= parametricModel->isSparseModel();
            result &= parametricModel->supportsParameters();
            auto dtmc = parametricModel->template as<SparseModelType>();
            result &= static_cast<bool>(dtmc);
            result &= checkTask.getFormula().isInFragment(storm::logic::reachability().setRewardOperatorsAllowed(true).setReachabilityRewardFormulasAllowed(true).setBoundedUntilFormulasAllowed(true).setCumulativeRewardFormulasAllowed(true).setStepBoundedUntilFormulasAllowed(true).setTimeBoundedUntilFormulasAllowed(true));
            return result;
        }
        
        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specify(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
            auto dtmc = parametricModel->template as<SparseModelType>();
            specify(dtmc, checkTask, false);
        }
        
        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specify(std::shared_ptr<SparseModelType> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool skipModelSimplification) {

            STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");
         
            reset();
            
            if (skipModelSimplification) {
                this->parametricModel = parametricModel;
                this->specifyFormula(checkTask);
            } else {
                auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<SparseModelType>(*parametricModel);
                if (!simplifier.simplify(checkTask.getFormula())) {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                }
                this->parametricModel = simplifier.getSimplifiedModel();
                this->specifyFormula(checkTask.substituteFormula(*simplifier.getSimplifiedFormula()));
            }
        }
        
        
        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyBoundedUntilFormula(CheckTask<storm::logic::BoundedUntilFormula, ConstantType> const& checkTask) {
            
            // get the step bound
            STORM_LOG_THROW(!checkTask.getFormula().hasLowerBound(), storm::exceptions::NotSupportedException, "Lower step bounds are not supported.");
            STORM_LOG_THROW(checkTask.getFormula().hasUpperBound(), storm::exceptions::NotSupportedException, "Expected a bounded until formula with an upper bound.");
            STORM_LOG_THROW(checkTask.getFormula().getTimeBoundReference().isStepBound(), storm::exceptions::NotSupportedException, "Expected a bounded until formula with step bounds.");
            stepBound = checkTask.getFormula().getUpperBound().evaluateAsInt();
            STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");
            if (checkTask.getFormula().isUpperBoundStrict()) {
                STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Expected a strict upper step bound that is greater than zero.");
                --(*stepBound);
            }
            STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");

            // get the results for the subformulas
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
            STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getLeftSubformula()) && propositionalChecker.canHandle(checkTask.getFormula().getRightSubformula()), storm::exceptions::NotSupportedException, "Parameter lifting with non-propositional subformulas is not supported");
            storm::storage::BitVector phiStates = std::move(propositionalChecker.check(checkTask.getFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            storm::storage::BitVector psiStates = std::move(propositionalChecker.check(checkTask.getFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            
            // get the maybeStates
            maybeStates = storm::utility::graph::performProbGreater0(this->parametricModel->getBackwardTransitions(), phiStates, psiStates, true, *stepBound);
            maybeStates &= ~psiStates;
            
            // set the result for all non-maybe states
            resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
            storm::utility::vector::setVectorValues(resultsForNonMaybeStates, psiStates, storm::utility::one<ConstantType>());
            
            // if there are maybestates, create the parameterLifter
            if (!maybeStates.empty()) {
                // Create the vector of one-step probabilities to go to target states.
                std::vector<typename SparseModelType::ValueType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), psiStates);
                
                parameterLifter = std::make_unique<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates);
            }
            
            // We know some bounds for the results so set them
            lowerResultBound = storm::utility::zero<ConstantType>();
            upperResultBound = storm::utility::one<ConstantType>();
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyUntilFormula(CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask) {
            
            // get the results for the subformulas
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
            STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getLeftSubformula()) && propositionalChecker.canHandle(checkTask.getFormula().getRightSubformula()), storm::exceptions::NotSupportedException, "Parameter lifting with non-propositional subformulas is not supported");
            storm::storage::BitVector phiStates = std::move(propositionalChecker.check(checkTask.getFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            storm::storage::BitVector psiStates = std::move(propositionalChecker.check(checkTask.getFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            
            // get the maybeStates
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->parametricModel->getBackwardTransitions(), phiStates, psiStates);
            maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
            
            // set the result for all non-maybe states
            resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
            storm::utility::vector::setVectorValues(resultsForNonMaybeStates, statesWithProbability01.second, storm::utility::one<ConstantType>());
            
            // if there are maybestates, create the parameterLifter
            if (!maybeStates.empty()) {
                // Create the vector of one-step probabilities to go to target states.
                std::vector<typename SparseModelType::ValueType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), psiStates);
                
                parameterLifter = std::make_unique<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates);
            }
            
            // We know some bounds for the results so set them
            lowerResultBound = storm::utility::zero<ConstantType>();
            upperResultBound = storm::utility::one<ConstantType>();
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityRewardFormula(CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask) {
            
            // get the results for the subformula
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
            STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getSubformula()), storm::exceptions::NotSupportedException, "Parameter lifting with non-propositional subformulas is not supported");
            storm::storage::BitVector targetStates = std::move(propositionalChecker.check(checkTask.getFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            
            // get the maybeStates
            storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(this->parametricModel->getBackwardTransitions(), storm::storage::BitVector(this->parametricModel->getNumberOfStates(), true), targetStates);
            infinityStates.complement();
            maybeStates = ~(targetStates | infinityStates);
            
            // set the result for all the non-maybe states
            resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
            storm::utility::vector::setVectorValues(resultsForNonMaybeStates, infinityStates, storm::utility::infinity<ConstantType>());
            
            // if there are maybestates, create the parameterLifter
            if (!maybeStates.empty()) {
                // Create the reward vector
                STORM_LOG_THROW((checkTask.isRewardModelSet() && this->parametricModel->hasRewardModel(checkTask.getRewardModel())) || (!checkTask.isRewardModelSet() && this->parametricModel->hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model specified by the CheckTask is not available in the given model.");

                typename SparseModelType::RewardModelType const& rewardModel = checkTask.isRewardModelSet() ? this->parametricModel->getRewardModel(checkTask.getRewardModel()) : this->parametricModel->getUniqueRewardModel();

                std::vector<typename SparseModelType::ValueType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());
                
                parameterLifter = std::make_unique<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates);
            }
            
            // We only know a lower bound for the result
            lowerResultBound = storm::utility::zero<ConstantType>();

        }

        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyCumulativeRewardFormula(CheckTask<storm::logic::CumulativeRewardFormula, ConstantType> const& checkTask) {
            
            // Obtain the stepBound
            stepBound = checkTask.getFormula().getBound().evaluateAsInt();
            if (checkTask.getFormula().isBoundStrict()) {
                STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Expected a strict upper step bound that is greater than zero.");
                --(*stepBound);
            }
            STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");
            
            //Every state is a maybeState
            maybeStates = storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getColumnCount(), true);
            resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates());
            
            // Create the reward vector
            STORM_LOG_THROW((checkTask.isRewardModelSet() && this->parametricModel->hasRewardModel(checkTask.getRewardModel())) || (!checkTask.isRewardModelSet() && this->parametricModel->hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model specified by the CheckTask is not available in the given model.");
            typename SparseModelType::RewardModelType const& rewardModel = checkTask.isRewardModelSet() ? this->parametricModel->getRewardModel(checkTask.getRewardModel()) : this->parametricModel->getUniqueRewardModel();
            std::vector<typename SparseModelType::ValueType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());
            
            parameterLifter = std::make_unique<storm::transformer::ParameterLifter<typename SparseModelType::ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates);
            
            
            // We only know a lower bound for the result
            lowerResultBound = storm::utility::zero<ConstantType>();
        }
        
        template <typename SparseModelType, typename ConstantType>
        storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>& SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationChecker() {
            if (!instantiationChecker) {
                instantiationChecker = std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
                instantiationChecker->specifyFormula(this->currentCheckTask->template convertValueType<typename SparseModelType::ValueType>());
                instantiationChecker->setInstantiationsAreGraphPreserving(true);
            }
            return *instantiationChecker;
        }
        
        template <typename SparseModelType, typename ConstantType>
        std::unique_ptr<CheckResult> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::computeQuantitativeValues(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
            
            if(maybeStates.empty()) {
                return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(resultsForNonMaybeStates);
            }
            
            parameterLifter->specifyRegion(region, dirForParameters);
            
            // Set up the solver
            if (storm::NumberTraits<ConstantType>::IsExact && solverFactory->getMinMaxMethod() == storm::solver::MinMaxMethod::ValueIteration) {
                STORM_LOG_INFO("Parameter Lifting: Setting solution method for exact MinMaxSolver to policy iteration");
                solverFactory->setMinMaxMethod(storm::solver::MinMaxMethod::PolicyIteration);
            }
            auto solver = solverFactory->create(parameterLifter->getMatrix());
            if (lowerResultBound) solver->setLowerBound(lowerResultBound.get());
            if (upperResultBound) solver->setUpperBound(upperResultBound.get());
            if (!stepBound) solver->setTrackScheduler(true);
            if (storm::solver::minimize(dirForParameters) && minSchedChoices && !stepBound) solver->setInitialScheduler(std::move(minSchedChoices.get()));
            if (storm::solver::maximize(dirForParameters) && maxSchedChoices && !stepBound) solver->setInitialScheduler(std::move(maxSchedChoices.get()));
            if (this->currentCheckTask->isBoundSet() && solver->hasInitialScheduler()) {
                // If we reach this point, we know that after applying the hint, the x-values can only become larger (if we maximize) or smaller (if we minimize).
                std::unique_ptr<storm::solver::TerminationCondition<ConstantType>> termCond;
                storm::storage::BitVector relevantStatesInSubsystem = this->currentCheckTask->isOnlyInitialStatesRelevantSet() ? this->parametricModel->getInitialStates() % maybeStates : storm::storage::BitVector(maybeStates.getNumberOfSetBits(), true);
                if (storm::solver::minimize(dirForParameters)) {
                    // Terminate if the value for ALL relevant states is already below the threshold
                    termCond = std::make_unique<storm::solver::TerminateIfFilteredExtremumBelowThreshold<ConstantType>> (relevantStatesInSubsystem, true, this->currentCheckTask->getBoundThreshold(), false);
                } else {
                    // Terminate if the value for ALL relevant states is already above the threshold
                    termCond = std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<ConstantType>> (relevantStatesInSubsystem, true, this->currentCheckTask->getBoundThreshold(), true);
                }
                solver->setTerminationCondition(std::move(termCond));
            }
            
            // Invoke the solver
            if(stepBound) {
                assert(*stepBound > 0);
                x = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
                solver->repeatedMultiply(dirForParameters, x, &parameterLifter->getVector(), *stepBound);
            } else {
                x.resize(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
                solver->solveEquations(dirForParameters, x, parameterLifter->getVector());
                if(storm::solver::minimize(dirForParameters)) {
                    minSchedChoices = solver->getSchedulerChoices();
                } else {
                    maxSchedChoices = solver->getSchedulerChoices();
                }
            }
            
            // Get the result for the complete model (including maybestates)
            std::vector<ConstantType> result = resultsForNonMaybeStates;
            auto maybeStateResIt = x.begin();
            for(auto const& maybeState : maybeStates) {
                result[maybeState] = *maybeStateResIt;
                ++maybeStateResIt;
            }
            return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(std::move(result));
        }
        
        
        template <typename SparseModelType, typename ConstantType>
        void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::reset() {
            maybeStates.resize(0);
            resultsForNonMaybeStates.clear();
            stepBound = boost::none;
            instantiationChecker = nullptr;
            parameterLifter = nullptr;
            minSchedChoices = boost::none;
            maxSchedChoices = boost::none;
            x.clear();
            lowerResultBound = boost::none;
            upperResultBound = boost::none;
        }
        
        template <typename SparseModelType, typename ConstantType>
        boost::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMinScheduler() {
            if (!minSchedChoices) {
                return boost::none;
            }
            
            storm::storage::Scheduler<ConstantType> result(minSchedChoices->size());
            uint_fast64_t state = 0;
            for (auto const& schedulerChoice : minSchedChoices.get()) {
                result.setChoice(schedulerChoice, state);
                ++state;
            }
            
            return result;
        }
        
        template <typename SparseModelType, typename ConstantType>
        boost::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMaxScheduler() {
            if (!maxSchedChoices) {
                return boost::none;
            }
            
            storm::storage::Scheduler<ConstantType> result(maxSchedChoices->size());
            uint_fast64_t state = 0;
            for (auto const& schedulerChoice : maxSchedChoices.get()) {
                result.setChoice(schedulerChoice, state);
                ++state;
            }
            
            return result;
        }
        
        template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
        template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;

    }
}
