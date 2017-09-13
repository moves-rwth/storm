#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

#include <string>
#include <set>
#include <functional>

#include "storm/utility/macros.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/transformer/EndComponentEliminator.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename ValueType, bool SingleObjectiveMode>
            MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives) : model(model), objectives(objectives) {
                initialize();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::shared_ptr<storm::logic::ProbabilityOperatorFormula const> objectiveFormula) : model(model) {
                
                STORM_LOG_THROW(objectiveFormula->hasOptimalityType(), storm::exceptions::InvalidPropertyException, "Formula needs to specify whether minimal or maximal values are to be computed on nondeterministic model.");
                if (objectiveFormula->getSubformula().isMultiObjectiveFormula()) {
                    for (auto const& subFormula : objectiveFormula->getSubformula().asMultiObjectiveFormula().getSubformulas()) {
                        STORM_LOG_THROW(subFormula->isBoundedUntilFormula(), storm::exceptions::InvalidPropertyException, "Formula " << objectiveFormula << " is not supported. Invalid subformula " << *subFormula << ".");
                    }
                } else {
                    STORM_LOG_THROW(objectiveFormula->getSubformula().isBoundedUntilFormula(), storm::exceptions::InvalidPropertyException, "Formula " << objectiveFormula << " is not supported. Invalid subformula " << objectiveFormula->getSubformula() << ".");
                }
                
                // Build an objective from the formula.
                storm::modelchecker::multiobjective::Objective<ValueType> objective;
                objective.formula = objectiveFormula;
                objective.originalFormula = objective.formula;
                objective.considersComplementaryEvent = false;
                objective.lowerResultBound = storm::utility::zero<ValueType>();
                objective.upperResultBound = storm::utility::one<ValueType>();
                objectives.push_back(std::move(objective));
                
                initialize();
            }
    
    
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::initialize() {
                
                maxSolutionsStored = 0;
                
                swInit.start();
                STORM_LOG_ASSERT(!SingleObjectiveMode || (this->objectives.size() == 1), "Enabled single objective mode but there are multiple objectives.");
                std::vector<Epoch> epochSteps;
                initializeObjectives(epochSteps);
                initializeMemoryProduct(epochSteps);
                swInit.stop();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::initializeObjectives(std::vector<Epoch>& epochSteps) {
                std::vector<std::vector<uint64_t>> dimensionWiseEpochSteps;
                // collect the time-bounded subobjectives
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula()) {
                        STORM_LOG_THROW(formula.getSubformula().isBoundedUntilFormula(), storm::exceptions::NotSupportedException, "Unexpected type of subformula for formula " << formula);
                        auto const& subformula = formula.getSubformula().asBoundedUntilFormula();
                        for (uint64_t dim = 0; dim < subformula.getDimension(); ++dim) {
                            subObjectives.push_back(std::make_pair(subformula.restrictToDimension(dim), objIndex));
                            std::string memLabel = "dim" + std::to_string(subObjectives.size()) + "_maybe";
                            while (model.getStateLabeling().containsLabel(memLabel)) {
                                memLabel = "_" + memLabel;
                            }
                            memoryLabels.push_back(memLabel);
                            if (subformula.getTimeBoundReference(dim).isTimeBound() || subformula.getTimeBoundReference(dim).isStepBound()) {
                                dimensionWiseEpochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                                scalingFactors.push_back(storm::utility::one<ValueType>());
                            } else {
                                STORM_LOG_ASSERT(subformula.getTimeBoundReference(dim).isRewardBound(), "Unexpected type of time bound.");
                                std::string const& rewardName = subformula.getTimeBoundReference(dim).getRewardName();
                                STORM_LOG_THROW(this->model.hasRewardModel(rewardName), storm::exceptions::IllegalArgumentException, "No reward model with name '" << rewardName << "' found.");
                                auto const& rewardModel = this->model.getRewardModel(rewardName);
                                STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported as reward bounds.");
                                std::vector<ValueType> actionRewards = rewardModel.getTotalRewardVector(this->model.getTransitionMatrix());
                                auto discretizedRewardsAndFactor = storm::utility::vector::toIntegralVector<ValueType, uint64_t>(actionRewards);
                                dimensionWiseEpochSteps.push_back(std::move(discretizedRewardsAndFactor.first));
                                scalingFactors.push_back(std::move(discretizedRewardsAndFactor.second));
                            }
                        }
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isCumulativeRewardFormula()) {
                        subObjectives.push_back(std::make_pair(formula.getSubformula().asSharedPointer(), objIndex));
                        dimensionWiseEpochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                        scalingFactors.push_back(storm::utility::one<ValueType>());
                        memoryLabels.push_back(boost::none);
                    }
                }
                
                // Compute a mapping for each objective to the set of dimensions it considers
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    storm::storage::BitVector dimensions(subObjectives.size(), false);
                    for (uint64_t subObjIndex = 0; subObjIndex < subObjectives.size(); ++subObjIndex) {
                        if (subObjectives[subObjIndex].second == objIndex) {
                            dimensions.set(subObjIndex, true);
                        }
                    }
                    objectiveDimensions.push_back(std::move(dimensions));
                }
                
                // Initialize the epoch manager
                epochManager = EpochManager(subObjectives.size());
                
                // Convert the epoch steps to a choice-wise representation
                epochSteps.reserve(model.getNumberOfChoices());
                for (uint64_t choice = 0; choice < model.getNumberOfChoices(); ++choice) {
                    Epoch step;
                    uint64_t dim = 0;
                    for (auto const& dimensionSteps : dimensionWiseEpochSteps) {
                        epochManager.setDimensionOfEpoch(step, dim, dimensionSteps[choice]);
                        ++dim;
                    }
                    epochSteps.push_back(step);
                }
                
                // collect which epoch steps are possible
                possibleEpochSteps.clear();
                for (auto const& step : epochSteps) {
                    possibleEpochSteps.insert(step);
                }
                
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::initializeMemoryProduct(std::vector<Epoch> const& epochSteps) {
                
                // build the memory structure
                auto memoryStructure = computeMemoryStructure();
                
                // build a mapping between the different representations of memory states
                auto memoryStateMap = computeMemoryStateMap(memoryStructure);

                memoryProduct = MemoryProduct(model, memoryStructure, std::move(memoryStateMap), epochSteps, objectiveDimensions);
                
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::Epoch MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getStartEpoch() {
                Epoch startEpoch;
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    storm::expressions::Expression bound;
                    bool isStrict = false;
                    storm::logic::Formula const& dimFormula = *subObjectives[dim].first;
                    if (dimFormula.isBoundedUntilFormula()) {
                        assert(!dimFormula.asBoundedUntilFormula().isMultiDimensional());
                        STORM_LOG_THROW(dimFormula.asBoundedUntilFormula().hasUpperBound() && !dimFormula.asBoundedUntilFormula().hasLowerBound(), storm::exceptions::NotSupportedException, "Until formulas with a lower or no upper bound are not supported.");
                        bound = dimFormula.asBoundedUntilFormula().getUpperBound();
                        isStrict = dimFormula.asBoundedUntilFormula().isUpperBoundStrict();
                    } else if (dimFormula.isCumulativeRewardFormula()) {
                        bound = dimFormula.asCumulativeRewardFormula().getBound();
                        isStrict = dimFormula.asCumulativeRewardFormula().isBoundStrict();
                    }
                    STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::NotSupportedException, "The bound " << bound << " contains undefined constants.");
                    ValueType discretizedBound = storm::utility::convertNumber<ValueType>(bound.evaluateAsRational());
                    discretizedBound /= scalingFactors[dim];
                    if (isStrict && discretizedBound == storm::utility::floor(discretizedBound)) {
                         discretizedBound = storm::utility::floor(discretizedBound) - storm::utility::one<ValueType>();
                    } else {
                        discretizedBound = storm::utility::floor(discretizedBound);
                    }
                    uint64_t dimensionValue = storm::utility::convertNumber<uint64_t>(discretizedBound);
                    STORM_LOG_THROW(epochManager.isValidDimensionValue(dimensionValue), storm::exceptions::NotSupportedException, "The bound " << bound << " is too high for the considered number of dimensions.");
                    epochManager.setDimensionOfEpoch(startEpoch, dim, dimensionValue);
                }
                //std::cout << "Start epoch is " << epochManager.toString(startEpoch) << std::endl;
                return startEpoch;
            }
    
            template<typename ValueType, bool SingleObjectiveMode>
            std::vector<typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::Epoch> MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getEpochComputationOrder(Epoch const& startEpoch) {
                // Perform a DFS to find all the reachable epochs
                std::vector<Epoch> dfsStack;
                std::set<Epoch, std::function<bool(Epoch const&, Epoch const&)>> collectedEpochs(std::bind(&EpochManager::epochClassZigZagOrder, &epochManager, std::placeholders::_1, std::placeholders::_2));
                
                collectedEpochs.insert(startEpoch);
                dfsStack.push_back(startEpoch);
                while (!dfsStack.empty()) {
                    Epoch currentEpoch = dfsStack.back();
                    dfsStack.pop_back();
                    for (auto const& step : possibleEpochSteps) {
                        Epoch successorEpoch = epochManager.getSuccessorEpoch(currentEpoch, step);
                        /*
                        for (auto const& e : collectedEpochs) {
                            std::cout << "Comparing " << epochManager.toString(e) << " and " << epochManager.toString(successorEpoch) << std::endl;
                            if (epochManager.epochClassZigZagOrder(e, successorEpoch)) {
                                std::cout << "    " << epochManager.toString(e) << " < " << epochManager.toString(successorEpoch) << std::endl;
                            }
                            if (epochManager.epochClassZigZagOrder(successorEpoch, e)) {
                                std::cout << "    " << epochManager.toString(e) << " > " << epochManager.toString(successorEpoch) << std::endl;
                            }
                        }
                         */
                        if (collectedEpochs.insert(successorEpoch).second) {
                            dfsStack.push_back(std::move(successorEpoch));
                        }
                    }
                }
                /*
                std::cout << "Resulting order: ";
                for (auto const& e : collectedEpochs) {
                    std::cout << epochManager.toString(e) << ", ";
                }
                std::cout << std::endl;
                */
                return std::vector<Epoch>(collectedEpochs.begin(), collectedEpochs.end());
                
                /*
                
                // perform DFS to get the 'reachable' epochs in the correct order.
                std::vector<Epoch> result, dfsStack;
                std::set<Epoch> seenEpochs;
                seenEpochs.insert(startEpoch);
                dfsStack.push_back(startEpoch);
                while (!dfsStack.empty()) {
                    bool hasUnseenSuccessor = false;
                    for (auto const& step : possibleEpochSteps) {
                        Epoch successorEpoch = epochManager.getSuccessorEpoch(dfsStack.back(), step);
                        if (seenEpochs.find(successorEpoch) == seenEpochs.end()) {
                            seenEpochs.insert(successorEpoch);
                            dfsStack.push_back(std::move(successorEpoch));
                            hasUnseenSuccessor = true;
                        }
                    }
                    if (!hasUnseenSuccessor) {
                        result.push_back(std::move(dfsStack.back()));
                        dfsStack.pop_back();
                    }
                }
                
                return result;
                 */
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::EpochModel& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setCurrentEpoch(Epoch const& epoch) {
                // std::cout << "Setting model for epoch " << epochManager.toString(epoch) << std::endl;
                
                // Check if we need to update the current epoch class
                if (!currentEpoch || !epochManager.compareEpochClass(epoch, currentEpoch.get())) {
                    setCurrentEpochClass(epoch);
                    epochModel.epochMatrixChanged = true;
                } else {
                    epochModel.epochMatrixChanged = false;
                }
                
                swSetEpoch.start();
                epochModel.objectiveRewardFilter.clear();
                for (auto const& objRewards : epochModel.objectiveRewards) {
                    epochModel.objectiveRewardFilter.push_back(storm::utility::vector::filterZero(objRewards));
                    epochModel.objectiveRewardFilter.back().complement();
                }
                
                epochModel.stepSolutions.resize(epochModel.stepChoices.getNumberOfSetBits());
                auto stepSolIt = epochModel.stepSolutions.begin();
                for (auto const& reducedChoice : epochModel.stepChoices) {
                    uint64_t productChoice = epochModelToProductChoiceMap[reducedChoice];
                    uint64_t productState = memoryProduct.getProductStateFromChoice(productChoice);
                    auto memoryState = memoryProduct.convertMemoryState(memoryProduct.getMemoryState(productState));
                    Epoch successorEpoch = epochManager.getSuccessorEpoch(epoch, memoryProduct.getSteps()[productChoice]);
                    
                    // Find out whether objective reward is earned for the current choice
                    // Objective reward is not earned if there is a subObjective that is still relevant but the corresponding reward bound is passed after taking the choice
                    for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                        if (epochModel.objectiveRewardFilter[objIndex].get(reducedChoice)) {
                            for (auto const& dim : objectiveDimensions[objIndex]) {
                                if (epochManager.isBottomDimension(successorEpoch, dim) && memoryState.get(dim)) {
                                    epochModel.objectiveRewardFilter[objIndex].set(reducedChoice, false);
                                    break;
                                }
                            }
                        }
                    }
                    
                    // compute the solution for the stepChoices
                    SolutionType choiceSolution;
                    bool firstSuccessor = true;
                    if (epochManager.compareEpochClass(epoch, successorEpoch)) {
                        for (auto const& successor : memoryProduct.getProduct().getTransitionMatrix().getRow(productChoice)) {
                            if (firstSuccessor) {
                                choiceSolution = getScaledSolution(getStateSolution(successorEpoch, successor.getColumn()), successor.getValue());
                                firstSuccessor = false;
                            } else {
                                addScaledSolution(choiceSolution, getStateSolution(successorEpoch, successor.getColumn()), successor.getValue());
                            }
                        }
                    } else {
                        storm::storage::BitVector successorRelevantDimensions(epochManager.getDimensionCount(), true);
                        for (auto const& dim : memoryState) {
                            if (epochManager.isBottomDimension(successorEpoch, dim)) {
                                successorRelevantDimensions &= ~objectiveDimensions[subObjectives[dim].second];
                            }
                        }
                        for (auto const& successor : memoryProduct.getProduct().getTransitionMatrix().getRow(productChoice)) {
                            storm::storage::BitVector successorMemoryState = memoryProduct.convertMemoryState(memoryProduct.getMemoryState(successor.getColumn())) & successorRelevantDimensions;
                            uint64_t successorProductState = memoryProduct.getProductState(memoryProduct.getModelState(successor.getColumn()), memoryProduct.convertMemoryState(successorMemoryState));
                            SolutionType const& successorSolution = getStateSolution(successorEpoch, successorProductState);
                            if (firstSuccessor) {
                                choiceSolution = getScaledSolution(successorSolution, successor.getValue());
                                firstSuccessor = false;
                            } else {
                                addScaledSolution(choiceSolution, successorSolution, successor.getValue());
                            }
                        }
                    }
                    *stepSolIt = std::move(choiceSolution);
                    ++stepSolIt;
                }
                
                assert(epochModel.objectiveRewards.size() == objectives.size());
                assert(epochModel.objectiveRewardFilter.size() == objectives.size());
                assert(epochModel.epochMatrix.getRowCount() == epochModel.stepChoices.size());
                assert(epochModel.stepChoices.size() == epochModel.objectiveRewards.front().size());
                assert(epochModel.objectiveRewards.front().size() == epochModel.objectiveRewards.back().size());
                assert(epochModel.objectiveRewards.front().size() == epochModel.objectiveRewardFilter.front().size());
                assert(epochModel.objectiveRewards.back().size() == epochModel.objectiveRewardFilter.back().size());
                assert(epochModel.stepChoices.getNumberOfSetBits() == epochModel.stepSolutions.size());
                
                currentEpoch = epoch;
                swSetEpoch.stop();
                /*
                std::cout << "Epoch model for epoch " << storm::utility::vector::toString(epoch) << std::endl;
                std::cout << "Matrix: " << std::endl << epochModel.epochMatrix << std::endl;
                std::cout << "ObjectiveRewards: " << storm::utility::vector::toString(epochModel.objectiveRewards[0]) << std::endl;
                std::cout << "steps: " << epochModel.stepChoices << std::endl;
                std::cout << "step solutions: ";
                for (int i = 0; i < epochModel.stepSolutions.size(); ++i) {
                    std::cout << "   " << epochModel.stepSolutions[i].weightedValue;
                }
                std::cout << std::endl;
                */
                return epochModel;
                
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setCurrentEpochClass(Epoch const& epoch) {
                // std::cout << "Setting epoch class for epoch " << storm::utility::vector::toString(epoch) << std::endl;
                swSetEpochClass.start();
                swAux1.start();
                auto productObjectiveRewards = computeObjectiveRewardsForProduct(epoch);
                
                storm::storage::BitVector stepChoices(memoryProduct.getProduct().getNumberOfChoices(), false);
                uint64_t choice = 0;
                for (auto const& step : memoryProduct.getSteps()) {
                    if (!epochManager.isZeroEpoch(step) && epochManager.getSuccessorEpoch(epoch, step) != epoch) {
                        stepChoices.set(choice, true);
                    }
                    ++choice;
                }
                epochModel.epochMatrix = memoryProduct.getProduct().getTransitionMatrix().filterEntries(~stepChoices);
                
                storm::storage::BitVector zeroObjRewardChoices(memoryProduct.getProduct().getNumberOfChoices(), true);
                for (auto const& objRewards : productObjectiveRewards) {
                    zeroObjRewardChoices &= storm::utility::vector::filterZero(objRewards);
                }
                swAux1.stop();
                swAux2.start();
                storm::storage::BitVector allProductStates(memoryProduct.getProduct().getNumberOfStates(), true);
                
                // Get the relevant states for this epoch. These are all states
                storm::storage::BitVector productInStates = computeProductInStatesForEpochClass(epoch);
                // The epoch model only needs to consider the states that are reachable from a relevant state
                storm::storage::BitVector consideredStates = storm::utility::graph::getReachableStates(epochModel.epochMatrix, productInStates, allProductStates, ~allProductStates);
                // std::cout << "numInStates = " << productInStates.getNumberOfSetBits() << std::endl;
                // std::cout << "numConsideredStates = " << consideredStates.getNumberOfSetBits() << std::endl;
                
                // We assume that there is no end component in which objective reward is earned
                STORM_LOG_ASSERT(!storm::utility::graph::checkIfECWithChoiceExists(epochModel.epochMatrix, epochModel.epochMatrix.transpose(true), allProductStates, ~zeroObjRewardChoices & ~stepChoices), "There is a scheduler that yields infinite reward for one objective. This case should be excluded");
                swAux2.stop();
                swAux3.start();
                auto ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(epochModel.epochMatrix, consideredStates, zeroObjRewardChoices & ~stepChoices, consideredStates);
                swAux3.stop();
                swAux4.start();
                epochModel.epochMatrix = std::move(ecElimResult.matrix);
                epochModelToProductChoiceMap = std::move(ecElimResult.newToOldRowMapping);
                productToEpochModelStateMap = std::move(ecElimResult.oldToNewStateMapping);
                
                epochModel.stepChoices = storm::storage::BitVector(epochModel.epochMatrix.getRowCount(), false);
                for (uint64_t choice = 0; choice < epochModel.epochMatrix.getRowCount(); ++choice) {
                    if (stepChoices.get(epochModelToProductChoiceMap[choice])) {
                        epochModel.stepChoices.set(choice, true);
                    }
                }
                
                epochModel.objectiveRewards.clear();
                for (auto const& productObjRew : productObjectiveRewards) {
                    std::vector<ValueType> reducedModelObjRewards;
                    reducedModelObjRewards.reserve(epochModel.epochMatrix.getRowCount());
                    for (auto const& productChoice : epochModelToProductChoiceMap) {
                        reducedModelObjRewards.push_back(productObjRew[productChoice]);
                    }
                    epochModel.objectiveRewards.push_back(std::move(reducedModelObjRewards));
                }
                
                epochModel.epochInStates = storm::storage::BitVector(epochModel.epochMatrix.getRowGroupCount(), false);
                for (auto const& productState : productInStates) {
                    STORM_LOG_ASSERT(productToEpochModelStateMap[productState] < epochModel.epochMatrix.getRowGroupCount(), "Selected product state does not exist in the epoch model.");
                    epochModel.epochInStates.set(productToEpochModelStateMap[productState], true);
                }
                
                epochModelInStateToProductStatesMap.assign(epochModel.epochInStates.getNumberOfSetBits(), std::vector<uint64_t>());
                for (auto const& productState : productInStates) {
                    epochModelInStateToProductStatesMap[epochModel.epochInStates.getNumberOfSetBitsBeforeIndex(productToEpochModelStateMap[productState])].push_back(productState);
                }
                
                swAux4.stop();
                swSetEpochClass.stop();
                epochModelSizes.push_back(epochModel.epochMatrix.getRowGroupCount());
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            storm::storage::BitVector MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::computeProductInStatesForEpochClass(Epoch const& epoch) {
                storm::storage::SparseMatrix<ValueType> const& productMatrix = memoryProduct.getProduct().getTransitionMatrix();
                
                // Initialize the result. Initial states are only considered if the epoch contains no bottom dimension.
                storm::storage::BitVector result;
                if (epochManager.hasBottomDimension(epoch)) {
                    result = storm::storage::BitVector(memoryProduct.getProduct().getNumberOfStates());
                } else {
                    result = memoryProduct.getProduct().getInitialStates();
                }
                
                // Compute the set of objectives that can not be satisfied anymore in the current epoch
                storm::storage::BitVector irrelevantObjectives(objectives.size(), false);
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    bool objIrrelevant = true;
                    for (auto const& dim : objectiveDimensions[objIndex]) {
                        if (!epochManager.isBottomDimension(epoch, dim)) {
                            objIrrelevant = false;
                        }
                    }
                    if (objIrrelevant) {
                        irrelevantObjectives.set(objIndex, true);
                    }
                }
    
                // Perform DFS
                storm::storage::BitVector reachableStates = memoryProduct.getProduct().getInitialStates();
                std::vector<uint_fast64_t> stack(reachableStates.begin(), reachableStates.end());
                
                while (!stack.empty()) {
                    uint64_t state = stack.back();
                    stack.pop_back();
                    for (uint64_t choice = productMatrix.getRowGroupIndices()[state]; choice < productMatrix.getRowGroupIndices()[state + 1]; ++choice) {
                        auto const& choiceStep = memoryProduct.getSteps()[choice];
                        if (!epochManager.isZeroEpoch(choiceStep)) {
 
                            // Compute the set of objectives that might or might not become irrelevant when the epoch is reached via the current choice
                            storm::storage::BitVector maybeIrrelevantObjectives(objectives.size(), false);
                            for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                                if (epochManager.isBottomDimension(epoch, dim) && epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                    maybeIrrelevantObjectives.set(subObjectives[dim].second);
                                }
                            }
                            maybeIrrelevantObjectives &= ~irrelevantObjectives;
                            
                            // For optimization purposes, we treat the case that all objectives will be relevant seperately
                            if (maybeIrrelevantObjectives.empty() && irrelevantObjectives.empty()) {
                                for (auto const& choiceSuccessor : productMatrix.getRow(choice)) {
                                    result.set(choiceSuccessor.getColumn(), true);
                                    if (!reachableStates.get(choiceSuccessor.getColumn())) {
                                        reachableStates.set(choiceSuccessor.getColumn());
                                        stack.push_back(choiceSuccessor.getColumn());
                                    }
                                }
                            } else {
                                // Enumerate all possible combinations of maybe relevant objectives
                                storm::storage::BitVector maybeObjSubset(maybeIrrelevantObjectives.getNumberOfSetBits(), false);
                                do {
                                    for (auto const& choiceSuccessor : productMatrix.getRow(choice)) {
                                        // Compute the successor memory state for the current objective-subset and transition
                                        storm::storage::BitVector successorMemoryState = memoryProduct.convertMemoryState(memoryProduct.getMemoryState(choiceSuccessor.getColumn()));
                                        // Unselect dimensions belonging to irrelevant objectives
                                        for (auto const& irrelevantObjIndex : irrelevantObjectives) {
                                            successorMemoryState &= ~objectiveDimensions[irrelevantObjIndex];
                                        }
                                        // Unselect objectives that are not in the current subset of maybe relevant objectives
                                        // We can skip a subset if it selects an objective that is irrelevant anyway (according to the original successor memorystate).
                                        bool skipThisSubSet = false;
                                        uint64_t i = 0;
                                        for (auto const& objIndex : maybeIrrelevantObjectives) {
                                            if (maybeObjSubset.get(i)) {
                                                if (successorMemoryState.isDisjointFrom(objectiveDimensions[objIndex])) {
                                                    skipThisSubSet = true;
                                                    break;
                                                } else {
                                                    successorMemoryState &= ~objectiveDimensions[objIndex];
                                                }
                                            }
                                            ++i;
                                        }
                                        if (!skipThisSubSet) {
                                            uint64_t successorState = memoryProduct.getProductState(memoryProduct.getModelState(choiceSuccessor.getColumn()), memoryProduct.convertMemoryState(successorMemoryState));
                                            result.set(successorState, true);
                                            if (!reachableStates.get(successorState)) {
                                                reachableStates.set(successorState);
                                                stack.push_back(successorState);
                                            }
                                        }
                                    }
                                    
                                    maybeObjSubset.increment();
                                } while (!maybeObjSubset.empty());
                            }
                        } else {
                            for (auto const& choiceSuccessor : productMatrix.getRow(choice)) {
                                if (!reachableStates.get(choiceSuccessor.getColumn())) {
                                    reachableStates.set(choiceSuccessor.getColumn());
                                    stack.push_back(choiceSuccessor.getColumn());
                                }
                            }
                        }
                    }
                }
                
                return result;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<SO, int>::type>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const {
                return solution * scalingFactor;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<!SO, int>::type>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const {
                SolutionType res;
                res.reserve(solution.size());
                for (auto const& sol : solution) {
                    res.push_back(sol * scalingFactor);
                }
                return res;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<SO, int>::type>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const {
                solution += solutionToAdd * scalingFactor;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<!SO, int>::type>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const {
                storm::utility::vector::addScaledVector(solution,  solutionToAdd, scalingFactor);
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<SO, int>::type>
            std::string MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::solutionToString(SolutionType const& solution) const {
                return std::to_string(solution);
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<!SO, int>::type>
            std::string MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::solutionToString(SolutionType const& solution) const {
                std::string res = "(";
                bool first = true;
                for (auto const& s : solution) {
                    if (first) {
                        first = false;
                    } else {
                        res += ", ";
                    }
                    res += std::to_string(s);
                }
                res += ")";
                return res;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setSolutionForCurrentEpoch(std::vector<SolutionType>& inStateSolutions) {
                swInsertSol.start();
                STORM_LOG_ASSERT(inStateSolutions.size() == epochModelInStateToProductStatesMap.size(), "Invalid number of solutions.");
                auto solIt = inStateSolutions.begin();
                for (auto const& productStates : epochModelInStateToProductStatesMap) {
                    assert(!productStates.empty());
                    auto productStateIt = productStates.begin();
                    // Skip the first product state for now. It's result will be std::move'd afterwards.
                    for (++productStateIt; productStateIt != productStates.end(); ++productStateIt) {
                        setSolutionForCurrentEpoch(*productStateIt, *solIt);
                    }
                    setSolutionForCurrentEpoch(productStates.front(), std::move(*solIt));
                    
                    ++solIt;
                }
                maxSolutionsStored = std::max((uint64_t) solutions.size(), maxSolutionsStored);
                
                swInsertSol.stop();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType const& solution) {
                STORM_LOG_ASSERT(currentEpoch, "Tried to set a solution for the current epoch, but no epoch was specified before.");
            //    std::cout << "Setting solution for state " << productState << " in epoch " << epochManager.toString(currentEpoch.get()) << std::endl;
                solutions[std::make_pair(currentEpoch.get(), productState)] = solution;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType&& solution) {
                STORM_LOG_ASSERT(currentEpoch, "Tried to set a solution for the current epoch, but no epoch was specified before.");
   //             std::cout << "Setting solution for state " << productState << " in epoch " << epochManager.toString(currentEpoch.get()) << std::endl;
                solutions[std::make_pair(currentEpoch.get(), productState)] = std::move(solution);
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getStateSolution(Epoch const& epoch, uint64_t const& productState) {
                swFindSol.start();
                //std::cout << "Getting solution for epoch " << epochManager.toString(epoch) << " and state " << productState  << std::endl;
                auto solutionIt = solutions.find(std::make_pair(epoch, productState));
                STORM_LOG_ASSERT(solutionIt != solutions.end(), "Requested unexisting solution for epoch " << epochManager.toString(epoch) << ".");
                //std::cout << "Retrieved solution for state " << productState << " in epoch " << epochManager.toString(epoch)  << std::endl;
                swFindSol.stop();
                return solutionIt->second;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getInitialStateResult(Epoch const& epoch) {
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "The model has multiple initial states.");
                STORM_LOG_ASSERT(memoryProduct.getProduct().getInitialStates().getNumberOfSetBits() == 1, "The product has multiple initial states.");
                return getStateSolution(epoch, *memoryProduct.getProduct().getInitialStates().begin());
            }

            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getInitialStateResult(Epoch const& epoch, uint64_t initialStateIndex) {
                STORM_LOG_ASSERT(model.getInitialStates().get(initialStateIndex), "The given model state is not an initial state.");
                for (uint64_t memState = 0; memState < memoryProduct.getNumberOfMemoryState(); ++memState) {
                    uint64_t productState = memoryProduct.getProductState(initialStateIndex, memState);
                    if (memoryProduct.getProduct().getInitialStates().get(productState)) {
                        return getStateSolution(epoch, productState);
                    }
                }
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not find the initial product state corresponding to the given initial model state.");
                return getStateSolution(epoch, -1ull);
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            storm::storage::MemoryStructure MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::computeMemoryStructure() const {
                
                storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(model);
                
                // Create a memory structure that remembers whether (sub)objectives are satisfied
                storm::storage::MemoryStructure memory = storm::storage::MemoryStructureBuilder<ValueType>::buildTrivialMemoryStructure(model);
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    if (!objectives[objIndex].formula->isProbabilityOperatorFormula()) {
                        continue;
                    }
                    
                    std::vector<uint64_t> dimensionIndexMap;
                    for (auto const& globalDimensionIndex : objectiveDimensions[objIndex]) {
                        dimensionIndexMap.push_back(globalDimensionIndex);
                    }
                    
                    // collect the memory states for this objective
                    std::vector<storm::storage::BitVector> objMemStates;
                    storm::storage::BitVector m(dimensionIndexMap.size(), false);
                    for (; !m.full(); m.increment()) {
                        objMemStates.push_back(~m);
                    }
                    objMemStates.push_back(~m);
                    assert(objMemStates.size() == 1ull << dimensionIndexMap.size());
                    
                    // build objective memory
                    auto objMemoryBuilder = storm::storage::MemoryStructureBuilder<ValueType>(objMemStates.size(), model);
                    
                    // Get the set of states that for all subobjectives satisfy either the left or the right subformula
                    storm::storage::BitVector constraintStates(model.getNumberOfStates(), true);
                    for (auto const& dim : objectiveDimensions[objIndex]) {
                        auto const& subObj = subObjectives[dim];
                        STORM_LOG_ASSERT(subObj.first->isBoundedUntilFormula(), "Unexpected Formula type");
                        constraintStates &=
                                (mc.check(subObj.first->asBoundedUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector() |
                                mc.check(subObj.first->asBoundedUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
                    }
                    
                    // Build the transitions between the memory states
                    for (uint64_t memState = 0; memState < objMemStates.size(); ++memState) {
                        auto const& memStateBV = objMemStates[memState];
                        for (uint64_t memStatePrime = 0; memStatePrime < objMemStates.size(); ++memStatePrime) {
                            auto const& memStatePrimeBV = objMemStates[memStatePrime];
                            if (memStatePrimeBV.isSubsetOf(memStateBV)) {
                                
                                std::shared_ptr<storm::logic::Formula const> transitionFormula = storm::logic::Formula::getTrueFormula();
                                for (auto const& subObjIndex : memStateBV) {
                                    std::shared_ptr<storm::logic::Formula const> subObjFormula = subObjectives[dimensionIndexMap[subObjIndex]].first->asBoundedUntilFormula().getRightSubformula().asSharedPointer();
                                    if (memStatePrimeBV.get(subObjIndex)) {
                                        subObjFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, subObjFormula);
                                    }
                                    transitionFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, transitionFormula, subObjFormula);
                                }
                                
                                storm::storage::BitVector transitionStates = mc.check(*transitionFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                                if (memStatePrimeBV.empty()) {
                                    transitionStates |= ~constraintStates;
                                } else {
                                    transitionStates &= constraintStates;
                                }
                                objMemoryBuilder.setTransition(memState, memStatePrime, transitionStates);
                                
                                // Set the initial states
                                if (memStateBV.full()) {
                                    storm::storage::BitVector initialTransitionStates = model.getInitialStates() & transitionStates;
                                    // At this point we can check whether there is an initial state that already satisfies all subObjectives.
                                    // Such a situation is not supported as we can not reduce this (easily) to an expected reward computation.
                                    STORM_LOG_THROW(!memStatePrimeBV.empty() || initialTransitionStates.empty() || initialTransitionStates.isDisjointFrom(constraintStates), storm::exceptions::NotSupportedException, "The objective " << *objectives[objIndex].formula << " is already satisfied in an initial state. This special case is not supported.");
                                    for (auto const& initState : initialTransitionStates) {
                                        objMemoryBuilder.setInitialMemoryState(initState, memStatePrime);
                                    }
                                }
                            }
                        }
                    }

                    // Build the memory labels
                    for (uint64_t memState = 0; memState < objMemStates.size(); ++memState) {
                        auto const& memStateBV = objMemStates[memState];
                        for (auto const& subObjIndex : memStateBV) {
                            objMemoryBuilder.setLabel(memState, memoryLabels[dimensionIndexMap[subObjIndex]].get());
                        }
                    }
                    auto objMemory = objMemoryBuilder.build();
                    memory = memory.product(objMemory);
                }
                return memory;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            std::vector<storm::storage::BitVector> MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::computeMemoryStateMap(storm::storage::MemoryStructure const& memory) const {
                // Compute a mapping between the different representations of memory states
                std::vector<storm::storage::BitVector> result;
                result.reserve(memory.getNumberOfStates());
                for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++memState) {
                    storm::storage::BitVector relevantSubObjectives(epochManager.getDimensionCount(), false);
                    std::set<std::string> stateLabels = memory.getStateLabeling().getLabelsOfState(memState);
                    for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                        if (memoryLabels[dim] && stateLabels.find(memoryLabels[dim].get()) != stateLabels.end()) {
                            relevantSubObjectives.set(dim, true);
                        }
                    }
                    result.push_back(std::move(relevantSubObjectives));
                }
                return result;
            }

            template<typename ValueType, bool SingleObjectiveMode>
            MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::MemoryProduct(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<Epoch> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) : memoryStateMap(std::move(memoryStateMap)) {
                storm::storage::SparseModelMemoryProduct<ValueType> productBuilder(memory.product(model));
                
                setReachableStates(productBuilder, originalModelSteps, objectiveDimensions);
                product = productBuilder.build()->template as<storm::models::sparse::Mdp<ValueType>>();
                
                uint64_t numModelStates = productBuilder.getOriginalModel().getNumberOfStates();
                uint64_t numMemoryStates = productBuilder.getMemory().getNumberOfStates();
                uint64_t numProductStates = getProduct().getNumberOfStates();
                
                // Compute a mappings from product states to model/memory states and back
                modelMemoryToProductStateMap.resize(numMemoryStates * numModelStates, std::numeric_limits<uint64_t>::max());
                productToModelStateMap.resize(numProductStates, std::numeric_limits<uint64_t>::max());
                productToMemoryStateMap.resize(numProductStates, std::numeric_limits<uint64_t>::max());
                for (uint64_t modelState = 0; modelState < numModelStates; ++modelState) {
                    for (uint64_t memoryState = 0; memoryState < numMemoryStates; ++memoryState) {
                        if (productBuilder.isStateReachable(modelState, memoryState)) {
                            uint64_t productState = productBuilder.getResultState(modelState, memoryState);
                            modelMemoryToProductStateMap[modelState * numMemoryStates + memoryState] = productState;
                            productToModelStateMap[productState] = modelState;
                            productToMemoryStateMap[productState] = memoryState;
                        }
                    }
                }
                
                // Map choice indices of the product to the state where it origins
                choiceToStateMap.reserve(getProduct().getNumberOfChoices());
                for (uint64_t productState = 0; productState < numProductStates; ++productState) {
                    uint64_t groupSize = getProduct().getTransitionMatrix().getRowGroupSize(productState);
                    for (uint64_t i = 0; i < groupSize; ++i) {
                        choiceToStateMap.push_back(productState);
                    }
                }
                
                // Compute the epoch steps for the product
                steps.resize(getProduct().getNumberOfChoices(), 0);
                for (uint64_t modelState = 0; modelState < numModelStates; ++modelState) {
                    uint64_t numChoices = productBuilder.getOriginalModel().getTransitionMatrix().getRowGroupSize(modelState);
                    uint64_t firstChoice = productBuilder.getOriginalModel().getTransitionMatrix().getRowGroupIndices()[modelState];
                    for (uint64_t choiceOffset = 0; choiceOffset < numChoices; ++choiceOffset) {
                        Epoch const& step  = originalModelSteps[firstChoice + choiceOffset];
                        if (step != 0) {
                            for (uint64_t memState = 0; memState < numMemoryStates; ++memState) {
                                if (productStateExists(modelState, memState)) {
                                    uint64_t productState = getProductState(modelState, memState);
                                    uint64_t productChoice = getProduct().getTransitionMatrix().getRowGroupIndices()[productState] + choiceOffset;
                                    assert(productChoice < getProduct().getTransitionMatrix().getRowGroupIndices()[productState + 1]);
                                    steps[productChoice] = step;
                                }
                            }
                        }
                    }
                }
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::setReachableStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) const {
                
                // todo: find something else to perform getDimensionOfEpoch at this point
                uint64_t dimensionCount = objectiveDimensions.front().size();
                uint64_t bitsPerDimension = 64 / dimensionCount;
                uint64_t dimensionBitMask;
                if (dimensionCount == 1) {
                    dimensionBitMask = -1ull;
                } else {
                    dimensionBitMask = (1ull << bitsPerDimension) - 1;
                }
                
             
                
                
                
                std::vector<storm::storage::BitVector> additionalReachableStates(memoryStateMap.size(), storm::storage::BitVector(productBuilder.getOriginalModel().getNumberOfStates(), false));
                for (uint64_t memState = 0; memState < memoryStateMap.size(); ++memState) {
                    auto const& memStateBv = memoryStateMap[memState];
                    storm::storage::BitVector consideredObjectives(objectiveDimensions.size(), false);
                    do {
                        storm::storage::BitVector memStatePrimeBv = memStateBv;
                        for (auto const& objIndex : consideredObjectives) {
                            memStatePrimeBv &= ~objectiveDimensions[objIndex];
                        }
                        if (memStatePrimeBv != memStateBv) {
                            for (uint64_t choice = 0; choice < productBuilder.getOriginalModel().getTransitionMatrix().getRowCount(); ++choice) {
                                bool consideredChoice = true;
                                for (auto const& objIndex : consideredObjectives) {
                                    bool objectiveHasStep = false;
                                    for (auto const& dim : objectiveDimensions[objIndex]) {
  // TODO: this can not be called currently                                      if (getDimensionOfEpoch(originalModelSteps[choice], dim) > 0) {
                                        // .. instead we do this
                                        if (((originalModelSteps[choice] >> (dim * bitsPerDimension)) & dimensionBitMask) > 0) {
                                            objectiveHasStep = true;
                                            break;
                                        }
                                    }
                                    if (!objectiveHasStep) {
                                        consideredChoice = false;
                                        break;
                                    }
                                }
                                if (consideredChoice) {
                                    for (auto const& successor : productBuilder.getOriginalModel().getTransitionMatrix().getRow(choice)) {
                                        if (productBuilder.isStateReachable(successor.getColumn(), memState)) {
                                            additionalReachableStates[convertMemoryState(memStatePrimeBv)].set(successor.getColumn());
                                        }
                                    }
                                }
                            }
                        }
                        consideredObjectives.increment();
                    } while (!consideredObjectives.empty());
                }
                
                for (uint64_t memState = 0; memState < memoryStateMap.size(); ++memState) {
                    for (auto const& modelState : additionalReachableStates[memState]) {
                        productBuilder.addReachableState(modelState, memState);
                    }
                }
            }

            template<typename ValueType, bool SingleObjectiveMode>
            storm::models::sparse::Mdp<ValueType> const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getProduct() const {
                return *product;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            std::vector<typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::Epoch> const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getSteps() const {
                return steps;
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            bool MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to retrieve whether a product state exists but the memoryStateMap is not yet initialized.");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState] < getProduct().getNumberOfStates();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getProductState(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(productStateExists(modelState, memoryState), "Tried to obtain a state in the model-memory-product that does not exist");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState];
            }

            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getModelState(uint64_t const& productState) const {
                return productToModelStateMap[productState];
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getMemoryState(uint64_t const& productState) const {
                return productToMemoryStateMap[productState];
            }

            template<typename ValueType, bool SingleObjectiveMode>
            storm::storage::BitVector const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::convertMemoryState(uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to convert a memory state, but the memoryStateMap is not yet initialized.");
                return memoryStateMap[memoryState];
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::convertMemoryState(storm::storage::BitVector const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to convert a memory state, but the memoryStateMap is not yet initialized.");
                auto memStateIt = std::find(memoryStateMap.begin(), memoryStateMap.end(), memoryState);
                return memStateIt - memoryStateMap.begin();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getNumberOfMemoryState() const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to retrieve the number of memory states but the memoryStateMap is not yet initialized.");
                return memoryStateMap.size();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            uint64_t MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::MemoryProduct::getProductStateFromChoice(uint64_t const& productChoice) const {
                return choiceToStateMap[productChoice];
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            std::vector<std::vector<ValueType>> MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::computeObjectiveRewardsForProduct(Epoch const& epoch) const {
                std::vector<std::vector<ValueType>> objectiveRewards;
                objectiveRewards.reserve(objectives.size());
                
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula()) {
                        storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(memoryProduct.getProduct());
                        std::vector<uint64_t> dimensionIndexMap;
                        for (auto const& globalDimensionIndex : objectiveDimensions[objIndex]) {
                            dimensionIndexMap.push_back(globalDimensionIndex);
                        }
                        
                        std::shared_ptr<storm::logic::Formula const> sinkStatesFormula;
                        for (auto const& dim : objectiveDimensions[objIndex]) {
                            auto memLabelFormula = std::make_shared<storm::logic::AtomicLabelFormula>(memoryLabels[dim].get());
                            if (sinkStatesFormula) {
                                sinkStatesFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::Or, sinkStatesFormula, memLabelFormula);
                            } else {
                                sinkStatesFormula = memLabelFormula;
                            }
                        }
                        sinkStatesFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, sinkStatesFormula);
                        
                        std::vector<ValueType> objRew(memoryProduct.getProduct().getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        storm::storage::BitVector relevantObjectives(objectiveDimensions[objIndex].getNumberOfSetBits());
                        
                        while (!relevantObjectives.full()) {
                            relevantObjectives.increment();
                            
                            // find out whether objective reward should be earned within this epoch
                            bool collectRewardInEpoch = true;
                            for (auto const& subObjIndex : relevantObjectives) {
                                if (epochManager.isBottomDimension(epoch, dimensionIndexMap[subObjIndex])) {
                                    collectRewardInEpoch = false;
                                    break;
                                }
                            }
                            
                            if (collectRewardInEpoch) {
                                std::shared_ptr<storm::logic::Formula const> relevantStatesFormula;
                                std::shared_ptr<storm::logic::Formula const> goalStatesFormula =  storm::logic::CloneVisitor().clone(*sinkStatesFormula);
                                for (uint64_t subObjIndex = 0; subObjIndex < dimensionIndexMap.size(); ++subObjIndex) {
                                    std::shared_ptr<storm::logic::Formula> memLabelFormula = std::make_shared<storm::logic::AtomicLabelFormula>(memoryLabels[dimensionIndexMap[subObjIndex]].get());
                                    if (relevantObjectives.get(subObjIndex)) {
                                        auto rightSubFormula = subObjectives[dimensionIndexMap[subObjIndex]].first->asBoundedUntilFormula().getRightSubformula().asSharedPointer();
                                        goalStatesFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, goalStatesFormula, rightSubFormula);
                                    } else {
                                        memLabelFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, memLabelFormula);
                                    }
                                    if (relevantStatesFormula) {
                                        relevantStatesFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, relevantStatesFormula, memLabelFormula);
                                    } else {
                                        relevantStatesFormula = memLabelFormula;
                                    }
                                }
                                
                                storm::storage::BitVector relevantStates = mc.check(*relevantStatesFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                                storm::storage::BitVector relevantChoices = memoryProduct.getProduct().getTransitionMatrix().getRowFilter(relevantStates);
                                storm::storage::BitVector goalStates = mc.check(*goalStatesFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                                for (auto const& choice : relevantChoices) {
                                    objRew[choice] += memoryProduct.getProduct().getTransitionMatrix().getConstrainedRowSum(choice, goalStates);
                                }
                            }
                        }
                        
                        objectiveRewards.push_back(std::move(objRew));
                        
                    } else if (formula.isRewardOperatorFormula()) {
                        auto const& rewModel = memoryProduct.getProduct().getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                        STORM_LOG_THROW(!rewModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Reward model has transition rewards which is not expected.");
                        bool rewardCollectedInEpoch = true;
                        if (formula.getSubformula().isCumulativeRewardFormula()) {
                            assert(objectiveDimensions[objIndex].getNumberOfSetBits() == 1);
                            rewardCollectedInEpoch = !epochManager.isBottomDimension(epoch, *objectiveDimensions[objIndex].begin());
                        } else {
                            STORM_LOG_THROW(formula.getSubformula().isTotalRewardFormula(), storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                        }
                        if (rewardCollectedInEpoch) {
                            objectiveRewards.push_back(rewModel.getTotalRewardVector(memoryProduct.getProduct().getTransitionMatrix()));
                        } else {
                            objectiveRewards.emplace_back(memoryProduct.getProduct().getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                    }
                }
                
                return objectiveRewards;
            }
            
            template class MultiDimensionalRewardUnfolding<double, true>;
            template class MultiDimensionalRewardUnfolding<double, false>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber, true>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber, false>;
            
        }
    }
}