#include "storm/modelchecker/multiobjective/rewardbounded/ProductModel.h"


#include "storm/utility/macros.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename ValueType>
            ProductModel<ValueType>::ProductModel(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<Dimension<ValueType>> const& dimensions, std::vector<storm::storage::BitVector> const& objectiveDimensions, EpochManager const& epochManager, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<Epoch> const& originalModelSteps) : dimensions(dimensions), objectiveDimensions(objectiveDimensions), epochManager(epochManager), memoryStateMap(std::move(memoryStateMap)) {
                
                storm::storage::SparseModelMemoryProduct<ValueType> productBuilder(memory.product(model));
                
                setReachableProductStates(productBuilder, originalModelSteps);
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
                
                computeReachableStatesInEpochClasses();
            }
            
            template<typename ValueType>
            void ProductModel<ValueType>::setReachableProductStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps) const {
                
                storm::storage::BitVector lowerBoundedDimensions(dimensions.size(), false);
                for (uint64_t dim = 0; dim < dimensions.size(); ++dim) {
                    if (!dimensions[dim].isUpperBounded) {
                        lowerBoundedDimensions.set(dim);
                    }
                }
                
                // We add additional reachable states until no more states are found
                std::vector<storm::storage::BitVector> additionalReachableStates(memoryStateMap.size(), storm::storage::BitVector(productBuilder.getOriginalModel().getNumberOfStates(), false));
                bool converged = false;
                while (!converged) {
                    for (uint64_t memState = 0; memState < memoryStateMap.size(); ++memState) {
                        auto const& memStateBv = memoryStateMap[memState];
                        storm::storage::BitVector consideredObjectives(objectiveDimensions.size(), false);
                        do {
                            // The current set of considered objectives can be skipped if it contains an objective that only consists of dimensions with lower reward bounds
                            bool skipThisObjectiveSet = false;
                            for (auto const& objindex : consideredObjectives) {
                                if (objectiveDimensions[objindex].isSubsetOf(lowerBoundedDimensions)) {
                                    skipThisObjectiveSet = true;
                                }
                            }
                            if (!skipThisObjectiveSet) {
                                storm::storage::BitVector memStatePrimeBv = memStateBv;
                                for (auto const& objIndex : consideredObjectives) {
                                    memStatePrimeBv &= ~objectiveDimensions[objIndex];
                                }
                                if (memStatePrimeBv != memStateBv) {
                                    uint64_t memStatePrime = convertMemoryState(memStatePrimeBv);
                                    for (uint64_t choice = 0; choice < productBuilder.getOriginalModel().getTransitionMatrix().getRowCount(); ++choice) {
                                        // Consider the choice only if for every considered objective there is one dimension for which the step of this choice is positive
                                        bool consideredChoice = true;
                                        for (auto const& objIndex : consideredObjectives) {
                                            bool objectiveHasStep = false;
                                            auto upperBoundedObjectiveDimensions = objectiveDimensions[objIndex] & (~lowerBoundedDimensions);
                                            for (auto const& dim : upperBoundedObjectiveDimensions) {
                                                if (epochManager.getDimensionOfEpoch(originalModelSteps[choice], dim) > 0) {
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
                                                if (productBuilder.isStateReachable(successor.getColumn(), memState) && !productBuilder.isStateReachable(successor.getColumn(), memStatePrime)) {
                                                    additionalReachableStates[memStatePrime].set(successor.getColumn());
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            consideredObjectives.increment();
                        } while (!consideredObjectives.empty());
                    }
                    
                    storm::storage::BitVector consideredDimensions(dimensions.size(), false);
                    do {
                        if (consideredDimensions.isSubsetOf(lowerBoundedDimensions)) {
                            for (uint64_t memoryState = 0; memoryState < productBuilder.getMemory().getNumberOfStates(); ++memoryState) {
                                uint64_t memoryStatePrime = convertMemoryState(convertMemoryState(memoryState) | consideredDimensions);
                                for (uint64_t modelState = 0; modelState < productBuilder.getOriginalModel().getNumberOfStates(); ++modelState) {
                                    if (productBuilder.isStateReachable(modelState, memoryState) && !productBuilder.isStateReachable(modelState, memoryStatePrime)) {
                                        additionalReachableStates[memoryStatePrime].set(modelState);
                                    }
                                }
                            }
                        }
                        consideredDimensions.increment();
                    } while (!consideredDimensions.empty());
                    
                    converged = true;
                    for (uint64_t memState = 0; memState < memoryStateMap.size(); ++memState) {
                        if (!additionalReachableStates[memState].empty()) {
                            converged = false;
                            for (auto const& modelState : additionalReachableStates[memState]) {
                                productBuilder.addReachableState(modelState, memState);
                            }
                            additionalReachableStates[memState].clear();
                        }
                    }
                }
            }

            template<typename ValueType>
            storm::models::sparse::Mdp<ValueType> const& ProductModel<ValueType>::getProduct() const {
                return *product;
            }
            
            template<typename ValueType>
            std::vector<typename ProductModel<ValueType>::Epoch> const& ProductModel<ValueType>::getSteps() const {
                return steps;
            }
            
            template<typename ValueType>
            bool ProductModel<ValueType>::productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to retrieve whether a product state exists but the memoryStateMap is not yet initialized.");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState] < getProduct().getNumberOfStates();
            }
            
            template<typename ValueType>
            uint64_t ProductModel<ValueType>::getProductState(uint64_t const& modelState, uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(productStateExists(modelState, memoryState), "Tried to obtain a state in the model-memory-product that does not exist");
                return modelMemoryToProductStateMap[modelState * memoryStateMap.size() + memoryState];
            }

            template<typename ValueType>
            uint64_t ProductModel<ValueType>::getModelState(uint64_t const& productState) const {
                return productToModelStateMap[productState];
            }
            
            template<typename ValueType>
            uint64_t ProductModel<ValueType>::getMemoryState(uint64_t const& productState) const {
                return productToMemoryStateMap[productState];
            }

            template<typename ValueType>
            storm::storage::BitVector const& ProductModel<ValueType>::convertMemoryState(uint64_t const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to convert a memory state, but the memoryStateMap is not yet initialized.");
                return memoryStateMap[memoryState];
            }
            
            template<typename ValueType>
            uint64_t ProductModel<ValueType>::convertMemoryState(storm::storage::BitVector const& memoryState) const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to convert a memory state, but the memoryStateMap is not yet initialized.");
                auto memStateIt = std::find(memoryStateMap.begin(), memoryStateMap.end(), memoryState);
                return memStateIt - memoryStateMap.begin();
            }
            
            template<typename ValueType>
            uint64_t ProductModel<ValueType>::getNumberOfMemoryState() const {
                STORM_LOG_ASSERT(!memoryStateMap.empty(), "Tried to retrieve the number of memory states but the memoryStateMap is not yet initialized.");
                return memoryStateMap.size();
            }
            
            template<typename ValueType>
            uint64_t ProductModel<ValueType>::getProductStateFromChoice(uint64_t const& productChoice) const {
                return choiceToStateMap[productChoice];
            }
            
            template<typename ValueType>
            std::vector<std::vector<ValueType>> ProductModel<ValueType>::computeObjectiveRewards(EpochClass const& epochClass, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives) const {
                std::vector<std::vector<ValueType>> objectiveRewards;
                objectiveRewards.reserve(objectives.size());
                
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    auto const& formula = *objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula()) {
                        storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(getProduct());
                        std::vector<uint64_t> dimensionIndexMap;
                        for (auto const& globalDimensionIndex : objectiveDimensions[objIndex]) {
                            dimensionIndexMap.push_back(globalDimensionIndex);
                        }
                        
                        std::shared_ptr<storm::logic::Formula const> sinkStatesFormula;
                        for (auto const& dim : objectiveDimensions[objIndex]) {
                            auto memLabelFormula = std::make_shared<storm::logic::AtomicLabelFormula>(dimensions[dim].memoryLabel.get());
                            if (sinkStatesFormula) {
                                sinkStatesFormula = std::make_shared<storm::logic::BinaryBooleanStateFormula>(storm::logic::BinaryBooleanStateFormula::OperatorType::Or, sinkStatesFormula, memLabelFormula);
                            } else {
                                sinkStatesFormula = memLabelFormula;
                            }
                        }
                        sinkStatesFormula = std::make_shared<storm::logic::UnaryBooleanStateFormula>(storm::logic::UnaryBooleanStateFormula::OperatorType::Not, sinkStatesFormula);
                        
                        std::vector<ValueType> objRew(getProduct().getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        storm::storage::BitVector relevantObjectives(objectiveDimensions[objIndex].getNumberOfSetBits());
                        
                        while (!relevantObjectives.full()) {
                            relevantObjectives.increment();
                            
                            // find out whether objective reward should be earned within this epoch class
                            bool collectRewardInEpoch = true;
                            for (auto const& subObjIndex : relevantObjectives) {
                                if (dimensions[dimensionIndexMap[subObjIndex]].isUpperBounded == epochManager.isBottomDimensionEpochClass(epochClass, dimensionIndexMap[subObjIndex])) {
                                    collectRewardInEpoch = false;
                                    break;
                                }
                            }
                            
                            if (collectRewardInEpoch) {
                                std::shared_ptr<storm::logic::Formula const> relevantStatesFormula;
                                std::shared_ptr<storm::logic::Formula const> goalStatesFormula =  storm::logic::CloneVisitor().clone(*sinkStatesFormula);
                                for (uint64_t subObjIndex = 0; subObjIndex < dimensionIndexMap.size(); ++subObjIndex) {
                                    std::shared_ptr<storm::logic::Formula> memLabelFormula = std::make_shared<storm::logic::AtomicLabelFormula>(dimensions[dimensionIndexMap[subObjIndex]].memoryLabel.get());
                                    if (relevantObjectives.get(subObjIndex)) {
                                        auto rightSubFormula = dimensions[dimensionIndexMap[subObjIndex]].formula->asBoundedUntilFormula().getRightSubformula().asSharedPointer();
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
                                storm::storage::BitVector relevantChoices = getProduct().getTransitionMatrix().getRowFilter(relevantStates);
                                storm::storage::BitVector goalStates = mc.check(*goalStatesFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                                for (auto const& choice : relevantChoices) {
                                    objRew[choice] += getProduct().getTransitionMatrix().getConstrainedRowSum(choice, goalStates);
                                }
                            }
                        }
                        
                        objectiveRewards.push_back(std::move(objRew));
                        
                    } else if (formula.isRewardOperatorFormula()) {
                        auto const& rewModel = getProduct().getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                        STORM_LOG_THROW(!rewModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Reward model has transition rewards which is not expected.");
                        bool rewardCollectedInEpoch = true;
                        if (formula.getSubformula().isCumulativeRewardFormula()) {
                            assert(objectiveDimensions[objIndex].getNumberOfSetBits() == 1);
                            rewardCollectedInEpoch = !epochManager.isBottomDimensionEpochClass(epochClass, *objectiveDimensions[objIndex].begin());
                        } else {
                            STORM_LOG_THROW(formula.getSubformula().isTotalRewardFormula(), storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                        }
                        if (rewardCollectedInEpoch) {
                            objectiveRewards.push_back(rewModel.getTotalRewardVector(getProduct().getTransitionMatrix()));
                        } else {
                            objectiveRewards.emplace_back(getProduct().getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                    }
                }
                
                return objectiveRewards;
            }
            
            template<typename ValueType>
            storm::storage::BitVector const& ProductModel<ValueType>::getInStates(EpochClass const& epochClass) const {
                STORM_LOG_ASSERT(inStates.find(epochClass) != inStates.end(), "Could not find InStates for the given epoch class");
                return inStates.find(epochClass)->second;
            }

            template<typename ValueType>
            void ProductModel<ValueType>::computeReachableStatesInEpochClasses() {
                std::set<Epoch> possibleSteps(steps.begin(), steps.end());
                std::set<EpochClass, std::function<bool(EpochClass const&, EpochClass const&)>> reachableEpochClasses(std::bind(&EpochManager::epochClassOrder, &epochManager, std::placeholders::_1, std::placeholders::_2));
                
                std::vector<Epoch> candidates({epochManager.getBottomEpoch()});
                std::set<Epoch> newCandidates;
                bool converged = false;
                while (!converged) {
                    converged = true;
                    for (auto const& candidate : candidates) {
                        converged &= !reachableEpochClasses.insert(epochManager.getEpochClass(candidate)).second;
                        for (auto const& step : possibleSteps) {
                            epochManager.gatherPredecessorEpochs(newCandidates, candidate, step);
                        }
                    }
                    candidates.assign(newCandidates.begin(), newCandidates.end());
                    newCandidates.clear();
                }
                
                for (auto epochClassIt = reachableEpochClasses.rbegin(); epochClassIt != reachableEpochClasses.rend(); ++epochClassIt) {
                    std::vector<EpochClass> predecessors;
                    for (auto predecessorIt = reachableEpochClasses.rbegin(); predecessorIt != epochClassIt; ++predecessorIt) {
                        if (epochManager.isPredecessorEpochClass(*predecessorIt, *epochClassIt)) {
                            predecessors.push_back(*predecessorIt);
                        }
                    }
                    computeReachableStates(*epochClassIt, predecessors);
                }
            }

            
            template<typename ValueType>
            void ProductModel<ValueType>::computeReachableStates(EpochClass const& epochClass, std::vector<EpochClass> const& predecessors) {
                
                storm::storage::BitVector bottomDimensions(epochManager.getDimensionCount(), false);
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    if (epochManager.isBottomDimensionEpochClass(epochClass, dim)) {
                        bottomDimensions.set(dim, true);
                    }
                }
                storm::storage::BitVector nonBottomDimensions = ~bottomDimensions;
                
                // Bottom dimensions corresponding to upper bounded subobjectives can not be relevant anymore
                // Dimensions with a lower bound where the epoch class is not bottom should stay relevant
                storm::storage::BitVector allowedRelevantDimensions(epochManager.getDimensionCount(), true);
                storm::storage::BitVector forcedRelevantDimensions(epochManager.getDimensionCount(), false);
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    if (dimensions[dim].isUpperBounded && bottomDimensions.get(dim)) {
                        allowedRelevantDimensions.set(dim, false);
                    } else if (!dimensions[dim].isUpperBounded && nonBottomDimensions.get(dim)) {
                        forcedRelevantDimensions.set(dim, false);
                    }
                }
                assert(forcedRelevantDimensions.isSubsetOf(allowedRelevantDimensions));
                
                storm::storage::BitVector ecInStates(getProduct().getNumberOfStates(), false);
                
                if (!epochManager.hasBottomDimensionEpochClass(epochClass)) {
                    for (auto const& initState : getProduct().getInitialStates()) {
                        uint64_t transformedInitState = transformProductState(initState, allowedRelevantDimensions, forcedRelevantDimensions);
                        ecInStates.set(transformedInitState, true);
                    }
                }
                
                for (auto const& predecessor : predecessors) {
                    storm::storage::BitVector positiveStepDimensions(epochManager.getDimensionCount(), false);
                    for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                        if (!epochManager.isBottomDimensionEpochClass(predecessor, dim) && bottomDimensions.get(dim)) {
                            positiveStepDimensions.set(dim, true);
                        }
                    }
                    STORM_LOG_ASSERT(reachableStates.find(predecessor) != reachableStates.end(), "Could not find reachable states of predecessor epoch class.");
                    storm::storage::BitVector predecessorChoices = getProduct().getTransitionMatrix().getRowFilter(reachableStates.find(predecessor)->second);
                    for (auto const& choice : predecessorChoices) {
                        bool choiceLeadsToThisClass = false;
                        Epoch const& choiceStep = getSteps()[choice];
                        for (auto const& dim : positiveStepDimensions) {
                            if (epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                choiceLeadsToThisClass = true;
                            }
                        }
                        
                        if (choiceLeadsToThisClass) {
                            for (auto const& transition : getProduct().getTransitionMatrix().getRow(choice)) {
                                uint64_t successorState = transformProductState(transition.getColumn(), allowedRelevantDimensions, forcedRelevantDimensions);
                                ecInStates.set(successorState, true);
                            }
                        }
                    }
                }
                
                // Find all states reachable from an InState via DFS.
                storm::storage::BitVector ecReachableStates = ecInStates;
                std::vector<uint64_t> dfsStack(ecReachableStates.begin(), ecReachableStates.end());
                
                while (!dfsStack.empty()) {
                    uint64_t currentState = dfsStack.back();
                    dfsStack.pop_back();
                    
                    for (uint64_t choice = getProduct().getTransitionMatrix().getRowGroupIndices()[currentState]; choice != getProduct().getTransitionMatrix().getRowGroupIndices()[currentState + 1]; ++choice) {
                        
                        bool choiceLeadsOutsideOfEpoch = false;
                        Epoch const& choiceStep = getSteps()[choice];
                        for (auto const& dim : nonBottomDimensions) {
                            if (epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                choiceLeadsOutsideOfEpoch = true;
                            }
                        }
                        
                        for (auto const& transition : getProduct().getTransitionMatrix().getRow(choice)) {
                            uint64_t successorState = transformProductState(transition.getColumn(), allowedRelevantDimensions, forcedRelevantDimensions);
                            if (choiceLeadsOutsideOfEpoch) {
                                ecInStates.set(successorState, true);
                            }
                            if (!ecReachableStates.get(successorState)) {
                                ecReachableStates.set(successorState, true);
                                dfsStack.push_back(successorState);
                            }
                        }
                    }
                }
                
                reachableStates[epochClass] = std::move(ecReachableStates);
                inStates[epochClass] = std::move(ecInStates);
            }

            template<typename ValueType>
            uint64_t ProductModel<ValueType>::transformProductState(uint64_t productState, storm::storage::BitVector const& allowedRelevantDimensions, storm::storage::BitVector const& forcedRelevantDimensions) const {
                return getProductState(getModelState(productState), transformMemoryState(getMemoryState(productState), allowedRelevantDimensions, forcedRelevantDimensions));
            }

            template<typename ValueType>
            uint64_t ProductModel<ValueType>::transformMemoryState(uint64_t memoryState, storm::storage::BitVector const& allowedRelevantDimensions, storm::storage::BitVector const& forcedRelevantDimensions) const {
                if (allowedRelevantDimensions.full() && forcedRelevantDimensions.empty()) {
                    return memoryState;
                }
                storm::storage::BitVector memoryStateBv = convertMemoryState(memoryState) | forcedRelevantDimensions;
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    if (!allowedRelevantDimensions.get(dim) && memoryStateBv.get(dim)) {
                        memoryStateBv &= ~objectiveDimensions[dimensions[dim].objectiveIndex];
                    }
                }
                return convertMemoryState(memoryStateBv);
            }

            
            template class ProductModel<double>;
            template class ProductModel<storm::RationalNumber>;
            
        }
    }
}