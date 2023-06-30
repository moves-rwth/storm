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
class BlackBoxExplorer {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;
 
    public:
     BlackBoxExplorer(std::shared_ptr<BlackboxMDP<StateType>> blackboxMDP, std::shared_ptr<heuristicSim::heuristicSim<StateType, ValueType>> heuristicSim);

     void performExploration(eMDP<StateType>& eMDP, StateType numExplorations);

    private:
     std::shared_ptr<BlackboxMDP<StateType>> blackboxMdp;
     std::shared_ptr<heuristicSim::heuristicSim<StateType, ValueType>> heuristicSim;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
