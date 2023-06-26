#ifndef STORM_EMDP_H
#define STORM_EMDP_H

#pragma once
#include <string.h>
#include "stdint.h"
#include <iostream>
#include <fstream>

#include "storage/HashStorage.h"


namespace storm {
namespace modelchecker {

namespace blackbox {


template<typename ValueType>

class eMDP {
   public:
    typedef ValueType index_type;
    
    /*!
     * Constructs an empty eMDP
     */
    eMDP();

    /*!
     * adds initial state to eMDP
     * 
     * @param state initial state 
     */
    void addInitialState(index_type state);
    
    /*!
     * increments the visits count of the given triple by 1
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     */
    void addVisit(index_type state, index_type action, index_type succ);

    /*!
     * increments the visits count of the given by visits
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     * @param visits  visits to be added to counter
     */
    void addVisits(index_type state, index_type action, index_type succ, ValueType visits);

    /*!
     * Add a new state to the eMDP
     * @param state  state index of new state
     * @param avail_actions vector of available actions at state
     */
    void addState(index_type state, std::vector<index_type> avail_actions);

    /*!
     * Adds a new Label to the state 
     * 
     * @param label 
     * @param state 
     */
    void addStateLabel(std::string label, index_type state);
    
    /*!
     * Removes a label from the state 
     * 
     * @param label 
     * @param state 
     */
    void removeStateLabel(std::string label, index_type state);
    
    /*!
     * Returns the vector of Labels for a state 
     * 
     * @param state 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> getStateLabels(index_type state);

    /*!
     * print the eMDP to std::cout
     */
    void print();

    /*!
     * Converts the eMDP to dot string representation 
     * 
     * @return std::string 
     */
    std::string toDotString();
    /*!
     * Writes toDotString() to a file with name filename 
     * 
     * @param filename  
     */
    void writeDotFile(std::string filename);

    /*!
     * prints toDotString to std::cout 
     * 
     */
    void printDot();

    /*!
     * returns true. if the state was already added to this eMDP. false otherwise
     * @param state  state index of tested state
     */
    bool isStateKnown(index_type state);

    /*!
     * returns how often this state action pair was sampled
     * @param state   state index 
     * @param action  action index
     */
    ValueType getSampleCount(index_type state, index_type action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    ValueType getSampleCount(index_type state, index_type action, index_type succ);

    /*!
     * returns toatla number of states in this eMDP
     */
    ValueType getSize();

    //? Save to disk

    /*!
     * Returns a KeyIterator over the states 
     * 
     * @return KeyIterator<index_type> 
     */
    storage::KeyIterator<index_type> get_state_itr();

    /*!
     * Returns a KeyIterator over the actions of a state
     * 
     * @param state 
     * @return KeyIterator<index_type> 
     */
    storage::KeyIterator<index_type> get_state_actions_itr(index_type state);

    /*!
     * Returns a KeyIterator over the successor of a state for a given action 
     * 
     * @param state 
     * @param action 
     * @return KeyIterator<index_type> 
     */
    storage::KeyIterator<index_type> get_state_action_succ_itr(index_type state, index_type action);
    
   private:
    /*!
     * Adds the state and its corresponding exploration time to explorationOrder
     *
     * @param state
     */
    void addStateToExplorationOrder(index_type state);

   storage::HashStorage<index_type> hashStorage;
   std::unordered_map<index_type, index_type> explorationOrder; // maps state to its position of when its been found
   std::unordered_map<index_type, std::vector<std::string> > stateLabeling; 
   index_type init_state = -1;
   index_type explorationCount = 0; //Number of explored states
};
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_EMDP_H
