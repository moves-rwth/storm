//
// Created by Maximilian Kamps on 25.05.23.
//

#include "heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

template<typename StateType, typename ValueType>
heuristicSim<StateType, ValueType>::heuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp) :
                                                blackboxMdp(blackboxMdp) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
naiveHeuristicSim<StateType, ValueType>::naiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp,
                                                           std::seed_seq seed) :
                                                           heuristicSim<StateType, ValueType>(blackboxMdp),
                                                           randomGenerator(seed) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
naiveHeuristicSim<StateType, ValueType>::naiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp) :
                                                           naiveHeuristicSim(blackboxMdp, std::chrono::system_clock::now().time_since_epoch().count()) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
bool naiveHeuristicSim<StateType, ValueType>::shouldStopSim(StateActionStack& pathHist) {
    // no extra logic here
    return false;
}

template<typename StateType, typename ValueType>
typename naiveHeuristicSim<StateType, ValueType>::ActionType naiveHeuristicSim<StateType, ValueType>::sampleAction(StateActionStack& pathHist) {
    // complete random sampling
    StateType availActions = heuristicSim<StateType, ValueType>::blackboxMdp->getAvailActions(pathHist.back().first);
    std::uniform_int_distribution<ActionType> distribution(0, availActions - 1);
    return distribution(randomGenerator);
}

template<typename StateType, typename ValueType>
void naiveHeuristicSim<StateType, ValueType>::reset() {
    // empty
}

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm