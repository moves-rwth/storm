#include "storm/modelchecker/multiobjective/SparseMultiObjectivePreprocessor.h"

#include <algorithm>
#include <storm/transformer/GoalStateMerger.h>

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/transformer/SubsystemBuilder.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
                
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::preprocess(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula) {
                
                PreprocessorData data(originalModel);
                
                //Invoke preprocessing on the individual objectives
                for (auto const& subFormula : originalFormula.getSubformulas()) {
                    STORM_LOG_INFO("Preprocessing objective " << *subFormula<< ".");
                    data.objectives.push_back(std::make_shared<Objective<ValueType>>());
                    data.objectives.back()->originalFormula = subFormula;
                    data.finiteRewardCheckObjectives.resize(data.objectives.size(), false);
                    if (data.objectives.back()->originalFormula->isOperatorFormula()) {
                        preprocessOperatorFormula(data.objectives.back()->originalFormula->asOperatorFormula(), data);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the subformula " << *subFormula << " of " << originalFormula << " because it is not supported");
                    }
                }
                
                // Incorporate the required memory into the state space
                storm::storage::SparseModelMemoryProduct<ValueType> product = data.memory->product(originalModel);
                std::shared_ptr<SparseModelType> preprocessedModel = std::dynamic_pointer_cast<SparseModelType>(product.build());
                
                auto backwardTransitions = preprocessedModel->getBackwardTransitions();
                
                // compute the end components of the model (if required)
                bool endComponentAnalysisRequired = false;
                for (auto& task : data.tasks) {
                    endComponentAnalysisRequired = endComponentAnalysisRequired || task->requiresEndComponentAnalysis();
                }
                if (endComponentAnalysisRequired) {
                    // TODO
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "End component analysis required but currently not implemented.");
                }
                
                for (auto& task : data.tasks) {
                    task->perform(*preprocessedModel);
                }
                
                // Build the actual result
                return buildResult(originalModel, originalFormula, data, preprocessedModel, backwardTransitions);
            }
            
            template <typename SparseModelType>
            SparseMultiObjectivePreprocessor<SparseModelType>::PreprocessorData::PreprocessorData(SparseModelType const& model) : originalModel(model) {
                storm::storage::MemoryStructureBuilder<ValueType, RewardModelType> memoryBuilder(1, model);
                memoryBuilder.setTransition(0,0, storm::storage::BitVector(model.getNumberOfStates(), true));
                memory = std::make_shared<storm::storage::MemoryStructure>(memoryBuilder.build());
                
                // The memoryLabelPrefix should not be a prefix of a state label of the given model to ensure uniqueness of label names
                memoryLabelPrefix = "mem";
                while (true) {
                    bool prefixIsUnique = true;
                    for (auto const& label : originalModel.getStateLabeling().getLabels()) {
                        if (memoryLabelPrefix.size() <= label.size()) {
                            if (std::mismatch(memoryLabelPrefix.begin(), memoryLabelPrefix.end(), label.begin()).first == memoryLabelPrefix.end()) {
                                prefixIsUnique = false;
                                memoryLabelPrefix = "_" + memoryLabelPrefix;
                                break;
                            }
                        }
                    }
                    if (prefixIsUnique) {
                        break;
                    }
                }
                
                // The rewardModelNamePrefix should not be a prefix of a reward model name of the given model to ensure uniqueness of reward model names
                rewardModelNamePrefix = "obj";
                while (true) {
                    bool prefixIsUnique = true;
                    for (auto const& label : originalModel.getStateLabeling().getLabels()) {
                        if (memoryLabelPrefix.size() <= label.size()) {
                            if (std::mismatch(memoryLabelPrefix.begin(), memoryLabelPrefix.end(), label.begin()).first == memoryLabelPrefix.end()) {
                                prefixIsUnique = false;
                                memoryLabelPrefix = "_" + memoryLabelPrefix;
                                break;
                            }
                        }
                    }
                    if (prefixIsUnique) {
                        break;
                    }
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessOperatorFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data) {
                
                Objective<ValueType>& objective = *data.objectives.back();
                
                objective.considersComplementaryEvent = false;
                
                if (formula.hasBound()) {
                    STORM_LOG_THROW(!formula.getBound().threshold.containsVariables(), storm::exceptions::InvalidPropertyException, "The formula " << formula << "considers a non-constant threshold");
                    objective.bound = formula.getBound();
                    if (storm::logic::isLowerBound(formula.getBound().comparisonType)) {
                        objective.optimizationDirection = storm::solver::OptimizationDirection::Maximize;
                    } else {
                        objective.optimizationDirection = storm::solver::OptimizationDirection::Minimize;
                    }
                    STORM_LOG_WARN_COND(!formula.hasOptimalityType() || formula.getOptimalityType() == objective.optimizationDirection, "Optimization direction of formula " << formula << " ignored as the formula also specifies a threshold.");
                } else if (formula.hasOptimalityType()){
                    objective.optimizationDirection = formula.getOptimalityType();
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " does not specify whether to minimize or maximize");
                }
                
                if (formula.isProbabilityOperatorFormula()){
                    preprocessProbabilityOperatorFormula(formula.asProbabilityOperatorFormula(), data);
                } else if (formula.isRewardOperatorFormula()){
                    preprocessRewardOperatorFormula(formula.asRewardOperatorFormula(), data);
                } else if (formula.isTimeOperatorFormula()){
                    preprocessTimeOperatorFormula(formula.asTimeOperatorFormula(), data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the objective " << formula << " because it is not supported");
                }
                
                // Invert the bound and optimization direction (if necessary)
                if (objective.considersComplementaryEvent) {
                    if (objective.bound) {
                        objective.bound->threshold = objective.bound->threshold.getManager().rational(storm::utility::one<storm::RationalNumber>()) - objective.bound->threshold;
                        switch (objective.bound->comparisonType) {
                            case storm::logic::ComparisonType::Greater:
                                objective.bound->comparisonType = storm::logic::ComparisonType::Less;
                                break;
                            case storm::logic::ComparisonType::GreaterEqual:
                                objective.bound->comparisonType = storm::logic::ComparisonType::LessEqual;
                                break;
                            case storm::logic::ComparisonType::Less:
                                objective.bound->comparisonType = storm::logic::ComparisonType::Greater;
                                break;
                            case storm::logic::ComparisonType::LessEqual:
                                objective.bound->comparisonType = storm::logic::ComparisonType::GreaterEqual;
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " has unexpected comparison type");
                        }
                    }
                    objective.optimizationDirection = storm::solver::invert(objective.optimizationDirection);
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& formula, PreprocessorData& data) {
                
                // Probabilities are between zero and one
                data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                data.objectives.back()->upperResultBound = storm::utility::one<ValueType>();
                
                if (formula.getSubformula().isUntilFormula()){
                    preprocessUntilFormula(formula.getSubformula().asUntilFormula(), data);
                } else if (formula.getSubformula().isBoundedUntilFormula()){
                    preprocessBoundedUntilFormula(formula.getSubformula().asBoundedUntilFormula(), data);
                } else if (formula.getSubformula().isGloballyFormula()){
                    preprocessGloballyFormula(formula.getSubformula().asGloballyFormula(), data);
                } else if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessRewardOperatorFormula(storm::logic::RewardOperatorFormula const& formula, PreprocessorData& data) {
                
                STORM_LOG_THROW((formula.hasRewardModelName() && data.originalModel.hasRewardModel(formula.getRewardModelName()))
                                || (!formula.hasRewardModelName() && data.originalModel.hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model is not unique or the formula " << formula << " does not specify an existing reward model.");
                
                std::string rewardModelName;
                if (formula.hasRewardModelName()) {
                    rewardModelName = formula.getRewardModelName();
                    STORM_LOG_THROW(data.originalModel.hasRewardModel(rewardModelName), storm::exceptions::InvalidPropertyException, "The reward model specified by formula " << formula << " does not exist in the model");
                } else {
                    STORM_LOG_THROW(data.originalModel.hasUniqueRewardModel(), storm::exceptions::InvalidOperationException, "The formula " << formula << " does not specify a reward model name and the reward model is not unique.");
                    rewardModelName = data.originalModel.getRewardModels().begin()->first;
                }
                
                data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                
                if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), data, rewardModelName);
                } else if (formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessCumulativeRewardFormula(formula.getSubformula().asCumulativeRewardFormula(), data, rewardModelName);
                } else if (formula.getSubformula().isTotalRewardFormula()) {
                    preprocessTotalRewardFormula(data, rewardModelName);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessTimeOperatorFormula(storm::logic::TimeOperatorFormula const& formula, PreprocessorData& data) {
                // Time formulas are only supported for Markov automata
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Time operator formulas are only supported for Markov automata.");
                
                data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                
                if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessUntilFormula(storm::logic::UntilFormula const& formula, PreprocessorData& data) {
                
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.originalModel);
                storm::storage::BitVector rightSubformulaResult = mc.check(formula.getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector leftSubformulaResult = mc.check(formula.getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                // Check if the formula is already satisfied in the initial state because then the transformation to expected rewards will fail.
                if (!data.objectives.back()->lowerTimeBound) {
                    if (!(data.originalModel.getInitialStates() & rightSubformulaResult).empty()) {
                        // TODO: Handle this case more properly
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The Probability for the objective " << *data.objectives.back()->originalFormula << " is always one as the rhs of the until formula is true in the initial state. This (trivial) case is currently not implemented.");
                    }
                }
                
                // Create a memory structure that stores whether a non-PhiState or a PsiState has already been reached
                storm::storage::MemoryStructureBuilder<ValueType, RewardModelType> builder(2, data.originalModel);
                std::string relevantStatesLabel = data.memoryLabelPrefix + "_obj" + std::to_string(data.objectives.size()) + "_relevant";
                builder.setLabel(0, relevantStatesLabel);
                storm::storage::BitVector nonRelevantStates = ~leftSubformulaResult | rightSubformulaResult;
                builder.setTransition(0, 0, ~nonRelevantStates);
                builder.setTransition(0, 1, nonRelevantStates);
                builder.setTransition(1, 1, storm::storage::BitVector(data.originalModel.getNumberOfStates(), true));
                for (auto const& initState : data.originalModel.getInitialStates()) {
                    builder.setInitialMemoryState(initState, nonRelevantStates.get(initState) ? 1 : 0);
                }
                storm::storage::MemoryStructure objectiveMemory = builder.build();
                data.memory = std::make_shared<storm::storage::MemoryStructure>(data.memory->product(objectiveMemory));
                
                data.objectives.back()->rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                
                auto relevantStatesFormula = std::make_shared<storm::logic::AtomicLabelFormula>(relevantStatesLabel);
                data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachProbToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula, formula.getRightSubformula().asSharedPointer()));
                
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, PreprocessorData& data) {
                STORM_LOG_THROW(!data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton) || !formula.isStepBounded(), storm::exceptions::InvalidPropertyException, "Multi-objective model checking currently does not support STEP-bounded properties for Markov automata.");
                
                if (formula.hasLowerBound()) {
                    STORM_LOG_THROW(!formula.getLowerBound().containsVariables(), storm::exceptions::InvalidPropertyException, "The lower time bound for the formula " << formula << " still contains variables");
                    if (!storm::utility::isZero(formula.getLowerBound<double>()) || formula.isLowerBoundStrict()) {
                        data.objectives.back()->lowerTimeBound = storm::logic::TimeBound(formula.isLowerBoundStrict(), formula.getLowerBound());
                    }
                }
                if (formula.hasUpperBound()) {
                    STORM_LOG_THROW(!formula.getUpperBound().containsVariables(), storm::exceptions::InvalidPropertyException, "The Upper time bound for the formula " << formula << " still contains variables");
                    if (!storm::utility::isInfinity(formula.getUpperBound<double>())) {
                        data.objectives.back()->upperTimeBound = storm::logic::TimeBound(formula.isUpperBoundStrict(), formula.getUpperBound());
                    }
                }
                preprocessUntilFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), data);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessGloballyFormula(storm::logic::GloballyFormula const& formula, PreprocessorData& data) {
                // The formula will be transformed to an until formula for the complementary event.
                data.objectives.back()->considersComplementaryEvent = true;
                
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                
                preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), data);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessEventuallyFormula(storm::logic::EventuallyFormula const& formula, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                if (formula.isReachabilityProbabilityFormula()){
                    preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), data);
                    return;
                }
                
                // Analyze the subformula
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.originalModel);
                storm::storage::BitVector subFormulaResult = mc.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                
                // Create a memory structure that stores whether a target state has already been reached
                storm::storage::MemoryStructureBuilder<ValueType, RewardModelType> builder(2, data.originalModel);
                // Get a unique label that is not already present in the model
                std::string relevantStatesLabel = data.memoryLabelPrefix + "_obj" + std::to_string(data.objectives.size()) + "_relevant";
                builder.setLabel(0, relevantStatesLabel);
                builder.setTransition(0, 0, ~subFormulaResult);
                builder.setTransition(0, 1, subFormulaResult);
                builder.setTransition(1, 1, storm::storage::BitVector(data.originalModel.getNumberOfStates(), true));
                for (auto const& initState : data.originalModel.getInitialStates()) {
                    builder.setInitialMemoryState(initState, subFormulaResult.get(initState) ? 1 : 0);
                }
                storm::storage::MemoryStructure objectiveMemory = builder.build();
                data.memory = std::make_shared<storm::storage::MemoryStructure>(data.memory->product(objectiveMemory));
                
                auto relevantStatesFormula = std::make_shared<storm::logic::AtomicLabelFormula>(relevantStatesLabel);
                
                data.objectives.back()->rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                
                if (formula.isReachabilityRewardFormula()) {
                    assert(optionalRewardModelName.is_initialized());
                    data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachRewToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula, optionalRewardModelName.get()));
                    data.finiteRewardCheckObjectives.set(data.objectives.size() - 1);
                } else if (formula.isReachabilityTimeFormula() && data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    data.finiteRewardCheckObjectives.set(data.objectives.size() - 1);
                    data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachTimeToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula));
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidPropertyException, "Cumulative reward formulas are not supported for the given model type.");
                
                STORM_LOG_THROW(!formula.getBound().containsVariables(), storm::exceptions::InvalidPropertyException, "The time bound for the formula " << formula << " still contains variables");
                if (!storm::utility::isInfinity(formula.getBound<double>())) {
                    data.objectives.back()->upperTimeBound = storm::logic::TimeBound(formula.isBoundStrict(), formula.getBound());
                }

                assert(optionalRewardModelName.is_initialized());
                data.objectives.back()->rewardModelName = *optionalRewardModelName;

            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessTotalRewardFormula(PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                assert(optionalRewardModelName.is_initialized());
                data.objectives.back()->rewardModelName = *optionalRewardModelName;
                data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
            }
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::buildResult(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula, PreprocessorData& data, std::shared_ptr<SparseModelType> const& preprocessedModel, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                        
                ReturnType result(originalFormula, originalModel);
                
                result.preprocessedModel = preprocessedModel;
                
                for (auto& obj : data.objectives) {
                    result.objectives.push_back(std::move(*obj));
                }
                
                result.queryType = getQueryType(result.objectives);
                
                auto minMaxNonZeroRewardStates = getStatesWithNonZeroRewardMinMax(result, backwardTransitions);
                auto finiteRewardStates = ensureRewardFiniteness(result, data.finiteRewardCheckObjectives, minMaxNonZeroRewardStates.first, backwardTransitions);
                
                std::set<std::string> relevantRewardModels;
                for (auto const& obj : result.objectives) {
                    relevantRewardModels.insert(*obj.rewardModelName);
                }
                
                // Build a subsystem that discards states that yield infinite reward for all schedulers.
                // We can also merge the states that will have reward zero anyway.
                storm::storage::BitVector zeroRewardStates = ~minMaxNonZeroRewardStates.second;
                storm::storage::BitVector maybeStates = finiteRewardStates & ~zeroRewardStates;
                storm::transformer::GoalStateMerger<SparseModelType> merger(*result.preprocessedModel);
                typename storm::transformer::GoalStateMerger<SparseModelType>::ReturnType mergerResult = merger.mergeTargetAndSinkStates(maybeStates, zeroRewardStates, storm::storage::BitVector(maybeStates.size(), false), std::vector<std::string>(relevantRewardModels.begin(), relevantRewardModels.end()));
                
                result.preprocessedModel = mergerResult.model;
                result.possibleBottomStates = (~minMaxNonZeroRewardStates.first) % maybeStates;
                if (mergerResult.targetState) {
                    storm::storage::BitVector targetStateAsVector(result.preprocessedModel->getNumberOfStates(), false);
                    targetStateAsVector.set(*mergerResult.targetState, true);
                    // The overapproximation for the possible ec choices consists of the states that can reach the target states with prob. 0 and the target state itself.
                    result.possibleECChoices = result.preprocessedModel->getTransitionMatrix().getRowFilter(storm::utility::graph::performProb0E(*result.preprocessedModel, result.preprocessedModel->getBackwardTransitions(), storm::storage::BitVector(targetStateAsVector.size(), true), targetStateAsVector));
                    result.possibleECChoices.set(result.preprocessedModel->getTransitionMatrix().getRowGroupIndices()[*mergerResult.targetState], true);
                    // There is an additional state in the result
                    result.possibleBottomStates.resize(result.possibleBottomStates.size() + 1, true);
                } else {
                    result.possibleECChoices = storm::storage::BitVector(result.preprocessedModel->getNumberOfChoices(), true);
                }
                assert(result.possibleBottomStates.size() == result.preprocessedModel->getNumberOfStates());
                
                return result;
            }
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType::QueryType SparseMultiObjectivePreprocessor<SparseModelType>::getQueryType(std::vector<Objective<ValueType>> const& objectives) {
                uint_fast64_t numOfObjectivesWithThreshold = 0;
                for (auto& obj : objectives) {
                    if (obj.bound) {
                        ++numOfObjectivesWithThreshold;
                    }
                }
                if (numOfObjectivesWithThreshold == objectives.size()) {
                    return ReturnType::QueryType::Achievability;
                } else if (numOfObjectivesWithThreshold + 1 == objectives.size()) {
                    // Note: We do not want to consider a Pareto query when the total number of objectives is one.
                    return ReturnType::QueryType::Quantitative;
                } else if (numOfObjectivesWithThreshold == 0) {
                    return ReturnType::QueryType::Pareto;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Invalid Multi-objective query: The numer of qualitative objectives should be either 0 (Pareto query), 1 (quantitative query), or #objectives (achievability query).");
                }
            }
            
            template<typename SparseModelType>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> SparseMultiObjectivePreprocessor<SparseModelType>::getStatesWithNonZeroRewardMinMax(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                
                uint_fast64_t stateCount = result.preprocessedModel->getNumberOfStates();
                auto const& transitions = result.preprocessedModel->getTransitionMatrix();
                std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                storm::storage::BitVector allStates(stateCount, true);

                // Get the choices that yield non-zero reward
                storm::storage::BitVector zeroRewardChoices(result.preprocessedModel->getNumberOfChoices(), true);
                for (auto const& obj : result.objectives) {
                    auto const& rewModel = result.preprocessedModel->getRewardModel(*obj.rewardModelName);
                    zeroRewardChoices &= rewModel.getChoicesWithZeroReward(transitions);
                }
                
                // Get the states that have reward for at least one (or for all) choices assigned to it.
                storm::storage::BitVector statesWithRewardForOneChoice = storm::storage::BitVector(stateCount, false);
                storm::storage::BitVector statesWithRewardForAllChoices = storm::storage::BitVector(stateCount, true);
                for (uint_fast64_t state = 0; state < stateCount; ++state) {
                    bool stateHasChoiceWithReward = false;
                    bool stateHasChoiceWithoutReward = false;
                    uint_fast64_t const& groupEnd = groupIndices[state + 1];
                    for (uint_fast64_t choice = groupIndices[state]; choice < groupEnd; ++choice) {
                        if (zeroRewardChoices.get(choice)) {
                            stateHasChoiceWithoutReward = true;
                        } else {
                            stateHasChoiceWithReward = true;
                        }
                    }
                    if (stateHasChoiceWithReward) {
                        statesWithRewardForOneChoice.set(state, true);
                    }
                    if (stateHasChoiceWithoutReward) {
                        statesWithRewardForAllChoices.set(state, false);
                    }
                }
                
                // get the states from which the minimal/maximal expected reward is always non-zero
                storm::storage::BitVector minStates = storm::utility::graph::performProbGreater0A(transitions, groupIndices, backwardTransitions, allStates, statesWithRewardForAllChoices, false, 0, zeroRewardChoices);
                storm::storage::BitVector maxStates = storm::utility::graph::performProbGreater0E(backwardTransitions, allStates, statesWithRewardForOneChoice);
                STORM_LOG_ASSERT(minStates.isSubsetOf(maxStates), "The computed set of states with minimal non-zero expected rewards is not a subset of the states with maximal non-zero rewards.");
                return std::make_pair(std::move(minStates), std::move(maxStates));
            }
         
            template<typename SparseModelType>
            storm::storage::BitVector SparseMultiObjectivePreprocessor<SparseModelType>::ensureRewardFiniteness(ReturnType& result, storm::storage::BitVector const& finiteRewardCheckObjectives, storm::storage::BitVector const& nonZeroRewardMin, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                
                auto const& transitions = result.preprocessedModel->getTransitionMatrix();
                std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                
                storm::storage::BitVector maxRewardsToCheck(result.preprocessedModel->getNumberOfChoices(), true);
                bool hasMinRewardToCheck = false;
                for (auto const& objIndex : finiteRewardCheckObjectives) {
                    auto const& rewModel = result.preprocessedModel->getRewardModel(result.objectives[objIndex].rewardModelName.get());
                    if (storm::solver::minimize(result.objectives[objIndex].optimizationDirection)) {
                        hasMinRewardToCheck = true;
                    } else {
                        maxRewardsToCheck &= rewModel.getChoicesWithZeroReward(transitions);
                    }
                }
                maxRewardsToCheck.complement();
                
                // Assert reward finitiness for maximizing objectives under all schedulers
                storm::storage::BitVector allStates(result.preprocessedModel->getNumberOfStates(), true);
                if (storm::utility::graph::checkIfECWithChoiceExists(transitions, backwardTransitions, allStates, maxRewardsToCheck)) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "At least one of the maximizing objectives induces infinite expected reward (or time). This is not supported");
                }
                    
                // Assert that there is one scheduler under which all rewards are finite.
                // This only has to be done if there are minimizing expected rewards that potentially can be infinite
                storm::storage::BitVector finiteRewardStates;
                if (hasMinRewardToCheck) {
                    finiteRewardStates = storm::utility::graph::performProb1E(transitions, groupIndices, backwardTransitions, allStates, ~nonZeroRewardMin);
                    if ((finiteRewardStates & result.preprocessedModel->getInitialStates()).empty()) {
                        // There is no scheduler that induces finite reward for the initial state
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "For every scheduler, at least one objective gets infinite reward.");
                    }
                } else {
                    finiteRewardStates = allStates;
                }
                return finiteRewardStates;
            }
        
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<double>>;
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
        }
    }
}
