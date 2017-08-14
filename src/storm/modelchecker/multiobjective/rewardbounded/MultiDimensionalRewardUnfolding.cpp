#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

#include "storm/utility/macros.h"
#include "storm/logic/Formulas.h"
#include "storm/logic/CloneVisitor.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

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
                                std::string memLabel = "obj" + std::to_string(objIndex) + "-" + std::to_string(dim) + "_maybe";
                                while (model.getStateLabeling().containsLabel(memLabel)) {
                                    memLabel = "_" + memLabel;
                                }
                                memoryLabels.push_back(memLabel);
                                if (boundedUntilFormula.getTimeBoundReference(dim).isTimeBound() || boundedUntilFormula.getTimeBoundReference(dim).isStepBound()) {
                                    scaledRewards.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
                                    scalingFactors.push_back(storm::utility::one<ValueType>());
                                } else {
                                    STORM_LOG_ASSERT(boundedUntilFormula.getTimeBoundReference(dim).isRewardBound(), "Unexpected type of time bound.");
                                    std::string const& rewardName = boundedUntilFormula.getTimeBoundReference(dim).getRewardName();
                                    STORM_LOG_THROW(this->model.hasRewardModel(rewardName), storm::exceptions::IllegalArgumentException, "No reward model with name '" << rewardName << "' found.");
                                    auto const& rewardModel = this->model.getRewardModel(rewardName);
                                    STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are currently not supported as reward bounds.");
                                    std::vector<ValueType> actionRewards = rewardModel.getTotalRewardVector(this->model.getTransitionMatrix());
                                    auto discretizedRewardsAndFactor = storm::utility::vector::toIntegralVector<ValueType, uint64_t>(actionRewards);
                                    scaledRewards.push_back(std::move(discretizedRewardsAndFactor.first));
                                    scalingFactors.push_back(std::move(discretizedRewardsAndFactor.second));
                                }
                            }

                        }
                    } else if (formula.isRewardOperatorFormula() && formula.getSubformula().isCumulativeRewardFormula()) {
                        subObjectives.push_back(std::make_pair(formula.getSubformula().asSharedPointer(), objIndex));
                        scaledRewards.push_back(std::vector<uint64_t>(model.getNumberOfChoices(), 1));
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
                // collect which 'epoch steps' are possible
                std::set<Epoch> steps;
                for (uint64_t choiceIndex = 0; choiceIndex < scaledRewards.front().size(); ++choiceIndex) {
                    Epoch step;
                    step.reserve(scaledRewards.size());
                    for (auto const& dimensionRewards : scaledRewards) {
                        step.push_back(dimensionRewards[choiceIndex]);
                    }
                    steps.insert(step);
                }
                
                // perform DFS to get the 'reachable' epochs in the correct order.
                std::vector<Epoch> result, dfsStack;
                std::set<Epoch> seenEpochs;
                seenEpochs.insert(startEpoch);
                dfsStack.push_back(startEpoch);
                while (!dfsStack.empty()) {
                    Epoch currEpoch = std::move(dfsStack.back());
                    dfsStack.pop_back();
                    for (auto const& step : steps) {
                        Epoch successorEpoch = getSuccessorEpoch(currEpoch, step);
                        if (seenEpochs.find(successorEpoch) == seenEpochs.end()) {
                            seenEpochs.insert(successorEpoch);
                            dfsStack.push_back(std::move(successorEpoch));
                        }
                    }
                    result.push_back(std::move(currEpoch));
                }
                
                return result;
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochModel const& MultiDimensionalRewardUnfolding<ValueType>::getModelForEpoch(Epoch const& epoch) {
                
                // Get the model for the considered class of epochs
                EpochClass classOfEpoch = getClassOfEpoch(epoch);
                auto findRes = epochModels.find(classOfEpoch);
                std::shared_ptr<EpochModel> epochModel;
                if (findRes != epochModels.end()) {
                    epochModel = findRes->second;
                } else {
                    epochModel = epochModels.insert(std::make_pair(classOfEpoch, computeModelForEpoch(epoch))).first->second;
                }
                
                // Filter out all objective rewards that we do not receive in this particular epoch
                epochModel->objectiveRewardFilter.resize(objectives.size());
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    storm::storage::BitVector& filter = epochModel->objectiveRewardFilter[objIndex];
                    filter.resize(epochModel->objectiveRewards[objIndex].size());
                    if (objectiveDimensions[objIndex].empty()) {
                        filter.clear();
                        filter.complement();
                    } else {
                        for (uint64_t state = 0; state < epochModel->rewardTransitions.getRowGroupCount(); ++state) {
                            for (uint64_t choice = epochModel->rewardTransitions.getRowGroupIndices()[state]; choice < epochModel->rewardTransitions.getRowGroupIndices()[state + 1]; ++choice) {
                                for (auto const& dim : objectiveDimensions[objIndex]) {
                                    auto successorEpoch = epoch[dim] - (epochModel->epochSteps[choice].is_initialized() ? epochModel->epochSteps[choice].get()[dim] : 0);
                                    if (successorEpoch >= 0) {
                                        filter.set(choice, true);
                                    } else if (epochModel->relevantStates[dim].get(state)) {
                                        filter.set(choice, false);
                                        break;
                                    } else {
                                        filter.set(choice, true);
                                    }
                                }
                            }
                        }
                    }
                }
                
                return *epochModel;
            }
            
            template<typename ValueType>
            std::shared_ptr<typename MultiDimensionalRewardUnfolding<ValueType>::EpochModel>  MultiDimensionalRewardUnfolding<ValueType>::computeModelForEpoch(Epoch const& epoch) {
                
                storm::storage::MemoryStructure memory = computeMemoryStructureForEpoch(epoch);
                auto modelMemoryProductBuilder = memory.product(model);
                modelMemoryProductBuilder.setBuildFullProduct();
                auto modelMemoryProduct = modelMemoryProductBuilder.build()->template as<storm::models::sparse::Mdp<ValueType>>();
                
                storm::storage::SparseMatrix<ValueType> const& allTransitions = modelMemoryProduct->getTransitionMatrix();
                std::shared_ptr<EpochModel> result = std::make_shared<EpochModel>();
                storm::storage::BitVector rewardChoices(allTransitions.getRowCount(), false);
                result->epochSteps.resize(modelMemoryProduct->getNumberOfChoices());
                for (uint64_t modelState = 0; modelState < model.getNumberOfStates(); ++modelState) {
                    uint64_t numChoices = model.getTransitionMatrix().getRowGroupSize(modelState);
                    uint64_t firstChoice = model.getTransitionMatrix().getRowGroupIndices()[modelState];
                    for (uint64_t choiceOffset = 0; choiceOffset < numChoices; ++choiceOffset) {
                        Epoch step;
                        bool isZeroStep = true;
                        for (uint64_t dim = 0; dim < epoch.size(); ++dim) {
                            step.push_back(scaledRewards[dim][firstChoice + choiceOffset]);
                            isZeroStep = isZeroStep && (step.back() == 0 || epoch[dim] < 0);
                        }
                        if (!isZeroStep) {
                            for (uint64_t memState = 0; memState < memory.getNumberOfStates(); ++memState) {
                                uint64_t productState = modelMemoryProductBuilder.getResultState(modelState, memState);
                                uint64_t productChoice = allTransitions.getRowGroupIndices()[productState] + choiceOffset;
                                assert(productChoice < allTransitions.getRowGroupIndices()[productState + 1]);
                                result->epochSteps[productChoice] = step;
                                rewardChoices.set(productChoice, true);
                            }
                        }
                    }
                }
                
                result->rewardTransitions = allTransitions.filterEntries(rewardChoices);
                result->intermediateTransitions = allTransitions.filterEntries(~rewardChoices);
                
                result->objectiveRewards = computeObjectiveRewardsForEpoch(epoch, modelMemoryProduct);
 
                storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(*modelMemoryProduct);
                result->relevantStates.reserve(subObjectives.size());
                for (auto const& relevantStatesLabel : memoryLabels) {
                    if (relevantStatesLabel) {
                        auto relevantStatesFormula = std::make_shared<storm::logic::AtomicLabelFormula>(relevantStatesLabel.get());
                        result->relevantStates.push_back(mc.check(*relevantStatesFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector());
                    } else {
                        result->relevantStates.push_back(storm::storage::BitVector(modelMemoryProduct->getNumberOfStates(), true));
                    }
                }
                
                return result;
            }
            
            template<typename ValueType>
            storm::storage::MemoryStructure MultiDimensionalRewardUnfolding<ValueType>::computeMemoryStructureForEpoch(Epoch const& epoch) const {
                
                storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(model);
                
                // Create a memory structure that remembers whether (sub)objectives are satisfied
                storm::storage::MemoryStructure memory = storm::storage::MemoryStructureBuilder<ValueType>::buildTrivialMemoryStructure(model);
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    if (!objectives[objIndex].formula->isProbabilityOperatorFormula()) {
                        continue;
                    }
                    
                    storm::storage::MemoryStructure objMemory = storm::storage::MemoryStructureBuilder<ValueType>::buildTrivialMemoryStructure(model);

                    for (auto const& dim : objectiveDimensions[objIndex]) {
                        auto const& subObj = subObjectives[dim];
                        if (subObj.first->isBoundedUntilFormula()) {
                            // Create a memory structure that stores whether a PsiState has already been reached
                            storm::storage::MemoryStructureBuilder<ValueType> subObjMemBuilder(2, model);
                            subObjMemBuilder.setLabel(0, memoryLabels[dim].get());
                            
                            if (epoch[dim] >= 0) {
                                storm::storage::BitVector rightSubformulaResult = mc.check(subObj.first->asBoundedUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                                
                                subObjMemBuilder.setTransition(0, 0, ~rightSubformulaResult);
                                subObjMemBuilder.setTransition(0, 1, rightSubformulaResult);
                                subObjMemBuilder.setTransition(1, 1, storm::storage::BitVector(model.getNumberOfStates(), true));
                                for (auto const& initState : model.getInitialStates()) {
                                    subObjMemBuilder.setInitialMemoryState(initState, rightSubformulaResult.get(initState) ? 1 : 0);
                                }
                            } else {
                                subObjMemBuilder.setTransition(0, 0, storm::storage::BitVector(model.getNumberOfStates(), true));
                                subObjMemBuilder.setTransition(1, 1, storm::storage::BitVector(model.getNumberOfStates(), true));
                            }
                            storm::storage::MemoryStructure subObjMem = subObjMemBuilder.build();
                            objMemory = objMemory.product(subObjMem);
                        }
                    }
                    
                    // find the memory state that represents that none of the subobjective is relative anymore
                    storm::storage::BitVector sinkStates(objMemory.getNumberOfStates(), true);
                    for (auto const& dim : objectiveDimensions[objIndex]) {
                        sinkStates = sinkStates & ~objMemory.getStateLabeling().getStates(memoryLabels[dim].get());
                    }
                    assert(sinkStates.getNumberOfSetBits() == 1);
                    
                    // When a constraint of at least one until formula is violated, we need to switch to the sink memory state
                    storm::storage::MemoryStructureBuilder<ValueType> objMemBuilder(objMemory, model);
                    for (auto const& dim : objectiveDimensions[objIndex]) {
                        auto const& subObj = subObjectives[dim];
                        storm::storage::BitVector constraintModelStates =
                                ~(mc.check(subObj.first->asBoundedUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector() |
                                mc.check(subObj.first->asBoundedUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
                        for (auto const& maybeState : objMemory.getStateLabeling().getStates(memoryLabels[dim].get())) {
                            objMemBuilder.setTransition(maybeState, *sinkStates.begin(), constraintModelStates);
                        }
                    }
                    objMemory = objMemBuilder.build();
                    memory = memory.product(objMemory);
                }
                
                return memory;
            }
         
            template<typename ValueType>
            std::vector<std::vector<ValueType>> MultiDimensionalRewardUnfolding<ValueType>::computeObjectiveRewardsForEpoch(Epoch const& epoch, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& modelMemoryProduct) const {
                std::vector<std::vector<ValueType>> objectiveRewards;
                objectiveRewards.reserve(objectives.size());
                
                for (uint64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
                    auto const& formula = *this->objectives[objIndex].formula;
                    if (formula.isProbabilityOperatorFormula()) {
                        storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(*modelMemoryProduct);
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
                        
                        std::vector<ValueType> objRew(modelMemoryProduct->getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        storm::storage::BitVector relevantObjectives(objectiveDimensions[objIndex].getNumberOfSetBits());
                        
                        while (!relevantObjectives.full()) {
                            relevantObjectives.increment();
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
                            storm::storage::BitVector goalStates = mc.check(*goalStatesFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
                            storm::utility::vector::addVectors(objRew, modelMemoryProduct->getTransitionMatrix().getConstrainedRowGroupSumVector(relevantStates, goalStates), objRew);
                        }
                        
                        objectiveRewards.push_back(std::move(objRew));
                        
                        // TODO
                        // Check if the formula is already satisfied in the initial state
                        //        STORM_LOG_THROW((data.originalModel.getInitialStates() & rightSubformulaResult).empty(), storm::exceptions::NotImplementedException, "The Probability for the objective " << *data.objectives.back()->originalFormula << " is always one as the rhs of the until formula is true in the initial state. This (trivial) case is currently not implemented.");
                        
                        
                    } else if (formula.isRewardOperatorFormula()) {
                        auto const& rewModel = modelMemoryProduct->getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                        STORM_LOG_THROW(!rewModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Reward model has transition rewards which is not expected.");
                        bool rewardCollectedInEpoch = true;
                        if (formula.getSubformula().isCumulativeRewardFormula()) {
                            assert(objectiveDimensions[objIndex].getNumberOfSetBits() == 1);
                            rewardCollectedInEpoch = epoch[*objectiveDimensions[objIndex].begin()] >= 0;
                        } else {
                            STORM_LOG_THROW(formula.getSubformula().isTotalRewardFormula(), storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                        }
                        if (rewardCollectedInEpoch) {
                            objectiveRewards.push_back(rewModel.getTotalRewardVector(modelMemoryProduct->getTransitionMatrix()));
                        } else {
                            objectiveRewards.emplace_back(modelMemoryProduct->getTransitionMatrix().getRowCount(), storm::utility::zero<ValueType>());
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of formula " << formula);
                    }
                }
                
                return objectiveRewards;
            }

            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setEpochSolution(Epoch const& epoch, EpochSolution const& solution) {
                epochSolutions.emplace(epoch, solution);
            }
    
            template<typename ValueType>
            void MultiDimensionalRewardUnfolding<ValueType>::setEpochSolution(Epoch const& epoch, EpochSolution&& solution) {
                epochSolutions.emplace(epoch, std::move(solution));
            }
    
            template<typename ValueType>
            typename MultiDimensionalRewardUnfolding<ValueType>::EpochSolution const& MultiDimensionalRewardUnfolding<ValueType>::getEpochSolution(Epoch const& epoch) {
                return epochSolutions.at(epoch);
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