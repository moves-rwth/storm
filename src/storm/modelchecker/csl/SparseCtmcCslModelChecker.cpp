#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"

#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/helper/infinitehorizon/SparseDeterministicInfiniteHorizonHelper.h"
#include "storm/modelchecker/helper/utility/SetInformationFromCheckTask.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/utility/solver.h"
#include "storm/utility/FilteredRewardModel.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/logic/FragmentSpecification.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"

#include "storm/automata/DeterministicAutomaton.h"
#include "storm/automata/AcceptanceCondition.h"
#include "storm/automata/LTL2DeterministicAutomaton.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template <typename SparseCtmcModelType>
        SparseCtmcCslModelChecker<SparseCtmcModelType>::SparseCtmcCslModelChecker(SparseCtmcModelType const& model) : SparsePropositionalModelChecker<SparseCtmcModelType>(model) {
            // Intentionally left empty.
        }
        
        template <typename ModelType>
        bool SparseCtmcCslModelChecker<ModelType>::canHandleStatic(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            auto fragment = storm::logic::csrlstar().setGloballyFormulasAllowed(false).setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true).setTimeAllowed(true).setTimeOperatorsAllowed(true).setTotalRewardFormulasAllowed(true).setRewardAccumulationAllowed(true);
            if (!storm::NumberTraits<ValueType>::SupportsExponential) {
                fragment.setBoundedUntilFormulasAllowed(false).setCumulativeRewardFormulasAllowed(false).setInstantaneousFormulasAllowed(false);
            }
            return checkTask.getFormula().isInFragment(fragment);
        }
        
        template <typename SparseCtmcModelType>
        bool SparseCtmcCslModelChecker<SparseCtmcModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            return canHandleStatic(checkTask);
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeBoundedUntilProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            STORM_LOG_THROW(pathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded or reward-bounded properties on CTMCs are not supported.");
            double lowerBound = 0;
            double upperBound = 0;
            if (pathFormula.hasLowerBound()) {
                lowerBound = pathFormula.getLowerBound<double>();
            }
            if (pathFormula.hasUpperBound()) {
                upperBound = pathFormula.getNonStrictUpperBound<double>();
            } else {
                upperBound = storm::utility::infinity<double>();
            }

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeBoundedUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), checkTask.isQualitativeSet(), lowerBound, upperBound);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeNextProbabilities(Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeNextProbabilities(env, this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeUntilProbabilities(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLTLProbabilities(Environment const& env, CheckTask<storm::logic::PathFormula, ValueType> const& checkTask) {
            storm::logic::PathFormula const& pathFormula = checkTask.getFormula();

            std::vector<storm::logic::ExtractMaximalStateFormulasVisitor::LabelFormulaPair> extracted;
            std::shared_ptr<storm::logic::Formula> ltlFormula = storm::logic::ExtractMaximalStateFormulasVisitor::extract(pathFormula, extracted);

            STORM_LOG_INFO("Extracting maximal state formulas and computing satisfaction sets for path formula: " << pathFormula);

            std::map<std::string, storm::storage::BitVector> apSets;

            for (auto& p : extracted) {
                STORM_LOG_INFO(" Computing satisfaction set for atomic proposition \"" << p.first << "\" <=> " << *p.second << "...");

                std::unique_ptr<CheckResult> subResultPointer = this->check(env, *p.second);
                ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
                auto sat = subResult.getTruthValuesVector();

                STORM_LOG_INFO(" Atomic proposition \"" << p.first << "\" is satisfied by " << sat.getNumberOfSetBits() << " states.");

                apSets[p.first] = std::move(sat);
            }

            STORM_LOG_INFO("Resulting LTL path formula: " << *ltlFormula);
            STORM_LOG_INFO(" in prefix format: " << ltlFormula->toPrefixString());

            std::shared_ptr<storm::automata::DeterministicAutomaton> da = storm::automata::LTL2DeterministicAutomaton::ltl2da(*ltlFormula);

            STORM_LOG_INFO("Deterministic automaton for LTL formula has "
                    << da->getNumberOfStates() << " states, "
                    << da->getAPSet().size() << " atomic propositions and "
                    << *da->getAcceptance()->getAcceptanceExpression() << " as acceptance condition.");

            const SparseCtmcModelType& ctmc = this->getModel();
            typedef typename storm::models::sparse::Dtmc<typename SparseCtmcModelType::ValueType> SparseDtmcModelType;

            STORM_LOG_INFO("Computing embedded DTMC...");
            // compute probability matrix (embedded DTMC)
            storm::storage::SparseMatrix<ValueType> probabilityMatrix = storm::modelchecker::helper::SparseCtmcCslHelper::computeProbabilityMatrix(ctmc.getTransitionMatrix(), ctmc.getExitRateVector());
            // copy of the state labelings of the CTMC
            storm::models::sparse::StateLabeling labeling(ctmc.getStateLabeling());

            // the embedded DTMC, used for building the product and computing the probabilities in the product
            SparseDtmcModelType embeddedDtmc(std::move(probabilityMatrix), std::move(labeling));
            storm::solver::SolveGoal<ValueType> goal(embeddedDtmc, checkTask);


            STORM_LOG_INFO("Performing DA product and probability computations in embedded DTMC...");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeDAProductProbabilities(env, embeddedDtmc, std::move(goal), *da, apSets, checkTask.isQualitativeSet());
            // we can directly return the numericResult vector as the state space of the CTMC and the embedded DTMC are exactly the same
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }


        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeInstantaneousRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!rewardPathFormula.isStepBounded(), storm::exceptions::NotImplementedException, "Currently step-bounded properties on CTMCs are not supported.");
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeInstantaneousRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<double>());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeCumulativeRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded and reward-bounded properties on CTMCs are not supported.");
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeCumulativeRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getExitRateVector(), rewardModel.get(), rewardPathFormula.getNonStrictBound<double>());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
                
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), rewardModel.get(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template<typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeTotalRewards(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) {
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeTotalRewards(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), rewardModel.get(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageProbabilities(Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
            ExplicitQualitativeCheckResult const& subResult = subResultPointer->asExplicitQualitativeCheckResult();
            
            auto probabilisticTransitions = this->getModel().computeProbabilityMatrix();
            storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(probabilisticTransitions, this->getModel().getExitRateVector());
            storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());
			auto values = helper.computeLongRunAverageProbabilities(env, subResult.getTruthValuesVector());
			
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(values)));
        }

        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeLongRunAverageRewards(Environment const& env, storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
            auto probabilisticTransitions = this->getModel().computeProbabilityMatrix();
            storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(probabilisticTransitions, this->getModel().getExitRateVector());
            storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());
			auto values = helper.computeLongRunAverageRewards(env, rewardModel.get());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(values)));
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeReachabilityTimes(Environment const& env, storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
            ExplicitQualitativeCheckResult& subResult = subResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseCtmcCslHelper::computeReachabilityTimes(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), this->getModel().getBackwardTransitions(), this->getModel().getExitRateVector(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        }

        template <typename SparseCtmcModelType>
        std::vector<typename SparseCtmcModelType::ValueType> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeAllTransientProbabilities(Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(pathFormula.getTimeBoundReference().isTimeBound(), storm::exceptions::NotImplementedException, "Currently step-bounded or reward-bounded properties on CTMCs are not supported.");
            STORM_LOG_THROW(pathFormula.hasUpperBound(), storm::exceptions::NotImplementedException, "Computation needs upper limit for time bound.");
            double upperBound = pathFormula.getNonStrictUpperBound<double>();

            std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
            ExplicitQualitativeCheckResult const& leftResult = leftResultPointer->asExplicitQualitativeCheckResult();;
            ExplicitQualitativeCheckResult const& rightResult = rightResultPointer->asExplicitQualitativeCheckResult();

            std::vector<ValueType> result = storm::modelchecker::helper::SparseCtmcCslHelper::computeAllTransientProbabilities(env, this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), this->getModel().getExitRateVector(), upperBound);
            return result;
        }
        
        template <typename SparseCtmcModelType>
        std::unique_ptr<CheckResult> SparseCtmcCslModelChecker<SparseCtmcModelType>::computeSteadyStateDistribution(Environment const& env) {
            // Initialize helper
            auto probabilisticTransitions = this->getModel().computeProbabilityMatrix();
            storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(probabilisticTransitions, this->getModel().getExitRateVector());
            
            // Compute result
            std::vector<ValueType> result;
            auto const& initialStates = this->getModel().getInitialStates();
            uint64_t numInitStates = initialStates.getNumberOfSetBits();
            if (numInitStates == 1) {
                result = helper.computeLongRunAverageStateDistribution(env, *initialStates.begin());
            } else {
                STORM_LOG_WARN("Multiple initial states found. A uniform distribution over initial states is assumed.");
                ValueType initProb = storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType, uint64_t>(numInitStates);
                result = helper.computeLongRunAverageStateDistribution(env, [&initialStates,&initProb](uint64_t const& stateIndex) { return initialStates.get(stateIndex) ? initProb : storm::utility::zero<ValueType>(); });
            }

            // Return CheckResult
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
        }


        // Explicitly instantiate the model checker.
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<double>>;

#ifdef STORM_HAVE_CARL
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalNumber>>;
        template class SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<storm::RationalFunction>>;
#endif

    } // namespace modelchecker
} // namespace storm
