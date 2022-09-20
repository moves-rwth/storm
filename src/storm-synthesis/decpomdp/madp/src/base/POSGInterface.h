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

#ifndef _POSGINTERFACE_H_
#define _POSGINTERFACE_H_ 1

/* the include directives */

#include <iostream>
#include "Globals.h"
#include "MultiAgentDecisionProcessInterface.h"
class State;
class JointAction;



/**\brief POSGInterface is an interface for POSGs. It declares a couple
 * of function that relate to the (types of) rewards and discount factor.
 *
 * Conceptually an MultiAgentDecisionProcess that implements this interface, is
 * a POSG: each agent has its own reward function.
 */
class POSGInterface : virtual public MultiAgentDecisionProcessInterface
{
    private:

    protected:

    public:

        /// Virtual destructor.
        virtual ~POSGInterface() {};
        
        /// Sets the discount parameter to 0 < d <= 1.
        virtual void SetDiscountForAgent(Index agentI, double d) = 0;

        /// Returns the discount parameter.
        virtual double GetDiscountForAgent(Index agentI) const = 0;

        /// Sets the reward type to reward_t r.
        virtual void SetRewardTypeForAgent(Index agentI, reward_t r) = 0;

        /// Returns the reward type.
        virtual reward_t GetRewardTypeForAgent(Index agentI) const  = 0;

        /// Function that sets the reward for an agent, state and joint action.
        /** This should be very generic.*/
        virtual void SetRewardForAgent(Index agentI, State* s, JointAction* ja,
                                       double r) = 0;
        /// Function that returns the reward for a state and joint action.
        /** This should be very generic.*/
        virtual double GetRewardForAgent(Index agentI, State* s,
                                         JointAction* ja) const = 0;
        
        /// Returns a pointer to a copy of this class.
        virtual POSGInterface* Clone() const = 0;
};

#endif //! _POSGINTERFACE_H_

// Local Variables: ***
// mode:c++ ***
// End: ***
