#ifndef STORM_MODELCHECKER_EXPLORATION_SPARSEMDPEXPLORATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_EXPLORATION_SPARSEMDPEXPLORATIONMODELCHECKER_H_

#include <random>

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"
#include "src/storage/sparse/StateStorage.h"

#include "src/generator/PrismNextStateGenerator.h"
#include "src/generator/CompressedState.h"
#include "src/generator/VariableInformation.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/ExplorationSettings.h"

#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"
#include "src/utility/constants.h"

namespace storm {
    namespace storage {
        namespace sparse {
            template<typename StateType>
            class StateStorage;
        }
        
        class MaximalEndComponent;
    }
    
    namespace generator {
        template<typename ValueType, typename StateType>
        class PrismNextStateGenerator;
    }
    
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMdpExplorationModelChecker : public AbstractModelChecker {
        public:
            typedef uint32_t StateType;
            typedef uint32_t ActionType;
            typedef boost::container::flat_set<StateType> StateSet;
            typedef boost::container::flat_set<ActionType> ActionSet;
            typedef std::shared_ptr<ActionSet> ActionSetPointer;
            typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;
            
            SparseMdpExplorationModelChecker(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            
        private:
            // A structure containing the data assembled during exploration.
            struct ExplorationInformation {
                ExplorationInformation(uint_fast64_t bitsPerBucket, ActionType const& unexploredMarker = std::numeric_limits<ActionType>::max()) : stateStorage(bitsPerBucket), unexploredMarker(unexploredMarker), localPrecomputation(false), numberOfExplorationStepsUntilPrecomputation(100000), numberOfSampledPathsUntilPrecomputation(), nextStateHeuristic(storm::settings::modules::ExplorationSettings::NextStateHeuristic::DifferenceWeightedProbability) {
                    
                    storm::settings::modules::ExplorationSettings const& settings = storm::settings::explorationSettings();
                    localPrecomputation = settings.isLocalPrecomputationSet();
                    if (settings.isNumberOfSampledPathsUntilPrecomputationSet()) {
                        numberOfSampledPathsUntilPrecomputation = settings.getNumberOfSampledPathsUntilPrecomputation();
                    }
                    
                    nextStateHeuristic = settings.getNextStateHeuristic();
                    STORM_LOG_ASSERT(useDifferenceWeightedProbabilityHeuristic() || useProbabilityHeuristic(), "Illegal next-state heuristic.");
                }
                
                storm::storage::sparse::StateStorage<StateType> stateStorage;
                
                std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;
                std::vector<StateType> rowGroupIndices;
                
                std::vector<StateType> stateToRowGroupMapping;
                StateType unexploredMarker;
                std::unordered_map<StateType, storm::generator::CompressedState> unexploredStates;
                
                storm::OptimizationDirection optimizationDirection;
                StateSet terminalStates;
                
                bool localPrecomputation;
                uint_fast64_t numberOfExplorationStepsUntilPrecomputation;
                boost::optional<uint_fast64_t> numberOfSampledPathsUntilPrecomputation;
                
                storm::settings::modules::ExplorationSettings::NextStateHeuristic nextStateHeuristic;
                
                void setInitialStates(std::vector<StateType> const& initialStates) {
                    stateStorage.initialStateIndices = initialStates;
                }
                
                StateType getFirstInitialState() const {
                    return stateStorage.initialStateIndices.front();
                }
                
                std::size_t getNumberOfInitialStates() const {
                    return stateStorage.initialStateIndices.size();
                }
                
                void addUnexploredState(storm::generator::CompressedState const& compressedState) {
                    stateToRowGroupMapping.push_back(unexploredMarker);
                    unexploredStates[stateStorage.numberOfStates] = compressedState;
                    ++stateStorage.numberOfStates;
                }
                
                void assignStateToRowGroup(StateType const& state, ActionType const& rowGroup) {
                    stateToRowGroupMapping[state] = rowGroup;
                }
                
                StateType assignStateToNextRowGroup(StateType const& state) {
                    stateToRowGroupMapping[state] = rowGroupIndices.size() - 1;
                    return stateToRowGroupMapping[state];
                }
                
                StateType getNextRowGroup() const {
                    return rowGroupIndices.size() - 1;
                }
                
                void newRowGroup(ActionType const& action) {
                    rowGroupIndices.push_back(action);
                }
                
                void newRowGroup() {
                    newRowGroup(matrix.size());
                }
                
                std::size_t getNumberOfUnexploredStates() const {
                    return unexploredStates.size();
                }
                
                std::size_t getNumberOfDiscoveredStates() const {
                    return stateStorage.numberOfStates;
                }
                
                StateType const& getRowGroup(StateType const& state) const {
                    return stateToRowGroupMapping[state];
                }
                
                StateType const& getUnexploredMarker() const {
                    return unexploredMarker;
                }
                
                bool isUnexplored(StateType const& state) const {
                    return stateToRowGroupMapping[state] == unexploredMarker;
                }
                
                bool isTerminal(StateType const& state) const {
                    return terminalStates.find(state) != terminalStates.end();
                }
                
                ActionType const& getStartRowOfGroup(StateType const& group) const {
                    return rowGroupIndices[group];
                }
                
                std::size_t getRowGroupSize(StateType const& group) const {
                    return rowGroupIndices[group + 1] - rowGroupIndices[group];
                }
                
                bool onlyOneActionAvailable(StateType const& group) const {
                    return getRowGroupSize(group) == 1;
                }
                
                void addTerminalState(StateType const& state) {
                    terminalStates.insert(state);
                }
                
                std::vector<storm::storage::MatrixEntry<StateType, ValueType>>& getRowOfMatrix(ActionType const& row) {
                    return matrix[row];
                }
                
                std::vector<storm::storage::MatrixEntry<StateType, ValueType>> const& getRowOfMatrix(ActionType const& row) const {
                    return matrix[row];
                }
                
                void addRowsToMatrix(std::size_t const& count) {
                    matrix.resize(matrix.size() + count);
                }
                
                bool maximize() const {
                    return optimizationDirection == storm::OptimizationDirection::Maximize;
                }
                
                bool minimize() const {
                    return !maximize();
                }
                
                bool performPrecomputationExcessiveExplorationSteps(std::size_t& numberExplorationStepsSinceLastPrecomputation) const {
                    bool result = numberExplorationStepsSinceLastPrecomputation > numberOfExplorationStepsUntilPrecomputation;
                    if (result) {
                        numberExplorationStepsSinceLastPrecomputation = 0;
                    }
                    return result;
                }
                
                bool performPrecomputationExcessiveSampledPaths(std::size_t& numberOfSampledPathsSinceLastPrecomputation) const {
                    if (!numberOfSampledPathsUntilPrecomputation) {
                        return false;
                    } else {
                        bool result = numberOfSampledPathsSinceLastPrecomputation > numberOfSampledPathsUntilPrecomputation.get();
                        if (result) {
                            numberOfSampledPathsSinceLastPrecomputation = 0;
                        }
                        return result;
                    }
                }
                
                bool useLocalPrecomputation() const {
                    return localPrecomputation;
                }
                
                bool useGlobalPrecomputation() const {
                    return !useLocalPrecomputation();
                }
                
                storm::settings::modules::ExplorationSettings::NextStateHeuristic const& getNextStateHeuristic() const {
                    return nextStateHeuristic;
                }
                
                bool useDifferenceWeightedProbabilityHeuristic() const {
                    return nextStateHeuristic == storm::settings::modules::ExplorationSettings::NextStateHeuristic::DifferenceWeightedProbability;
                }

                bool useProbabilityHeuristic() const {
                    return nextStateHeuristic == storm::settings::modules::ExplorationSettings::NextStateHeuristic::Probability;
                }
            };
            
            // A struct that keeps track of certain statistics during the computation.
            struct Statistics {
                Statistics() : pathsSampled(0), pathsSampledSinceLastPrecomputation(0), explorationSteps(0), explorationStepsSinceLastPrecomputation(0), maxPathLength(0), numberOfTargetStates(0), numberOfExploredStates(0), numberOfPrecomputations(0), ecDetections(0), failedEcDetections(0), totalNumberOfEcDetected(0) {
                    // Intentionally left empty.
                }
                
                void explorationStep() {
                    ++explorationSteps;
                    ++explorationStepsSinceLastPrecomputation;
                }
                
                void sampledPath() {
                    ++pathsSampled;
                    ++pathsSampledSinceLastPrecomputation;
                }
                
                void updateMaxPathLength(std::size_t const& currentPathLength) {
                    maxPathLength = std::max(maxPathLength, currentPathLength);
                }
                
                std::size_t pathsSampled;
                std::size_t pathsSampledSinceLastPrecomputation;
                std::size_t explorationSteps;
                std::size_t explorationStepsSinceLastPrecomputation;
                std::size_t maxPathLength;
                std::size_t numberOfTargetStates;
                std::size_t numberOfExploredStates;
                std::size_t numberOfPrecomputations;
                std::size_t ecDetections;
                std::size_t failedEcDetections;
                std::size_t totalNumberOfEcDetected;
                
                void printToStream(std::ostream& out, ExplorationInformation const& explorationInformation) const {
                    out << std::endl << "Exploration statistics:" << std::endl;
                    out << "Discovered states: " << explorationInformation.getNumberOfDiscoveredStates() << " (" << numberOfExploredStates << " explored, " << explorationInformation.getNumberOfUnexploredStates() << " unexplored, " << numberOfTargetStates << " target)" << std::endl;
                    out << "Exploration steps: " << explorationSteps << std::endl;
                    out << "Sampled paths: " << pathsSampled << std::endl;
                    out << "Maximal path length: " << maxPathLength << std::endl;
                    out << "Precomputations: " << numberOfPrecomputations << std::endl;
                    out << "EC detections: " << ecDetections << " (" << failedEcDetections << " failed, " << totalNumberOfEcDetected << " EC(s) detected)" << std::endl;
                }
            };
            
            // A struct containing the data required for state exploration.
            struct StateGeneration {
                StateGeneration(storm::prism::Program const& program, storm::generator::VariableInformation const& variableInformation, storm::expressions::Expression const& conditionStateExpression, storm::expressions::Expression const& targetStateExpression) : generator(program, variableInformation, false), conditionStateExpression(conditionStateExpression), targetStateExpression(targetStateExpression) {
                    // Intentionally left empty.
                }
                
                void load(storm::generator::CompressedState const& state) {
                    generator.load(state);
                }
                
                std::vector<StateType> getInitialStates() {
                    return generator.getInitialStates(stateToIdCallback);
                }
                
                storm::generator::StateBehavior<ValueType, StateType> expand() {
                    return generator.expand(stateToIdCallback);
                }
                
                bool isConditionState() const {
                    return generator.satisfies(conditionStateExpression);
                }

                bool isTargetState() const {
                    return generator.satisfies(targetStateExpression);
                }
                
                storm::generator::PrismNextStateGenerator<ValueType, StateType> generator;
                std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback;
                storm::expressions::Expression conditionStateExpression;
                storm::expressions::Expression targetStateExpression;
            };
            
            std::function<StateType (storm::generator::CompressedState const&)> createStateToIdCallback(ExplorationInformation& explorationInformation) const;
            
            std::tuple<StateType, ValueType, ValueType> performExploration(StateGeneration& stateGeneration, ExplorationInformation& explorationInformation) const;

            bool samplePathFromInitialState(StateGeneration& stateGeneration, ExplorationInformation& explorationInformation, StateActionStack& stack, BoundValues& bounds, Statistics& stats) const;
            
            bool exploreState(StateGeneration& stateGeneration, StateType const& currentStateId, storm::generator::CompressedState const& currentState, ExplorationInformation& explorationInformation, BoundValues& bounds, Statistics& stats) const;
            
            ActionType sampleActionOfState(StateType const& currentStateId, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;

            StateType sampleSuccessorFromAction(ActionType const& chosenAction, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            
            bool performPrecomputation(StateActionStack const& stack, ExplorationInformation& explorationInformation, BoundValues& bounds, Statistics& stats) const;
            
            void collapseMec(storm::storage::MaximalEndComponent const& mec, std::vector<StateType> const& relevantStates, storm::storage::SparseMatrix<ValueType> const& relevantStatesMatrix, ExplorationInformation& explorationInformation, BoundValues& bounds) const;
            
            void updateProbabilityBoundsAlongSampledPath(StateActionStack& stack, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;

            void updateProbabilityOfAction(StateType const& state, ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;
            
            std::pair<ValueType, ValueType> computeBoundsOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeBoundOverAllOtherActions(storm::OptimizationDirection const& direction, StateType const& state, ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            std::pair<ValueType, ValueType> computeBoundsOfState(StateType const& currentStateId, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeLowerBoundOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeUpperBoundOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            
            std::pair<ValueType, ValueType> getLowestBounds(storm::OptimizationDirection const& direction) const;
            ValueType getLowestBound(storm::OptimizationDirection const& direction) const;
            std::pair<ValueType, ValueType> combineBounds(storm::OptimizationDirection const& direction, std::pair<ValueType, ValueType> const& bounds1, std::pair<ValueType, ValueType> const& bounds2) const;
            
            // The program that defines the model to check.
            storm::prism::Program program;
            
            // The variable information.
            storm::generator::VariableInformation variableInformation;
            
            // The random number generator.
            mutable std::default_random_engine randomGenerator;
            
            // A comparator used to determine whether values are equal.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
    }
}

#endif /* STORM_MODELCHECKER_EXPLORATION_SPARSEMDPEXPLORATIONMODELCHECKER_H_ */