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

#ifndef _DECPOMDPINTERFACE_H_
#define _DECPOMDPINTERFACE_H_ 1

/* the include directives */

#include <iostream>
#include "Globals.h"
#include "POSGInterface.h"
class State;
class JointAction;

/**\brief DecPOMDPInterface is an interface for DecPOMDPs. It declares a couple
 * of function that relate to the (types of) rewards and discount factor.
 *
 * Conceptually an MultiAgentDecisionProcess that implements this interface, is
 * a Dec-POMDP: the system is cooperative and there is only 1 reward function.
 */
class DecPOMDPInterface : virtual public POSGInterface
{
    private:

    protected:

    public:
        /*using POSGInterface::SetDiscount;
        using POSGInterface::GetDiscount;
        using POSGInterface::GetRewardType;
        using POSGInterface::SetRewardType;
        using POSGInterface::GetReward;
        using POSGInterface::SetReward;*/

        /// Virtual destructor.
        virtual ~DecPOMDPInterface() {};
        
        /// Sets the discount parameter to 0 < d <= 1.
        virtual void SetDiscount(double d) = 0;

        /// Returns the discount parameter.
        virtual double GetDiscount() const = 0;

        /// Sets the reward type to reward_t r.
        virtual void SetRewardType(reward_t r) = 0;

        /// Returns the reward type.
        virtual reward_t GetRewardType() const  = 0;

        /// Function that returns the reward for a state and joint action.
        /** This should be very generic.*/
        virtual double GetReward(State* s, JointAction* ja) const = 0;
        
        /// Function that sets the reward for a state and joint action.
        /** This should be very generic.*/
        virtual void SetReward(State* s, JointAction* ja, double r) = 0;
        
        /// Returns a pointer to a copy of this class.
        virtual DecPOMDPInterface* Clone() const = 0;
};

#endif //! _DECPOMDPINTERFACE_H_

// Local Variables: ***
// mode:c++ ***
// End: ***
