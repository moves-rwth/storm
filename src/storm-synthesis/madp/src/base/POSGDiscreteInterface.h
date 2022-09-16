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
#ifndef _POSGDISCRETEINTERFACE_H_
#define _POSGDISCRETEINTERFACE_H_ 1

/* the include directives */
#include <vector>
#include <string>
#include "Globals.h"
#include "MultiAgentDecisionProcessDiscreteInterface.h"
#include "POSGInterface.h"

/**\brief POSGDiscreteInterface is the interface for
 * a discrete POSG model: it defines the set/get reward functions.
 *
 * POSGDiscreteInterface is an interface (i.e. pure abstract class) for
 * a discrete POSG model. This means that there is a single reward function
 * and that states, actions and observations are discrete.
 *
 * Classes that implement this interface are, for instance, POSGDiscrete
 * and TransitionObservationIndependentPOSGDiscrete.
 **/
class POSGDiscreteInterface :
    virtual public MultiAgentDecisionProcessDiscreteInterface,
    virtual public POSGInterface
{
    private:        

    protected:        
        
    public:
        /// Destructor.
        virtual ~POSGDiscreteInterface() {};
        
        /// Creates a new reward model mapping.
        virtual void CreateNewRewardModelForAgent(
                Index agentI) = 0;
        /// Set the reward for state, joint action indices 
        virtual void SetRewardForAgent(Index agentI, Index sI, Index jaI, 
                                       double r) = 0;

        /// Set the reward for state, joint action , suc. state indices 
        virtual void SetRewardForAgent(Index agentI, Index sI, Index jaI,
                                       Index sucSI, double r) = 0;

        /// Set the reward for state, joint action, suc.state, joint obs indices
        virtual void SetRewardForAgent(Index agentI, Index sI, Index jaI, 
                Index sucSI, Index joI, double r) = 0;

        // 'get' functions:         
        /// Return the reward for state, joint action indices 
        virtual double GetRewardForAgent(Index agentI, Index sI, Index jaI) 
                const = 0;

        /// Returns a copy of this class.
        virtual POSGDiscreteInterface* Clone() const = 0;
        
};

#endif /* !_POSGDISCRETEINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
