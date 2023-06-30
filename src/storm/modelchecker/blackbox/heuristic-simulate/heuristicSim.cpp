//
// Created by Maximilian Kamps on 25.05.23.
//

#include "heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

template<typename StateType, typename ValueType>
HeuristicSim<StateType, ValueType>::HeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp) :
                                                blackboxMdp(blackboxMdp) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
NaiveHeuristicSim<StateType, ValueType>::NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp,
                                                           std::seed_seq seed) :
                                                           HeuristicSim<StateType, ValueType>(blackboxMdp),
                                                           randomGenerator(seed) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
NaiveHeuristicSim<StateType, ValueType>::NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp) :
                                                           NaiveHeuristicSim(blackboxMdp, std::chrono::system_clock::now().time_since_epoch().count()) {
    // intentionally left empty
}

template<typename StateType, typename ValueType>
bool NaiveHeuristicSim<StateType, ValueType>::shouldStopSim(StateActionStack& pathHist) {
    // no extra logic here
    return false;
}

template<typename StateType, typename ValueType>
typename NaiveHeuristicSim<StateType, ValueType>::ActionType NaiveHeuristicSim<StateType, ValueType>::sampleAction(StateActionStack& pathHist) {
    // complete random sampling
    StateType availActions = HeuristicSim<StateType, ValueType>::blackboxMdp->getAvailActions(pathHist.back().first);
    std::uniform_int_distribution<ActionType> distribution(0, availActions - 1);
    return distribution(randomGenerator);
}

template<typename StateType, typename ValueType>
void NaiveHeuristicSim<StateType, ValueType>::reset() {
    // empty, nothing to reset
}

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm