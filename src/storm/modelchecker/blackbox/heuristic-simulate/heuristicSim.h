//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_HEURISTICSIM_H
#define STORM_HEURISTICSIM_H

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

template <typename StateType>
class heuristicSim {
    typedef StateType ActionType;
    public:

     bool shouldStopSim();
 
     ActionType sampleAction(StateType state);

     void reset();




};

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
