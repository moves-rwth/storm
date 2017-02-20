#include "storm/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/transformer/GoalStateMerger.h"
#include "storm/utility/graph.h"


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
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->originalModel.getBackwardTransitions(), phiStates, psiStates);
            // Only consider the maybestates that are reachable from one initial state without hopping over a target (i.e., prob1) state
            storm::storage::BitVector reachableGreater0States = storm::utility::graph::getReachableStates(this->originalModel.getTransitionMatrix(), this->originalModel.getInitialStates(), ~statesWithProbability01.first, statesWithProbability01.second);
            storm::storage::BitVector maybeStates = reachableGreater0States & ~statesWithProbability01.second;
            
            // obtain the resulting subsystem
            this->simplifiedModel = storm::transformer::GoalStateMerger<SparseModelType>::mergeTargetAndSinkStates(this->originalModel, maybeStates, statesWithProbability01.second, statesWithProbability01.first);
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
            // If this method was not overriden by any subclass, simplification is not possible
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricDtmcSimplifier<SparseModelType>::simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula) {
            // If this method was not overriden by any subclass, simplification is not possible
            return false;
        }

        template class SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>;
    }
}