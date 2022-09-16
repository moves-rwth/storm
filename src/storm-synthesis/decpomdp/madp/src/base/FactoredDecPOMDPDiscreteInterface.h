/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */

/* Only include this header file once. */
#ifndef _FACTOREDDECPOMDPDISCRETEINTERFACE_H_
#define _FACTOREDDECPOMDPDISCRETEINTERFACE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "DecPOMDPDiscreteInterface.h"
#include "MultiAgentDecisionProcessDiscreteFactoredStatesInterface.h"
#include "Scope.h"

class RewardModel; 
//class DecPOMDPDiscrete;
class JointBeliefInterface;
class FactoredQFunctionScopeForStage;

/**\brief FactoredDecPOMDPDiscreteInterface is the interface for a Dec-POMDP
 * with factored states. It declares the get/set reward functions.
 *
 * FactoredDecPOMDPDiscrete represents a factored DecPOMDPDiscrete 
 * FactoredDecPOMDPDiscreteInterface is an interface (i.e. pure abstract class)
 * for a discrete factored DEC-POMDP model. Classes that implement this 
 * interface are, for instance, FactoredDecPOMDPDiscrete.
 *
 * Because a factored Dec-POMDP can also be accessed as a regular Dec-POMDP
 * (i.e., by indexing 'flat' states), this interface also derives from 
 * DecPOMDPDiscreteInterface (which defines the get/set-reward functions for
 * a regular (non-factored) discrete Dec-POMDP).
 *
 * The `LRFs' in this code refer to edges of the graph constructed from
 * the factored immediate reward function. (i.e., each `edge' corresponds
 * to a local reward function)
 *
 * a `flat state' - a state s=<x_1...x_nrFactors> 
 *      (which specifies a value for each state factor).
 *
 * a `full joint action' - a joint action that specifies an action for each
 *      individual agent.
 *
 * a `joint/group action' - an action for a subset of agents.
 *
 *
 * */
class FactoredDecPOMDPDiscreteInterface :
    virtual public DecPOMDPDiscreteInterface,
    virtual public MultiAgentDecisionProcessDiscreteFactoredStatesInterface
{
private:    

protected:

public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor

    /// Destructor.Can't make a virt.destr. pure abstract!
    virtual ~FactoredDecPOMDPDiscreteInterface() {};

    ///Returns a pointer to the LRF-th reward model component.
    virtual RewardModel* GetLRF(Index LRF) const = 0;
    /**Returns reward for LRF, given 
     *  ja_e     -  the joint index for a group action for the subset of agents
     *              as specified by the agent scope of LRF.
     *  s_e      -  the (joint) index of the subset of factors specified by the
     *              state factor scope of LRF.
     *
     *  For instance, let the agents scope of LRF be <1,3>, then
     *  group action <3,5> means that agent 1 select action 3, while agent
     *  3 performs its 5th action. 
     *  Using indextools we can find agSc_jaI. E.g. 
     *      agSc_jaI = IndividualToJointIndices( <3,5>, <6,6> )
     *      (where <6,6> is a vector which specifies the number of actions per 
     *      agent, see IndexTools.h for more info).
     *
     * */
    virtual double GetLRFReward(Index LRF, Index s_e, Index ja_e) const=0;
    virtual double GetLRFReward(Index LRF, 
            const std::vector<Index>& sI_e, const std::vector<Index>& jaI_e)const=0;
    /**might be necessary?
     * Returns reward for LRF, given a flat state index, and a full joint
     * action.*/
    virtual double GetLRFRewardFlat(Index LRF, Index flat_s,
        Index full_ja) const=0;
   
    



    /// Get the number of LRFs in the factored representation.
    virtual size_t GetNrLRFs() const = 0;

    /// Get all the state factors in the scope of an LRF.
    virtual const Scope& GetStateFactorScopeForLRF(Index LRF) const = 0;
    /// Get all the agents in the scope of an LRF.
    virtual const Scope& GetAgentScopeForLRF(Index LRF) const = 0;
    /// Returns all scopes of the immediate reward function in one object
    virtual const FactoredQFunctionScopeForStage GetImmediateRewardScopes() 
        const = 0;
    /// Get the vector of Factor indices corresponding to stateI 
    // this function doesn't belong here...
    // this functionality should be provided by MultiAgentDecisionProcessDiscreteFactoredStates
    //virtual std::vector<Index> GetStateFactorValuesForLRF(Index e, Index stateI) const = 0;

    /**\brief convert a state vector of restricted scope to a joint index s_e.
     *
     * This is a convenience function that performs indiv->joint state index 
     * conversion for a specific edge e (LRF).
     *
     * \li stateVec_e is an assignment of all state factors in the state factor 
     *  scope of e.
     * \li the funtion returns a joint (group) index s_e.
     */
    virtual Index RestrictedStateVectorToJointIndex(Index LRF, 
            const std::vector<Index>& stateVec_e) const = 0;
    /**\brief convert an action vector of restricted scope to a joint index a_e.
     *
     * This is a convenience function that performs indiv->joint action index 
     * conversion for a specific edge e (LRF). (i.e., this function is 
     * typically called when requesting the immediate reward)
     *
     * \li actionVec_e is an assignment of all actions in the agent scope 
     *  scope of e.
     * \li the funtion returns a joint (group) index a_e.
     */
    virtual Index RestrictedActionVectorToJointIndex(Index LRF, 
            const std::vector<Index>& actionVec_e) const = 0;

//rewards:

    // Get the reward for a state factor given its value.
    //this function doesn't make sense?!
    //virtual double GetFactorReward(Index factor, Index sx) const = 0;
    
    using DecPOMDPDiscreteInterface::GetReward;
    virtual double GetReward(const std::vector<Index> &sIs,
                             const std::vector<Index> &aIs) const = 0;

    /// Returns a pointer to a copy of this class.
    virtual FactoredDecPOMDPDiscreteInterface* Clone() const = 0;

    virtual std::string SoftPrint() const = 0;
    virtual void Print() const = 0;
};


#endif /* !_FACTOREDDECPOMDPDISCRETEINTERFACE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
