//
// Created by Maximilian Kamps on 25.05.23.
//

#include "heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

template<typename StateType, typename ValueType>
heuristicSim<StateType, ValueType>::heuristicSim(std::shared_ptr<storm::modelchecker::blackbox::blackboxMDP<StateType>> blackboxMdp) :
                                                blackboxMdp(blackboxMdp) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
naiveHeuristicSim<StateType, ValueType>::naiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::blackboxMDP<StateType>> blackboxMdp) :
                                                           heuristicSim<StateType, ValueType>(blackboxMdp) {
    // intentionally left empty
}


} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm