#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_

#include <random>

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"
#include "src/storage/sparse/StateStorage.h"

#include "src/generator/PrismNextStateGenerator.h"
#include "src/generator/CompressedState.h"
#include "src/generator/VariableInformation.h"

#include "src/utility/ConstantsComparator.h"
#include "src/utility/constants.h"

namespace storm {
    namespace storage {
        namespace sparse {
            template<typename StateType>
            class StateStorage;
        }
    }
    
    namespace generator {
        template<typename ValueType, typename StateType>
        class PrismNextStateGenerator;
    }
    
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMdpLearningModelChecker : public AbstractModelChecker {
        public:
            typedef uint32_t StateType;
            typedef uint32_t ActionType;
            typedef boost::container::flat_set<StateType> StateSet;
            typedef boost::container::flat_set<ActionType> ActionSet;
            typedef std::shared_ptr<ActionSet> ActionSetPointer;
            typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;
            
            SparseMdpLearningModelChecker(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            
            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            // A struct that keeps track of certain statistics during the computation.
            struct Statistics {
                Statistics() : iterations(0), maxPathLength(0), numberOfTargetStates(0), numberOfExploredStates(0), pathLengthUntilEndComponentDetection(27) {
                    // Intentionally left empty.
                }
                
                std::size_t iterations;
                std::size_t maxPathLength;
                std::size_t numberOfTargetStates;
                std::size_t numberOfExploredStates;
                std::size_t pathLengthUntilEndComponentDetection;
            };
            
            // A struct containing the data required for state exploration.
            struct StateGeneration {
                StateGeneration(storm::generator::PrismNextStateGenerator<ValueType, StateType>&& generator, storm::expressions::Expression const& targetStateExpression) : generator(std::move(generator)), targetStateExpression(targetStateExpression) {
                    // Intentionally left empty.
                }
                
                std::vector<StateType> getInitialStates() {
                    return generator.getInitialStates(stateToIdCallback);
                }
                
                storm::generator::StateBehavior<ValueType, StateType> expand() {
                    return generator.expand(stateToIdCallback);
                }
                
                bool isTargetState() const {
                    return generator.satisfies(targetStateExpression);
                }
                
                storm::generator::PrismNextStateGenerator<ValueType, StateType> generator;
                std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback;
                storm::expressions::Expression targetStateExpression;
            };
            
            // A structure containing the data assembled during exploration.
            struct ExplorationInformation {
                ExplorationInformation(uint_fast64_t bitsPerBucket, ActionType const& unexploredMarker = std::numeric_limits<ActionType>::max()) : stateStorage(bitsPerBucket), unexploredMarker(unexploredMarker) {
                    // Intentionally left empty.
                }
                
                storm::storage::sparse::StateStorage<StateType> stateStorage;
                
                std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;
                std::vector<StateType> rowGroupIndices;
                
                std::vector<StateType> stateToRowGroupMapping;
                StateType unexploredMarker;
                std::unordered_map<StateType, storm::generator::CompressedState> unexploredStates;
                
                storm::OptimizationDirection optimizationDirection;
                StateSet terminalStates;
                
                std::unordered_map<StateType, ActionSetPointer> stateToLeavingChoicesOfEndComponent;
                
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
                
                void assignStateToNextRowGroup(StateType const& state) {
                    stateToRowGroupMapping[state] = rowGroupIndices.size() - 1;
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
                    return unexploredStates.find(state) == unexploredStates.end();
                }
                
                bool isTerminal(StateType const& state) const {
                    return terminalStates.find(state) != terminalStates.end();
                }
                
                ActionType const& getStartRowOfGroup(StateType const& group) const {
                    return rowGroupIndices[group];
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
            };
            
            // A struct containg the lower and upper bounds per state and action.
            struct BoundValues {
                std::vector<ValueType> lowerBoundsPerState;
                std::vector<ValueType> upperBoundsPerState;
                std::vector<ValueType> lowerBoundsPerAction;
                std::vector<ValueType> upperBoundsPerAction;
                
                std::pair<ValueType, ValueType> getBoundsForState(StateType const& state, ExplorationInformation const& explorationInformation) const {
                    ActionType index = explorationInformation.getRowGroup(state);
                    if (index == explorationInformation.getUnexploredMarker()) {
                        return std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                    } else {
                        return std::make_pair(lowerBoundsPerState[index], upperBoundsPerState[index]);
                    }
                }
                
                ValueType getLowerBoundForState(StateType const& state, ExplorationInformation const& explorationInformation) const {
                    ActionType index = explorationInformation.getRowGroup(state);
                    if (index == explorationInformation.getUnexploredMarker()) {
                        return storm::utility::zero<ValueType>();
                    } else {
                        return getLowerBoundForRowGroup(index, explorationInformation);
                    }
                }
                
                ValueType getLowerBoundForRowGroup(StateType const& rowGroup, ExplorationInformation const& explorationInformation) const {
                    return lowerBoundsPerState[rowGroup];
                }
                
                ValueType getUpperBoundForState(StateType const& state, ExplorationInformation const& explorationInformation) const {
                    ActionType index = explorationInformation.getRowGroup(state);
                    if (index == explorationInformation.getUnexploredMarker()) {
                        return storm::utility::one<ValueType>();
                    } else {
                        return getUpperBoundForRowGroup(index, explorationInformation);
                    }
                }
                
                ValueType getUpperBoundForRowGroup(StateType const& rowGroup, ExplorationInformation const& explorationInformation) const {
                    return upperBoundsPerState[rowGroup];
                }
                
                std::pair<ValueType, ValueType> getBoundsForAction(ActionType const& action) const {
                    return std::make_pair(lowerBoundsPerAction[action], upperBoundsPerAction[action]);
                }
                
                ValueType const& getLowerBoundForAction(ActionType const& action) const {
                    return lowerBoundsPerAction[action];
                }

                ValueType const& getUpperBoundForAction(ActionType const& action) const {
                    return upperBoundsPerAction[action];
                }
                
                ValueType getDifferenceOfStateBounds(StateType const& state, ExplorationInformation const& explorationInformation) {
                    std::pair<ValueType, ValueType> bounds = getBoundsForState(state, explorationInformation);
                    return bounds.second - bounds.first;
                }
                
                void initializeStateBoundsForNextState(std::pair<ValueType, ValueType> const& vals = std::pair<ValueType, ValueType>(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {
                    lowerBoundsPerState.push_back(vals.first);
                    upperBoundsPerState.push_back(vals.second);
                }
                
                void initializeActionBoundsForNextAction(std::pair<ValueType, ValueType> const& vals = std::pair<ValueType, ValueType>(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {
                    lowerBoundsPerAction.push_back(vals.first);
                    upperBoundsPerAction.push_back(vals.second);
                }
                
                void setLowerBoundForState(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& value) {
                    lowerBoundsPerState[explorationInformation.getRowGroup(state)] = value;
                }
                
                void setUpperBoundForState(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& value) {
                    upperBoundsPerState[explorationInformation.getRowGroup(state)] = value;
                }
                
                void setBoundsForAction(ActionType const& action, std::pair<ValueType, ValueType> const& values) {
                    lowerBoundsPerAction[action] = values.first;
                    upperBoundsPerAction[action] = values.second;
                }

                void setBoundsForState(StateType const& state, ExplorationInformation const& explorationInformation, std::pair<ValueType, ValueType> const& values) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    lowerBoundsPerState[rowGroup] = values.first;
                    upperBoundsPerState[rowGroup] = values.second;
                }
                
                bool setNewLowerBoundOfStateIfGreaterThanOld(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& newLowerValue) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    if (lowerBoundsPerState[rowGroup] < newLowerValue) {
                        lowerBoundsPerState[rowGroup] = newLowerValue;
                    }
                }

                bool setNewUpperBoundOfStateIfLessThanOld(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& newUpperValue) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    if (newUpperValue < upperBoundsPerState[rowGroup]) {
                        upperBoundsPerState[rowGroup] = newUpperValue;
                    }
                }
            };
            
            storm::expressions::Expression getTargetStateExpression(storm::logic::Formula const& subformula) const;
            
            std::function<StateType (storm::generator::CompressedState const&)> createStateToIdCallback(ExplorationInformation& explorationInformation) const;
            
            std::tuple<StateType, ValueType, ValueType> performLearningProcedure(StateGeneration& stateGeneration, ExplorationInformation& explorationInformation) const;

            bool samplePathFromState(StateGeneration& stateGeneration, ExplorationInformation& explorationInformation, StateActionStack& stack, BoundValues& bounds, Statistics& stats) const;
            
            bool exploreState(StateGeneration& stateGeneration, StateType const& currentStateId, storm::generator::CompressedState const& currentState, ExplorationInformation& explorationInformation, BoundValues& bounds, Statistics& stats) const;
            
            uint32_t sampleMaxAction(StateType const& currentStateId, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;

            StateType sampleSuccessorFromAction(ActionType const& chosenAction, ExplorationInformation const& explorationInformation) const;
            
            void detectEndComponents(StateActionStack const& stack, ExplorationInformation& explorationInformation, BoundValues& bounds) const;
            
            void updateProbabilityBoundsAlongSampledPath(StateActionStack& stack, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;

            void updateProbabilityOfAction(StateType const& state, ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues& bounds) const;
            
            std::pair<ValueType, ValueType> computeBoundsOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            std::pair<ValueType, ValueType> computeBoundsOfState(StateType const& currentStateId, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeLowerBoundOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeUpperBoundOfAction(ActionType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeLowerBoundOfState(StateType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            ValueType computeUpperBoundOfState(StateType const& action, ExplorationInformation const& explorationInformation, BoundValues const& bounds) const;
            
            // The program that defines the model to check.
            storm::prism::Program program;
            
            // The variable information.
            storm::generator::VariableInformation variableInformation;
            
            // The random number generator.
            std::default_random_engine randomGenerator;
            
            // A comparator used to determine whether values are equal.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
    }
}

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_ */