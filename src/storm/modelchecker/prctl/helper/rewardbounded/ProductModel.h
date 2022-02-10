#pragma once

#include <boost/optional.hpp>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/Dimension.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/EpochManager.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/MemoryStateManager.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"
#include "storm/utility/vector.h"

namespace storm {
namespace modelchecker {
namespace helper {
namespace rewardbounded {

template<typename ValueType>
class ProductModel {
   public:
    typedef typename EpochManager::Epoch Epoch;
    typedef typename EpochManager::EpochClass EpochClass;
    typedef typename MemoryStateManager::MemoryState MemoryState;

    ProductModel(storm::models::sparse::Model<ValueType> const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives,
                 std::vector<Dimension<ValueType>> const& dimensions, std::vector<storm::storage::BitVector> const& objectiveDimensions,
                 EpochManager const& epochManager, std::vector<Epoch> const& originalModelSteps);

    storm::models::sparse::Model<ValueType> const& getProduct() const;
    std::vector<Epoch> const& getSteps() const;

    bool productStateExists(uint64_t const& modelState, uint64_t const& memoryState) const;
    uint64_t getProductState(uint64_t const& modelState, uint64_t const& memoryState) const;
    uint64_t getInitialProductState(uint64_t const& initialModelState, storm::storage::BitVector const& initialModelStates, EpochClass const& epochClass) const;
    uint64_t getModelState(uint64_t const& productState) const;
    MemoryState getMemoryState(uint64_t const& productState) const;
    MemoryStateManager const& getMemoryStateManager() const;

    uint64_t getProductStateFromChoice(uint64_t const& productChoice) const;

    std::vector<std::vector<ValueType>> computeObjectiveRewards(EpochClass const& epochClass,
                                                                std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives) const;
    storm::storage::BitVector const& getInStates(EpochClass const& epochClass) const;

    MemoryState transformMemoryState(MemoryState const& memoryState, EpochClass const& epochClass, MemoryState const& predecessorMemoryState) const;
    uint64_t transformProductState(uint64_t const& productState, EpochClass const& epochClass, MemoryState const& predecessorMemoryState) const;

    /// returns the initial states (with respect to the original model) that already satisfy the given objective with probability one, assuming that the cost
    /// bounds at the current epoch allow for the objective to be satisfied.
    boost::optional<storm::storage::BitVector> const& getProb1InitialStates(uint64_t objectiveIndex) const;

   private:
    storm::storage::MemoryStructure computeMemoryStructure(storm::models::sparse::Model<ValueType> const& model,
                                                           std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives);
    std::vector<MemoryState> computeMemoryStateMap(storm::storage::MemoryStructure const& memory) const;

    void setReachableProductStates(storm::storage::SparseModelMemoryProduct<ValueType>& productBuilder, std::vector<Epoch> const& originalModelSteps,
                                   std::vector<MemoryState> const& memoryStateMap) const;

    void collectReachableEpochClasses(std::set<EpochClass, std::function<bool(EpochClass const&, EpochClass const&)>>& reachableEpochClasses,
                                      std::set<Epoch> const& possibleSteps) const;

    void computeReachableStatesInEpochClasses();
    void computeReachableStates(EpochClass const& epochClass, std::vector<EpochClass> const& predecessors);

    std::vector<Dimension<ValueType>> const& dimensions;
    std::vector<storm::storage::BitVector> const& objectiveDimensions;
    EpochManager const& epochManager;
    MemoryStateManager memoryStateManager;

    std::shared_ptr<storm::models::sparse::Model<ValueType>> product;
    std::vector<Epoch> steps;
    std::map<EpochClass, storm::storage::BitVector> reachableStates;
    std::map<EpochClass, storm::storage::BitVector> inStates;

    std::vector<uint64_t> modelMemoryToProductStateMap;
    std::vector<uint64_t> productToModelStateMap;
    std::vector<MemoryState> productToMemoryStateMap;
    std::vector<uint64_t> choiceToStateMap;
    std::vector<boost::optional<storm::storage::BitVector>>
        prob1InitialStates;  /// For each objective the set of initial states that already satisfy the objective
};
}  // namespace rewardbounded
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm