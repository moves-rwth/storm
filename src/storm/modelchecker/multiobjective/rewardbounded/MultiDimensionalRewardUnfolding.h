#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/rewardbounded/EpochManager.h"
#include "storm/modelchecker/multiobjective/rewardbounded/ProductModel.h"
#include "storm/modelchecker/multiobjective/rewardbounded/Dimension.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename ValueType, bool SingleObjectiveMode>
            class MultiDimensionalRewardUnfolding {
            public:
                
                typedef typename EpochManager::Epoch Epoch; // The number of reward steps that are "left" for each dimension
                typedef typename EpochManager::EpochClass EpochClass;
                
                typedef typename std::conditional<SingleObjectiveMode, ValueType, std::vector<ValueType>>::type SolutionType;

                struct EpochModel {
                    bool epochMatrixChanged;
                    storm::storage::SparseMatrix<ValueType> epochMatrix;
                    storm::storage::BitVector stepChoices;
                    std::vector<SolutionType> stepSolutions;
                    std::vector<std::vector<ValueType>> objectiveRewards;
                    std::vector<storm::storage::BitVector> objectiveRewardFilter;
                    storm::storage::BitVector epochInStates;
                };
                
                /*
                 *
                 * @param model The (preprocessed) model
                 * @param objectives The (preprocessed) objectives
                 *
                 */
                MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives);
                MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model, std::shared_ptr<storm::logic::ProbabilityOperatorFormula const> objectiveFormula);
                
                ~MultiDimensionalRewardUnfolding() {
                    std::cout << "Unfolding statistics: " << std::endl;
                    std::cout << "              init: "  << swInit << " seconds." << std::endl;
                    std::cout << "          setEpoch: " <<  swSetEpoch << " seconds." << std::endl;
                    std::cout << "     setEpochClass: " << swSetEpochClass << " seconds." << std::endl;
                    std::cout << "     findSolutions: " << swFindSol << " seconds." << std::endl;
                    std::cout << "   insertSolutions: " << swInsertSol << " seconds." << std::endl;
                    std::cout << "     aux1StopWatch: " << swAux1 << " seconds." << std::endl;
                    std::cout << "     aux2StopWatch: " << swAux2 << " seconds." << std::endl;
                    std::cout << "     aux3StopWatch: " << swAux3 << " seconds." << std::endl;
                    std::cout << "     aux4StopWatch: " << swAux4 << " seconds." << std::endl;
                    std::cout << "---------------------------------------------" << std::endl;
                    std::cout << "      Product size: " << productModel->getProduct().getNumberOfStates() << std::endl;
                    std::cout << "maxSolutionsStored: " << maxSolutionsStored << std::endl;
                    std::cout << " Epoch model sizes: ";
                    for (auto const& i : epochModelSizes) {
                        std::cout << i << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "---------------------------------------------" << std::endl;
                    std::cout << std::endl;
                    
                }
                
                Epoch getStartEpoch();
                std::vector<Epoch> getEpochComputationOrder(Epoch const& startEpoch);
                
                EpochModel& setCurrentEpoch(Epoch const& epoch);
                
                void setSolutionForCurrentEpoch(std::vector<SolutionType>&& inStateSolutions);
                SolutionType const& getInitialStateResult(Epoch const& epoch); // Assumes that the initial state is unique
                SolutionType const& getInitialStateResult(Epoch const& epoch, uint64_t initialStateIndex);
                
                
            private:
            
                void setCurrentEpochClass(Epoch const& epoch);
                void initialize();
                
                void initializeObjectives(std::vector<Epoch>& epochSteps);
                void computeMaxDimensionValues();

                
                void initializeMemoryProduct(std::vector<Epoch> const& epochSteps);
                
                template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
                SolutionType getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const;
                template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
                SolutionType getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const;
                
                template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
                void addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const;
                template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
                void addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const;
                
                template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
                std::string solutionToString(SolutionType const& solution) const;
                template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
                std::string solutionToString(SolutionType const& solution) const;
                
                SolutionType const& getStateSolution(Epoch const& epoch, uint64_t const& productState);
                
                storm::models::sparse::Mdp<ValueType> const& model;
                std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> objectives;
                
                std::unique_ptr<ProductModel<ValueType>> productModel;
                
                std::vector<uint64_t> epochModelToProductChoiceMap;
                std::shared_ptr<std::vector<uint64_t> const> productStateToEpochModelInStateMap;
                std::vector<std::vector<uint64_t>> epochModelInStateToProductStatesMap;
                std::set<Epoch> possibleEpochSteps;
                
                EpochModel epochModel;
                boost::optional<Epoch> currentEpoch;
                
                EpochManager epochManager;
                
                std::vector<Dimension<ValueType>> dimensions;
                std::vector<storm::storage::BitVector> objectiveDimensions;
                
                struct EpochSolution {
                    uint64_t count;
                    std::shared_ptr<std::vector<uint64_t> const> productStateToSolutionVectorMap;
                    std::vector<SolutionType> solutions;
                };
                std::map<Epoch, EpochSolution> epochSolutions;
                
                
                storm::utility::Stopwatch swInit, swFindSol, swInsertSol, swSetEpoch, swSetEpochClass, swAux1, swAux2, swAux3, swAux4;
                std::vector<uint64_t> epochModelSizes;
                uint64_t maxSolutionsStored;
            };
        }
    }
}