#ifndef STORM_BLACKBOX_INTERFACE
#define STORM_BLACKBOX_INTERFACE

/*
 * This header defines the general interface expected from a blackbox MDP
 * 
*/
#include "storage/HashStorage.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

class blackboxMDP {
    public:
     typedef uint_fast64_t index_type;

     /*!
      * returns the state indentifier of the initial state
     */
     virtual index_type get_initial_state() = 0;

     /*!
      * returns a KeyIterator over the available actions of the given state 
      * 
      * @param state 
      * @return KeyIterator<index_type> 
      */
     virtual storage::KeyIterator<index_type> get_avail_actions(index_type state) = 0;
     

     /*!
      * sample a random successor from the action on the given state and return the successors state identifier. 
      * 
      * @param state 
      * @param action
      * @return successor state identfier
      */
     virtual index_type sample_suc(index_type state, index_type action) = 0;

     /*!
      * returns a lower bound for all transition probilities in this MDP 
      */
    virtual double get_pmin();

     /*!
      * returns true if this MDP is a greybox MDP, false if it is a blackbox MDP 
      */
     virtual bool is_greybox() = 0;
     
     /*!
      * greybox method
      * returns how many successors a state has for a given action 
      * 
      * @param state 
      * @param action
      * 
      * @throws NotSupportedException, NotImplementedException 
      */
     virtual index_type get_suc_count(index_type state, index_type action);
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOX_INTERFACE