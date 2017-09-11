#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/transformer/EndComponentEliminator.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename ValueType, bool SingleObjectiveMode>
            class MultiDimensionalRewardUnfolding {
            public:
                
                typedef uint64_t Epoch; // The number of reward steps that are "left" for each dimension
                
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
                    std::cout << "      Product size: " << memoryProduct.getProduct().getNumberOfStates() << std::endl;
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
                
                void setSolutionForCurrentEpoch(std::vector<SolutionType>& inStateSolutions);
                SolutionType const& getInitialStateResult(Epoch const& epoch); // Assumes that the initial state is unique
                SolutionType const& getInitialStateResult(Epoch const& epoch, uint64_t initialStateIndex);
                
                
            private:
            
                class MemoryProduct {
                public:
                    MemoryProduct() = default;
                    MemoryProduct(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<Epoch> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions);
                    
                    storm::models::sparse::Mdp<ValueType> const& getProduct() const;
                    std::vector<Epoch> const& getSteps() const;
                    
                    bool productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const;
                    uint64_t getProductState(uint64_t const& modelState, uint64_t const& memoryState) const;
                    uint64_t getModelState(uint64_t const& productState) const;
                    uint64_t getMemoryState(uint64_t const& productState) const;
                    
                    uint64_t convertMemoryState(storm::storage::BitVector const& memoryState) const;
                    storm::storage::BitVector const& convertMemoryState(uint64_t const& memoryState) const;
                    
                    uint64_t getNumberOfMemoryState() const;
                    
                    uint64_t getProductStateFromChoice(uint64_t const& productChoice) const;
                
                private:
                    
                    void setReachableStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps, std::vector<storm::storage::BitVector> const& objectiveDimensions) const;

                    
                    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> product;
                    std::vector<Epoch> steps;
                    
                    std::vector<uint64_t> modelMemoryToProductStateMap;
                    std::vector<uint64_t> productToModelStateMap;
                    std::vector<uint64_t> productToMemoryStateMap;
                    std::vector<uint64_t> choiceToStateMap;
                    std::vector<storm::storage::BitVector> memoryStateMap;
                    
                };
                
                void setCurrentEpochClass(Epoch const& epoch);
                storm::storage::BitVector computeProductInStatesForEpochClass(Epoch const& epoch);
            
                void initialize();
                
                void initializeObjectives(std::vector<Epoch>& epochSteps);
                
                void initializeMemoryProduct(std::vector<Epoch> const& epochSteps);
                storm::storage::MemoryStructure computeMemoryStructure() const;
                std::vector<storm::storage::BitVector> computeMemoryStateMap(storm::storage::MemoryStructure const& memory) const;
                std::vector<std::vector<ValueType>> computeObjectiveRewardsForProduct(Epoch const& epoch) const;
                
                bool sameEpochClass(Epoch const& epoch1, Epoch const& epoch2) const;
                Epoch getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const;
                bool isZeroEpoch(Epoch const& epoch) const;
                bool isValidDimensionValue(uint64_t const& value) const;
                void setBottomDimension(Epoch& epoch, uint64_t const& dimension) const;
                void setDimensionOfEpoch(Epoch& epoch, uint64_t const& dimension, uint64_t const& value) const; // assumes that the value is small enough
                bool isBottomDimension(Epoch const& epoch, uint64_t const& dimension) const;
                uint64_t getDimensionOfEpoch(Epoch const& epoch, uint64_t const& dimension) const; // assumes that the dimension is not bottom
                std::string epochToString(Epoch const& epoch) const;
            public:
                bool epochClassZigZagOrder(Epoch const& epoch1, Epoch const& epoch2) const;
            private:

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
                
                void setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType const& solution);
                void setSolutionForCurrentEpoch(uint64_t const& productState, SolutionType&& solution);
                SolutionType const& getStateSolution(Epoch const& epoch, uint64_t const& productState);
                
                storm::models::sparse::Mdp<ValueType> const& model;
                std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> objectives;
                
                MemoryProduct memoryProduct;
                
                std::vector<uint64_t> epochModelToProductChoiceMap;
                std::vector<uint64_t> productToEpochModelStateMap;
                std::vector<std::vector<uint64_t>> epochModelInStateToProductStatesMap;
                std::set<Epoch> possibleEpochSteps;
                
                EpochModel epochModel;
                boost::optional<Epoch> currentEpoch;
                

                uint64_t dimensionCount;
                uint64_t bitsPerDimension;
                uint64_t dimensionBitMask;
                
                std::vector<storm::storage::BitVector> objectiveDimensions;
                std::vector<std::pair<std::shared_ptr<storm::logic::Formula const>, uint64_t>> subObjectives;
                std::vector<boost::optional<std::string>> memoryLabels;
                std::vector<ValueType> scalingFactors;
                
                
                std::map<std::pair<Epoch, uint64_t>, SolutionType> solutions;
                
                
                storm::utility::Stopwatch swInit, swFindSol, swInsertSol, swSetEpoch, swSetEpochClass, swAux1, swAux2, swAux3, swAux4;
                std::vector<uint64_t> epochModelSizes;
            };
        }
    }
}