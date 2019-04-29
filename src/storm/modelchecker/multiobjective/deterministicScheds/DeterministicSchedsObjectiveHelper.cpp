#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsObjectiveHelper.h"

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/BitVector.h"
#include "storm/utility/graph.h"
#include "storm/utility/FilteredRewardModel.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"


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
            
            template <typename ModelType>
            typename ModelType::ValueType const& DeterministicSchedsObjectiveHelper<ModelType>::getUpperValueBoundAtState(Environment const& env, uint64_t state) const{
                //return objective.upperResultBound.get();
                if (!upperResultBounds) {
                    upperResultBounds = computeValueBounds(env, false, model, *objective.formula);
                    storm::utility::vector::clip(upperResultBounds.get(), objective.lowerResultBound, objective.upperResultBound);
                    STORM_LOG_THROW(!storm::utility::vector::hasInfinityEntry(upperResultBounds.get()), storm::exceptions::NotSupportedException, "The upper bound for objective " << *objective.originalFormula << " is infinity at some state. This is not supported.");
                }
                return upperResultBounds.get()[state];
                
            }
            
            template <typename ModelType>
            typename ModelType::ValueType const& DeterministicSchedsObjectiveHelper<ModelType>::getLowerValueBoundAtState(Environment const& env, uint64_t state) const{
                // return objective.lowerResultBound.get();
                if (!lowerResultBounds) {
                    lowerResultBounds = computeValueBounds(env, true, model, *objective.formula);
                    storm::utility::vector::clip(lowerResultBounds.get(), objective.lowerResultBound, objective.upperResultBound);
                    STORM_LOG_THROW(!storm::utility::vector::hasInfinityEntry(lowerResultBounds.get()), storm::exceptions::NotSupportedException, "The lower bound for objective " << *objective.originalFormula << " is infinity at some state. This is not supported.");
                }
                return lowerResultBounds.get()[state];
            }
            
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::Mdp<double>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::MarkovAutomaton<double>>;
            template class DeterministicSchedsObjectiveHelper<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
        }
    }
}
