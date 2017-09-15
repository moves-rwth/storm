#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/rewardbounded/EpochManager.h"
#include "storm/modelchecker/multiobjective/rewardbounded/Dimension.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename ValueType>
            class ProductModel {
            public:
                
                typedef typename EpochManager::Epoch Epoch;
                typedef typename EpochManager::EpochClass EpochClass;

                
                ProductModel(storm::models::sparse::Mdp<ValueType> const& model, storm::storage::MemoryStructure const& memory, std::vector<Dimension<ValueType>> const& dimensions, std::vector<storm::storage::BitVector> const& objectiveDimensions, EpochManager const& epochManager, std::vector<storm::storage::BitVector>&& memoryStateMap, std::vector<Epoch> const& originalModelSteps);
                
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
                
                std::vector<std::vector<ValueType>> computeObjectiveRewards(EpochClass const& epochClass, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives) const;
                storm::storage::BitVector const& getInStates(EpochClass const& epochClass) const;


                uint64_t transformMemoryState(uint64_t const& memoryState, EpochClass const& epochClass, uint64_t const& predecessorMemoryState) const;
                uint64_t transformProductState(uint64_t const& productState, EpochClass const& epochClass, uint64_t const& predecessorMemoryState) const;

                
            private:
                
                void setReachableProductStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps) const;
                
                void collectReachableEpochClasses(std::set<EpochClass, std::function<bool(EpochClass const&, EpochClass const&)>>& reachableEpochClasses, std::set<Epoch> const& possibleSteps) const;
                
                void computeReachableStatesInEpochClasses();
                void computeReachableStates(EpochClass const& epochClass, std::vector<EpochClass> const& predecessors);
   
                std::vector<Dimension<ValueType>> const& dimensions;
                std::vector<storm::storage::BitVector> const& objectiveDimensions;
                EpochManager const& epochManager;

                std::shared_ptr<storm::models::sparse::Mdp<ValueType>> product;
                std::vector<Epoch> steps;
                std::map<EpochClass, storm::storage::BitVector> reachableStates;
                std::map<EpochClass, storm::storage::BitVector> inStates;
                
                std::vector<uint64_t> modelMemoryToProductStateMap;
                std::vector<uint64_t> productToModelStateMap;
                std::vector<uint64_t> productToMemoryStateMap;
                std::vector<uint64_t> choiceToStateMap;
                std::vector<storm::storage::BitVector> memoryStateMap;
                
            };
        }
    }
}