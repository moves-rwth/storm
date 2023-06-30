#include "storm/modelchecker/blackbox/blackbox_interface.h"

#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/logic/Formula.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/utility/macros.h"

#include "storm/generator/CompressedState.h"
#include "storm/solver/OptimizationDirection.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType>
StateType BlackboxMDP<StateType>::getSucCount(StateType state, StateType action) {
    STORM_LOG_THROW(isGreybox(), storm::exceptions::NotImplementedException, "getSucCount is not implemented for this greybox MDP");
    STORM_LOG_THROW(!isGreybox(), storm::exceptions::NotSupportedException, "getSucCount is not implemented for this blackbox MDP");
    return 0;
}

template <typename StateType>
double BlackboxMDP<StateType>::getPmin() {
    throw storm::exceptions::NotImplementedException();
}

template <typename StateType, typename ValueType>
BlackboxWrapperOnWhitebox<StateType, ValueType>::BlackboxWrapperOnWhitebox(storm::prism::Program const& program)
                                                : program(program.substituteConstantsFormulas()),
                                                  explorationInformation(storm::OptimizationDirection::Maximize),
                                                  stateGeneration(this->program, explorationInformation, 
                                                  storm::expressions::ExpressionManager().boolean(true), storm::expressions::ExpressionManager().boolean(true)) {
    // intentionally left empty
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::getInitialState() {
    stateGeneration.computeInitialStates();
    return stateGeneration.getFirstInitialState();
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::getAvailActions(StateType state) {
    StateType stateRow = explorationInformation.getRowGroup(state);
    return explorationInformation.getRowGroupSize(stateRow);
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::sampleSuc(StateType state, StateType action) {
    StateType stateRowIdx = explorationInformation.getStartRowOfGroup(explorationInformation.getRowGroup(state));
    auto& actionRow = explorationInformation.getRowOfMatrix(stateRowIdx + action);

    std::uniform_int_distribution<StateType> distribution(0, actionRow.size() - 1);
    StateType successor = actionRow[distribution(randomGenerator)].getColumn();
    
    // explore successor and add new unexplored states if necessary
    if (explorationInformation.isUnexplored(successor)) {
        exploreState(successor);
    }
    return successor;
}

template <typename StateType, typename ValueType>
bool BlackboxWrapperOnWhitebox<StateType, ValueType>::isGreybox() {
    return false;
}

template <typename StateType, typename ValueType>
void BlackboxWrapperOnWhitebox<StateType, ValueType>::exploreState(StateType state) {
    // TODO optimization: terminal and target states don't need to explored

    auto unexploredIt = explorationInformation.findUnexploredState(state);
    if (unexploredIt == explorationInformation.unexploredStatesEnd()) {
        return;
    }

    storm::generator::CompressedState comprState = unexploredIt->second;

    // get actions; store them and their successors in explorationInformation
    stateGeneration.load(comprState);
    storm::generator::StateBehavior<ValueType, StateType> behavior = stateGeneration.expand();    
    
    StateType startAction = explorationInformation.getActionCount();
    explorationInformation.addActionsToMatrix(behavior.getNumberOfChoices());
    StateType localAction = 0;
    
    for (auto const& choice : behavior) {
        for (auto const& entry : choice) {
           explorationInformation.getRowOfMatrix(startAction + localAction).emplace_back(entry.first, entry.second);
        }
        ++localAction;
    }

    explorationInformation.terminateCurrentRowGroup();
    explorationInformation.removeUnexploredState(unexploredIt);
}

template class BlackboxMDP<uint32_t>;

template class BlackboxWrapperOnWhitebox<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
