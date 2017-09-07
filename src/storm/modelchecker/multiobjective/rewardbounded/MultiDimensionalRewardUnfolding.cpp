#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

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

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename ValueType>
            MultiDimensionalRewardUnfolding<ValueType>::MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives, storm::storage::BitVector const& possibleECActions, storm::storage::BitVector const& allowedBottomStates) : model(model), objectives(objectives), possibleECActions(possibleECActions), allowedBottomStates(allowedBottomStates) {
            
                initialize();
            }
    
    
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::initialize() {
                swInit.start();
                std::vector<std::vector<uint64_t>> epochSteps;
                initializeObjectives(epochSteps);
                initializePossibleEpochSteps(epochSteps);
                initializeMemoryProduct(epochSteps);
                swInit.stop();
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::initializeObjectives(std::vector<std::vector<uint64_t>>& epochSteps) {

                // collect the time-bounded subobjectives
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula()) {
                        std::vector<std::shared_ptr<storm::logic::Formula const>> subformulas;
                        if (formula.getSubformula().isBoundedUntilFormula()) {
                            subformulas.push_back(formula.getSubformula().asSharedPointer());
                        } else if (formula.getSubformula().isMultiObjectiveFormula()) {
                            subformulas = formula.getSubformula().asMultiObjectiveFormula().getSubformulas();
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unexpected type of subformula for formula " << formula);
                        }
                        for (auto const& subformula : subformulas) {
                            auto const& boundedUntilFormula = subformula->asBoundedUntilFormula();
                            for (uint64_t dim = 0; dim < boundedUntilFormula.getDimension(); ++dim) {
                                subObjectives.push_back(std::make_pair(boundedUntilFormula.restrictToDimension(dim), objIndex));
                                std::string memLabel = "dim" + std::to_string(subObjectives.size()) + "_maybe";
                                while (model.getStateLabeling().containsLabel(memLabel)) {
                                    memLabel = "_" + memLabel;
                                }
                                memoryLabels.push_back(memLabel);
                                if (boundedUntilFormula.getTimeBoundReference(dim).isTimeBound() || boundedUntilFormula.getTimeBoundReference(dim).isStepBound()) {
                                    epochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                                    scalingFactors.push_back(storm::utility::one<ValueType>());
                                } else {
                                    STORM_LOG_ASSERT(boundedUntilFormula.getTimeBoundReference(dim).isRewardBound(), "Unexpected type of time bound.");
                                    std::string const& rewardName = boundedUntilFormula.getTimeBoundReference(dim).getRewardName();
                                    STORM_LOG_THROW(this->model.hasRewardModel(rewardName), storm::exceptions::IllegalArgumentException, "No reward model with name '" << rewardName << "' found.");
                                    auto const& rewardModel = this->model.getRewardModel(rewardName);
                                    STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported as reward bounds.");
                                    std::vector<ValueType> actionRewards = rewardModel.getTotalRewardVector(this->model.getTransitionMatrix());
                                    auto discretizedRewardsAndFactor = storm::utility::vector::toIntegralVector<ValueType, uint64_t>(actionRewards);
                                    epochSteps.push_back(std::move(discretizedRewardsAndFactor.first));
                                    scalingFactors.push_back(std::move(discretizedRewardsAndFactor.second));
                                }
                            }

                        }
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isCumulativeRewardFormula()) {
                        subObjectives.push_back(std::make_pair(formula.getSubformula().asSharedPointer(), objIndex));
                        epochSteps.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
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
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::initializePossibleEpochSteps(std::vector<std::vector<uint64_t>> const& epochSteps) {
                // collect which epoch steps are possible
                possibleEpochSteps.clear();
                for (uint64_t choiceIndex = 0; choiceIndex < epochSteps.front().size(); ++choiceIndex) {
                    Epoch step;
                    step.reserve(epochSteps.size());
                    for (auto const& dimensionRewards : epochSteps) {
                        step.push_back(dimensionRewards[choiceIndex]);
                    }
                    possibleEpochSteps.insert(step);
                }
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::initializeMemoryProduct(std::vector<std::vector<uint64_t>> const& epochSteps) {
                
                // build the memory structure
                auto memoryStructure = computeMemoryStructure();
                
                // build a mapping between the different representations of memory states
                auto memoryStateMap = computeMemoryStateMap(memoryStructure);

                memoryProduct = MemoryProduct(model, memoryStructure, std::move(memoryStateMap), epochSteps, objectiveDimensions);
                
            }
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::Epoch MultiDimensionalRewardUnfolding<ValueType>::getStartEpoch() {
                Epoch startEpoch;
                for (uint64_t dim = 0; dim < this->subObjectives.size(); ++dim) {
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
                    startEpoch.push_back(storm::utility::convertNumber<uint64_t>(discretizedBound));
                    
                }
                return startEpoch;
            }
    
            template<typename ValueType>
            std::vector<typename MultiDimensionalRewardUnfolding<ValueType>::Epoch> MultiDimensionalRewardUnfolding<ValueType>::getEpochComputationOrder(Epoch const& startEpoch) {
                
                // perform DFS to get the 'reachable' epochs in the correct order.
                std::vector<Epoch> result, dfsStack;
                std::set<Epoch> seenEpochs;
                seenEpochs.insert(startEpoch);
                dfsStack.push_back(startEpoch);
                while (!dfsStack.empty()) {
                    bool hasUnseenSuccessor = false;
                    for (auto const& step : possibleEpochSteps) {
                        Epoch successorEpoch = getSuccessorEpoch(dfsStack.back(), step);
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
            }
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochModel const& MultiDimensionalRewardUnfolding<ValueType>::setCurrentEpoch(Epoch const& epoch) {
                // Check if we need to update the current epoch class
                if (!currentEpoch || getClassOfEpoch(epoch) != getClassOfEpoch(currentEpoch.get())) {
                    setCurrentEpochClass(epoch);
                }
                
                swSetEpoch.start();
                epochModel.objectiveRewardFilter = std::vector<storm::storage::BitVector>(objectives.size(), storm::storage::BitVector(epochModel.objectiveRewards.front().size(), true));
                epochModel.stepSolutions.resize(epochModel.stepChoices.getNumberOfSetBits());
                auto stepSolIt = epochModel.stepSolutions.begin();
                for (auto const& reducedChoice : epochModel.stepChoices) {
                    uint64_t productChoice = ecElimResult.newToOldRowMapping[reducedChoice];
                    uint64_t productState = memoryProduct.getProductStateFromChoice(productChoice);
                    auto memoryState = memoryProduct.convertMemoryState(memoryProduct.getMemoryState(productState));
                    Epoch successorEpoch = getSuccessorEpoch(epoch, memoryProduct.getSteps()[productChoice].get());
                    
                    // Find out whether objective reward is earned for the current choice
                    swAux1.start();
                    for (uint64_t dim = 0; dim < successorEpoch.size(); ++dim) {
                        if (successorEpoch[dim] < 0 && memoryState.get(dim)) {
                            epochModel.objectiveRewardFilter[subObjectives[dim].second].set(reducedChoice, false);
                        }
                    }
                    swAux1.stop();
                    
                    // compute the solution for the stepChoices
                    swAux2.start();
                    SolutionType choiceSolution = getZeroSolution();
                    if (getClassOfEpoch(epoch) == getClassOfEpoch(successorEpoch)) {
                        swAux3.start();
                        for (auto const& successor : memoryProduct.getProduct().getTransitionMatrix().getRow(productChoice)) {
                            addScaledSolution(choiceSolution, getStateSolution(successorEpoch, successor.getColumn()), successor.getValue());
                        }
                        swAux3.stop();
                    } else {
                        swAux4.start();
                        storm::storage::BitVector successorRelevantDimensions(successorEpoch.size(), true);
                        for (auto const& dim : memoryState) {
                            if (successorEpoch[dim] < 0) {
                                successorRelevantDimensions &= ~objectiveDimensions[subObjectives[dim].second];
                            }
                        }
                        for (auto const& successor : memoryProduct.getProduct().getTransitionMatrix().getRow(productChoice)) {
                            storm::storage::BitVector successorMemoryState = memoryProduct.convertMemoryState(memoryProduct.getMemoryState(successor.getColumn())) & successorRelevantDimensions;
                            uint64_t successorProductState = memoryProduct.getProductState(memoryProduct.getModelState(successor.getColumn()), memoryProduct.convertMemoryState(successorMemoryState));
                            SolutionType const& successorSolution = getStateSolution(successorEpoch, successorProductState);
                            addScaledSolution(choiceSolution, successorSolution, successor.getValue());
                        }
                        swAux4.stop();
                    }
                    swAux2.stop();
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
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setCurrentEpochClass(Epoch const& epoch) {
                // std::cout << "Setting epoch class for epoch " << storm::utility::vector::toString(epoch) << std::endl;
                swSetEpochClass.start();
                auto productObjectiveRewards = computeObjectiveRewardsForProduct(epoch);
                
                storm::storage::BitVector stepChoices(memoryProduct.getProduct().getNumberOfChoices(), false);
                uint64_t choice = 0;
                for (auto const& step : memoryProduct.getSteps()) {
                    if (step) {
                        auto eIt = epoch.begin();
                        for (auto const& s : step.get()) {
                            if (s != 0 && *eIt >= 0) {
                                stepChoices.set(choice, true);
                                break;
                            }
                            ++eIt;
                        }
                    }
                    ++choice;
                }
                epochModel.epochMatrix = memoryProduct.getProduct().getTransitionMatrix().filterEntries(~stepChoices);
                
                storm::storage::BitVector zeroObjRewardChoices(memoryProduct.getProduct().getNumberOfChoices(), true);
                for (auto const& objRewards : productObjectiveRewards) {
                    zeroObjRewardChoices &= storm::utility::vector::filterZero(objRewards);
                }
                
                storm::storage::BitVector allProductStates(memoryProduct.getProduct().getNumberOfStates(), true);
                
                // Get the relevant states for this epoch. These are all states
                storm::storage::BitVector productInStates = computeProductInStatesForEpochClass(epoch);
                // The epoch model only needs to consider the states that are reachable from a relevant state
                storm::storage::BitVector consideredStates = storm::utility::graph::getReachableStates(epochModel.epochMatrix, productInStates, allProductStates, ~allProductStates);
                // std::cout << "numInStates = " << productInStates.getNumberOfSetBits() << std::endl;
                // std::cout << "numConsideredStates = " << consideredStates.getNumberOfSetBits() << std::endl;
                
                // We assume that there is no end component in which objective reward is earned
                STORM_LOG_ASSERT(!storm::utility::graph::checkIfECWithChoiceExists(epochModel.epochMatrix, epochModel.epochMatrix.transpose(true), allProductStates, ~zeroObjRewardChoices & ~stepChoices), "There is a scheduler that yields infinite reward for one objective. This case should be excluded");
                ecElimResult = storm::transformer::EndComponentEliminator<ValueType>::transform(epochModel.epochMatrix, consideredStates, zeroObjRewardChoices & ~stepChoices, consideredStates);
                epochModel.epochMatrix = std::move(ecElimResult.matrix);
                
                epochModel.stepChoices = storm::storage::BitVector(epochModel.epochMatrix.getRowCount(), false);
                for (uint64_t choice = 0; choice < epochModel.epochMatrix.getRowCount(); ++choice) {
                    if (stepChoices.get(ecElimResult.newToOldRowMapping[choice])) {
                        epochModel.stepChoices.set(choice, true);
                    }
                }
                //STORM_LOG_ASSERT(epochModel.stepChoices.getNumberOfSetBits() == stepChoices.getNumberOfSetBits(), "The number of choices leading outside of the epoch does not match for the reduced and unreduced epochMatrix");
                
                epochModel.objectiveRewards.clear();
                for (auto const& productObjRew : productObjectiveRewards) {
                    std::vector<ValueType> reducedModelObjRewards;
                    reducedModelObjRewards.reserve(epochModel.epochMatrix.getRowCount());
                    for (auto const& productChoice : ecElimResult.newToOldRowMapping) {
                        reducedModelObjRewards.push_back(productObjRew[productChoice]);
                    }
                    epochModel.objectiveRewards.push_back(std::move(reducedModelObjRewards));
                }
                
                epochModel.inStates = storm::storage::BitVector(epochModel.epochMatrix.getRowGroupCount(), false);
                for (auto const& productState : productInStates) {
                    epochModel.inStates.set(ecElimResult.oldToNewStateMapping[productState], true);
                }
                
                swSetEpochClass.stop();
                epochModelSizes.push_back(epochModel.epochMatrix.getRowGroupCount());
            }
            
            template<typename ValueType>
            storm::storage::BitVector MultiDimensionalRewardUnfolding<ValueType>::computeProductInStatesForEpochClass(Epoch const& epoch) {
                storm::storage::SparseMatrix<ValueType> const& productMatrix = memoryProduct.getProduct().getTransitionMatrix();
                
                storm::storage::BitVector result(productMatrix.getRowGroupCount(), false);
                result.set(*memoryProduct.getProduct().getInitialStates().begin(), true);
                // Perform DFS
                storm::storage::BitVector reachableStates = result;
                std::vector<uint_fast64_t> stack(reachableStates.begin(), reachableStates.end());
                
                while (!stack.empty()) {
                    uint64_t state = stack.back();
                    stack.pop_back();
                
                    for (uint64_t choice = productMatrix.getRowGroupIndices()[state]; choice < productMatrix.getRowGroupIndices()[state + 1]; ++choice) {
                        auto const& choiceStep = memoryProduct.getSteps()[choice];
                        if (choiceStep) {
                            storm::storage::BitVector objectiveSet(objectives.size(), false);
                            for (uint64_t dim = 0; dim < epoch.size(); ++dim) {
                                if (epoch[dim] < 0 && choiceStep.get()[dim] > 0) {
                                    objectiveSet.set(subObjectives[dim].second);
                                }
                            }
                            
                            if (objectiveSet.empty()) {
                                for (auto const& choiceSuccessor : productMatrix.getRow(choice)) {
                                    result.set(choiceSuccessor.getColumn(), true);
                                    if (!reachableStates.get(choiceSuccessor.getColumn())) {
                                        reachableStates.set(choiceSuccessor.getColumn());
                                        stack.push_back(choiceSuccessor.getColumn());
                                    }
                                }
                            } else {
                                storm::storage::BitVector objectiveSubSet(objectiveSet.getNumberOfSetBits(), false);
                                do {
                                    for (auto const& choiceSuccessor : productMatrix.getRow(choice)) {
                                        uint64_t modelState = memoryProduct.getModelState(choiceSuccessor.getColumn());
                                        uint64_t memoryState = memoryProduct.getMemoryState(choiceSuccessor.getColumn());
                                        storm::storage::BitVector memoryStatePrimeBv = memoryProduct.convertMemoryState(memoryState);
                                        uint64_t i = 0;
                                        for (auto const& objIndex : objectiveSet) {
                                            if (objectiveSubSet.get(i)) {
                                                memoryStatePrimeBv &= ~objectiveDimensions[objIndex];
                                            }
                                            ++i;
                                        }
                                        uint64_t successorState = memoryProduct.getProductState(modelState, memoryProduct.convertMemoryState(memoryStatePrimeBv));
                                        result.set(successorState, true);
                                        if (!reachableStates.get(successorState)) {
                                            reachableStates.set(successorState);
                                            stack.push_back(successorState);
                                        }
                                    }
                                    
                                    objectiveSubSet.increment();
                                } while (!objectiveSubSet.empty());
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
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::SolutionType MultiDimensionalRewardUnfolding<ValueType>::getZeroSolution() const {
                SolutionType res;
                res.weightedValue = storm::utility::zero<ValueType>();
                res.objectiveValues = std::vector<ValueType>(objectives.size(), storm::utility::zero<ValueType>());
                return res;
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const {
                solution.weightedValue += solutionToAdd.weightedValue * scalingFactor;
                storm::utility::vector::addScaledVector(solution.objectiveValues,  solutionToAdd.objectiveValues, scalingFactor);
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setSolutionForCurrentEpoch(std::vector<SolutionType> const& inStateSolutions) {
                swInsertSol.start();
                for (uint64_t productState = 0; productState < memoryProduct.getProduct().getNumberOfStates(); ++productState) {
                    uint64_t reducedModelState = ecElimResult.oldToNewStateMapping[productState];
                    if (reducedModelState < epochModel.epochMatrix.getRowGroupCount() && epochModel.inStates.get(reducedModelState)) {
                        setSolutionForCurrentEpoch(productState, inStateSolutions[epochModel.inStates.getNumberOfSetBitsBeforeIndex(reducedModelState)]);
                    }
                }
                swInsertSol.stop();
            }
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType const& solution) {
                STORM_LOG_ASSERT(currentEpoch, "Tried to set a solution for the current epoch, but no epoch was specified before.");
                std::vector<int64_t> solutionKey = currentEpoch.get();
                solutionKey.push_back(productState);
                solutions[solutionKey] = solution;
            }
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType>::getStateSolution(Epoch const& epoch, uint64_t const& productState) {
                swFindSol.start();
                std::vector<int64_t> solutionKey = epoch;
                solutionKey.push_back(productState);
                auto solutionIt = solutions.find(solutionKey);
                STORM_LOG_ASSERT(solutionIt != solutions.end(), "Requested unexisting solution for epoch " << storm::utility::vector::toString(epoch) << ".");
                swFindSol.stop();
                return solutionIt->second;
            }
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::SolutionType const& MultiDimensionalRewardUnfolding<ValueType>::getInitialStateResult(Epoch const& epoch) {
                return getStateSolution(epoch, *memoryProduct.getProduct().getInitialStates().begin());
            }

            
            template<typename ValueType>
            storm::storage::MemoryStructure MultiDimensionalRewardUnfolding<ValueType>::computeMemoryStructure() const {
                
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
            
            template<typename ValueType>
            std::vector<storm::storage::BitVector> MultiDimensionalRewardUnfolding<ValueType>::computeMemoryStateMap(storm::storage::MemoryStructure const& memory) const {
                // Compute a mapping between the different representations of memory states
                std::vector<storm::storage::BitVector> result;
                result.reserve(memory.getNumberOfStates());
                for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++memState) {
                    storm::storage::BitVector relevantSubObjectives(memoryLabels.size(), false);
                    std::set<std::string> stateLabels = memory.getStateLabeling().getLabelsOfState(memState);
                    for (uint64_t dim = 0; dim < memoryLabels.size(); ++dim) {
                        if (memoryLabels[dim] && stateLabels.find(memoryLabels[dim].get()) != stateLabels.end()) {
                            relevantSubObjectives.set(dim, true);
                        }
                    }
                    result.push_back(std::move(relevantSubObjectives));
                }
                return result;
            }

            template<typename ValueType>
            MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::MemoryProduct(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<std::vector<uint64_t>> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) : memoryStateMap(std::move(memoryStateMap)) {
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
                steps.resize(getProduct().getNumberOfChoices());
                for (uint64_t modelState = 0; modelState < numModelStates; ++modelState) {
                    uint64_t numChoices = productBuilder.getOriginalModel().getTransitionMatrix().getRowGroupSize(modelState);
                    uint64_t firstChoice = productBuilder.getOriginalModel().getTransitionMatrix().getRowGroupIndices()[modelState];
                    for (uint64_t choiceOffset = 0; choiceOffset < numChoices; ++choiceOffset) {
                        Epoch step;
                        bool isZeroStep = true;
                        for (uint64_t dim = 0; dim < originalModelSteps.size(); ++dim) {
                            step.push_back(originalModelSteps[dim][firstChoice + choiceOffset]);
                            isZeroStep = isZeroStep && step.back() == 0;
                        }
                        if (!isZeroStep) {
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
            
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::setReachableStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<std::vector<uint64_t>> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) const {
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
                                        if (originalModelSteps[dim][choice] > 0) {
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

            template<typename ValueType>
            storm::models::sparse::Mdp<ValueType> const& MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getProduct() const {
                return *product;
            }
            
            template<typename ValueType>
            std::vector<boost::optional<typename MultiDimensionalRewardUnfolding<ValueType>::Epoch>> const& MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getSteps() const {
                return steps;
            }
            
            template<typename ValueType>
            bool MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to retrieve whether a product state exists but the memoryStateMap is not yet initialized.");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState] < getProduct().getNumberOfStates();
            }
            
            template<typename ValueType>
            uint64_t MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getProductState(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(productStateExists(modelState, memoryState), "Tried to obtain a state in the model-memory-product that does not exist");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState];
            }

            template<typename ValueType>
            uint64_t MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getModelState(uint64_t const& productState) const {
                return productToModelStateMap[productState];
            }
            
            template<typename ValueType>
            uint64_t MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getMemoryState(uint64_t const& productState) const {
                return productToMemoryStateMap[productState];
            }

            template<typename ValueType>
            storm::storage::BitVector const& MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::convertMemoryState(uint64_t const& memoryState) const {
                return memoryStateMap[memoryState];
            }
            
            template<typename ValueType>
            uint64_t MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::convertMemoryState(storm::storage::BitVector const& memoryState) const {
                auto memStateIt = std::find(memoryStateMap.begin(), memoryStateMap.end(), memoryState);
                return memStateIt - memoryStateMap.begin();
            }
            
            template<typename ValueType>
            uint64_t MultiDimensionalRewardUnfolding<ValueType>::MemoryProduct::getProductStateFromChoice(uint64_t const& productChoice) const {
                return choiceToStateMap[productChoice];
            }
            
            template<typename ValueType>
            std::vector<std::vector<ValueType>> MultiDimensionalRewardUnfolding<ValueType>::computeObjectiveRewardsForProduct(Epoch const& epoch) const {
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
                                if (epoch[dimensionIndexMap[subObjIndex]] < 0) {
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
                            rewardCollectedInEpoch = epoch[*objectiveDimensions[objIndex].begin()] >= 0;
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
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochClass MultiDimensionalRewardUnfolding<ValueType>::getClassOfEpoch(Epoch const& epoch) const {
                // Get a BitVector that is 1 wherever the epoch is non-negative
                storm::storage::BitVector classAsBitVector(epoch.size(), false);
                uint64_t i = 0;
                for (auto const& e : epoch) {
                    if (e >= 0) {
                        classAsBitVector.set(i, true);
                    }
                    ++i;
                }
                return classAsBitVector.getAsInt(0, epoch.size());
            }
            
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::Epoch MultiDimensionalRewardUnfolding<ValueType>::getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const {
                assert(epoch.size() == step.size());
                Epoch result;
                result.reserve(epoch.size());
                auto stepIt = step.begin();
                for (auto const& e : epoch) {
                    result.push_back(std::max((int64_t) -1, e - *stepIt));
                    ++stepIt;
                }
                return result;
            }

            
            template class MultiDimensionalRewardUnfolding<double>;
            template class MultiDimensionalRewardUnfolding<storm::RationalNumber>;
            
        }
    }
}