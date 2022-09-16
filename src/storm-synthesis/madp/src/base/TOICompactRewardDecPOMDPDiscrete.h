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
#ifndef _TOICOMPACTREWARDDECPOMDPDISCRETE_H_
#define _TOICOMPACTREWARDDECPOMDPDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "TOIDecPOMDPDiscrete.h"

/**\brief TOICompactRewardDecPOMDPDiscrete is a class that represents a
 * transition observation independent Dec-POMDP, in which the reward
 * is the sum of each agent's individual reward plus some shared
 * reward. The shared reward is defined for two agents only, and
 * replicated for all combinations. */
class TOICompactRewardDecPOMDPDiscrete :
    public TOIDecPOMDPDiscrete
{
private:    
    /**Boolean that tracks whether this TOICompactRewardDecPOMDPDiscrete is initialized.*/
    bool _m_initialized;         

    std::vector<size_t>_m_nrTwoAgentStates, _m_nrTwoAgentActions;

    double GetTwoAgentReward(Index i, Index j,
                             const std::vector<Index> &indSIs,
                             const std::vector<Index> &indAIs) const;

protected:
    std::vector<RewardModel*> _m_p_rModels;    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    TOICompactRewardDecPOMDPDiscrete(
        const std::string &name="received unspec. by TOICompactRewardDecPOMDPDiscrete", 
        const std::string &descr="received unspec. by TOICompactRewardDecPOMDPDiscrete", 
        const std::string &pf="received unspec. by TOICompactRewardDecPOMDPDiscrete",
        bool cacheFlatModels=false);

    TOICompactRewardDecPOMDPDiscrete
    (const TOICompactRewardDecPOMDPDiscrete& o);

    virtual ~TOICompactRewardDecPOMDPDiscrete();

    TOICompactRewardDecPOMDPDiscrete& operator=
    (const TOICompactRewardDecPOMDPDiscrete& o);

    /** Sets _m_initialized to b. When setting to true, a verification of
     * member elements is performed. (i.e. a check whether all vectors
     * have the correct size and non-zero entries) */
    virtual bool SetInitialized(bool b);   

    void SetIndividualRewardModel(RewardModel* rewardModel,
                                  Index agentID);

    //get (data) functions:
    ///**return the reward for state, joint action indices */
    double GetReward(Index sI, Index jaI) const;
    double GetReward(const std::vector<Index> &sIs,
                     const std::vector<Index> &aIs) const;

    double GetIndividualReward(Index indSI, Index indAI, Index agentID) const;

    /// Returns a pointer to a copy of this class.
    virtual TOICompactRewardDecPOMDPDiscrete* Clone() const
        { return new TOICompactRewardDecPOMDPDiscrete(*this); }

    /** SoftPrints some information on the DecPOMDPDiscrete.*/        
    std::string SoftPrint() const;
};


#endif /* !_TOICOMPACTREWARDDECPOMDPDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
