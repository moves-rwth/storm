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
#ifndef _TOIDECPOMDPDISCRETE_H_
#define _TOIDECPOMDPDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "TransitionObservationIndependentMADPDiscrete.h"
#include "RewardModelTOISparse.h"
#include "DecPOMDP.h"
#include "DecPOMDPDiscrete.h"
#include "DecPOMDPDiscreteInterface.h"

/**\brief TOIDecPOMDPDiscrete is a class that represents a transition
 * observation independent discrete DecPOMDP. */
class TOIDecPOMDPDiscrete :
    virtual public DecPOMDPDiscreteInterface,
    public TransitionObservationIndependentMADPDiscrete,
    public DecPOMDP
{
private:    
    /**Boolean that tracks whether this TOIDecPOMDPDiscrete is
     * initialized.*/
    bool _m_initialized;         

protected:
#if 0
    /**The reward model used by TOIDecPOMDPDiscrete is a
     * RewardModelMapping*/
    RewardModel* _m_p_rModel;    
#else
    RewardModelTOISparse* _m_p_rModel;
#endif
    /// A vector storing pointers to the individual DecPOMDPs.
    std::vector<DecPOMDPDiscrete*> _m_individualDecPOMDPDs;
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    /** Default constructor.
     * Constructor that sets the name, description, and problem file.
     * The problem file for a TransitionObservationIndependentMADPDiscrete
     * is only the base name, e.g. test.toi-dpomdp.
     * The files parsed in such a case are:
     *      test.toi-dpomdp.base
     *      test.toi-dpomdp.agent0
     *      ...
     *      test.toi-dpomdp.agentN [where N-1 is the number of agents]
     *      test.toi-dpomdp.rewards
     *      
     * The .base file specifies the number of agents, discount and reward_t
     * The .agenti files specifies the local model for agent i.
     * The .rewards files specifies the rewards.
     *
     */
    TOIDecPOMDPDiscrete(
        const std::string &name="received unspec. by TOIDecPOMDPDiscrete", 
        const std::string &descr="received unspec. by TOIDecPOMDPDiscrete", 
        const std::string &pf="received unspec. by TOIDecPOMDPDiscrete",
        bool cacheFlatModels=false);

    /// Copy constructor.
    TOIDecPOMDPDiscrete(const TOIDecPOMDPDiscrete& a);
    /// Destructor.
    virtual ~TOIDecPOMDPDiscrete();

    /// Copy assignment operator
    TOIDecPOMDPDiscrete& operator= 
    (const TOIDecPOMDPDiscrete& o);
    
    //operators:
    
    //data manipulation (set) functions:
    /** Sets _m_initialized to b. When setting to true, a verification of
     * member elements is performed. (i.e. a check whether all vectors
     * have the correct size and non-zero entries) */
    virtual bool SetInitialized(bool b);   

    /// Creates a new reward model.
    void CreateNewRewardModel();

    /// Set the reward for state, joint action indices.
    void SetReward(Index sI, Index jaI, double r)
        { _m_p_rModel->Set(JointToIndividualStateIndices(sI),
                           JointToIndividualActionIndices(jaI),
                           r);
        }

    void SetReward(const std::vector<Index> &sIs,
                   const std::vector<Index> &aIs, double r)
        { _m_p_rModel->Set(sIs, aIs, r);
        }

    /**set the reward for state, joint action , suc. state indices */
    void SetReward(Index sI, Index jaI, Index sucSI, double r)
        {throw(E("SetReward(sI,jaI,sucSI,r) is not implemented yet..."));}
    /**set the reward for state, joint action, suc.state, joint observation
     * indices */
    void SetReward(Index sI, Index jaI, Index sucSI, Index joI, 
                   double r)
        {throw(E("SetReward(sI,jaI,sucSI,joI, r) not implemented yet..."));}
    
    void SetIndividualDecPOMDPD(DecPOMDPDiscrete *model, Index agentI);
    
        //get (data) functions:
    
    DecPOMDPDiscrete* GetIndividualDecPOMDPD(Index agentI)
        const
        {return _m_individualDecPOMDPDs[agentI];}
    
    ///**return the reward for state, joint action indices */
    virtual double GetReward(Index sI, Index jaI) const
        { return(GetReward(JointToIndividualStateIndices(sI),
                           JointToIndividualActionIndices(jaI)));
        }
    virtual double GetReward(const std::vector<Index> &sIs,
                             const std::vector<Index> &aIs) const
        { return(_m_p_rModel->Get(sIs, aIs));
        }
    
    std::vector<double> GetRewards(Index sI, Index jaI) const;
    
    /** SoftPrints some information on the DecPOMDPDiscrete.*/        
    virtual std::string SoftPrint() const;
    /** Prints some information on the DecPOMDPDiscrete.*/        
    void Print() const
        {std::cout << SoftPrint();}
    

    //functions to implement the DecPOMDPInterface:        
    ///implements the DecPOMDPInterface
    double GetReward(State* s, JointAction* ja) const    
        {
            return GetReward(
                ((StateDiscrete*)s)->GetIndex(), 
                ((JointActionDiscrete*)ja)->GetIndex());
        }
    ///implements the DecPOMDPInterface
    void SetReward(State* s, JointAction* ja, double r)
        {
            return SetReward(
                ((StateDiscrete*)s)->GetIndex(), 
                ((JointActionDiscrete*)ja)->GetIndex(), r);
        }
    
    //functions to implement the POSGInterface:
        
    /// Function that sets the reward for an agent, state and joint action.
    /** This should be very generic.*/
    void SetRewardForAgent(Index agentI, State* s, JointAction* ja, double r)
        {SetReward(s, ja, r); }
    /// Function that returns the reward for a state and joint action.
    /** This should be very generic.*/
    double GetRewardForAgent(Index agentI, State* s, JointAction* ja) const
        {return GetReward(s, ja); }
    
    //functions to implement the POSGDiscreteInterface:
    
    ///implementation of POSGDiscreteInterface 
    void CreateNewRewardModelForAgent(Index agentI)
        {CreateNewRewardModel();}
    
    void SetRewardForAgent(Index agentI, Index sI, Index jaI, double r)
        {SetReward(sI, jaI, r);}
    
    /// Set the reward for state, joint action , suc. state indices 
        void SetRewardForAgent(Index agentI, Index sI, Index jaI,
                               Index sucSI, double r) 
        {SetReward(sI, jaI, sucSI, r);}
    
    /// Set the reward for state, joint action, suc.state, joint obs indices
    void SetRewardForAgent(Index agentI, Index sI, Index jaI, 
                           Index sucSI, Index joI, double r)
        {SetReward(sI, jaI, sucSI, joI, r);}
    
    /// Return the reward for state, joint action indices 
    double GetRewardForAgent(Index agentI, Index sI, Index jaI) const
        {return GetReward(sI, jaI);}

    RGet* GetRGet() const { return(0); }
    
    /// Returns a pointer to a copy of this class.
    virtual TOIDecPOMDPDiscrete* Clone() const
        { return new TOIDecPOMDPDiscrete(*this); }

};


#endif /* !_TOIDECPOMDPDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
