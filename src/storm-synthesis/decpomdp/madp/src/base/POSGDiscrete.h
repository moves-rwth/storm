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
#ifndef _POSGDISCRETE_H_
#define _POSGDISCRETE_H_ 1

/* the include directives */
#include <vector>
#include <string>
#include "Globals.h"
#include "MultiAgentDecisionProcessDiscrete.h"
#include "RewardModelMapping.h"
#include "POSG.h"
#include "POSGDiscreteInterface.h"

/**\brief POSGDiscrete represent a discrete POSG model.
 *
 * It implements POSGDiscreteInterface. 
 *
 * Also it inherits 
 * -MultiAgentDecisionProcessDiscrete 
 * -POSG
 *
 * and thus implements 
 * -POSGInterface
 * -MultiAgentDecisionProcessDiscreteInterface
 * -MultiAgentDecisionProcessInterface
 * */
class POSGDiscrete : 
    virtual public POSGDiscreteInterface,
    public MultiAgentDecisionProcessDiscrete, 
    public POSG
{
    private:        
        ///Boolean that tracks whether this POSG is initialized.
        bool _m_initialized;        

    protected:        
        
        /// The reward model used by POSGDiscrete is a RewardModelMapping
        std::vector<RewardModelMapping*> _m_p_rModel;

    public:

        // Constructor, destructor and copy assignment.
        /// Default constructor.
        /** Constructor that sets the name, description, and problem file,
         * and subsequently loads this problem file. */
        POSGDiscrete(const std::string &name="received unspec. by POSGDiscrete", 
                     const std::string &descr="received unspec. by POSGDiscrete", 
                     const std::string &pf="received unspec. by POSGDiscrete");
        /// Destructor.
        ~POSGDiscrete();
        
        //data manipulation (set) functions:
        /// Sets _m_initialized to b.
        /** When setting to true, a verification of member elements is
         * performed. (i.e. a check whether all vectors have the
         * correct size and non-zero entries) */
        bool SetInitialized(bool b);   
        /// Creates a new reward model.
        void CreateNewRewardModel( Index agentI, size_t nrS, size_t nrJA);
        /// Set the reward for state, joint action indices 
        void SetReward(Index agentI, Index sI, Index jaI, double r)
            { _m_p_rModel.at(agentI)->Set(sI, jaI, r);}

        /// Set the reward for state, joint action , suc. state indices 
        void SetReward(Index agentI, Index sI, Index jaI, Index sucSI, 
                       double r);

        /// Set the reward for state, joint action, suc.state, joint obs indices
        void SetReward(Index agentI, Index sI, Index jaI, Index sucSI, 
                Index joI, double r);

        // 'get' functions:         
        /// Return the reward for state, joint action indices 
        double GetReward(Index agentI, Index sI, Index jaI) const
            { return(_m_p_rModel.at(agentI)->Get(sI, jaI));}
        
        ///  Prints some information on the POSGDiscrete.        
        std::string SoftPrint() const;

        //We need to implement this for POSG:        
        double GetReward(Index agentI, State* s, JointAction* ja) const;

        /// Get a pointer to the reward model.
        RewardModelMapping* GetRewardModelPtr(Index agentI) const
            { return(_m_p_rModel.at(agentI)); }

        /// Returns a copy of this class.
        virtual POSGDiscrete* Clone() const = 0;

};

#endif /* !_POSGDISCRETE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
