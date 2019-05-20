#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsObjectiveHelper.h"

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/utility/graph.h"
#include "storm/utility/FilteredRewardModel.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/transformer/EndComponentEliminator.h"


#include "storm/exceptions/UnexpectedException.h"
namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <typename ModelType>
            DeterministicSchedsObjectiveHelper<ModelType>::DeterministicSchedsObjectiveHelper(ModelType const& model, storm::modelchecker::multiobjective::Objective<ValueType> const& objective) : model(model), objective(objective) {
                // Intentionally left empty
            }

            template <typename ModelType>
            storm::storage::BitVector evaluatePropositionalFormula(ModelType const& model, storm::logic::Formula const& formula) {
                storm::modelchecker::SparsePropositionalModelChecker<ModelType> mc(model);
                auto checkResult = mc.check(formula);
                STORM_LOG_THROW(checkResult && checkResult->isExplicitQualitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected type of check result for subformula " << formula << ".");
                return checkResult->asExplicitQualitativeCheckResult().getTruthValuesVector();
            }
            
            template <typename ModelType>
            std::map<uint64_t, typename ModelType::ValueType> const& DeterministicSchedsObjectiveHelper<ModelType>::getSchedulerIndependentStateValues() const {
                if (!schedulerIndependentStateValues) {
                    auto const& formula = *objective.formula;
                    std::map<uint64_t, ValueType> result;
                    if (formula.isProbabilityOperatorFormula() && formula.getSubformula().isUntilFormula()) {
                        storm::storage::BitVector phiStates = evaluatePropositionalFormula(model, formula.getSubformula().asUntilFormula().getLeftSubformula());
                        storm::storage::BitVector psiStates = evaluatePropositionalFormula(model, formula.getSubformula().asUntilFormula().getRightSubformula());
                        auto backwardTransitions = model.getBackwardTransitions();
                        {
                            storm::storage::BitVector prob1States = storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), backwardTransitions, phiStates, psiStates);
                            for (auto const& prob1State : prob1States) {
                                result[prob1State] = storm::utility::one<ValueType>();
                            }
                        }
                        {
                            storm::storage::BitVector prob0States = storm::utility::graph::performProb0A(backwardTransitions, phiStates, psiStates);
                            for (auto const& prob0State : prob0States) {
                                result[prob0State] = storm::utility::zero<ValueType>();
                            }
                        }
                    } else if (formula.getSubformula().isEventuallyFormula() && (formula.isRewardOperatorFormula() || formula.isTimeOperatorFormula())) {
                        storm::storage::BitVector rew0States = evaluatePropositionalFormula(model, formula.getSubformula().asEventuallyFormula().getSubformula());
                        if (formula.isRewardOperatorFormula()) {
                            auto const& baseRewardModel = formula.asRewardOperatorFormula().hasRewardModelName() ? model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : model.getUniqueRewardModel();
                            auto rewardModel = storm::utility::createFilteredRewardModel(baseRewardModel, model.isDiscreteTimeModel(), formula.getSubformula().asEventuallyFormula());
                            storm::storage::BitVector statesWithoutReward = rewardModel.get().getStatesWithZeroReward(model.getTransitionMatrix());
                            rew0States = storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getNondeterministicChoiceIndices(), model.getBackwardTransitions(), statesWithoutReward, rew0States);
                        }
                        for (auto const& rew0State : rew0States) {
                            result[rew0State] = storm::utility::zero<ValueType>();
                        }
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isTotalRewardFormula()) {
                        auto const& baseRewardModel = formula.asRewardOperatorFormula().hasRewardModelName() ? model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : model.getUniqueRewardModel();
                        auto rewardModel = storm::utility::createFilteredRewardModel(baseRewardModel, model.isDiscreteTimeModel(), formula.getSubformula().asTotalRewardFormula());
                        storm::storage::BitVector statesWithoutReward = rewardModel.get().getStatesWithZeroReward(model.getTransitionMatrix());
                        storm::storage::BitVector rew0States = storm::utility::graph::performProbGreater0E(model.getBackwardTransitions(), statesWithoutReward, ~statesWithoutReward);
                        rew0States.complement();
                        for (auto const& rew0State : rew0States) {
                            result[rew0State] = storm::utility::zero<ValueType>();
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The given formula " << formula << " is not supported.");
                    }
                    schedulerIndependentStateValues = std::move(result);
                }
                return schedulerIndependentStateValues.get();
            }
            
            template <typename ModelType>
            std::map<uint64_t, typename ModelType::ValueType> const& DeterministicSchedsObjectiveHelper<ModelType>::getChoiceValueOffsets() const {
                if (!choiceValueOffsets) {
                    auto const& formula = *objective.formula;
                    auto const& subformula = formula.getSubformula();
                    std::map<uint64_t, ValueType> result;
                    if (formula.isProbabilityOperatorFormula() && subformula.isUntilFormula()) {
                        // In this case, there is nothing to be done.
                    } else if (formula.isRewardOperatorFormula() && (subformula.isTotalRewardFormula() || subformula.isEventuallyFormula())) {
                        auto const& baseRewardModel = formula.asRewardOperatorFormula().hasRewardModelName() ? model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : model.getUniqueRewardModel();
                        auto rewardModel = subformula.isEventuallyFormula() ? storm::utility::createFilteredRewardModel(baseRewardModel, model.isDiscreteTimeModel(), subformula.asEventuallyFormula()) : storm::utility::createFilteredRewardModel(baseRewardModel, model.isDiscreteTimeModel(), subformula.asTotalRewardFormula());
                        std::vector<ValueType> choiceBasedRewards = rewardModel.get().getTotalRewardVector(model.getTransitionMatrix());
                        // Set entries for all non-zero reward choices at states whose value is not already known.
                        // This relies on the fact that for goal states in reachability reward formulas, getSchedulerIndependentStateValues()[state] is set to zero.
                        auto const& rowGroupIndices = model.getTransitionMatrix().getRowGroupIndices();
                        auto const& stateValues = getSchedulerIndependentStateValues();
                        for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
                            if (stateValues.find(state) == stateValues.end()) {
                                for (uint64_t choice = rowGroupIndices[state]; choice < rowGroupIndices[state + 1]; ++choice) {
                                    if (!storm::utility::isZero(choiceBasedRewards[choice])) {
                                        result[choice] = choiceBasedRewards[choice];
                                    }
                                }
                            }
                        }
                    } else if (formula.isTimeOperatorFormula() && subformula.isEventuallyFormula()) {
                        auto const& rowGroupIndices = model.getTransitionMatrix().getRowGroupIndices();
                        auto const& stateValues = getSchedulerIndependentStateValues();
                        std::vector<ValueType> const* rates = nullptr;
                        storm::storage::BitVector const* ms = nullptr;
                        if (model.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                            auto ma = model.template as<storm::models::sparse::MarkovAutomaton<ValueType>>();
                            rates = &ma->getExitRates();
                            ms = &ma->getMarkovianStates();
                        }
                        if (model.isOfType(storm::models::ModelType::Mdp)) {
                            // Set all choice offsets to one, except for the ones at states in scheduerIndependentStateValues.
                            for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
                                if (stateValues.find(state) == stateValues.end()) {
                                    ValueType value = storm::utility::one<ValueType>();
                                    if (rates) {
                                        if (ms->get(state)) {
                                            value /= (*rates)[state];
                                        } else {
                                            // Nothing to be done for probabilistic states
                                            continue;
                                        }
                                    }
                                    for (uint64_t choice = rowGroupIndices[state]; choice < rowGroupIndices[state + 1]; ++choice) {
                                        result[choice] = value;
                                    }
                                }
                            }
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The given formula " << formula << " is not supported.");
                    }
                    choiceValueOffsets = std::move(result);
                }
                return choiceValueOffsets.get();
            }
            
            template <typename ValueType>
            std::vector<ValueType> evaluateOperatorFormula(Environment const& env, storm::models::sparse::Mdp<ValueType> const& model, storm::logic::Formula const& formula) {
                storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> mc(model);
                storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(formula, false);
                auto checkResult = mc.check(env, task);
                STORM_LOG_THROW(checkResult && checkResult->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected type of check result for subformula " << formula << ".");
                return checkResult->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector();
            }
            
            template <typename ValueType>
            std::vector<ValueType> evaluateOperatorFormula(Environment const& env, storm::models::sparse::MarkovAutomaton<ValueType> const& model, storm::logic::Formula const& formula) {
                storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> mc(model);
                storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(formula, false);
                auto checkResult = mc.check(env, task);
                STORM_LOG_THROW(checkResult && checkResult->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected type of check result for subformula " << formula << ".");
                return checkResult->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector();
            }
            
            template <typename ModelType>
            std::vector<typename ModelType::ValueType> computeValueBounds(Environment const& env, bool lowerValueBounds, ModelType const& model, storm::logic::Formula const& formula) {
                // Change the optimization direction in the formula.
                auto newFormula = storm::logic::CloneVisitor().clone(formula);
                newFormula->asOperatorFormula().setOptimalityType(lowerValueBounds ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                
                if (std::is_same<typename ModelType::ValueType, storm::RationalNumber>::value) {
                    // don't have to worry about precision in exact mode.
                    return evaluateOperatorFormula(env, model, *newFormula);
                } else {
                    // Create an environment where sound results are enforced
                    storm::Environment soundEnv(env);
                    soundEnv.solver().setForceSoundness(true);
                    auto result = evaluateOperatorFormula(soundEnv, model, *newFormula);
                    
                    auto eps = storm::utility::convertNumber<typename ModelType::ValueType>(soundEnv.solver().minMax().getPrecision());
                    // Add/substract eps to all entries to make up for precision errors
                    if (lowerValueBounds) {
                        eps = -eps;
                    }
                    for (auto& v : result) {
                        v += eps;
                    }
                    return result;
                }
            }
            
            template <typename ValueType>
            std::vector<ValueType> getTotalRewardVector(storm::models::sparse::MarkovAutomaton<ValueType> const& model, storm::logic::Formula const& formula) {
                boost::optional<std::string> rewardModelName = formula.asRewardOperatorFormula().getOptionalRewardModelName();
                typename storm::models::sparse::MarkovAutomaton<ValueType>::RewardModelType const& rewardModel = rewardModelName.is_initialized() ? model.getRewardModel(rewardModelName.get()) : model.getUniqueRewardModel();

                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(model.getNumberOfStates(), storm::utility::zero<ValueType>());
                for (auto const markovianState : model.getMarkovianStates()) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / model.getExitRate(markovianState);
                }
                return rewardModel.getTotalActionRewardVector(model.getTransitionMatrix(), stateRewardWeights);
            }
            
            template <typename ValueType>
            std::vector<ValueType> getTotalRewardVector(storm::models::sparse::Mdp<ValueType> const& model, storm::logic::Formula const& formula) {
                boost::optional<std::string> rewardModelName = formula.asRewardOperatorFormula().getOptionalRewardModelName();
                typename storm::models::sparse::Mdp<ValueType>::RewardModelType const& rewardModel = rewardModelName.is_initialized() ? model.getRewardModel(rewardModelName.get()) : model.getUniqueRewardModel();
                return rewardModel.getTotalRewardVector(model.getTransitionMatrix());
            }
            
            template <typename ModelType>
            typename ModelType::ValueType const& DeterministicSchedsObjectiveHelper<ModelType>::getUpperValueBoundAtState(Environment const& env, uint64_t state) const{
                if (!upperResultBounds) {
                    upperResultBounds = computeValueBounds(env, false, model, *objective.formula);
                    auto upperResultBound = objective.upperResultBound;
                    if (storm::utility::vector::hasInfinityEntry(upperResultBounds.get())) {
                        STORM_LOG_THROW(objective.formula->isRewardOperatorFormula(), storm::exceptions::NotSupportedException, "The upper bound for objective " << *objective.originalFormula << " is infinity at some state. This is only supported for reachability rewards / total rewards.");
                        STORM_LOG_THROW(objective.formula->getSubformula().isTotalRewardFormula() || objective.formula->getSubformula().isEventuallyFormula(), storm::exceptions::NotSupportedException, "The upper bound for objective " << *objective.originalFormula << " is infinity at some state. This is only supported for reachability rewards / total rewards.");
                        auto rewards = getTotalRewardVector(model, *objective.formula);
                        auto zeroValuedStates = storm::utility::vector::filterZero(upperResultBounds.get());
                        auto expVisits = computeUpperBoundOnExpectedVisitingTimes(model.getTransitionMatrix(), zeroValuedStates, ~zeroValuedStates, true);
                        ValueType upperBound = storm::utility::zero<ValueType>();
                        for (uint64_t state = 0; state < expVisits.size(); ++state) {
                            ValueType maxReward = storm::utility::zero<ValueType>();
                            for (auto row = model.getTransitionMatrix().getRowGroupIndices()[state], endRow = model.getTransitionMatrix().getRowGroupIndices()[state + 1]; row < endRow; ++row) {
                                maxReward = std::max(maxReward, rewards[row]);
                            }
                            upperBound += expVisits[state] * maxReward;
                        }
                    }
                    storm::utility::vector::clip(upperResultBounds.get(), objective.lowerResultBound, upperResultBound);
                }
                return upperResultBounds.get()[state];
            }
            
            template <typename ModelType>
            typename ModelType::ValueType const& DeterministicSchedsObjectiveHelper<ModelType>::getLowerValueBoundAtState(Environment const& env, uint64_t state) const{
                if (!lowerResultBounds) {
                    lowerResultBounds = computeValueBounds(env, true, model, *objective.formula);
                    storm::utility::vector::clip(lowerResultBounds.get(), objective.lowerResultBound, objective.upperResultBound);
                    STORM_LOG_THROW(!storm::utility::vector::hasInfinityEntry(lowerResultBounds.get()), storm::exceptions::NotSupportedException, "The lower bound for objective " << *objective.originalFormula << " is infinity at some state. This is not supported.");
                }
                return lowerResultBounds.get()[state];
            }
            
            template <typename ModelType>
            bool DeterministicSchedsObjectiveHelper<ModelType>::minimizing() const {
                return storm::solver::minimize(objective.formula->getOptimalityType());
            }
            
            template <typename ModelType>
            bool DeterministicSchedsObjectiveHelper<ModelType>::isTotalRewardObjective() const {
                return objective.formula->isRewardOperatorFormula() && objective.formula->getSubformula().isTotalRewardFormula();
            }
            
            template <typename ModelType>
            std::vector<typename ModelType::ValueType> DeterministicSchedsObjectiveHelper<ModelType>::computeUpperBoundOnExpectedVisitingTimes(storm::storage::SparseMatrix<ValueType> const& modelTransitions, storm::storage::BitVector const& bottomStates, storm::storage::BitVector const& nonBottomStates, bool hasEndComponents) {
                storm::storage::SparseMatrix<ValueType> transitions;
                std::vector<ValueType> probabilitiesToBottomStates;
                boost::optional<std::vector<uint64_t>> modelToSubsystemStateMapping;
                if (hasEndComponents) {
                    // We assume that end components will always be left (or form a sink state).
                    // The approach is to give a lower bound lpath on a path that leaves the end component.
                    // Then we use end component elimination and add a self loop on the 'ec' states with probability 1-lpath
                    storm::storage::MaximalEndComponentDecomposition<ValueType> mecs(modelTransitions, modelTransitions.transpose(true), nonBottomStates);
                    auto mecElimination = storm::transformer::EndComponentEliminator<ValueType>::transform(modelTransitions, mecs, nonBottomStates, nonBottomStates, true);
                    transitions = std::move(mecElimination.matrix);
                    modelToSubsystemStateMapping = std::move(mecElimination.oldToNewStateMapping);
                    probabilitiesToBottomStates.reserve(transitions.getRowCount());
                    for (uint64_t row = 0; row < transitions.getRowCount(); ++row) {
                        probabilitiesToBottomStates.push_back(modelTransitions.getConstrainedRowSum(mecElimination.newToOldRowMapping[row], bottomStates));
                    }
                    // replace 'selfloop probability' for mec states by 1-lpath
                    for (auto const& mec : mecs) {
                        ValueType lpath = storm::utility::one<ValueType>();
                        for (auto const& stateChoices : mec) {
                            ValueType minProb = storm::utility::one<ValueType>();
                            for (auto const& choice : stateChoices.second) {
                                for (auto const& transition : modelTransitions.getRow(choice)) {
                                    if (!storm::utility::isZero(transition.getValue())) {
                                        minProb = std::min(minProb, transition.getValue());
                                    }
                                }
                            }
                            lpath *= minProb;
                        }
                        STORM_LOG_ASSERT(!storm::utility::isZero(lpath), "unexpected value of lpath");
                        uint64_t mecState = mecElimination.oldToNewStateMapping[mec.begin()->first];
                        bool foundEntry = false;
                        for (uint64_t mecChoice = transitions.getRowGroupIndices()[mecState]; mecChoice < transitions.getRowGroupIndices()[mecState + 1]; ++mecChoice) {
                            if (transitions.getRow(mecChoice).getNumberOfEntries() == 1) {
                                auto& entry = *transitions.getRow(mecChoice).begin();
                                if (entry.getColumn() == mecState && storm::utility::isOne(entry.getValue())) {
                                    entry.setValue(storm::utility::one<ValueType>() - lpath);
                                    foundEntry = true;
                                    probabilitiesToBottomStates[mecChoice] = lpath;
                                    break;
                                }
                            }
                        }
                        STORM_LOG_THROW(foundEntry, storm::exceptions::UnexpectedException, "Unable to find self loop entry at mec state.");
                    }
                } else {
                    transitions = modelTransitions.getSubmatrix(true, nonBottomStates, nonBottomStates);
                    probabilitiesToBottomStates = modelTransitions.getConstrainedRowGroupSumVector(nonBottomStates, bottomStates);
                }
                
                auto subsystemBounds = storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ValueType>::computeUpperBoundOnExpectedVisitingTimes(transitions, probabilitiesToBottomStates);
                uint64_t subsystemState = 0;
                
                std::vector<ValueType> visitingTimesUpperBounds;
                visitingTimesUpperBounds.reserve(bottomStates.size());
                for (uint64_t state = 0; state < bottomStates.size(); ++state) {
                    if (bottomStates.get(state)) {
                        visitingTimesUpperBounds.push_back(storm::utility::zero<ValueType>());
                    } else {
                        if (modelToSubsystemStateMapping) {
                            visitingTimesUpperBounds.push_back(subsystemBounds[modelToSubsystemStateMapping.get()[state]]);
                        } else {
                            visitingTimesUpperBounds.push_back(subsystemBounds[subsystemState]);
                        }
                        ++subsystemState;
                    }
                }
                assert(subsystemState == subsystemBounds.size());
            }
            

            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::Mdp<double>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::MarkovAutomaton<double>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
        }
    }
}
