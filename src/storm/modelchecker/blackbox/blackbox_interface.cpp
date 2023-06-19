#include "storm/modelchecker/blackbox/blackbox_interface.h"

#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

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
StateType blackboxMDP<StateType>::get_suc_count(StateType state, StateType action) {
    STORM_LOG_THROW(is_greybox(), storm::exceptions::NotImplementedException, "get_suc_count is not implemented for this greybox MDP");
    STORM_LOG_THROW(!is_greybox(), storm::exceptions::NotSupportedException, "get_suc_count is not implemented for this greybox MDP");
    return 0;
}

template <typename StateType>
double blackboxMDP<StateType>::get_pmin() {
    throw storm::exceptions::NotImplementedException();
}

template <typename StateType, typename ValueType>
blackboxWrapperOnWhitebox<StateType, ValueType>::blackboxWrapperOnWhitebox(storm::prism::Program const& program, storm::logic::Formula const& conditionFormula, storm::logic::Formula const& targetFormula)
                                                : program(program.substituteConstantsFormulas()),
                                                  explorationInformation(storm::OptimizationDirection::Maximize),
                                                  stateGeneration(this->program, explorationInformation, 
                                                  conditionFormula.toExpression(this->program.getManager(), this->program.getLabelToExpressionMapping()), targetFormula.toExpression(this->program.getManager(), this->program.getLabelToExpressionMapping())) {
    // intentionally left empty
}

template <typename StateType, typename ValueType>
StateType blackboxWrapperOnWhitebox<StateType, ValueType>::get_initial_state() {
    stateGeneration.computeInitialStates();
    return stateGeneration.getFirstInitialState();
}

template <typename StateType, typename ValueType>
StateType blackboxWrapperOnWhitebox<StateType, ValueType>::get_avail_actions(StateType state) {
    StateType stateRow = explorationInformation.getRowGroup(state);
    return explorationInformation.getRowGroupSize(stateRow);
}

template <typename StateType, typename ValueType>
StateType blackboxWrapperOnWhitebox<StateType, ValueType>::sample_suc(StateType state, StateType action) {
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
bool blackboxWrapperOnWhitebox<StateType, ValueType>::is_greybox() {
    return false;
}

template <typename StateType, typename ValueType>
void blackboxWrapperOnWhitebox<StateType, ValueType>::exploreState(StateType state) {
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

template class blackboxWrapperOnWhitebox<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
