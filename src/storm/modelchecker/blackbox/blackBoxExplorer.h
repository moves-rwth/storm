//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/blackbox_interface.h"
#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

#ifndef STORM_BLACKBOXEXPLORER_H
#define STORM_BLACKBOXEXPLORER_H

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
class blackBoxExplorer {
    public:
     blackBoxExplorer(blackboxMDP<StateType> blackBoxMDP, heuristicSim::heuristicSim<StateType> heuristicSim);

     void performExploration(eMDP<StateType>& eMDP, StateType numExplorations);

    private:

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
