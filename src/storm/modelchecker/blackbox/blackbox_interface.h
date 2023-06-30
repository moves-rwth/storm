#ifndef STORM_BLACKBOX_INTERFACE
#define STORM_BLACKBOX_INTERFACE

/*
 * This header defines the general interface expected from a blackbox MDP
 * 
*/
#include <random>

#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/logic/Formula.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

// TODO add reward system
template <typename StateType>
class BlackboxMDP {
    public:

     /*!
      * returns the state indentifier of the initial state
     */
     virtual StateType getInitialState() = 0;

     /*!
      * returns a KeyIterator over the available actions of the given state 
      * 
      * @param state 
      * @return KeyIterator<index_type> 
      */
     virtual StateType getAvailActions(StateType state) = 0;
     

     /*!
      * sample a random successor from the action on the given state and return the successors state identifier. 
      * 
      * @param state 
      * @param action
      * @return successor state identfier
      */
     virtual StateType sampleSuc(StateType state, StateType action) = 0;

     /*!
      * returns a lower bound for all transition probilities in this MDP 
      */
     virtual double getPmin();

     /*!
      * returns true if this MDP is a greybox MDP, false if it is a blackbox MDP 
      */
     virtual bool isGreybox() = 0;
     
     /*!
      * greybox method
      * returns how many successors a state has for a given action 
      * 
      * @param state 
      * @param action
      * 
      * @throws NotSupportedException, NotImplementedException 
      */
     virtual StateType getSucCount(StateType state, StateType action);
};

template <typename StateType, typename ValueType>
class BlackboxWrapperOnWhitebox: BlackboxMDP<StateType> {
    public:
     BlackboxWrapperOnWhitebox(storm::prism::Program const& program);
    
     /*!
      * returns the state indentifier of the initial state
     */
     StateType getInitialState();

     /*!
      * returns a KeyIterator over the available actions of the given state 
      * 
      * @param state 
      * @return index_type number of available actions; actions are labeled in ascending order from 0
      */
     StateType getAvailActions(StateType state);
     

     /*!
      * sample a random successor from the action on the given state and return the successors state identifier. 
      * 
      * @param state 
      * @param action
      * @return successor state identfier
      */
     StateType sampleSuc(StateType state, StateType action);

     /*!
      * returns true if this MDP is a greybox MDP, false if it is a blackbox MDP 
      */
     bool isGreybox();

    private:
     void exploreState(StateType state);

     storm::prism::Program program;
     storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGeneration;
     storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explorationInformation;
     mutable std::default_random_engine randomGenerator;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOX_INTERFACE