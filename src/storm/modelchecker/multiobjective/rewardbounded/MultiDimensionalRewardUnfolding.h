#pragma once


#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename ValueType>
            class MultiDimensionalRewardUnfolding {
            public:
                
                typedef std::vector<int64_t> Epoch; // The number of reward steps that are "left" for each dimension
                typedef uint64_t EpochClass; // Collection of epochs that consider the same epoch model
                
                struct EpochModel {
                    storm::storage::SparseMatrix<ValueType> rewardTransitions;
                    storm::storage::SparseMatrix<ValueType> intermediateTransitions;
                    std::vector<std::vector<ValueType>> objectiveRewards;
                    std::vector<boost::optional<Epoch>> epochSteps;
                };
                
                struct EpochSolution {
                    std::vector<ValueType> weightedValues;
                    std::vector<std::vector<ValueType>> objectiveValues;
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
                
                
                Epoch getStartEpoch();
                std::vector<Epoch> getEpochComputationOrder(Epoch const& startEpoch);
                EpochModel const& getModelForEpoch(Epoch const& epoch);
                
                void setEpochSolution(Epoch const& epoch, EpochSolution const& solution);
                void setEpochSolution(Epoch const& epoch, EpochSolution&& solution);
                EpochSolution const& getEpochSolution(Epoch const& epoch);
                
            private:
            
                void initialize();
                EpochClass getClassOfEpoch(Epoch const& epoch) const;
                Epoch getSuccessorEpoch(Epoch const& epoch, Epoch const& step) const;
                
                EpochModel computeModelForEpoch(Epoch const& epoch);
                storm::storage::MemoryStructure computeMemoryStructureForEpoch(Epoch const& epoch) const;
                
                
                storm::models::sparse::Mdp<ValueType> const& model;
                std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives;
                storm::storage::BitVector possibleECActions;
                storm::storage::BitVector allowedBottomStates;
                
                std::vector<std::pair<std::shared_ptr<storm::logic::Formula const>, uint64_t>> subObjectives;
                std::vector<std::vector<uint64_t>> scaledRewards;
                std::vector<ValueType> scalingFactors;
                
                std::map<EpochClass, EpochModel> epochModels;
                std::map<Epoch, EpochSolution> epochSolutions;
            };
        }
    }
}