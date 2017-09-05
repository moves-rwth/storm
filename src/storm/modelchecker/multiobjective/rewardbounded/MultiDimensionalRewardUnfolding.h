#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/transformer/EndComponentEliminator.h"

#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename ValueType>
            class MultiDimensionalRewardUnfolding {
            public:
                
                typedef std::vector<int64_t> Epoch; // The number of reward steps that are "left" for each dimension
                typedef uint64_t EpochClass; // Collection of epochs that consider the same epoch model
                
                struct SolutionType {
                    ValueType weightedValue;
                    std::vector<ValueType> objectiveValues;
                };

                struct EpochModel {
                    storm::storage::SparseMatrix<ValueType> epochMatrix;
                    storm::storage::BitVector stepChoices;
                    std::vector<SolutionType> stepSolutions;
                    std::vector<std::vector<ValueType>> objectiveRewards;
                    std::vector<storm::storage::BitVector> objectiveRewardFilter;
                    
                };
                
                /*
                 *
                 * @param model The (preprocessed) model
                 * @param objectives The (preprocessed) objectives
                 * @param possibleECActions Overapproximation of the actions that are part of an EC
                 * @param allowedBottomStates The states which are allowed to become a bottom state under a considered scheduler
                 *
                 */
                MultiDimensionalRewardUnfolding(storm::models::sparse::Mdp<ValueType> const& model,
                                                        std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives,
                                                        storm::storage::BitVector const& possibleECActions,
                                                        storm::storage::BitVector const& allowedBottomStates);
                
                ~MultiDimensionalRewardUnfolding() {
                    std::cout << "Unfolding statistics: " << std::endl;
                    std::cout << "              init: "  << swInit << " seconds." << std::endl;
                    std::cout << "          setEpoch: " <<  swSetEpoch << " seconds." << std::endl;
                    std::cout << "     setEpochClass: " << swSetEpochClass << " seconds." << std::endl;
                    std::cout << "     findSolutions: " << swFindSol << " seconds." << std::endl;
                    std::cout << "   insertSolutions: " << swInsertSol << " seconds." << std::endl;
                    std::cout << "---------------------------------------------" << std::endl;
                    std::cout << "---------------------------------------------" << std::endl;
                    std::cout << std::endl;
                    
                }
                
                Epoch getStartEpoch();
                std::vector<Epoch> getEpochComputationOrder(Epoch const& startEpoch);
                
                EpochModel const& setCurrentEpoch(Epoch const& epoch);
                
                void setSolutionForCurrentEpoch(std::vector<SolutionType> const& reducedModelStateSolutions);
                SolutionType const& getInitialStateResult(Epoch const& epoch);
                
                
            private:
            
                class MemoryProduct {
                public:
                    MemoryProduct() = default;
                    MemoryProduct(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<std::vector<uint64_t>> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions);
                    
                    storm::models::sparse::Mdp<ValueType> const& getProduct() const;
                    std::vector<boost::optional<Epoch>> const& getSteps() const;
                    
                    bool productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const;
                    uint64_t getProductState(uint64_t const& modelState, uint64_t const& memoryState) const;
                    uint64_t getModelState(uint64_t const& productState) const;
                    uint64_t getMemoryState(uint64_t const& productState) const;
                    
                    uint64_t convertMemoryState(storm::storage::BitVector const& memoryState) const;
                    storm::storage::BitVector const& convertMemoryState(uint64_t const& memoryState) const;
                    
                    uint64_t getProductStateFromChoice(uint64_t const& productChoice) const;
                
                private:
                    
                    void setReachableStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<std::vector<uint64_t>> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) const;

                    
                    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> product;
                    std::vector<boost::optional<Epoch>> steps;
                    
                    std::vector<uint64_t> modelMemoryToProductStateMap;
                    std::vector<uint64_t> productToModelStateMap;
                    std::vector<uint64_t> productToMemoryStateMap;
                    std::vector<uint64_t> choiceToStateMap;
                    std::vector<storm::storage::BitVector> memoryStateMap;
                    
                };
                
                void setCurrentEpochClass(Epoch const& epoch);
            
                void initialize();
                
                void initializeObjectives(std::vector<std::vector<uint64_t>>& epochSteps);
                void initializePossibleEpochSteps(std::vector<std::vector<uint64_t>> const& epochSteps);
                
                void initializeMemoryProduct(std::vector<std::vector<uint64_t>> const& epochSteps);
                storm::storage::MemoryStructure computeMemoryStructure() const;
                std::vector<storm::storage::BitVector> computeMemoryStateMap(storm::storage::MemoryStructure const& memory) const;
                std::vector<std::vector<ValueType>> computeObjectiveRewardsForProduct(Epoch const& epoch) const;
                
                
                EpochClass getClassOfEpoch(Epoch const& epoch) const;
                Epoch getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const;
                
                SolutionType getZeroSolution() const;
                void addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const;
                
                void setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType const& solution);
                SolutionType const& getStateSolution(Epoch const& epoch, uint64_t const& productState);
                
                storm::models::sparse::Mdp<ValueType> const& model;
                std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives;
                storm::storage::BitVector possibleECActions;
                storm::storage::BitVector allowedBottomStates;
                
                MemoryProduct memoryProduct;
                
                typename storm::transformer::EndComponentEliminator<ValueType>::EndComponentEliminatorReturnType ecElimResult;
                std::set<Epoch> possibleEpochSteps;
                
                EpochModel epochModel;
                boost::optional<Epoch> currentEpoch;
                

                std::vector<storm::storage::BitVector> objectiveDimensions;
                std::vector<std::pair<std::shared_ptr<storm::logic::Formula const>, uint64_t>> subObjectives;
                std::vector<boost::optional<std::string>> memoryLabels;
                std::vector<ValueType> scalingFactors;
                
                std::map<std::vector<int64_t>, SolutionType> solutions;
                
                
                storm::utility::Stopwatch swInit, swFindSol, swInsertSol, swSetEpoch, swSetEpochClass;
            };
        }
    }
}