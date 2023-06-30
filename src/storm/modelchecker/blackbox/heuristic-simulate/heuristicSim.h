//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_HEURISTICSIM_H
#define STORM_HEURISTICSIM_H

#include <random>

#include "storm/modelchecker/blackbox/blackbox_interface.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

enum HeuristicsSim{NAIVE};

// TODO this heuristic class should be able to return the action to take
//      given the latest path, state and 'other' information.
//      because 'other' is not known right now it has to be implemented later on
template <typename StateType, typename ValueType>
class heuristicSim {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;

    public:
     heuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     virtual HeuristicsSim getType() = 0;

     virtual bool shouldStopSim(StateActionStack& pathHist) = 0;
 
     virtual ActionType sampleAction(StateActionStack& pathHist) = 0;

     virtual void reset() = 0;

    protected:
     std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp;
};


template <typename StateType, typename ValueType>
class naiveHeuristicSim : public heuristicSim<StateType, ValueType> {
    using ActionType = typename heuristicSim<StateType, ValueType>::ActionType;
    using StateActionStack = typename heuristicSim<StateType, ValueType>::StateActionStack;

    public:
     naiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp, std::seed_seq seed);
     naiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     HeuristicsSim getType() {
        return HeuristicsSim::NAIVE;
     }

     bool shouldStopSim(StateActionStack& pathHist);
 
     ActionType sampleAction(StateActionStack& pathHist);

     void reset();

    private:
     mutable std::default_random_engine randomGenerator;

};

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
