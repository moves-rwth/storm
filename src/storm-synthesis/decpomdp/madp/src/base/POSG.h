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

#ifndef _POSG_H_
#define _POSG_H_ 1

/* the include directives */

#include <iostream>
#include "Globals.h"
#include "POSGInterface.h"


/**\brief POSG is a simple implementation of POSGInterface.
 *
 *  It defines a couple of functions that relate to the (types of)
 *  rewards and discount factors for each agent.
 *
 * Conceptually an MultiAgentDecisionProcess that implements this interface, is
 * a POSG: each agent his its own reward function.
  */
class POSG : 
    virtual public POSGInterface
{
    private:
        /// is this initialized?
        bool _m_initialized;
        /// the number of agents
        size_t _m_nrAgents;

        /// The discount parameter.
        /** When agents have different interests (the POSG setting),
         * they may also have different discount factors.
         * For a
         * POSG, however, we have one global discount factor
         * (which typically is 1.0 in the finite horizon case).
         **/
        std::vector<double> _m_discount;
        ///  Do the agents get rewards or costs?
        std::vector<reward_t> _m_rewardType;
    protected:

    public:

        // constructors etc.
        // Default constructor. sets initialized to false
        POSG();

        ///changed initialized status
        bool SetInitialized(bool b);

        ///Sets the number of agents
        void SetNrAgents (size_t nrAgents);

        /// Sets the discount parameter of \a agentI to \a d.
        void SetDiscount(Index agentI, double d);
        /// Returns the discount parameter for agent \a agentI.
        double GetDiscount(Index agentI) const {return _m_discount.at(agentI);}
        /// Sets the reward type to reward_t r.
        /** At the moment only REWARD is supported. */
        void SetRewardType(Index agentI, reward_t r);
        /// Returns the reward type.
        reward_t GetRewardType(Index agentI) const 
        {return _m_rewardType.at(agentI);}
        
        /// SoftPrints some information on the POSG.        
        std::string SoftPrint() const;

};

#endif //! _POSG_H_

// Local Variables: ***
// mode:c++ ***
// End: ***
