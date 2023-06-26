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
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(storm::modelchecker::blackbox::blackboxMDP<StateType>& blackboxMDP, heuristicSim::heuristicSim<StateType>& heuristicSim) :
                                                         blackboxMDP(blackboxMDP), heuristicSim(heuristicSim) {
    // intentionally empty
}

template <typename StateType, typename ValueType>
void blackBoxExplorer<StateType, ValueType>::performExploration(eMDP<StateType>& eMDP, StateType numExplorations) {
    
    
    for (StateType i = 0; i < numExplorations; i++) {
        // do exploratioon
    }
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
