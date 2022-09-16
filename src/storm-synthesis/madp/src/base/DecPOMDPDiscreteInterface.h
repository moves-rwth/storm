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
#ifndef _DECPOMDPDISCRETEINTERFACE_H_
#define _DECPOMDPDISCRETEINTERFACE_H_ 1

/* the include directives */
#include "Globals.h"
#include "DecPOMDPInterface.h"
#include "MultiAgentDecisionProcessDiscreteInterface.h"

class RGet;

/**\brief DecPOMDPDiscreteInterface is the interface for
 * a discrete DEC-POMDP model: it defines the set/get reward functions.
 *
 * DecPOMDPDiscreteInterface is an interface (i.e. pure abstract class) for
 * a discrete DEC-POMDP model. This means that there is a single reward function
 * and that states, actions and observations are discrete.
 *
 * Classes that implement this interface are, for instance, DecPOMDPDiscrete
 * and TransitionObservationIndependentDecPOMDPDiscrete.
 **/
class DecPOMDPDiscreteInterface : 
    virtual public DecPOMDPInterface,
    virtual public MultiAgentDecisionProcessDiscreteInterface
{
    private:        

    protected:        
        
    public:
        ///import the GetReward function from the base class in current scope.
/*
        using POSGDiscreteInterface::GetReward;
        using POSGDiscreteInterface::SetReward;
        using DecPOMDPInterface::GetReward;*/

        /// Destructor.
        virtual ~DecPOMDPDiscreteInterface() {};
        
        //data manipulation (set) functions:

        /// Creates a new reward model mapping.
        virtual void CreateNewRewardModel() = 0;


        /// Set the reward for state, joint action indices 
        virtual void SetReward(Index sI, Index jaI, double r) = 0;

        /// Set the reward for state, joint action , suc. state indices 
        virtual void SetReward(Index sI, Index jaI,
                               Index sucSI, double r) = 0;

        /// Set the reward for state, joint action, suc.state, joint obs indices
        virtual void SetReward(Index sI, Index jaI, Index sucSI, Index joI, 
                               double r) = 0;

        // 'get' functions:         
        /// Return the reward for state, joint action indices 
        virtual double GetReward(Index sI, Index jaI) const = 0;
        virtual RGet * GetRGet() const = 0;

        /// Returns a pointer to a copy of this class.
        virtual DecPOMDPDiscreteInterface* Clone() const = 0;

};

#endif /* !_DECPOMDPDISCRETEINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
