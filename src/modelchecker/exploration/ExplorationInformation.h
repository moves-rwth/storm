#ifndef STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_EXPLORATIONINFORMATION_H_
#define STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_EXPLORATIONINFORMATION_H_

#include <vector>
#include <limits>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/container/flat_set.hpp>

#include "src/solver/OptimizationDirection.h"

#include "src/generator/CompressedState.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateStorage.h"

#include "src/settings/modules/ExplorationSettings.h"

namespace storm {    
    namespace modelchecker {
        namespace exploration_detail {
            template<typename StateType, typename ValueType>
            class ExplorationInformation {
            public:
                typedef StateType ActionType;
                typedef boost::container::flat_set<StateType> StateSet;
                
                ExplorationInformation(uint_fast64_t bitsPerBucket, storm::OptimizationDirection const& direction, ActionType const& unexploredMarker = std::numeric_limits<ActionType>::max());
                
                void setInitialStates(std::vector<StateType> const& initialStates);
                
                StateType getFirstInitialState() const;
                
                std::size_t getNumberOfInitialStates() const;
                
                void addUnexploredState(storm::generator::CompressedState const& compressedState);
                
                void assignStateToRowGroup(StateType const& state, ActionType const& rowGroup);
                
                StateType assignStateToNextRowGroup(StateType const& state);
                
                StateType getNextRowGroup() const;
                
                void newRowGroup(ActionType const& action);
                
                void newRowGroup();
                
                std::size_t getNumberOfUnexploredStates() const;
                
                std::size_t getNumberOfDiscoveredStates() const;
                
                StateType const& getRowGroup(StateType const& state) const;
                
                StateType const& getUnexploredMarker() const;
                
                bool isUnexplored(StateType const& state) const;
                
                bool isTerminal(StateType const& state) const;
                
                ActionType const& getStartRowOfGroup(StateType const& group) const;
                
                std::size_t getRowGroupSize(StateType const& group) const;
                
                bool onlyOneActionAvailable(StateType const& group) const;
                
                void addTerminalState(StateType const& state);
                
                std::vector<storm::storage::MatrixEntry<StateType, ValueType>>& getRowOfMatrix(ActionType const& row);
                
                std::vector<storm::storage::MatrixEntry<StateType, ValueType>> const& getRowOfMatrix(ActionType const& row) const;
                
                void addRowsToMatrix(std::size_t const& count);
                
                bool maximize() const;
                
                bool minimize() const;
                
                bool performPrecomputationExcessiveExplorationSteps(std::size_t& numberExplorationStepsSinceLastPrecomputation) const;
                
                bool performPrecomputationExcessiveSampledPaths(std::size_t& numberOfSampledPathsSinceLastPrecomputation) const;
                
                bool useLocalPrecomputation() const;
                
                bool useGlobalPrecomputation() const;
                
                storm::settings::modules::ExplorationSettings::NextStateHeuristic const& getNextStateHeuristic() const;
                
                bool useDifferenceWeightedProbabilityHeuristic() const;
                
                bool useProbabilityHeuristic() const;
                
                storm::OptimizationDirection const& getOptimizationDirection() const;
                
                void setOptimizationDirection(storm::OptimizationDirection const& direction);
                
            private:
                storm::storage::sparse::StateStorage<StateType> stateStorage;
                
                std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;
                std::vector<StateType> rowGroupIndices;
                
                std::vector<StateType> stateToRowGroupMapping;
                StateType unexploredMarker;
                std::unordered_map<StateType, storm::generator::CompressedState> unexploredStates;
                
                storm::OptimizationDirection optimizationDirection;
                StateSet terminalStates;
                
                bool localPrecomputation;
                std::size_t numberOfExplorationStepsUntilPrecomputation;
                boost::optional<std::size_t> numberOfSampledPathsUntilPrecomputation;
                
                storm::settings::modules::ExplorationSettings::NextStateHeuristic nextStateHeuristic;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_EXPLORATION_EXPLORATION_DETAIL_EXPLORATIONINFORMATION_H_ */