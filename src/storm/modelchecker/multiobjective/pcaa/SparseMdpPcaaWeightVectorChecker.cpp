#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/logic/Formulas.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"


#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpPcaaWeightVectorChecker(SparseMdpModelType const& model,
                                                                                                   std::vector<Objective<ValueType>> const& objectives,
                                                                                                   storm::storage::BitVector const& possibleECActions,
                                                                                                   storm::storage::BitVector const& possibleBottomStates) :
                SparsePcaaWeightVectorChecker<SparseMdpModelType>(model, objectives, possibleECActions, possibleBottomStates) {
                // set the state action rewards. Also do some sanity checks on the objectives.
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& formula = *objectives[objIndex].formula;
                    STORM_LOG_THROW(formula.isRewardOperatorFormula() && formula.asRewardOperatorFormula().hasRewardModelName(), storm::exceptions::UnexpectedException, "Unexpected type of operator formula: " << formula);
                    STORM_LOG_THROW(formula.getSubformula().isCumulativeRewardFormula() || formula.getSubformula().isTotalRewardFormula(), storm::exceptions::UnexpectedException, "Unexpected type of sub-formula: " << formula.getSubformula());
                    typename SparseMdpModelType::RewardModelType const& rewModel = this->model.getRewardModel(formula.asRewardOperatorFormula().getRewardModelName());
                    STORM_LOG_THROW(!rewModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.getTotalRewardVector(this->model.getTransitionMatrix());
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Currently, only step bounds are considered.
                bool containsRewardBoundedObjectives = false;
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    if (this->objectives[objIndex].formula->getSubformula().isBoundedUntilFormula()) {
                        containsRewardBoundedObjectives = true;
                        break;
                    }
                }
                
                if (containsRewardBoundedObjectives) {
                    boundedPhaseWithRewardBounds(weightVector, weightedRewardVector);
                } else {
                    boundedPhaseOnlyStepBounds(weightVector, weightedRewardVector);
                }
            }

            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhaseOnlyStepBounds(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(this->model.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(this->model.getNumberOfStates());
                // Get for each occurring timeBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> stepBounds;
                for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    if (this->objectives[objIndex].formula->getSubformula().isCumulativeRewardFormula()) {
                        auto const& subformula = this->objectives[objIndex].formula->getSubformula().asCumulativeRewardFormula();
                        uint_fast64_t stepBound = subformula.template getBound<uint_fast64_t>();
                        if (subformula.isBoundStrict()) {
                            --stepBound;
                        }
                        auto stepBoundIt = stepBounds.insert(std::make_pair(stepBound, storm::storage::BitVector(this->objectives.size(), false))).first;
                        stepBoundIt->second.set(objIndex);
                        
                        // There is no error for the values of these objectives.
                        this->offsetsToUnderApproximation[objIndex] = storm::utility::zero<ValueType>();
                        this->offsetsToOverApproximation[objIndex] = storm::utility::zero<ValueType>();
                    }
                }
                
                // Stores the objectives for which we need to compute values in the current time epoch.
                storm::storage::BitVector consideredObjectives = this->objectivesWithNoUpperTimeBound;
                
                auto stepBoundIt = stepBounds.begin();
                uint_fast64_t currentEpoch = stepBounds.empty() ? 0 : stepBoundIt->first;
                
                while (currentEpoch > 0) {
                    
                    if (stepBoundIt != stepBounds.end() && currentEpoch == stepBoundIt->first) {
                        consideredObjectives |= stepBoundIt->second;
                        for(auto objIndex : stepBoundIt->second) {
                            // This objective now plays a role in the weighted sum
                            ValueType factor = storm::solver::minimize(this->objectives[objIndex].formula->getOptimalityType()) ? -weightVector[objIndex] : weightVector[objIndex];
                            storm::utility::vector::addScaledVector(weightedRewardVector, this->discreteActionRewards[objIndex], factor);
                        }
                        ++stepBoundIt;
                    }
                    
                    // Get values and scheduler for weighted sum of objectives
                    this->model.getTransitionMatrix().multiplyWithVector(this->weightedResult, choiceValues);
                    storm::utility::vector::addVectors(choiceValues, weightedRewardVector, choiceValues);
                    storm::utility::vector::reduceVectorMax(choiceValues, this->weightedResult, this->model.getTransitionMatrix().getRowGroupIndices(), &optimalChoicesInCurrentEpoch);
                    
                    // get values for individual objectives
                    // TODO we could compute the result for one of the objectives from the weighted result, the given weight vector, and the remaining objective results.
                    for (auto objIndex : consideredObjectives) {
                        std::vector<ValueType>& objectiveResult = this->objectiveResults[objIndex];
                        std::vector<ValueType> const& objectiveRewards = this->discreteActionRewards[objIndex];
                        auto rowGroupIndexIt = this->model.getTransitionMatrix().getRowGroupIndices().begin();
                        auto optimalChoiceIt = optimalChoicesInCurrentEpoch.begin();
                        for(ValueType& stateValue : temporaryResult){
                            uint_fast64_t row = (*rowGroupIndexIt) + (*optimalChoiceIt);
                            ++rowGroupIndexIt;
                            ++optimalChoiceIt;
                            stateValue = objectiveRewards[row];
                            for(auto const& entry : this->model.getTransitionMatrix().getRow(row)) {
                                stateValue += entry.getValue() * objectiveResult[entry.getColumn()];
                            }
                        }
                        objectiveResult.swap(temporaryResult);
                    }
                    --currentEpoch;
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhaseWithRewardBounds(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                if (!rewardUnfolding) {
                    rewardUnfolding = std::make_unique<MultiDimensionalRewardUnfolding<ValueType>>(this->model, this->objectives, this->possibleECActions, this->possibleBottomStates);
                }
                auto initEpoch = rewardUnfolding->getStartEpoch();
                auto epochOrder = rewardUnfolding->getEpochComputationOrder(initEpoch);
                for (auto const& epoch : epochOrder) {
                    computeEpochSolution(epoch, weightVector);
                }
                
                auto solution = rewardUnfolding->getEpochSolution(initEpoch);
                uint64_t initStateInUnfoldedModel = *rewardUnfolding->getModelForEpoch(initEpoch).initialStates.begin();
                this->weightedResult[*this->model.getInitialStates().begin()] = solution.weightedValues[initStateInUnfoldedModel];
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    this->objectiveResults[objIndex][*this->model.getInitialStates().begin()] = solution.objectiveValues[objIndex][initStateInUnfoldedModel];
                    // Todo: we currently assume precise results...
                    this->offsetsToUnderApproximation[objIndex] = storm::utility::zero<ValueType>();
                    this->offsetsToOverApproximation[objIndex] = storm::utility::zero<ValueType>();
                }
                
            }
    
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::computeEpochSolution(typename MultiDimensionalRewardUnfolding<ValueType>::Epoch const& epoch, std::vector<ValueType> const& weightVector) {
                auto const& epochModel = rewardUnfolding->getModelForEpoch(epoch);
                typename MultiDimensionalRewardUnfolding<ValueType>::EpochSolution result;
                result.weightedValues.resize(epochModel.intermediateTransitions.getRowGroupCount());
                
                // Formulate a min-max equation system max(A*x+b)=x for the weighted sum of the objectives
                std::vector<ValueType> b(epochModel.rewardChoices.size(), storm::utility::zero<ValueType>());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    ValueType const& weight = weightVector[objIndex];
                    if (!storm::utility::isZero(weight)) {
                        std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                        for (auto const& choice : epochModel.objectiveRewardFilter[objIndex]) {
                            b[choice] += weight * objectiveReward[choice];
                        }
                    }
                }
                for (auto const& choice : epochModel.rewardChoices) {
                    typename MultiDimensionalRewardUnfolding<ValueType>::Epoch const& step = epochModel.epochSteps[choice].get();
                    assert(step.size() == epoch.size());
                    typename MultiDimensionalRewardUnfolding<ValueType>::Epoch targetEpoch;
                    targetEpoch.reserve(epoch.size());
                    for (auto eIt = epoch.begin(), sIt = step.begin(); eIt != epoch.end(); ++eIt, ++sIt) {
                        targetEpoch.push_back(std::max(*eIt - *sIt, (int64_t) -1));
                    }
                    b[choice] = epochModel.rewardTransitions.multiplyRowWithVector(choice, rewardUnfolding->getEpochSolution(targetEpoch).weightedValues);
                }
                
                // Invoke the min max solver
                storm::solver::GeneralMinMaxLinearEquationSolverFactory<ValueType> minMaxSolverFactory;
                auto minMaxSolver = minMaxSolverFactory.create(epochModel.intermediateTransitions);
                minMaxSolver->setOptimizationDirection(storm::solver::OptimizationDirection::Maximize);
                minMaxSolver->setTrackScheduler(true);
                //minMaxSolver->setCachingEnabled(true);
                minMaxSolver->solveEquations(result.weightedValues, b);
                
                // Formulate for each objective the linear equation system induced by the performed choices
                auto const& choices = minMaxSolver->getSchedulerChoices();
                storm::storage::SparseMatrix<ValueType> subMatrix = epochModel.intermediateTransitions.selectRowsFromRowGroups(choices, true);
                subMatrix.convertToEquationSystem();
                storm::solver::GeneralLinearEquationSolverFactory<ValueType> linEqSolverFactory;
                auto linEqSolver = linEqSolverFactory.create(std::move(subMatrix));
                b.resize(choices.size());
                result.objectiveValues.reserve(this->objectives.size());
                for (uint64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    std::vector<ValueType> const& objectiveReward = epochModel.objectiveRewards[objIndex];
                    // TODO: start with a better initial guess
                    result.objectiveValues.emplace_back(choices.size(), storm::utility::convertNumber<ValueType>(0.5));
                    for (uint64_t state = 0; state < choices.size(); ++state) {
                        uint64_t choice = epochModel.intermediateTransitions.getRowGroupIndices()[state] + choices[state];
                        if (epochModel.objectiveRewardFilter[objIndex].get(choice)) {
                            b[state] = objectiveReward[choice];
                        } else {
                            b[state] = storm::utility::zero<ValueType>();
                        }
                        if (epochModel.rewardChoices.get(choice)) {
                            typename MultiDimensionalRewardUnfolding<ValueType>::Epoch const& step = epochModel.epochSteps[choice].get();
                            assert(step.size() == epoch.size());
                            typename MultiDimensionalRewardUnfolding<ValueType>::Epoch targetEpoch;
                            targetEpoch.reserve(epoch.size());
                            for (auto eIt = epoch.begin(), sIt = step.begin(); eIt != epoch.end(); ++eIt, ++sIt) {
                                targetEpoch.push_back(std::max(*eIt - *sIt, (int64_t) -1));
                            }
                            b[state] += epochModel.rewardTransitions.multiplyRowWithVector(choice, rewardUnfolding->getEpochSolution(targetEpoch).objectiveValues[objIndex]);
                        }
                    }
                    
                    linEqSolver->solveEquations(result.objectiveValues.back(), b);
                }
                
                rewardUnfolding->setEpochSolution(epoch, std::move(result));
            }

            
            
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
            
        }
    }
}
