//
// Created by Maximilian Kamps on 25.05.23.
//

#include "blackBoxExplorer.h"
#include "storm/modelchecker/blackbox/blackbox_interface.h"
#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(std::shared_ptr<blackboxMDP<StateType>> blackboxMDP, std::shared_ptr<heuristicSim::heuristicSim<StateType>> heuristicSim) :
                                                         blackboxMdp(blackboxMDP), heuristicSim(heuristicSim) {
    // intentionally empty
}

template <typename StateType, typename ValueType>
void blackBoxExplorer<StateType, ValueType>::performExploration(eMDP<StateType>& eMDP, StateType numExplorations) {
    StateActionStack stack;
    StateType maxPathLen = 10; // TODO magicNumber, collect constants

    // set initial state
    eMDP.addInitialState(blackboxMdp->get_initial_state());

    for (StateType i = 0; i < numExplorations; i++) {
        stack.push_back(std::make_pair(blackboxMdp->get_initial_state(), 0));
        ActionType actionTaken;
        StateType suc;
        // do exploration
        while (!heuristicSim->shouldStopSim()) {
            actionTaken = heuristicSim->sampleAction(stack.back().first);
            suc = blackboxMdp->sample_suc((stack.back().first), actionTaken);

            // save in stack
            stack.back().second = actionTaken;
            stack.push_back(std::make_pair(suc, 0));
        }

        // save stack in eMDP
        StateType state;
        suc = stack.back().first;
        stack.pop_back();
        while (!stack.empty()) {
            state = stack.back().first;
            actionTaken = stack.back().second;
            eMDP.addVisit(state, actionTaken, suc);
            suc = state;
            stack.pop_back();
        }

        // update maxPathLen
        maxPathLen = 3 * eMDP.getSize();
    }
}

template class blackBoxExplorer<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
