#ifndef STORM_MODELCHECKER_EXPLORATION_SPARSEMDPEXPLORATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPEXPLORATIONMODELCHECKER_H_

namespace storm {
    namespace modelchecker {
        namespace exploration_detail {
         
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
                        return getLowerBoundForRowGroup(index);
                    }
                }
                
                ValueType const& getLowerBoundForRowGroup(StateType const& rowGroup) const {
                    return lowerBoundsPerState[rowGroup];
                }
                
                ValueType getUpperBoundForState(StateType const& state, ExplorationInformation const& explorationInformation) const {
                    ActionType index = explorationInformation.getRowGroup(state);
                    if (index == explorationInformation.getUnexploredMarker()) {
                        return storm::utility::one<ValueType>();
                    } else {
                        return getUpperBoundForRowGroup(index);
                    }
                }
                
                ValueType const& getUpperBoundForRowGroup(StateType const& rowGroup) const {
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
                
                ValueType const& getBoundForAction(storm::OptimizationDirection const& direction, ActionType const& action) const {
                    if (direction == storm::OptimizationDirection::Maximize) {
                        return getUpperBoundForAction(action);
                    } else {
                        return getLowerBoundForAction(action);
                    }
                }
                
                ValueType getDifferenceOfStateBounds(StateType const& state, ExplorationInformation const& explorationInformation) const {
                    std::pair<ValueType, ValueType> bounds = getBoundsForState(state, explorationInformation);
                    return bounds.second - bounds.first;
                }
                
                void initializeBoundsForNextState(std::pair<ValueType, ValueType> const& vals = std::pair<ValueType, ValueType>(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {
                    lowerBoundsPerState.push_back(vals.first);
                    upperBoundsPerState.push_back(vals.second);
                }
                
                void initializeBoundsForNextAction(std::pair<ValueType, ValueType> const& vals = std::pair<ValueType, ValueType>(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>())) {
                    lowerBoundsPerAction.push_back(vals.first);
                    upperBoundsPerAction.push_back(vals.second);
                }
                
                void setLowerBoundForState(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& value) {
                    setLowerBoundForRowGroup(explorationInformation.getRowGroup(state), value);
                }
                
                void setLowerBoundForRowGroup(StateType const& group, ValueType const& value) {
                    lowerBoundsPerState[group] = value;
                }
                
                void setUpperBoundForState(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& value) {
                    setUpperBoundForRowGroup(explorationInformation.getRowGroup(state), value);
                }
                
                void setUpperBoundForRowGroup(StateType const& group, ValueType const& value) {
                    upperBoundsPerState[group] = value;
                }
                
                void setBoundsForAction(ActionType const& action, std::pair<ValueType, ValueType> const& values) {
                    lowerBoundsPerAction[action] = values.first;
                    upperBoundsPerAction[action] = values.second;
                }
                
                void setBoundsForState(StateType const& state, ExplorationInformation const& explorationInformation, std::pair<ValueType, ValueType> const& values) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    setBoundsForRowGroup(rowGroup, values);
                }
                
                void setBoundsForRowGroup(StateType const& rowGroup, std::pair<ValueType, ValueType> const& values) {
                    lowerBoundsPerState[rowGroup] = values.first;
                    upperBoundsPerState[rowGroup] = values.second;
                }
                
                bool setLowerBoundOfStateIfGreaterThanOld(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& newLowerValue) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    if (lowerBoundsPerState[rowGroup] < newLowerValue) {
                        lowerBoundsPerState[rowGroup] = newLowerValue;
                        return true;
                    }
                    return false;
                }
                
                bool setUpperBoundOfStateIfLessThanOld(StateType const& state, ExplorationInformation const& explorationInformation, ValueType const& newUpperValue) {
                    StateType const& rowGroup = explorationInformation.getRowGroup(state);
                    if (newUpperValue < upperBoundsPerState[rowGroup]) {
                        upperBoundsPerState[rowGroup] = newUpperValue;
                        return true;
                    }
                    return false;
                }
            };
            
        }
    }
}