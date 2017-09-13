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
            ProductModel<ValueType>::ProductModel(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<storm::storage::BitVector> const& objectiveDimensions, EpochManager const& epochManager, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<Epoch> const& originalModelSteps) : objectiveDimensions(objectiveDimensions), epochManager(epochManager), memoryStateMap(std::move(memoryStateMap)) {
                
                storm::storage::SparseModelMemoryProduct<ValueType> productBuilder(memory.product(model));
                
                setReachableStates(productBuilder, originalModelSteps);
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
            
            template<typename ValueType>
            void ProductModel<ValueType>::setReachableStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps) const {
                
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
            std::vector<std::vector<ValueType>> ProductModel<ValueType>::computeObjectiveRewards(Epoch const& epoch, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives, std::vector<std::pair<std::shared_ptr<storm::logic::Formula const>, uint64_t>> subObjectives, std::vector<boost::optional<std::string>> const& memoryLabels) const {
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
                            auto memLabelFormula = std::make_shared<storm::logic::AtomicLabelFormula>(memoryLabels[dim].get());
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
                            rewardCollectedInEpoch = !epochManager.isBottomDimension(epoch, *objectiveDimensions[objIndex].begin());
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
            storm::storage::BitVector ProductModel<ValueType>::computeInStates(Epoch const& epoch) const {
                storm::storage::SparseMatrix<ValueType> const& productMatrix = getProduct().getTransitionMatrix();
                
                // Initialize the result. Initial states are only considered if the epoch contains no bottom dimension.
                storm::storage::BitVector result;
                if (epochManager.hasBottomDimension(epoch)) {
                    result = storm::storage::BitVector(getProduct().getNumberOfStates());
                } else {
                    result = getProduct().getInitialStates();
                }
                
                // Compute the set of objectives that can not be satisfied anymore in the current epoch
                storm::storage::BitVector irrelevantObjectives(objectiveDimensions.size(), false);
                for (uint64_t objIndex = 0; objIndex < objectiveDimensions.size(); ++objIndex) {
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
                storm::storage::BitVector reachableStates = getProduct().getInitialStates();
                std::vector<uint_fast64_t> stack(reachableStates.begin(), reachableStates.end());
                
                while (!stack.empty()) {
                    uint64_t state = stack.back();
                    stack.pop_back();
                    for (uint64_t choice = productMatrix.getRowGroupIndices()[state]; choice < productMatrix.getRowGroupIndices()[state + 1]; ++choice) {
                        auto const& choiceStep = getSteps()[choice];
                        if (!epochManager.isZeroEpoch(choiceStep)) {
 
                            // Compute the set of objectives that might or might not become irrelevant when the epoch is reached via the current choice
                            storm::storage::BitVector maybeIrrelevantObjectives(objectiveDimensions.size(), false);
                            for (uint64_t objIndex = 0; objIndex < objectiveDimensions.size(); ++objIndex) {
                                for (auto const& dim : objectiveDimensions[objIndex]) {
                                    if (epochManager.isBottomDimension(epoch, dim) && epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                        maybeIrrelevantObjectives.set(objIndex);
                                        break;
                                    }
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
                                        storm::storage::BitVector successorMemoryState = convertMemoryState(getMemoryState(choiceSuccessor.getColumn()));
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
                                            uint64_t successorState = getProductState(getModelState(choiceSuccessor.getColumn()), convertMemoryState(successorMemoryState));
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
            
            template class ProductModel<double>;
            template class ProductModel<storm::RationalNumber>;
            
        }
    }
}