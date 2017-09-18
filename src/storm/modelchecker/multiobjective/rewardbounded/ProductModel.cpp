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

                auto const& memory = productBuilder.getMemory();
                auto const& model = productBuilder.getOriginalModel();
                auto const& modelTransitions = model.getTransitionMatrix();
                
                std::vector<storm::storage::BitVector> reachableStates(memory.getNumberOfStates(), storm::storage::BitVector(model.getNumberOfStates(), false));
                
                // Initialize the reachable states with the initial states
                EpochClass initEpochClass = epochManager.getEpochClass(epochManager.getZeroEpoch());
                storm::storage::BitVector initMemState(epochManager.getDimensionCount(), true);
                auto memStateIt = memory.getInitialMemoryStates().begin();
                for (auto const& initState : model.getInitialStates()) {
                    uint64_t transformedMemoryState = transformMemoryState(*memStateIt, initEpochClass, convertMemoryState(initMemState));
                    reachableStates[transformedMemoryState].set(initState, true);
                    ++memStateIt;
                }
                assert(memStateIt == memory.getInitialMemoryStates().end());

                // Find the reachable epoch classes
                std::set<Epoch> possibleSteps(originalModelSteps.begin(), originalModelSteps.end());
                std::set<EpochClass, std::function<bool(EpochClass const&, EpochClass const&)>> reachableEpochClasses(std::bind(&EpochManager::epochClassOrder, &epochManager, std::placeholders::_1, std::placeholders::_2));
                collectReachableEpochClasses(reachableEpochClasses, possibleSteps);
                
 
                // Iterate over all epoch classes starting from the initial one (i.e., no bottom dimension).
                for (auto epochClassIt = reachableEpochClasses.rbegin(); epochClassIt != reachableEpochClasses.rend(); ++epochClassIt) {
                    auto const& epochClass = *epochClassIt;
                    
                    // Find the remaining set of reachable states via DFS.
                    std::vector<std::pair<uint64_t, uint64_t>> dfsStack;
                    for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++memState) {
                        for (auto const& modelState : reachableStates[memState]) {
                            dfsStack.emplace_back(modelState, memState);
                        }
                    }
                
                    while (!dfsStack.empty()) {
                        uint64_t currentModelState = dfsStack.back().first;
                        uint64_t currentMemoryState = dfsStack.back().second;
                        dfsStack.pop_back();
                        
                        for (uint64_t choice = modelTransitions.getRowGroupIndices()[currentModelState]; choice != modelTransitions.getRowGroupIndices()[currentModelState + 1]; ++choice) {
                            
                            for (auto transitionIt = modelTransitions.getRow(choice).begin(); transitionIt < modelTransitions.getRow(choice).end(); ++transitionIt) {
                                
                                uint64_t successorMemoryState = memory.getSuccessorMemoryState(currentMemoryState, transitionIt - modelTransitions.begin());
                                successorMemoryState = transformMemoryState(successorMemoryState, epochClass, currentMemoryState);
                                if (!reachableStates[successorMemoryState].get(transitionIt->getColumn())) {
                                    reachableStates[successorMemoryState].set(transitionIt->getColumn(), true);
                                    dfsStack.emplace_back(transitionIt->getColumn(), successorMemoryState);
                                }
                            }
                        }
                    }
                }
                for (uint64_t memState = 0; memState < memoryStateMap.size(); ++memState) {
                    for (auto const& modelState : reachableStates[memState]) {
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
                                if (dimensions[dimensionIndexMap[subObjIndex]].isUpperBounded && epochManager.isBottomDimensionEpochClass(epochClass, dimensionIndexMap[subObjIndex])) {
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

                collectReachableEpochClasses(reachableEpochClasses, possibleSteps);
                
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
            void ProductModel<ValueType>::collectReachableEpochClasses(std::set<EpochClass, std::function<bool(EpochClass const&, EpochClass const&)>>& reachableEpochClasses, std::set<Epoch> const& possibleSteps) const {
                
                Epoch startEpoch = epochManager.getZeroEpoch();
                for (uint64_t dim = 0; dim < epochManager.getDimensionCount(); ++dim) {
                    STORM_LOG_ASSERT(dimensions[dim].maxValue,  "No max-value for dimension " << dim << " was given.");
                    epochManager.setDimensionOfEpoch(startEpoch, dim, dimensions[dim].maxValue.get());
                }
                
                std::set<Epoch> seenEpochs({startEpoch});
                std::vector<Epoch> dfsStack({startEpoch});
                
                reachableEpochClasses.insert(epochManager.getEpochClass(startEpoch));

                // Perform a DFS to find all the reachable epochs
                while (!dfsStack.empty()) {
                    Epoch currentEpoch = dfsStack.back();
                    dfsStack.pop_back();
                    for (auto const& step : possibleSteps) {
                        Epoch successorEpoch = epochManager.getSuccessorEpoch(currentEpoch, step);
                        if (seenEpochs.insert(successorEpoch).second) {
                            reachableEpochClasses.insert(epochManager.getEpochClass(successorEpoch));
                            dfsStack.push_back(std::move(successorEpoch));
                        }
                    }
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
                
                storm::storage::BitVector ecInStates(getProduct().getNumberOfStates(), false);
                
                if (!epochManager.hasBottomDimensionEpochClass(epochClass)) {
                    uint64_t initMemState = convertMemoryState(storm::storage::BitVector(epochManager.getDimensionCount(), true));
                    for (auto const& initState : getProduct().getInitialStates()) {
                        uint64_t transformedInitState = transformProductState(initState, epochClass, initMemState);
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
                    storm::storage::BitVector predecessorStates = reachableStates.find(predecessor)->second;
                    for (auto const& predecessorState : predecessorStates) {
                        uint64_t predecessorMemoryState = getMemoryState(predecessorState);
                        for (uint64_t choice = getProduct().getTransitionMatrix().getRowGroupIndices()[predecessorState]; choice < getProduct().getTransitionMatrix().getRowGroupIndices()[predecessorState + 1]; ++choice) {
                            bool choiceLeadsToThisClass = false;
                            Epoch const& choiceStep = getSteps()[choice];
                            for (auto const& dim : positiveStepDimensions) {
                                if (epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                    choiceLeadsToThisClass = true;
                                }
                            }
                            
                            if (choiceLeadsToThisClass) {
                                for (auto const& transition : getProduct().getTransitionMatrix().getRow(choice)) {
                                    uint64_t successorState = transformProductState(transition.getColumn(), epochClass, predecessorMemoryState);

                                    ecInStates.set(successorState, true);
                                }
                            }
                        }
                    }
                }
                
                // Find all states reachable from an InState via DFS.
                storm::storage::BitVector ecReachableStates = ecInStates;
                std::vector<uint64_t> dfsStack(ecReachableStates.begin(), ecReachableStates.end());
                
                while (!dfsStack.empty()) {
                    uint64_t currentState = dfsStack.back();
                    uint64_t currentMemoryState = getMemoryState(currentState);
                    dfsStack.pop_back();
                    
                    for (uint64_t choice = getProduct().getTransitionMatrix().getRowGroupIndices()[currentState]; choice != getProduct().getTransitionMatrix().getRowGroupIndices()[currentState + 1]; ++choice) {
                        
                        bool choiceLeadsOutsideOfEpoch = false;
                        Epoch const& choiceStep = getSteps()[choice];
                        for (auto const& dim : nonBottomDimensions) {
                            if (epochManager.getDimensionOfEpoch(choiceStep, dim) > 0) {
                                choiceLeadsOutsideOfEpoch = true;
                                break;
                            }
                        }
                        
                        for (auto const& transition : getProduct().getTransitionMatrix().getRow(choice)) {
                            uint64_t successorState = transformProductState(transition.getColumn(), epochClass, currentMemoryState);
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
            uint64_t ProductModel<ValueType>::transformMemoryState(uint64_t const& memoryState, EpochClass const& epochClass, uint64_t const& predecessorMemoryState) const {
                storm::storage::BitVector memoryStateBv = convertMemoryState(memoryState);
                storm::storage::BitVector const& predecessorMemoryStateBv = convertMemoryState(predecessorMemoryState);
                
                for (auto const& objDimensions : objectiveDimensions) {
                    for (auto const& dim : objDimensions) {
                        auto const& dimension = dimensions[dim];
                        bool dimUpperBounded = dimension.isUpperBounded;
                        bool dimBottom = epochManager.isBottomDimensionEpochClass(epochClass, dim);
                        if (dimUpperBounded && dimBottom && predecessorMemoryStateBv.get(dim)) {
                            STORM_LOG_ASSERT(objDimensions == dimension.dependentDimensions, "Unexpected set of dependent dimensions");
                            memoryStateBv &= ~objDimensions;
                            break;
                        } else if (!dimUpperBounded && !dimBottom && predecessorMemoryStateBv.get(dim)) {
                            memoryStateBv |= dimension.dependentDimensions;
                        }
                    }
                }
    
                return convertMemoryState(memoryStateBv);
            }

            template<typename ValueType>
            uint64_t ProductModel<ValueType>::transformProductState(uint64_t const& productState, EpochClass const& epochClass, uint64_t const& predecessorMemoryState) const {
                return getProductState(getModelState(productState), transformMemoryState(getMemoryState(productState), epochClass, predecessorMemoryState));
            }
            
            template class ProductModel<double>;
            template class ProductModel<storm::RationalNumber>;
            
        }
    }
}