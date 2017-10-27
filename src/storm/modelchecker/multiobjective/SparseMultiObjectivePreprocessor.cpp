#include "storm/modelchecker/multiobjective/SparseMultiObjectivePreprocessor.h"

#include <algorithm>
#include <set>

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"


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
                    data.upperResultBoundObjectives.resize(data.objectives.size(), false);
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
                
                // Remove reward models that are not needed anymore
                std::set<std::string> relevantRewardModels;
                for (auto const& obj : data.objectives) {
                    obj->formula->gatherReferencedRewardModels(relevantRewardModels);
                }
                preprocessedModel->restrictRewardModels(relevantRewardModels);
                
                // Build the actual result
                return buildResult(originalModel, originalFormula, data, preprocessedModel, backwardTransitions);
            }
            
            template <typename SparseModelType>
            SparseMultiObjectivePreprocessor<SparseModelType>::PreprocessorData::PreprocessorData(SparseModelType const& model) : originalModel(model) {
                memory = std::make_shared<storm::storage::MemoryStructure>(storm::storage::MemoryStructureBuilder<ValueType, RewardModelType>::buildTrivialMemoryStructure(model));
                
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
                
                // Check whether the complementary event is considered
                objective.considersComplementaryEvent = formula.isProbabilityOperatorFormula() && formula.getSubformula().isGloballyFormula();
                
                storm::logic::OperatorInformation opInfo;
                if (formula.hasBound()) {
                    opInfo.bound = formula.getBound();
                    // Invert the bound (if necessary)
                    if (objective.considersComplementaryEvent) {
                        opInfo.bound->threshold = opInfo.bound->threshold.getManager().rational(storm::utility::one<storm::RationalNumber>()) - opInfo.bound->threshold;
                        switch (opInfo.bound->comparisonType) {
                            case storm::logic::ComparisonType::Greater:
                                opInfo.bound->comparisonType = storm::logic::ComparisonType::Less;
                                break;
                            case storm::logic::ComparisonType::GreaterEqual:
                                opInfo.bound->comparisonType = storm::logic::ComparisonType::LessEqual;
                                break;
                            case storm::logic::ComparisonType::Less:
                                opInfo.bound->comparisonType = storm::logic::ComparisonType::Greater;
                                break;
                            case storm::logic::ComparisonType::LessEqual:
                                opInfo.bound->comparisonType = storm::logic::ComparisonType::GreaterEqual;
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Current objective " << formula << " has unexpected comparison type");
                        }
                    }
                    if (storm::logic::isLowerBound(opInfo.bound->comparisonType)) {
                        opInfo.optimalityType = storm::solver::OptimizationDirection::Maximize;
                    } else {
                        opInfo.optimalityType = storm::solver::OptimizationDirection::Minimize;
                    }
                    STORM_LOG_WARN_COND(!formula.hasOptimalityType(), "Optimization direction of formula " << formula << " ignored as the formula also specifies a threshold.");
                } else if (formula.hasOptimalityType()){
                    opInfo.optimalityType = formula.getOptimalityType();
                    // Invert the optimality type (if necessary)
                    if (objective.considersComplementaryEvent) {
                        opInfo.optimalityType = storm::solver::invert(opInfo.optimalityType.get());
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Objective " << formula << " does not specify whether to minimize or maximize");
                }
                
                if (formula.isProbabilityOperatorFormula()){
                    preprocessProbabilityOperatorFormula(formula.asProbabilityOperatorFormula(), opInfo, data);
                } else if (formula.isRewardOperatorFormula()){
                    preprocessRewardOperatorFormula(formula.asRewardOperatorFormula(), opInfo, data);
                } else if (formula.isTimeOperatorFormula()){
                    preprocessTimeOperatorFormula(formula.asTimeOperatorFormula(), opInfo, data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not preprocess the objective " << formula << " because it is not supported");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                
                // Probabilities are between zero and one
                data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                data.objectives.back()->upperResultBound = storm::utility::one<ValueType>();
                
                if (formula.getSubformula().isUntilFormula()){
                    preprocessUntilFormula(formula.getSubformula().asUntilFormula(), opInfo, data);
                } else if (formula.getSubformula().isBoundedUntilFormula()){
                    preprocessBoundedUntilFormula(formula.getSubformula().asBoundedUntilFormula(), opInfo, data);
                } else if (formula.getSubformula().isGloballyFormula()){
                    preprocessGloballyFormula(formula.getSubformula().asGloballyFormula(), opInfo, data);
                } else if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), opInfo, data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessRewardOperatorFormula(storm::logic::RewardOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                
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
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), opInfo, data, rewardModelName);
                } else if (formula.getSubformula().isCumulativeRewardFormula()) {
                    preprocessCumulativeRewardFormula(formula.getSubformula().asCumulativeRewardFormula(), opInfo, data, rewardModelName);
                } else if (formula.getSubformula().isTotalRewardFormula()) {
                    preprocessTotalRewardFormula(opInfo, data, rewardModelName);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }

            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessTimeOperatorFormula(storm::logic::TimeOperatorFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                // Time formulas are only supported for Markov automata
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Time operator formulas are only supported for Markov automata.");
                
                data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                
                if (formula.getSubformula().isEventuallyFormula()){
                    preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), opInfo, data);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessUntilFormula(storm::logic::UntilFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, std::shared_ptr<storm::logic::Formula const> subformula) {
                
                storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(data.originalModel);
                storm::storage::BitVector rightSubformulaResult = mc.check(formula.getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                storm::storage::BitVector leftSubformulaResult = mc.check(formula.getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                // Check if the formula is already satisfied in the initial state because then the transformation to expected rewards will fail.
                // TODO: Handle this case more properly
                STORM_LOG_THROW((data.originalModel.getInitialStates() & rightSubformulaResult).empty(), storm::exceptions::NotImplementedException, "The Probability for the objective " << *data.objectives.back()->originalFormula << " is always one as the rhs of the until formula is true in the initial state. This (trivial) case is currently not implemented.");
                
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
                
                std::string rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                if (subformula == nullptr) {
                    subformula = std::make_shared<storm::logic::TotalRewardFormula>();
                }
                data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(subformula, rewardModelName, opInfo);
                
                auto relevantStatesFormula = std::make_shared<storm::logic::AtomicLabelFormula>(relevantStatesLabel);
                data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachProbToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula, formula.getRightSubformula().asSharedPointer()));
                
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                
                // Check how to handle this query
                if (formula.isMultiDimensional() || formula.getTimeBoundReference().isRewardBound()) {
                    data.objectives.back()->formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(formula.asSharedPointer(), opInfo);
                } else if (!formula.hasLowerBound() || (!formula.isLowerBoundStrict() && storm::utility::isZero(formula.template getLowerBound<storm::RationalNumber>()))) {
                    std::shared_ptr<storm::logic::Formula const> subformula;
                    if (!formula.hasUpperBound()) {
                        // The formula is actually unbounded
                        subformula = std::make_shared<storm::logic::TotalRewardFormula>();
                    } else {
                        STORM_LOG_THROW(!data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton) || formula.getTimeBoundReference().isTimeBound(), storm::exceptions::InvalidPropertyException, "Bounded until formulas for Markov Automata are only allowed when time bounds are considered.");
                        storm::logic::TimeBound bound(formula.isUpperBoundStrict(), formula.getUpperBound());
                        subformula = std::make_shared<storm::logic::CumulativeRewardFormula>(bound, formula.getTimeBoundReference());
                    }
                    preprocessUntilFormula(storm::logic::UntilFormula(formula.getLeftSubformula().asSharedPointer(), formula.getRightSubformula().asSharedPointer()), opInfo, data, subformula);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Property " << formula << "is not supported");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessGloballyFormula(storm::logic::GloballyFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                // The formula is transformed to an until formula for the complementary event.
                auto negatedSubformula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, formula.getSubformula().asSharedPointer());
                
                preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), negatedSubformula), opInfo, data);
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessEventuallyFormula(storm::logic::EventuallyFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                if (formula.isReachabilityProbabilityFormula()){
                    preprocessUntilFormula(storm::logic::UntilFormula(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), opInfo, data);
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
                
                std::string auxRewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                auto totalRewardFormula = std::make_shared<storm::logic::TotalRewardFormula>();
                data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(totalRewardFormula, auxRewardModelName, opInfo);
                data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                data.upperResultBoundObjectives.set(data.objectives.size() - 1, true);
                
                if (formula.isReachabilityRewardFormula()) {
                    assert(optionalRewardModelName.is_initialized());
                    data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachRewToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula, optionalRewardModelName.get()));
                } else if (formula.isReachabilityTimeFormula() && data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    data.tasks.push_back(std::make_shared<SparseMultiObjectivePreprocessorReachTimeToTotalRewTask<SparseModelType>>(data.objectives.back(), relevantStatesFormula));
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (data.originalModel.isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                STORM_LOG_THROW(data.originalModel.isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidPropertyException, "Cumulative reward formulas are not supported for the given model type.");
                
                storm::logic::TimeBound bound(formula.isBoundStrict(), formula.getBound());
                storm::logic::TimeBoundReference tbr(formula.getTimeBoundReference());
                auto cumulativeRewardFormula = std::make_shared<storm::logic::CumulativeRewardFormula>(bound, tbr);
                data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(cumulativeRewardFormula, *optionalRewardModelName, opInfo);
                data.upperResultBoundObjectives.set(data.objectives.size() - 1, true);
                if (tbr.isRewardBound()) {
                    data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                }
            }
            
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessTotalRewardFormula(storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                
                auto totalRewardFormula = std::make_shared<storm::logic::TotalRewardFormula>();
                data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(totalRewardFormula, *optionalRewardModelName, opInfo);
                data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                data.upperResultBoundObjectives.set(data.objectives.size() - 1, true);
            }
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::buildResult(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula, PreprocessorData& data, std::shared_ptr<SparseModelType> const& preprocessedModel, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                        
                ReturnType result(originalFormula, originalModel);
                
                result.preprocessedModel = preprocessedModel;
                
                for (auto& obj : data.objectives) {
                    result.objectives.push_back(std::move(*obj));
                }
                
                result.queryType = getQueryType(result.objectives);
                
                setReward0States(result, backwardTransitions);
                checkRewardFiniteness(result, data.finiteRewardCheckObjectives, backwardTransitions);
                
                // We compute upper result bounds if the 'sound' option has been enabled
                if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet()) {
                    for (auto const& objIndex : data.upperResultBoundObjectives) {
                        result.objectives[objIndex].upperResultBound = computeUpperResultBound(result, objIndex, backwardTransitions);
                    }
                }
                
                return result;
            }
            
            template<typename SparseModelType>
            typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType::QueryType SparseMultiObjectivePreprocessor<SparseModelType>::getQueryType(std::vector<Objective<ValueType>> const& objectives) {
                uint_fast64_t numOfObjectivesWithThreshold = 0;
                for (auto& obj : objectives) {
                    if (obj.formula->hasBound()) {
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
            void SparseMultiObjectivePreprocessor<SparseModelType>::setReward0States(ReturnType& result, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                
                uint_fast64_t stateCount = result.preprocessedModel->getNumberOfStates();
                auto const& transitions = result.preprocessedModel->getTransitionMatrix();
                std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                storm::storage::BitVector allStates(stateCount, true);

                // Get the choices that yield non-zero reward
                storm::storage::BitVector zeroRewardChoices(result.preprocessedModel->getNumberOfChoices(), true);
                for (auto const& obj : result.objectives) {
                    if (obj.formula->isRewardOperatorFormula()) {
                        auto const& rewModel = result.preprocessedModel->getRewardModel(obj.formula->asRewardOperatorFormula().getRewardModelName());
                        zeroRewardChoices &= rewModel.getChoicesWithZeroReward(transitions);
                    }
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
                
                // get the states for which there is a scheduler yielding reward zero
                result.reward0EStates = storm::utility::graph::performProbGreater0A(transitions, groupIndices, backwardTransitions, allStates, statesWithRewardForAllChoices, false, 0, zeroRewardChoices);
                result.reward0EStates.complement();
                result.reward0AStates = storm::utility::graph::performProb0A(backwardTransitions, allStates, statesWithRewardForOneChoice);
                assert(result.reward0AStates.isSubsetOf(result.reward0EStates));
            }
         
            template<typename SparseModelType>
            void SparseMultiObjectivePreprocessor<SparseModelType>::checkRewardFiniteness(ReturnType& result, storm::storage::BitVector const& finiteRewardCheckObjectives, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                
                result.rewardFinitenessType = ReturnType::RewardFinitenessType::AllFinite;
                
                auto const& transitions = result.preprocessedModel->getTransitionMatrix();
                std::vector<uint_fast64_t> const& groupIndices = transitions.getRowGroupIndices();
                
                storm::storage::BitVector maxRewardsToCheck(result.preprocessedModel->getNumberOfChoices(), true);
                storm::storage::BitVector minRewardsToCheck(result.preprocessedModel->getNumberOfChoices(), true);
                for (auto const& objIndex : finiteRewardCheckObjectives) {
                    STORM_LOG_ASSERT(result.objectives[objIndex].formula->isRewardOperatorFormula(), "Objective needs to be checked for finite reward but has no reward operator.");
                    auto const& rewModel = result.preprocessedModel->getRewardModel(result.objectives[objIndex].formula->asRewardOperatorFormula().getRewardModelName());
                    auto unrelevantChoices = rewModel.getChoicesWithZeroReward(transitions);
                    // For (upper) reward bounded cumulative reward formulas, we do not need to consider the choices where boundReward is collected.
                    if (result.objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                        auto const& timeBoundReference = result.objectives[objIndex].formula->getSubformula().asCumulativeRewardFormula().getTimeBoundReference();
                        // Only reward bounded formulas need a finiteness check
                        assert(timeBoundReference.isRewardBound());
                        auto const& rewModelOfBound = result.preprocessedModel->getRewardModel(timeBoundReference.getRewardName());
                        unrelevantChoices |= ~rewModelOfBound.getChoicesWithZeroReward(transitions);
                    }
                    if (storm::solver::minimize(result.objectives[objIndex].formula->getOptimalityType())) {
                        minRewardsToCheck &= unrelevantChoices;
                    } else {
                        maxRewardsToCheck &= unrelevantChoices;
                    }
                }
                maxRewardsToCheck.complement();
                minRewardsToCheck.complement();
                
                // Check reward finiteness under all schedulers
                storm::storage::BitVector allStates(result.preprocessedModel->getNumberOfStates(), true);
                if (storm::utility::graph::checkIfECWithChoiceExists(transitions, backwardTransitions, allStates, maxRewardsToCheck | minRewardsToCheck)) {
                    // Check whether there is a scheduler yielding infinite reward for a maximizing objective
                    if (storm::utility::graph::checkIfECWithChoiceExists(transitions, backwardTransitions, allStates, maxRewardsToCheck)) {
                        result.rewardFinitenessType = ReturnType::RewardFinitenessType::Infinite;
                    } else {
                        // Check whether there is a scheduler under which all rewards are finite.
                        result.rewardLessInfinityEStates = storm::utility::graph::performProb1E(transitions, groupIndices, backwardTransitions, allStates, result.reward0EStates);
                        if ((result.rewardLessInfinityEStates.get() & result.preprocessedModel->getInitialStates()).empty()) {
                            // There is no scheduler that induces finite reward for the initial state
                            result.rewardFinitenessType = ReturnType::RewardFinitenessType::Infinite;
                        } else {
                            result.rewardFinitenessType = ReturnType::RewardFinitenessType::ExistsParetoFinite;
                        }
                    }
                } else {
                    result.rewardLessInfinityEStates = allStates;
                }
                
            }
        
            template<typename SparseModelType>
            boost::optional<typename SparseModelType::ValueType> SparseMultiObjectivePreprocessor<SparseModelType>::computeUpperResultBound(ReturnType const& result, uint64_t objIndex, storm::storage::SparseMatrix<ValueType> const& backwardTransitions) {
                boost::optional<ValueType> upperBound;
                
                if (!result.originalModel.isOfType(storm::models::ModelType::Mdp)) {
                    return upperBound;
                }
                
                auto const& transitions = result.preprocessedModel->getTransitionMatrix();
                
                if (result.objectives[objIndex].formula->isRewardOperatorFormula()) {
                    auto const& rewModel = result.preprocessedModel->getRewardModel(result.objectives[objIndex].formula->asRewardOperatorFormula().getRewardModelName());
                    auto actionRewards = rewModel.getTotalRewardVector(transitions);
                    
                    if (result.objectives[objIndex].formula->getSubformula().isTotalRewardFormula() || result.objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                        // We have to eliminate ECs here to treat zero-reward ECs
 
                        storm::storage::BitVector allStates(result.preprocessedModel->getNumberOfStates(), true);
                        
                        // Get the set of states from which no reward is reachable
                        auto nonZeroRewardStates = rewModel.getStatesWithZeroReward(transitions);
                        nonZeroRewardStates.complement();
                        auto expRewGreater0EStates = storm::utility::graph::performProbGreater0E(backwardTransitions, allStates, nonZeroRewardStates);
                        
                        auto zeroRewardChoices = rewModel.getChoicesWithZeroReward(transitions);
                        
                        auto ecElimRes = storm::transformer::EndComponentEliminator<ValueType>::transform(transitions, expRewGreater0EStates, zeroRewardChoices, ~allStates);
                        
                        allStates.resize(ecElimRes.matrix.getRowGroupCount());
                        storm::storage::BitVector outStates(allStates.size(), false);
                        std::vector<ValueType> rew0StateProbs;
                        rew0StateProbs.reserve(ecElimRes.matrix.getRowCount());
                        for (uint64_t state = 0; state < allStates.size(); ++ state) {
                            for (uint64_t choice = ecElimRes.matrix.getRowGroupIndices()[state]; choice < ecElimRes.matrix.getRowGroupIndices()[state + 1]; ++choice) {
                                // Check whether the choice lead to a state with expRew 0 in the original model
                                bool isOutChoice = false;
                                uint64_t originalModelChoice = ecElimRes.newToOldRowMapping[choice];
                                for (auto const& entry : transitions.getRow(originalModelChoice)) {
                                    if (!expRewGreater0EStates.get(entry.getColumn())) {
                                        isOutChoice = true;
                                        outStates.set(state, true);
                                        rew0StateProbs.push_back(storm::utility::one<ValueType>() - ecElimRes.matrix.getRowSum(choice));
                                        assert (!storm::utility::isZero(rew0StateProbs.back()));
                                        break;
                                    }
                                }
                                if (!isOutChoice) {
                                    rew0StateProbs.push_back(storm::utility::zero<ValueType>());
                                }
                            }
                        }
                        
                        // An upper reward bound can only be computed if it is below infinity
                        if (storm::utility::graph::performProb1A(ecElimRes.matrix, ecElimRes.matrix.getRowGroupIndices(), ecElimRes.matrix.transpose(true), allStates, outStates).full()) {
                            
                            std::vector<ValueType> rewards;
                            rewards.reserve(ecElimRes.matrix.getRowCount());
                            for (auto row : ecElimRes.newToOldRowMapping) {
                                rewards.push_back(actionRewards[row]);
                            }
                            
                            storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ValueType> baier(ecElimRes.matrix, rewards, rew0StateProbs);
                            if (upperBound) {
                                upperBound = std::min(upperBound.get(), baier.computeUpperBound());
                            } else {
                                upperBound = baier.computeUpperBound();
                            }
                        }
                    }
                }
                
                if (upperBound) {
                    STORM_LOG_INFO("Computed upper result bound " << upperBound.get() << " for objective " << *result.objectives[objIndex].formula << ".");
                } else {
                    STORM_LOG_WARN("Could not compute upper result bound for objective " << *result.objectives[objIndex].formula);
                }
                return upperBound;

            }

            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<double>>;
            
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<storm::RationalNumber>>;
            template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
        }
    }
}
