#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

#include <string>
#include <set>
#include <functional>

#include "storm/utility/macros.h"
#include "storm/logic/Formulas.h"

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
                            Dimension<ValueType> dimension;
                            dimension.formula = subformula.restrictToDimension(dim);
                            dimension.objectiveIndex = objIndex;
                            std::string memLabel = "dim" + std::to_string(dimensions.size()) + "_maybe";
                            while (model.getStateLabeling().containsLabel(memLabel)) {
                                memLabel = "_" + memLabel;
                            }
                            dimension.memoryLabel = memLabel;
                            dimension.isUpperBounded = subformula.hasUpperBound(dim);
                            // for simplicity we do not allow intervals or unbounded formulas.
                            STORM_LOG_THROW(subformula.hasLowerBound(dim) != dimension.isUpperBounded, storm::exceptions::NotSupportedException, "Bounded until formulas are only supported by this method if they consider either an upper bound or a lower bound. Got " << subformula << " instead.");
                            // lower bounded until formulas with non-trivial left hand side are excluded as this would require some additional effort (in particular the ProductModel::transformMemoryState method).
                            STORM_LOG_THROW(dimension.isUpperBounded || subformula.getLeftSubformula(dim).isTrueFormula(), storm::exceptions::NotSupportedException, "Lower bounded until formulas are only supported by this method if the left subformula is 'true'. Got " << subformula << " instead.");
                            if (subformula.getTimeBoundReference(dim).isTimeBound() || subformula.getTimeBoundReference(dim).isStepBound()) {
                                dimensionWiseEpochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                                dimension.scalingFactor = storm::utility::one<ValueType>();
                            } else {
                                STORM_LOG_ASSERT(subformula.getTimeBoundReference(dim).isRewardBound(), "Unexpected type of time bound.");
                                std::string const& rewardName = subformula.getTimeBoundReference(dim).getRewardName();
                                STORM_LOG_THROW(this->model.hasRewardModel(rewardName), storm::exceptions::IllegalArgumentException, "No reward model with name '" << rewardName << "' found.");
                                auto const& rewardModel = this->model.getRewardModel(rewardName);
                                STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported as reward bounds.");
                                std::vector<ValueType> actionRewards = rewardModel.getTotalRewardVector(this->model.getTransitionMatrix());
                                auto discretizedRewardsAndFactor = storm::utility::vector::toIntegralVector<ValueType, uint64_t>(actionRewards);
                                dimensionWiseEpochSteps.push_back(std::move(discretizedRewardsAndFactor.first));
                                dimension.scalingFactor = std::move(discretizedRewardsAndFactor.second);
                            }
                            dimensions.emplace_back(std::move(dimension));
                        }
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isCumulativeRewardFormula()) {
                        Dimension<ValueType> dimension;
                        dimension.formula = formula.getSubformula().asSharedPointer();
                        dimension.objectiveIndex = objIndex;
                        dimension.isUpperBounded = true;
                        dimension.scalingFactor = storm::utility::one<ValueType>();
                        dimensions.emplace_back(std::move(dimension));
                        dimensionWiseEpochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                    }
                }
                
                // Compute a mapping for each objective to the set of dimensions it considers
                // Also store for each dimension dim, which dimensions should be unsatisfiable whenever the bound of dim is violated
                uint64_t dim = 0;
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    storm::storage::BitVector objDimensions(dimensions.size(), false);
                    if (objectives[objIndex].formula->isProbabilityOperatorFormula() && objectives[objIndex].formula->getSubformula().isBoundedUntilFormula()) {
                        auto const& boundedUntilFormula = objectives[objIndex].formula->getSubformula().asBoundedUntilFormula();
                        for (uint64_t currDim = dim; currDim < dim + boundedUntilFormula.getDimension(); ++currDim ) {
                            objDimensions.set(currDim);
                        }
                        for (uint64_t currDim = dim; currDim < dim + boundedUntilFormula.getDimension(); ++currDim ) {
                            if (!boundedUntilFormula.hasMultiDimensionalSubformulas() || dimensions[currDim].isUpperBounded) {
                                dimensions[currDim].dependentDimensions = objDimensions;
                            } else {
                                dimensions[currDim].dependentDimensions = storm::storage::BitVector(dimensions.size(), false);
                                dimensions[currDim].dependentDimensions.set(currDim, true);
                            }
                            //  std::cout << "dimension " << currDim << " has depDims: " << dimensions[currDim].dependentDimensions << std::endl;
                        }
                        dim += boundedUntilFormula.getDimension();
                    } else if (objectives[objIndex].formula->isRewardOperatorFormula() && objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                        objDimensions.set(dim, true);
                        dimensions[dim].dependentDimensions = objDimensions;
                        ++dim;
                    }
                    
                    objectiveDimensions.push_back(std::move(objDimensions));
                }
                assert(dim == dimensions.size());
                
                
                // Initialize the epoch manager
                epochManager = EpochManager(dimensions.size());
                
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
                
                // Set the maximal values we need to consider for each dimension
                computeMaxDimensionValues();
                
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::initializeMemoryProduct(std::vector<Epoch> const& epochSteps) {
                productModel = std::make_unique<ProductModel<ValueType>>(model, objectives, dimensions, objectiveDimensions, epochManager, epochSteps);
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::computeMaxDimensionValues() {
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    storm::expressions::Expression bound;
                    bool isStrict = false;
                    storm::logic::Formula const& dimFormula = *dimensions[dim].formula;
                    if (dimFormula.isBoundedUntilFormula()) {
                        assert(!dimFormula.asBoundedUntilFormula().isMultiDimensional());
                        if (dimFormula.asBoundedUntilFormula().hasUpperBound()) {
                            STORM_LOG_ASSERT(!dimFormula.asBoundedUntilFormula().hasLowerBound(), "Bounded until formulas with interval bounds are not supported.");
                            bound = dimFormula.asBoundedUntilFormula().getUpperBound();
                            isStrict = dimFormula.asBoundedUntilFormula().isUpperBoundStrict();
                        } else {
                            STORM_LOG_ASSERT(dimFormula.asBoundedUntilFormula().hasLowerBound(), "Bounded until formulas without any bounds are not supported.");
                            bound = dimFormula.asBoundedUntilFormula().getLowerBound();
                            isStrict = dimFormula.asBoundedUntilFormula().isLowerBoundStrict();
                        }
                    } else if (dimFormula.isCumulativeRewardFormula()) {
                        bound = dimFormula.asCumulativeRewardFormula().getBound();
                        isStrict = dimFormula.asCumulativeRewardFormula().isBoundStrict();
                    }
                    STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::NotSupportedException, "The bound " << bound << " contains undefined constants.");
                    ValueType discretizedBound = storm::utility::convertNumber<ValueType>(bound.evaluateAsRational());
                    STORM_LOG_THROW(dimensions[dim].isUpperBounded || isStrict || !storm::utility::isZero(discretizedBound), storm::exceptions::NotSupportedException, "Lower bounds need to be either strict or greater than zero.");
                    discretizedBound /= dimensions[dim].scalingFactor;
                    if (storm::utility::isInteger(discretizedBound)) {
                        if (isStrict == dimensions[dim].isUpperBounded) {
                            discretizedBound -= storm::utility::one<ValueType>();
                        }
                    } else {
                        discretizedBound = storm::utility::floor(discretizedBound);
                    }
                    uint64_t dimensionValue = storm::utility::convertNumber<uint64_t>(discretizedBound);
                    STORM_LOG_THROW(epochManager.isValidDimensionValue(dimensionValue), storm::exceptions::NotSupportedException, "The bound " << bound << " is too high for the considered number of dimensions.");
                    dimensions[dim].maxValue = dimensionValue;
                }
            }
    
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::Epoch MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getStartEpoch() {
                Epoch startEpoch = epochManager.getZeroEpoch();
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    STORM_LOG_ASSERT(dimensions[dim].maxValue,  "No max-value for dimension " << dim << " was given.");
                    epochManager.setDimensionOfEpoch(startEpoch, dim, dimensions[dim].maxValue.get());
                }
                STORM_LOG_TRACE("Start epoch is " << epochManager.toString(startEpoch));
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
                bool containsLowerBoundedObjective = false;
                for (auto const& dimension : dimensions) {
                    if (!dimension.isUpperBounded) {
                        containsLowerBoundedObjective = true;
                        break;
                    }
                }
                
                epochModel.stepSolutions.resize(epochModel.stepChoices.getNumberOfSetBits());
                auto stepSolIt = epochModel.stepSolutions.begin();
                for (auto const& reducedChoice : epochModel.stepChoices) {
                    uint64_t productChoice = epochModelToProductChoiceMap[reducedChoice];
                    uint64_t productState = productModel->getProductStateFromChoice(productChoice);
                    auto const& memoryState = productModel->getMemoryState(productState);
                    Epoch successorEpoch = epochManager.getSuccessorEpoch(epoch, productModel->getSteps()[productChoice]);
                    EpochClass successorEpochClass = epochManager.getEpochClass(successorEpoch);
                    // Find out whether objective reward is earned for the current choice
                    // Objective reward is not earned if
                    // a) there is an upper bounded subObjective that is __still_relevant__ but the corresponding reward bound is passed after taking the choice
                    // b) there is a lower bounded subObjective and the corresponding reward bound is not passed yet.
                    for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                        bool rewardEarned = !storm::utility::isZero(epochModel.objectiveRewards[objIndex][reducedChoice]);
                        if (rewardEarned) {
                            for (auto const& dim : objectiveDimensions[objIndex]) {
                                if (dimensions[dim].isUpperBounded == epochManager.isBottomDimension(successorEpoch, dim) && productModel->getMemoryStateManager().isRelevantDimension(memoryState, dim)) {
                                    rewardEarned = false;
                                    break;
                                }
                            }
                        }
                        epochModel.objectiveRewardFilter[objIndex].set(reducedChoice, rewardEarned);
                    }
                    // compute the solution for the stepChoices
                    // For optimization purposes, we distinguish the case where the memory state does not have to be transformed
                    SolutionType choiceSolution;
                    bool firstSuccessor = true;
                    if (!containsLowerBoundedObjective && epochManager.compareEpochClass(epoch, successorEpoch)) {
                        for (auto const& successor : productModel->getProduct().getTransitionMatrix().getRow(productChoice)) {
                            if (firstSuccessor) {
                                choiceSolution = getScaledSolution(getStateSolution(successorEpoch, successor.getColumn()), successor.getValue());
                                firstSuccessor = false;
                            } else {
                                addScaledSolution(choiceSolution, getStateSolution(successorEpoch, successor.getColumn()), successor.getValue());
                            }
                        }
                    } else {
                        for (auto const& successor : productModel->getProduct().getTransitionMatrix().getRow(productChoice)) {
                            uint64_t successorProductState = productModel->transformProductState(successor.getColumn(), successorEpochClass, memoryState);
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
                EpochClass epochClass = epochManager.getEpochClass(epoch);
                // std::cout << "Setting epoch class for epoch " << epochManager.toString(epoch) << std::endl;
                swSetEpochClass.start();
                swAux1.start();
                auto productObjectiveRewards = productModel->computeObjectiveRewards(epochClass, objectives);
                
                storm::storage::BitVector stepChoices(productModel->getProduct().getNumberOfChoices(), false);
                uint64_t choice = 0;
                for (auto const& step : productModel->getSteps()) {
                    if (!epochManager.isZeroEpoch(step) && epochManager.getSuccessorEpoch(epoch, step) != epoch) {
                        stepChoices.set(choice, true);
                    }
                    ++choice;
                }
                epochModel.epochMatrix = productModel->getProduct().getTransitionMatrix().filterEntries(~stepChoices);
                // redirect transitions for the case where the lower reward bounds are not met yet
                storm::storage::BitVector violatedLowerBoundedDimensions(dimensions.size(), false);
                for (uint64_t dim = 0; dim < dimensions.size(); ++dim) {
                    if (!dimensions[dim].isUpperBounded && !epochManager.isBottomDimensionEpochClass(epochClass, dim)) {
                        violatedLowerBoundedDimensions.set(dim);
                    }
                }
                if (!violatedLowerBoundedDimensions.empty()) {
                    for (uint64_t state = 0; state < epochModel.epochMatrix.getRowGroupCount(); ++state) {
                        auto const& memoryState = productModel->getMemoryState(state);
                        for (auto& entry : epochModel.epochMatrix.getRowGroup(state)) {
                            entry.setColumn(productModel->transformProductState(entry.getColumn(), epochClass, memoryState));
                        }
                    }
                }
                
                storm::storage::BitVector zeroObjRewardChoices(productModel->getProduct().getNumberOfChoices(), true);
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    if (violatedLowerBoundedDimensions.isDisjointFrom(objectiveDimensions[objIndex])) {
                        zeroObjRewardChoices &= storm::utility::vector::filterZero(productObjectiveRewards[objIndex]);
                    }
                }
                storm::storage::BitVector allProductStates(productModel->getProduct().getNumberOfStates(), true);
                
                // Get the relevant states for this epoch.
                storm::storage::BitVector productInStates = productModel->getInStates(epochClass);
                // The epoch model only needs to consider the states that are reachable from a relevant state
                storm::storage::BitVector consideredStates = storm::utility::graph::getReachableStates(epochModel.epochMatrix, productInStates, allProductStates, ~allProductStates);
                // std::cout << "numInStates = " << productInStates.getNumberOfSetBits() << std::endl;
                // std::cout << "numConsideredStates = " << consideredStates.getNumberOfSetBits() << std::endl;
                
                // We assume that there is no end component in which objective reward is earned
                STORM_LOG_ASSERT(!storm::utility::graph::checkIfECWithChoiceExists(epochModel.epochMatrix, epochModel.epochMatrix.transpose(true), allProductStates, ~zeroObjRewardChoices & ~stepChoices), "There is a scheduler that yields infinite reward for one objective. This case should be excluded");
                swAux1.stop();
                swAux2.start();
                auto ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(epochModel.epochMatrix, consideredStates, zeroObjRewardChoices & ~stepChoices, consideredStates);
                swAux2.stop();
                swAux3.start();
                epochModel.epochMatrix = std::move(ecElimResult.matrix);
                epochModelToProductChoiceMap = std::move(ecElimResult.newToOldRowMapping);
                
                epochModel.stepChoices = storm::storage::BitVector(epochModel.epochMatrix.getRowCount(), false);
                for (uint64_t choice = 0; choice < epochModel.epochMatrix.getRowCount(); ++choice) {
                    if (stepChoices.get(epochModelToProductChoiceMap[choice])) {
                        epochModel.stepChoices.set(choice, true);
                    }
                }
                
                epochModel.objectiveRewards.clear();
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    std::vector<ValueType> const& productObjRew = productObjectiveRewards[objIndex];
                    std::vector<ValueType> reducedModelObjRewards;
                    reducedModelObjRewards.reserve(epochModel.epochMatrix.getRowCount());
                    for (auto const& productChoice : epochModelToProductChoiceMap) {
                        reducedModelObjRewards.push_back(productObjRew[productChoice]);
                    }
                    // Check if the objective is violated in the current epoch
                    if (!violatedLowerBoundedDimensions.isDisjointFrom(objectiveDimensions[objIndex])) {
                        storm::utility::vector::setVectorValues(reducedModelObjRewards, ~epochModel.stepChoices, storm::utility::zero<ValueType>());
                    }
                    epochModel.objectiveRewards.push_back(std::move(reducedModelObjRewards));
                }
                swAux3.stop();
                swAux4.start();
                
                epochModel.epochInStates = storm::storage::BitVector(epochModel.epochMatrix.getRowGroupCount(), false);
                for (auto const& productState : productInStates) {
                    STORM_LOG_ASSERT(ecElimResult.oldToNewStateMapping[productState] < epochModel.epochMatrix.getRowGroupCount(), "Selected product state does not exist in the epoch model.");
                    epochModel.epochInStates.set(ecElimResult.oldToNewStateMapping[productState], true);
                }
                
                epochModelInStateToProductStatesMap.assign(epochModel.epochInStates.getNumberOfSetBits(), std::vector<uint64_t>());
                std::vector<uint64_t> toEpochModelInStatesMap(productModel->getProduct().getNumberOfStates(), std::numeric_limits<uint64_t>::max());
                for (auto const& productState : productInStates) {
                    toEpochModelInStatesMap[productState] = epochModel.epochInStates.getNumberOfSetBitsBeforeIndex(ecElimResult.oldToNewStateMapping[productState]);
                    epochModelInStateToProductStatesMap[epochModel.epochInStates.getNumberOfSetBitsBeforeIndex(ecElimResult.oldToNewStateMapping[productState])].push_back(productState);
                }
                productStateToEpochModelInStateMap = std::make_shared<std::vector<uint64_t> const>(std::move(toEpochModelInStatesMap));
                
                epochModel.objectiveRewardFilter.clear();
                for (auto const& objRewards : epochModel.objectiveRewards) {
                    epochModel.objectiveRewardFilter.push_back(storm::utility::vector::filterZero(objRewards));
                    epochModel.objectiveRewardFilter.back().complement();
                }

                swAux4.stop();
                swSetEpochClass.stop();
                epochModelSizes.push_back(epochModel.epochMatrix.getRowGroupCount());
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
                std::stringstream stringstream;
                stringstream << solution;
                return stringstream.str();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            template<bool SO, typename std::enable_if<!SO, int>::type>
            std::string MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::solutionToString(SolutionType const& solution) const {
                std::stringstream stringstream;
                stringstream << "(";
                bool first = true;
                for (auto const& s : solution) {
                    if (first) {
                        first = false;
                    } else {
                        stringstream << ", ";
                    }
                    stringstream << s;
                }
                stringstream << ")";
                return stringstream.str();
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            void MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::setSolutionForCurrentEpoch(std::vector<SolutionType>&& inStateSolutions) {
                swInsertSol.start();
                STORM_LOG_ASSERT(currentEpoch, "Tried to set a solution for the current epoch, but no epoch was specified before.");
                STORM_LOG_ASSERT(inStateSolutions.size() == epochModelInStateToProductStatesMap.size(), "Invalid number of solutions.");

                std::set<Epoch> predecessorEpochs, successorEpochs;
                for (auto const& step : possibleEpochSteps) {
                    epochManager.gatherPredecessorEpochs(predecessorEpochs, currentEpoch.get(), step);
                    successorEpochs.insert(epochManager.getSuccessorEpoch(currentEpoch.get(), step));
                }
                predecessorEpochs.erase(currentEpoch.get());
                successorEpochs.erase(currentEpoch.get());
                STORM_LOG_ASSERT(!predecessorEpochs.empty(), "There are no predecessors for the epoch " << epochManager.toString(currentEpoch.get()));
                
                // clean up solutions that are not needed anymore
                for (auto const& successorEpoch : successorEpochs) {
                    auto successorEpochSolutionIt = epochSolutions.find(successorEpoch);
                    STORM_LOG_ASSERT(successorEpochSolutionIt != epochSolutions.end(), "Solution for successor epoch does not exist (anymore).");
                    --successorEpochSolutionIt->second.count;
                    if (successorEpochSolutionIt->second.count == 0) {
                        epochSolutions.erase(successorEpochSolutionIt);
                    }
                }
                
                // add the new solution
                EpochSolution solution;
                solution.count = predecessorEpochs.size();
                solution.productStateToSolutionVectorMap = productStateToEpochModelInStateMap;
                solution.solutions = std::move(inStateSolutions);
                epochSolutions[currentEpoch.get()] = std::move(solution);
                
                maxSolutionsStored = std::max((uint64_t) epochSolutions.size(), maxSolutionsStored);
                swInsertSol.stop();
                
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getStateSolution(Epoch const& epoch, uint64_t const& productState) {
                auto epochSolutionIt = epochSolutions.find(epoch);
                STORM_LOG_ASSERT(epochSolutionIt != epochSolutions.end(), "Requested unexisting solution for epoch " << epochManager.toString(epoch) << ".");
                auto const& epochSolution = epochSolutionIt->second;
                STORM_LOG_ASSERT(productState < epochSolution.productStateToSolutionVectorMap->size(), "Requested solution for epoch " << epochManager.toString(epoch) << " at an unexisting product state.");
                STORM_LOG_ASSERT((*epochSolution.productStateToSolutionVectorMap)[productState] < epochSolution.solutions.size(), "Requested solution for epoch " << epochManager.toString(epoch) << " at a state for which no solution was stored.");
                return epochSolution.solutions[(*epochSolution.productStateToSolutionVectorMap)[productState]];
            }
            
            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getInitialStateResult(Epoch const& epoch) {
                STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1, "The model has multiple initial states.");
                STORM_LOG_ASSERT(!epochManager.hasBottomDimension(epoch), "Tried to get the initial state result in an epoch that still contains at least one bottom dimension.");
                return getStateSolution(epoch, productModel->getInitialProductState(*model.getInitialStates().begin(), model.getInitialStates()));
            }

            template<typename ValueType, bool SingleObjectiveMode>
            typename MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType, SingleObjectiveMode>::getInitialStateResult(Epoch const& epoch, uint64_t initialStateIndex) {
                STORM_LOG_ASSERT(model.getInitialStates().get(initialStateIndex), "The given model state is not an initial state.");
                STORM_LOG_ASSERT(!epochManager.hasBottomDimension(epoch), "Tried to get the initial state result in an epoch that still contains at least one bottom dimension.");
                return getStateSolution(epoch, productModel->getInitialProductState(initialStateIndex, model.getInitialStates()));
            }

            template class MultiDimensionalRewardUnfolding<double, true>;
            template class MultiDimensionalRewardUnfolding<double, false>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber, true>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber, false>;
            
        }
    }
}