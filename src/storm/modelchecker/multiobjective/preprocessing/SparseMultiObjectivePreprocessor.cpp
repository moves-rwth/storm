#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessor.h"

#include <algorithm>
#include <set>

#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/transformer/MemoryIncorporation.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
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
            namespace preprocessing {
                
                template<typename SparseModelType>
                typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::preprocess(Environment const& env, SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula) {
                    
                    std::shared_ptr<SparseModelType> model;
                    
                    // Incorporate the necessary memory
                    if (env.modelchecker().multi().isSchedulerRestrictionSet()) {
                        auto const& schedRestr = env.modelchecker().multi().getSchedulerRestriction();
                        if (schedRestr.getMemoryPattern() == storm::storage::SchedulerClass::MemoryPattern::GoalMemory) {
                            model = storm::transformer::MemoryIncorporation<SparseModelType>::incorporateGoalMemory(originalModel, originalFormula.getSubformulas());
                        } else if (schedRestr.getMemoryPattern() == storm::storage::SchedulerClass::MemoryPattern::Arbitrary && schedRestr.getMemoryStates() > 1) {
                            model = storm::transformer::MemoryIncorporation<SparseModelType>::incorporateFullMemory(originalModel, schedRestr.getMemoryStates());
                        } else if (schedRestr.isPositional()) {
                            model = std::make_shared<SparseModelType>(originalModel);
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The given scheduler restriction has not been implemented.");
                        }
                    } else {
                         model = storm::transformer::MemoryIncorporation<SparseModelType>::incorporateGoalMemory(originalModel, originalFormula.getSubformulas());
                    }
                    
                    PreprocessorData data(model);
                    
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
                    
                    // Remove reward models that are not needed anymore
                    std::set<std::string> relevantRewardModels;
                    for (auto const& obj : data.objectives) {
                        obj->formula->gatherReferencedRewardModels(relevantRewardModels);
                    }
                    data.model->restrictRewardModels(relevantRewardModels);
                    
                    // Build the actual result
                    return buildResult(originalModel, originalFormula, data);
                }
                
                template <typename SparseModelType>
                SparseMultiObjectivePreprocessor<SparseModelType>::PreprocessorData::PreprocessorData(std::shared_ptr<SparseModelType> model) : model(model) {
                    
                    // The rewardModelNamePrefix should not be a prefix of a reward model name of the given model to ensure uniqueness of new reward model names
                    rewardModelNamePrefix = "obj";
                    while (true) {
                        bool prefixIsUnique = true;
                        for (auto const& rewardModels : model->getRewardModels()) {
                            if (rewardModelNamePrefix.size() <= rewardModels.first.size()) {
                                if (std::mismatch(rewardModelNamePrefix.begin(), rewardModelNamePrefix.end(), rewardModels.first.begin()).first == rewardModelNamePrefix.end()) {
                                    prefixIsUnique = false;
                                    rewardModelNamePrefix = "_" + rewardModelNamePrefix;
                                    break;
                                }
                            }
                        }
                        if (prefixIsUnique) {
                            break;
                        }
                    }
                }
                
                
                storm::logic::OperatorInformation getOperatorInformation(storm::logic::OperatorFormula const& formula, bool considerComplementaryEvent) {
                    storm::logic::OperatorInformation opInfo;
                    if (formula.hasBound()) {
                        opInfo.bound = formula.getBound();
                        // Invert the bound (if necessary)
                        if (considerComplementaryEvent) {
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
                        if (considerComplementaryEvent) {
                            opInfo.optimalityType = storm::solver::invert(opInfo.optimalityType.get());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Objective " << formula << " does not specify whether to minimize or maximize");
                    }
                    return opInfo;
                }
                
                
                template<typename SparseModelType>
                void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessOperatorFormula(storm::logic::OperatorFormula const& formula, PreprocessorData& data) {
                    
                    Objective<ValueType>& objective = *data.objectives.back();
                    
                    // Check whether the complementary event is considered
                    objective.considersComplementaryEvent = formula.isProbabilityOperatorFormula() && formula.getSubformula().isGloballyFormula();
                    
                    // Extract the operator information from the formula and potentially invert it for the complementary event
                    storm::logic::OperatorInformation opInfo = getOperatorInformation(formula, objective.considersComplementaryEvent);

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
                    
                    STORM_LOG_THROW((formula.hasRewardModelName() && data.model->hasRewardModel(formula.getRewardModelName()))
                                    || (!formula.hasRewardModelName() && data.model->hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model is not unique or the formula " << formula << " does not specify an existing reward model.");
                    
                    std::string rewardModelName;
                    if (formula.hasRewardModelName()) {
                        rewardModelName = formula.getRewardModelName();
                        STORM_LOG_THROW(data.model->hasRewardModel(rewardModelName), storm::exceptions::InvalidPropertyException, "The reward model specified by formula " << formula << " does not exist in the model");
                    } else {
                        STORM_LOG_THROW(data.model->hasUniqueRewardModel(), storm::exceptions::InvalidOperationException, "The formula " << formula << " does not specify a reward model name and the reward model is not unique.");
                        rewardModelName = data.model->getRewardModels().begin()->first;
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
                    STORM_LOG_THROW(data.model->isOfType(storm::models::ModelType::MarkovAutomaton), storm::exceptions::InvalidPropertyException, "Time operator formulas are only supported for Markov automata.");
                    
                    data.objectives.back()->lowerResultBound = storm::utility::zero<ValueType>();
                    
                    if (formula.getSubformula().isEventuallyFormula()){
                        preprocessEventuallyFormula(formula.getSubformula().asEventuallyFormula(), opInfo, data);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The subformula of " << formula << " is not supported.");
                    }
                }
                
                template<typename SparseModelType>
                void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessUntilFormula(storm::logic::UntilFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, std::shared_ptr<storm::logic::Formula const> subformula) {
                    
                    // Try to transform the formula to expected total (or cumulative) rewards
                    
                    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(*data.model);
                    storm::storage::BitVector rightSubformulaResult = mc.check(formula.getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    // Check if the formula is already satisfied in the initial state because then the transformation to expected rewards will fail.
                    // TODO: Handle this case more properly
                    STORM_LOG_THROW((data.model->getInitialStates() & rightSubformulaResult).empty(), storm::exceptions::NotImplementedException, "The Probability for the objective " << *data.objectives.back()->originalFormula << " is always one as the rhs of the until formula is true in the initial state. This (trivial) case is currently not implemented.");
                    
                    // Whenever a state that violates the left subformula or satisfies the right subformula is reached, the objective is 'decided', i.e., no more reward should be collected from there
                    storm::storage::BitVector notLeftOrRight = mc.check(formula.getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    notLeftOrRight.complement();
                    notLeftOrRight |=rightSubformulaResult;
                    
                    // Get the states that are reachable from a notLeftOrRight state
                    storm::storage::BitVector allStates(data.model->getNumberOfStates(), true), noStates(data.model->getNumberOfStates(), false);
                    storm::storage::BitVector reachableFromGoal = storm::utility::graph::getReachableStates(data.model->getTransitionMatrix(), notLeftOrRight, allStates, noStates);
                    // Get the states that are reachable from an initial state, stopping at the states reachable from goal
                    storm::storage::BitVector reachableFromInit = storm::utility::graph::getReachableStates(data.model->getTransitionMatrix(), data.model->getInitialStates(), ~notLeftOrRight, reachableFromGoal);
                    // Exclude the actual notLeftOrRight states from the states that are reachable from init
                    reachableFromInit &= ~notLeftOrRight;
                    // If we can reach a state that is reachable from goal, but which is not a goal state, it means that the transformation to expected rewards is not possible.
                    if ((reachableFromInit & reachableFromGoal).empty()) {
                        STORM_LOG_INFO("Objective " << *data.objectives.back()->originalFormula << " is transformed to an expected total/cumulative reward property.");
                        // Transform to expected total rewards:
                        // build stateAction reward vector that gives (one*transitionProbability) reward whenever a transition leads from a reachableFromInit state to a goalState
                        std::vector<typename SparseModelType::ValueType> objectiveRewards(data.model->getTransitionMatrix().getRowCount(), storm::utility::zero<typename SparseModelType::ValueType>());
                        for (auto const& state : reachableFromInit) {
                            for (uint_fast64_t row = data.model->getTransitionMatrix().getRowGroupIndices()[state]; row < data.model->getTransitionMatrix().getRowGroupIndices()[state + 1]; ++row) {
                                objectiveRewards[row] = data.model->getTransitionMatrix().getConstrainedRowSum(row, rightSubformulaResult);
                            }
                        }
                        std::string rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                        data.model->addRewardModel(rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(objectiveRewards)));
                        if (subformula == nullptr) {
                            subformula = std::make_shared<storm::logic::TotalRewardFormula>();
                        }
                        data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(subformula, rewardModelName, opInfo);
                    } else {
                        STORM_LOG_INFO("Objective " << *data.objectives.back()->originalFormula << " can not be transformed to an expected total/cumulative reward property.");
                        data.objectives.back()->formula =  std::make_shared<storm::logic::ProbabilityOperatorFormula>(formula.asSharedPointer(), opInfo);
                    }
                }
                
                template<typename SparseModelType>
                void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessBoundedUntilFormula(storm::logic::BoundedUntilFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data) {
                    
                    // Check how to handle this query
                    if (formula.isMultiDimensional() || formula.getTimeBoundReference().isRewardBound()) {
                        STORM_LOG_INFO("Objective " << data.objectives.back()->originalFormula << " is not transformed to an expected cumulative reward property.");
                        data.objectives.back()->formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(formula.asSharedPointer(), opInfo);
                    } else if (!formula.hasLowerBound() || (!formula.isLowerBoundStrict() && storm::utility::isZero(formula.template getLowerBound<storm::RationalNumber>()))) {
                        std::shared_ptr<storm::logic::Formula const> subformula;
                        if (!formula.hasUpperBound()) {
                            // The formula is actually unbounded
                            subformula = std::make_shared<storm::logic::TotalRewardFormula>();
                        } else {
                            STORM_LOG_THROW(!data.model->isOfType(storm::models::ModelType::MarkovAutomaton) || formula.getTimeBoundReference().isTimeBound(), storm::exceptions::InvalidPropertyException, "Bounded until formulas for Markov Automata are only allowed when time bounds are considered.");
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
                        preprocessUntilFormula(*std::make_shared<storm::logic::UntilFormula>(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asSharedPointer()), opInfo, data);
                        return;
                    }
                    
                    // Analyze the subformula
                    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> mc(*data.model);
                    storm::storage::BitVector subFormulaResult = mc.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    
                    
                    // Get the states that are reachable from a goal state
                    storm::storage::BitVector allStates(data.model->getNumberOfStates(), true), noStates(data.model->getNumberOfStates(), false);
                    storm::storage::BitVector reachableFromGoal = storm::utility::graph::getReachableStates(data.model->getTransitionMatrix(), subFormulaResult, allStates, noStates);
                    // Get the states that are reachable from an initial state, stopping at the states reachable from goal
                    storm::storage::BitVector reachableFromInit = storm::utility::graph::getReachableStates(data.model->getTransitionMatrix(), data.model->getInitialStates(), allStates, reachableFromGoal);
                    // Exclude the actual goal states from the states that are reachable from an initial state
                    reachableFromInit &= ~subFormulaResult;
                    // If we can reach a state that is reachable from goal but which is not a goal state, it means that the transformation to expected total rewards is not possible.
                    if ((reachableFromInit & reachableFromGoal).empty()) {
                        STORM_LOG_INFO("Objective " << *data.objectives.back()->originalFormula << " is transformed to an expected total reward property.");
                        // Transform to expected total rewards:
                        
                        std::string rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                        auto totalRewardFormula = std::make_shared<storm::logic::TotalRewardFormula>();
                        data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(totalRewardFormula, rewardModelName, opInfo);
                        
                        if (formula.isReachabilityRewardFormula()) {
                            // build stateAction reward vector that only gives reward for states that are reachable from init
                            assert(optionalRewardModelName.is_initialized());
                            typename SparseModelType::RewardModelType objectiveRewards = data.model->getRewardModel(optionalRewardModelName.get());
                            objectiveRewards.reduceToStateBasedRewards(data.model->getTransitionMatrix(), false);
                            if (objectiveRewards.hasStateRewards()) {
                                storm::utility::vector::setVectorValues(objectiveRewards.getStateRewardVector(), reachableFromGoal, storm::utility::zero<typename SparseModelType::ValueType>());
                            }
                            if (objectiveRewards.hasStateActionRewards()) {
                                for (auto state : reachableFromGoal) {
                                    std::fill_n(objectiveRewards.getStateActionRewardVector().begin() + data.model->getTransitionMatrix().getRowGroupIndices()[state], data.model->getTransitionMatrix().getRowGroupSize(state), storm::utility::zero<typename SparseModelType::ValueType>());
                                }
                            }
                            data.model->addRewardModel(rewardModelName, std::move(objectiveRewards));
                        } else if (formula.isReachabilityTimeFormula() && data.model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                            
                            // build stateAction reward vector that only gives reward for relevant states
                            std::vector<typename SparseModelType::ValueType> timeRewards(data.model->getNumberOfStates(), storm::utility::zero<typename SparseModelType::ValueType>());
                            storm::utility::vector::setVectorValues(timeRewards, dynamic_cast<storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType> const&>(*data.model).getMarkovianStates() & reachableFromInit, storm::utility::one<typename SparseModelType::ValueType>());
                            data.model->addRewardModel(rewardModelName, typename SparseModelType::RewardModelType(std::move(timeRewards)));
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (data.model->isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                        }
                    } else {
                        STORM_LOG_INFO("Objective " << *data.objectives.back()->originalFormula << " can not be transformed to an expected total/cumulative reward property.");
                        if (formula.isReachabilityRewardFormula()) {
                            assert(optionalRewardModelName.is_initialized());
                            data.objectives.back()->formula =  std::make_shared<storm::logic::RewardOperatorFormula>(formula.asSharedPointer(), optionalRewardModelName.get(), opInfo);
                        } else if (formula.isReachabilityTimeFormula() && data.model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                            // Reduce to reachability rewards so that time formulas do not have to be treated seperately later.
                            std::string rewardModelName = data.rewardModelNamePrefix + std::to_string(data.objectives.size());
                            auto newsubformula = std::make_shared<storm::logic::EventuallyFormula>(formula.getSubformula().asSharedPointer(), storm::logic::FormulaContext::Reward);
                            data.objectives.back()->formula =  std::make_shared<storm::logic::RewardOperatorFormula>(newsubformula, rewardModelName, opInfo);
                            std::vector<typename SparseModelType::ValueType> timeRewards(data.model->getNumberOfStates(), storm::utility::zero<typename SparseModelType::ValueType>());
                            storm::utility::vector::setVectorValues(timeRewards, dynamic_cast<storm::models::sparse::MarkovAutomaton<typename SparseModelType::ValueType> const&>(*data.model).getMarkovianStates(), storm::utility::one<typename SparseModelType::ValueType>());
                            data.model->addRewardModel(rewardModelName, typename SparseModelType::RewardModelType(std::move(timeRewards)));
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The formula " << formula << " neither considers reachability probabilities nor reachability rewards " << (data.model->isOfType(storm::models::ModelType::MarkovAutomaton) ?  "nor reachability time" : "") << ". This is not supported.");
                        }
                    }
                    data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                }
                
                template<typename SparseModelType>
                void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessCumulativeRewardFormula(storm::logic::CumulativeRewardFormula const& formula, storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                    STORM_LOG_THROW(data.model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidPropertyException, "Cumulative reward formulas are not supported for the given model type.");
                    
                    auto cumulativeRewardFormula = std::make_shared<storm::logic::CumulativeRewardFormula>(formula);
                    data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(cumulativeRewardFormula, *optionalRewardModelName, opInfo);
                    bool onlyRewardBounds = true;
                    for (uint64_t i = 0; i < cumulativeRewardFormula->getDimension(); ++i) {
                        if (!cumulativeRewardFormula->getTimeBoundReference(i).isRewardBound()) {
                            onlyRewardBounds = false;
                            break;
                        }
                    }
                    if (onlyRewardBounds) {
                        data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                    }
                }
                
                template<typename SparseModelType>
                void SparseMultiObjectivePreprocessor<SparseModelType>::preprocessTotalRewardFormula(storm::logic::OperatorInformation const& opInfo, PreprocessorData& data, boost::optional<std::string> const& optionalRewardModelName) {
                    
                    auto totalRewardFormula = std::make_shared<storm::logic::TotalRewardFormula>();
                    data.objectives.back()->formula = std::make_shared<storm::logic::RewardOperatorFormula>(totalRewardFormula, *optionalRewardModelName, opInfo);
                    data.finiteRewardCheckObjectives.set(data.objectives.size() - 1, true);
                }
                
                template<typename SparseModelType>
                typename SparseMultiObjectivePreprocessor<SparseModelType>::ReturnType SparseMultiObjectivePreprocessor<SparseModelType>::buildResult(SparseModelType const& originalModel, storm::logic::MultiObjectiveFormula const& originalFormula, PreprocessorData& data) {
                    ReturnType result(originalFormula, originalModel);
                    auto backwardTransitions = data.model->getBackwardTransitions();
                    result.preprocessedModel = data.model;
                    
                    for (auto& obj : data.objectives) {
                        result.objectives.push_back(std::move(*obj));
                    }
                    result.queryType = getQueryType(result.objectives);
                    result.maybeInfiniteRewardObjectives = std::move(data.finiteRewardCheckObjectives);
                    
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
                
                template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<double>>;
                template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<double>>;
                
                template class SparseMultiObjectivePreprocessor<storm::models::sparse::Mdp<storm::RationalNumber>>;
                template class SparseMultiObjectivePreprocessor<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
            }
        }
    }
}
