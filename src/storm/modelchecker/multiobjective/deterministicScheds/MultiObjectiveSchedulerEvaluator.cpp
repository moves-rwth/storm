#include "storm/modelchecker/multiobjective/deterministicScheds/MultiObjectiveSchedulerEvaluator.h"


#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/utility/graph.h"
#include "storm/utility/vector.h"

#include "storm/utility/constants.h"
#include "storm/storage/Scheduler.h"

#include "storm/exceptions/NotSupportedException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class ModelType>
            MultiObjectiveSchedulerEvaluator<ModelType>::MultiObjectiveSchedulerEvaluator(preprocessing::SparseMultiObjectivePreprocessorResult<ModelType>& preprocessorResult) : model(*preprocessorResult.preprocessedModel), objectives(preprocessorResult.objectives), currSchedHasBeenChecked(false) {
                results.resize(this->objectives.size());
                currSched.resize(this->getModel().getNumberOfStates(), 0);
                initializeSchedulerIndependentStates();
            }
            
            template <class ModelType>
            void MultiObjectiveSchedulerEvaluator<ModelType>::initializeSchedulerIndependentStates() {
                storm::modelchecker::SparsePropositionalModelChecker<ModelType> mc(getModel());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula() && formula.getSubformula().isUntilFormula()) {
                        storm::storage::BitVector phiStates = mc.check(formula.getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        storm::storage::BitVector psiStates = mc.check(formula.getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        auto backwardTransitions = getModel().getBackwardTransitions();
                        storm::storage::BitVector prob1States = storm::utility::graph::performProb1A(getModel().getTransitionMatrix(), getModel().getNondeterministicChoiceIndices(), backwardTransitions, phiStates, psiStates);
                        storm::storage::BitVector prob0States = storm::utility::graph::performProb0A(backwardTransitions, phiStates, psiStates);
                        storm::utility::vector::setVectorValues(results[objIndex], prob0States, storm::utility::zero<ValueType>());
                        storm::utility::vector::setVectorValues(results[objIndex], prob1States, storm::utility::one<ValueType>());
                        schedulerIndependentStates.push_back(prob1States | prob0States);
                    } else if (formula.getSubformula().isEventuallyFormula() && (formula.isRewardOperatorFormula() || formula.isTimeOperatorFormula())) {
                        storm::storage::BitVector rew0States = mc.check(formula.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        if (formula.isRewardOperatorFormula()) {
                            auto const& rewModel = formula.asRewardOperatorFormula().hasRewardModelName() ? getModel().getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : getModel().getUniqueRewardModel();
                            storm::storage::BitVector statesWithoutReward = rewModel.getStatesWithZeroReward(getModel().getTransitionMatrix());
                            rew0States = storm::utility::graph::performProb1A(getModel().getTransitionMatrix(), getModel().getNondeterministicChoiceIndices(), getModel().getBackwardTransitions(), statesWithoutReward, rew0States);
                        }
                        storm::utility::vector::setVectorValues(results[objIndex], rew0States, storm::utility::zero<ValueType>());
                        schedulerIndependentStates.push_back(std::move(rew0States));
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isTotalRewardFormula()) {
                        auto const& rewModel = formula.asRewardOperatorFormula().hasRewardModelName() ? getModel().getRewardModel(formula.asRewardOperatorFormula().getRewardModelName()) : getModel().getUniqueRewardModel();
                        storm::storage::BitVector statesWithoutReward = rewModel.getStatesWithZeroReward(getModel().getTransitionMatrix());
                        storm::storage::BitVector rew0States = storm::utility::graph::performProbGreater0E(getModel().getBackwardTransitions(), statesWithoutReward, ~statesWithoutReward);
                        rew0States.complement();
                        storm::utility::vector::setVectorValues(results[objIndex], rew0States, storm::utility::zero<ValueType>());
                        schedulerIndependentStates.push_back(std::move(rew0States));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The given formula " << formula << " is not supported.");
                    }
                }
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::modelchecker::CheckResult> invokeModelChecker(Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
                if (model->getType() == storm::models::ModelType::Dtmc) {
                    return storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>>(*model->template as<storm::models::sparse::Dtmc<ValueType>>()).check(env, task);
                } else if(model->getType() == storm::models::ModelType::Ctmc) {
                    return storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>>(*model->template as<storm::models::sparse::Ctmc<ValueType>>()).check(env, task);
                } else {
                    STORM_LOG_ASSERT(false, "invalid model type");
                }
            }
            
            template <class ModelType>
            void MultiObjectiveSchedulerEvaluator<ModelType>::check(Environment const& env) {
                if (!currSchedHasBeenChecked) {
                    storm::storage::Scheduler<ValueType> scheduler(model.getNumberOfStates());
                    for (uint64_t state = 0; state < model.getNumberOfStates(); ++state) {
                        scheduler.setChoice(currSched[state], state);
                    }
                    
                    auto detModel = model.applyScheduler(scheduler, false);
                    
                    for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                        storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(*this->objectives[objIndex].formula, false);
                        auto res = invokeModelChecker<ValueType>(env, detModel, task);
                        results[objIndex] = std::move(res->template asExplicitQuantitativeCheckResult<ValueType>().getValueVector());
                    }
                    currSchedHasBeenChecked = true;
                }
            }
            
            template <class ModelType>
            ModelType const& MultiObjectiveSchedulerEvaluator<ModelType>::getModel() const {
                return model;
            }
            
            template <class ModelType>
            std::vector<Objective<typename MultiObjectiveSchedulerEvaluator<ModelType>::ValueType>> const& MultiObjectiveSchedulerEvaluator<ModelType>::getObjectives() const {
                return objectives;
            }
            
            template <class ModelType>
            std::vector<uint64_t> const& MultiObjectiveSchedulerEvaluator<ModelType>::getScheduler() const {
                return currSched;
            }
            
            template <class ModelType>
            bool MultiObjectiveSchedulerEvaluator<ModelType>::hasCurrentSchedulerBeenChecked() const {
                return currSchedHasBeenChecked;
            }
            
            template <class ModelType>
            uint64_t const& MultiObjectiveSchedulerEvaluator<ModelType>::getChoiceAtState(uint64_t state) const {
                return currSched[state];
            }
            
            template <class ModelType>
            void MultiObjectiveSchedulerEvaluator<ModelType>::setChoiceAtState(uint64_t state, uint64_t choice) {
                if (currSched[state] != choice) {
                    STORM_LOG_ASSERT(choice < this->getModel().getTransitionMatrix().getRowGroupSize(state), "Invalid choice index.");
                    currSched[state] = choice;
                    currSchedHasBeenChecked = false;
                }
            }
 
            template <class ModelType>
            std::vector<typename MultiObjectiveSchedulerEvaluator<ModelType>::ValueType> const& MultiObjectiveSchedulerEvaluator<ModelType>::getResultForObjective(uint64_t objIndex) const {
                STORM_LOG_ASSERT(currSchedHasBeenChecked, "Tried to get results for a scheduler that has not yet been analyzed.");
                return results[objIndex];
            }
            
            template <class ModelType>
            std::vector<std::vector<typename MultiObjectiveSchedulerEvaluator<ModelType>::ValueType>> const& MultiObjectiveSchedulerEvaluator<ModelType>::getResults() const {
                STORM_LOG_ASSERT(currSchedHasBeenChecked, "Tried to get results for a scheduler that has not yet been analyzed.");
                return results;
            }

            template <class ModelType>
            storm::storage::BitVector const& MultiObjectiveSchedulerEvaluator<ModelType>::getSchedulerIndependentStates(uint64_t objIndex) const {
                return schedulerIndependentStates[objIndex];
            }
            
            template <class ModelType>
            std::vector<typename MultiObjectiveSchedulerEvaluator<ModelType>::ValueType> MultiObjectiveSchedulerEvaluator<ModelType>::getInitialStateResults() const {
                STORM_LOG_ASSERT(currSchedHasBeenChecked, "Tried to get results for a scheduler that has not yet been analyzed.");
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "Getting initial state result ist only supported for models with a single initial state.");
                std::vector<ValueType> res;
                for (auto objResult : results) {
                    res.push_back(objResult[*model.getInitialStates().begin()]);
                }
                return res;
            }
            
            
            
            template class MultiObjectiveSchedulerEvaluator<storm::models::sparse::Mdp<double>>;
            template class MultiObjectiveSchedulerEvaluator<storm::models::sparse::MarkovAutomaton<double>>;
            template class MultiObjectiveSchedulerEvaluator<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class MultiObjectiveSchedulerEvaluator<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            
        }
    }
}