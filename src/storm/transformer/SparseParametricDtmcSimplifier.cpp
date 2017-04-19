#include "storm/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/logic/CloneVisitor.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/transformer/GoalStateMerger.h"
#include "storm/utility/graph.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace transformer {

        template<typename SparseModelType>
        SparseParametricDtmcSimplifier<SparseModelType>::SparseParametricDtmcSimplifier(SparseModelType const& model) : SparseParametricModelSimplifier<SparseModelType>(model) {
            // intentionally left empty
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // Get the prob0, prob1 and the maybeStates
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(this->originalModel);
            if(!propositionalChecker.canHandle(formula.getSubformula().asUntilFormula().getLeftSubformula()) || !propositionalChecker.canHandle(formula.getSubformula().asUntilFormula().getRightSubformula())) {
                STORM_LOG_DEBUG("Can not simplify when Until-formula has non-propositional subformula(s). Formula: " << formula);
                return false;
            }
            storm::storage::BitVector phiStates = std::move(propositionalChecker.check(formula.getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            storm::storage::BitVector psiStates = std::move(propositionalChecker.check(formula.getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->originalModel, phiStates, psiStates);
            // Only consider the maybestates that are reachable from one initial state without hopping over a target (i.e., prob1) state
            storm::storage::BitVector reachableGreater0States = storm::utility::graph::getReachableStates(this->originalModel.getTransitionMatrix(), this->originalModel.getInitialStates() & ~statesWithProbability01.first, ~statesWithProbability01.first, statesWithProbability01.second);
            storm::storage::BitVector maybeStates = reachableGreater0States & ~statesWithProbability01.second;
            
            // obtain the resulting subsystem
            storm::transformer::GoalStateMerger<SparseModelType> goalStateMerger(this->originalModel);
            this->simplifiedModel = goalStateMerger.mergeTargetAndSinkStates(maybeStates, statesWithProbability01.second, statesWithProbability01.first);
            this->simplifiedModel->getStateLabeling().addLabel("target", statesWithProbability01.second);
            this->simplifiedModel->getStateLabeling().addLabel("sink", statesWithProbability01.first);
            
            // obtain the simplified formula for the simplified model
            auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula const> ("target");
            auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula const>(labelFormula, storm::logic::FormulaContext::Probability);
            this->simplifiedFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula const>(eventuallyFormula, formula.getOperatorInformation());
            
            // Eliminate all states for which all outgoing transitions are constant
            this->simplifiedModel = this->eliminateConstantDeterministicStates(*this->simplifiedModel);
                
            return true;
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForBoundedUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            STORM_LOG_THROW(!formula.getSubformula().asBoundedUntilFormula().hasLowerBound(), storm::exceptions::NotSupportedException, "Lower step bounds are not supported.");
            STORM_LOG_THROW(formula.getSubformula().asBoundedUntilFormula().hasUpperBound(), storm::exceptions::UnexpectedException, "Expected a bounded until formula with an upper bound.");
            STORM_LOG_THROW(formula.getSubformula().asBoundedUntilFormula().getUpperBound().getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::UnexpectedException, "Expected a bounded until formula with integral bounds.");
                        
            uint_fast64_t upperStepBound = formula.getSubformula().asBoundedUntilFormula().getUpperBound().evaluateAsInt();
            if (formula.getSubformula().asBoundedUntilFormula().isUpperBoundStrict()) {
                STORM_LOG_THROW(upperStepBound > 0, storm::exceptions::UnexpectedException, "Expected a strict upper bound that is greater than zero.");
                --upperStepBound;
            }
            
            // Get the prob0, target, and the maybeStates
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(this->originalModel);
            if(!propositionalChecker.canHandle(formula.getSubformula().asBoundedUntilFormula().getLeftSubformula()) || !propositionalChecker.canHandle(formula.getSubformula().asBoundedUntilFormula().getRightSubformula())) {
                STORM_LOG_DEBUG("Can not simplify when Until-formula has non-propositional subformula(s). Formula: " << formula);
                return false;
            }
            storm::storage::BitVector phiStates = std::move(propositionalChecker.check(formula.getSubformula().asBoundedUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            storm::storage::BitVector psiStates = std::move(propositionalChecker.check(formula.getSubformula().asBoundedUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            storm::storage::BitVector probGreater0States = storm::utility::graph::performProbGreater0(this->originalModel.getBackwardTransitions(), phiStates, psiStates, true, upperStepBound);
            
            // Only consider the maybestates that are reachable from one initial probGreater0 state within the given amount of steps and without hopping over a target state
            storm::storage::BitVector reachableGreater0States = storm::utility::graph::getReachableStates(this->originalModel.getTransitionMatrix(), this->originalModel.getInitialStates() & probGreater0States, probGreater0States, psiStates, true, upperStepBound);
            storm::storage::BitVector maybeStates = reachableGreater0States & ~psiStates;
            storm::storage::BitVector prob0States = ~reachableGreater0States & ~psiStates;
            
            // obtain the resulting subsystem
            storm::transformer::GoalStateMerger<SparseModelType> goalStateMerger(this->originalModel);
            this->simplifiedModel = goalStateMerger.mergeTargetAndSinkStates(maybeStates, prob0States, psiStates);
            this->simplifiedModel->getStateLabeling().addLabel("target", psiStates);
            this->simplifiedModel->getStateLabeling().addLabel("sink", prob0States);
            
            // obtain the simplified formula for the simplified model
            auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula const> ("target");
            auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula const>(storm::logic::Formula::getTrueFormula(), labelFormula, boost::none, storm::logic::TimeBound(formula.getSubformula().asBoundedUntilFormula().isUpperBoundStrict(), formula.getSubformula().asBoundedUntilFormula().getUpperBound()), storm::logic::TimeBoundType::Steps);
            this->simplifiedFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula const>(boundedUntilFormula, formula.getOperatorInformation());
            
            return true;
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula) {
            typename SparseModelType::RewardModelType const& originalRewardModel = formula.hasRewardModelName() ? this->originalModel.getRewardModel(formula.getRewardModelName()) : this->originalModel.getUniqueRewardModel();
            
            // Get the prob1 and the maybeStates
            storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(this->originalModel);
            if(!propositionalChecker.canHandle(formula.getSubformula().asEventuallyFormula().getSubformula())) {
                STORM_LOG_DEBUG("Can not simplify when reachability reward formula has non-propositional subformula(s). Formula: " << formula);
                return false;
            }
            storm::storage::BitVector targetStates = std::move(propositionalChecker.check(formula.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
            // The set of target states can be extended by the states that reach target with probability 1 without collecting any reward
            targetStates = storm::utility::graph::performProb1(this->originalModel.getBackwardTransitions(), originalRewardModel.getStatesWithZeroReward(this->originalModel.getTransitionMatrix()), targetStates);
            storm::storage::BitVector statesWithProb1 = storm::utility::graph::performProb1(this->originalModel.getBackwardTransitions(), storm::storage::BitVector(this->originalModel.getNumberOfStates(), true), targetStates);
            storm::storage::BitVector infinityStates = ~statesWithProb1;
            // Only consider the states that are reachable from an initial state without hopping over a target state
            storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->originalModel.getTransitionMatrix(), this->originalModel.getInitialStates() & statesWithProb1, statesWithProb1, targetStates);
            storm::storage::BitVector maybeStates = reachableStates & ~targetStates;
            
            // obtain the resulting subsystem
            std::vector<std::string> rewardModelNameAsVector(1, formula.hasRewardModelName() ? formula.getRewardModelName() : this->originalModel.getRewardModels().begin()->first);
            storm::transformer::GoalStateMerger<SparseModelType> goalStateMerger(this->originalModel);
            this->simplifiedModel = goalStateMerger.mergeTargetAndSinkStates(maybeStates, targetStates, infinityStates, rewardModelNameAsVector);
            this->simplifiedModel->getStateLabeling().addLabel("target", targetStates);
            this->simplifiedModel->getStateLabeling().addLabel("sink", infinityStates);
            
            // obtain the simplified formula for the simplified model
            auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula const> ("target");
            auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula const>(labelFormula, storm::logic::FormulaContext::Reward);
            this->simplifiedFormula = std::make_shared<storm::logic::RewardOperatorFormula const>(eventuallyFormula, rewardModelNameAsVector.front(), formula.getOperatorInformation(), storm::logic::RewardMeasureType::Expectation);
            
            // Eliminate all states for which all outgoing transitions are constant
            this->simplifiedModel = this->eliminateConstantDeterministicStates(*this->simplifiedModel, rewardModelNameAsVector.front());
                
            return true;
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula) {
            STORM_LOG_THROW(formula.getSubformula().asCumulativeRewardFormula().getBound().getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::UnexpectedException, "Expected a cumulative reward formula with integral bound.");

            typename SparseModelType::RewardModelType const& originalRewardModel = formula.hasRewardModelName() ? this->originalModel.getRewardModel(formula.getRewardModelName()) : this->originalModel.getUniqueRewardModel();

            uint_fast64_t stepBound = formula.getSubformula().asCumulativeRewardFormula().getBound().evaluateAsInt();
            if (formula.getSubformula().asCumulativeRewardFormula().isBoundStrict()) {
                STORM_LOG_THROW(stepBound > 0, storm::exceptions::UnexpectedException, "Expected a strict upper bound that is greater than zero.");
                --stepBound;
            }
            
            // Get the states with non-zero reward
            storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(this->originalModel.getBackwardTransitions(), storm::storage::BitVector(this->originalModel.getNumberOfStates(), true), ~originalRewardModel.getStatesWithZeroReward(this->originalModel.getTransitionMatrix()), true, stepBound);
            storm::storage::BitVector zeroRewardStates = ~maybeStates;
            storm::storage::BitVector noStates(this->originalModel.getNumberOfStates(), false);
            
            // obtain the resulting subsystem
            std::vector<std::string> rewardModelNameAsVector(1, formula.hasRewardModelName() ? formula.getRewardModelName() : this->originalModel.getRewardModels().begin()->first);
            storm::transformer::GoalStateMerger<SparseModelType> goalStateMerger(this->originalModel);
            this->simplifiedModel = goalStateMerger.mergeTargetAndSinkStates(maybeStates, noStates, zeroRewardStates, rewardModelNameAsVector);
            
            // obtain the simplified formula for the simplified model
            this->simplifiedFormula = storm::logic::CloneVisitor().clone(formula);
            
            return true;
        }

        template class SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>;
    }
}