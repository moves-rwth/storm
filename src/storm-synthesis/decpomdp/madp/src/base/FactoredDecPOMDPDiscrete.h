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
#ifndef _FACTOREDDECPOMDPDISCRETE_H_
#define _FACTOREDDECPOMDPDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "FactoredDecPOMDPDiscreteInterface.h"
#include "MultiAgentDecisionProcessDiscreteFactoredStates.h"
#include "DecPOMDP.h"

//class DecPOMDPDiscrete;
//class JointBeliefInterface;
#include "RewardModel.h" 
#include "StateDiscrete.h" 
#include "FactoredQFunctionScopeForStage.h"

/**FactoredDecPOMDPDiscrete is implements a factored DecPOMDPDiscrete.
 *
 * This class implements FactoredDecPOMDPDiscreteInterface is the interface for
 * a Dec-POMDP with factored states. It defines the get/set reward functions.
 *
 * This implementation maintains a vector of Local reward functions (LRFs), 
 * each of which is a RewardModel. The implementation of each RewardModel
 * (which subclass of RewardModel is actually used) can differ per LRF.
 *
 * Each reward model is defined over a subset of state factors and states: 
 * the state factor and agent scope of the particular LRF. This class is
 * responsible of all index conversions. (so the referred RewardModels do
 * not know anything about scopes etc.)
 *
 * The `edges' in this code refer to edges of the graph constructed from
 * the factored immediate reward function. (i.e., each `edge' corresponds
 * to a local reward function)
 *
 *
 * */
class FactoredDecPOMDPDiscrete :
    virtual public FactoredDecPOMDPDiscreteInterface,
    public MultiAgentDecisionProcessDiscreteFactoredStates,
    public DecPOMDP
{
private:   
    //maintain the number of local reward functions
    size_t _m_nrLRFs;
    //maintain the state factor scopes
    std::vector< Scope > _m_sfScopes;
    //maintain the agent scopes
    std::vector< Scope > _m_agScopes;
    //maintaint the actual reward models
    std::vector< RewardModel* > _m_LRFs;

    ///GetImmediateRewardScopes returns a reference to the following:
    FactoredQFunctionScopeForStage _m_immRewScope;

//We maintain additional instantiation information for each LRF
//this information is computed by the function 
//InitializeInstantiationInformation, which should be called after the scopes
//of all LRFs are specified.

    ///for each LRF we maintain the nr of X instantiations 
    /**I.e., the size of its local state space.
    */
    std::vector< size_t> _m_nrXIs;
    ///for each LRF we maintain the nr of action instantiations 
    /**I.e., the number of local joint actions (group actions).
    */
    std::vector< size_t> _m_nrAIs;
    ///We maintain the nr of values for each state factor in the scope of lrf
    /**these vectors are used in the conversion from/to joint indices
     **/
    std::vector< std::vector<size_t> > _m_nrSFVals;
    ///For each LRF, we maintain the nr of actions for each agent in its scope
    std::vector< std::vector<size_t> > _m_nrActionVals;

    //variables for caching flat rewards: 
    /// Pointer to model
    RewardModel* _m_p_rModel;
    bool _m_cached_FlatRM;
    bool _m_sparse_FlatRM;

     
protected:
    ///Auxiliary function that returns whether v1 is consistent with v2.
    /**The scope of v2 (scope 2) should be a subset of scope 1
     */
    static bool ConsistentVectorsOnSpecifiedScopes( 
                                        const std::vector<Index>& v1, 
                                        const Scope& scope1,
                                        const std::vector<Index>& v2,
                                        const Scope& scope2);
public:
    /**\brief Default constructor.
     * Constructor that sets the name, description, and problem file,
     * and subsequently loads this problem file. */
    FactoredDecPOMDPDiscrete(std::string 
            name="received unspec. by FactoredDecPOMDPDiscrete", 
            std::string descr="received unspec. by FactoredDecPOMDPDiscrete", 
            std::string pf="received unspec. by FactoredDecPOMDPDiscrete");

    /// Destructor.
    virtual ~FactoredDecPOMDPDiscrete();


    virtual std::string SoftPrint() const;
    void Print() const { std::cout << SoftPrint() << std::endl; }

//functions to create the reward model
    void SetNrLRFs(size_t nr)
    {
        _m_nrLRFs = nr; 
        InitializeStorage();
    }
    ///makes sure that _m_sfScopes, _m_agScopes, _m_LRFs  are set to proper size
    void InitializeStorage();
    virtual void SetYScopes(){}
    virtual void SetOScopes(){}

    virtual double ComputeTransitionProb(
        Index y,
        Index yVal,
        const std::vector< Index>& Xs,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys
        ) const
    {return 0;}
    virtual double ComputeObservationProb(
        Index o,
        Index oVal,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys,
        const std::vector< Index>& Os
        ) const
    {return 0;}

    ///Sets the scope for LRF. 
    /**It is in the desired form, so can copied directly in to _m_sfScopes and
     * _m_agScopes.
     */
    void SetScopeForLRF(Index LRF, 
            const Scope& X, //the X scope
            const Scope& A //the A scope
        )
    {
        _m_sfScopes.at(LRF) = X;
        _m_agScopes.at(LRF) = A;
    }
    ///Sets the scope for LRF.
    /**Sets the scopes that will be used to set the reward with 
     * SetRewardForLRF().
     *
     * The scopes Y and O are back-projected to obtain augmented scopes
     * X' and A', through:
     * \f[ X' = X \cup \Gamma_x( Y \cup O ) \f]
     * \f[ A' = A \cup \Gamma_a( Y \cup O ) \f]
     * where \f$ \Gamma_x \f$ is the state factor scope backup and
     * \f$ \Gamma_a \f$ is the agent scope backup.
     * \sa
     * StateScopeBackup
     * AgentScopeBackup
     */
    void SetScopeForLRF(Index LRF, 
            const Scope& X, //the X scope
            const Scope& A, //the A scope
            const Scope& Y,
            const Scope& O
        );

    ///Initializes some meta information computed from the scopes.
    /**This function computes the number of X and A instantiations,
     * the vectors with the number of values for sfacs and step-size
     * arrays used in conversion from individual <-> joint indices.
     */
    void InitializeInstantiationInformation();

    ///Get the number of X instantiations for LRF.
    /**Each LRF has a local state space X. This function returns the size 
     * of this local state space.
     * Should be called after InitializeInstantiationInformation()
     */
    size_t GetNrXIs(Index LRF) const;
    ///Get the number of action instantiations. I
    /**I.e., the number of local joint (group) actions
     * Should be called after InitializeInstantiationInformation()
     */
    size_t GetNrAIs(Index LRF) const;

    ///Add the LRF-th reward model
    /**It can already contain all rewards, or the reward model can now be
     * filled using the SetRewardForLRF functions.
     */
    void SetRM(Index LRF, RewardModel* rm)
    { _m_LRFs.at(LRF) = rm; }

    ///Set reward.
    /**The reward model should already be added. This function does not require
     * any back-projection or whatsoever.
     */
    void SetRewardForLRF(Index LRF,
            const std::vector<Index>& Xs,
            const std::vector<Index>& As,
            double reward
        );

    ///Set reward.
    /**The reward model should already be added. Because we maintain reward
     * functions of the form R(s,a), this function distributes the reward 
     * according the the expectation of y, o.
     * I.e., Let X, A be the registered scope for LRF, and x,a be instantiations
     * of those scopes. Then
     *
     * \f[ R(x,a) = \sum_{y,o} R(x,a,y,o) \Pr(y,o|x,a) \f]
     *
     * Therefore this function performs 
     * \f[ R(x,a) += R(x,a,y,o) \Pr(y,o|x,a) \f]
     * (by calling SetRewardForLRF(Index LRF, const std::vector<Index>& Xs,const std::vector<Index>& As, double reward) )
     * 
     * Repeatedly calling this function (for all y, o) should result in a
     * valid R(x,a).
     *
     * This means that 
     * \li Xs should be a vector of SFValue indices for registered scope 
     *      _m_sfScopes[LRF].
     * \li As should be a vector specifying an action for each agent specified
     *      by agent scope _m_agScopes[LRF].
     * \li \f$ \Pr(y,o|x,a) \f$ should be defined. (i.e., the scope backup
     *      of o and y should not involve variables not in X,A).
     */
    void SetRewardForLRF(Index LRF,
            const std::vector<Index>& Xs,
            const std::vector<Index>& As,
            const Scope& Y,
            const std::vector<Index>& Ys,
            const Scope& O,
            const std::vector<Index>& Os,
            double reward
        );

    ///Set Reward.
    /**This set reward function is best illustrated by an example of the 
     * factored firefighting problem [Oliehoek08AAMAS]. There we have that
     * the reward function for the first house can be expressed as.
     * \f[ \forall_{f_1,f_2,a_1} \quad R(f_1,f_2,a_1) = \sum_{f_1'} 
     *      \Pr(f_1'|f_1,f_2,a_1) R(f_1') \f]
     *
     * That is, if the scope is underspecified (X and A only are subsets of the
     * registered scopes) than it holds \em{for all} non-specified factors and
     * actions.
     *
     * This version of SetRewardForLRF should be called with 
     * \li X a subset of the registered state factor scope for LRF
     * \li A a subset of the registered agent scope for LRF
     *
     * Consequently it will call 
     * void SetRewardForLRF(Index LRF, const vector<Index>& Xs, const vector<Index>& As, const Scope& Y, const vector<Index>& Ys, const Scope& O, const vector<Index>& Os, double reward )
     * for all non specified state factors and actions.
     *
     */
    void SetRewardForLRF(Index LRF,
            const Scope& X,
            const std::vector<Index>& Xs,
            const Scope& A,
            const std::vector<Index>& As,
            const Scope& Y,
            const std::vector<Index>& Ys,
            const Scope& O,
            const std::vector<Index>& Os,
            double reward
        );





//implement the FactoredDecPOMDPDiscreteInterface
    RewardModel* GetLRF(Index LRF) const
    { return _m_LRFs.at(LRF); }
    double GetLRFReward(Index LRF, Index sI_e, Index jaI_e) const
    { return _m_LRFs.at(LRF)->Get(sI_e, jaI_e); }
    /**might be necessary?
     * Returns reward for LRF, given a flat state index, and a full joint
     * action.*/
    double GetLRFRewardFlat(Index LRF, Index flat_s, Index full_ja) const;
    double GetLRFRewardFlat(Index lrf,
                            const std::vector<Index>& sfacs,
                            const std::vector<Index>& as) const;
    double GetLRFReward(Index LRF, 
            const std::vector<Index>& sI_e, const std::vector<Index>& jaI_e)const;

    size_t GetNrLRFs() const
    {return _m_nrLRFs;}
    const Scope& GetStateFactorScopeForLRF(Index LRF) const
    {return _m_sfScopes.at(LRF); }
    const Scope& GetAgentScopeForLRF(Index LRF) const
    {return _m_agScopes.at(LRF); }
    const FactoredQFunctionScopeForStage GetImmediateRewardScopes() const;
    Index RestrictedStateVectorToJointIndex(
            Index LRF, const std::vector<Index>& stateVec_e) const;
    Index RestrictedActionVectorToJointIndex(Index LRF, 
            const std::vector<Index>& actionVec_e) const;
    
    double GetReward(const std::vector<Index> &sIs,
                     const std::vector<Index> &aIs) const;

//implement the POSGDiscreteInterface
    void CreateNewRewardModelForAgent(Globals::Index)
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

//implement the DecPOMDPDiscreteInterface
    void CreateNewRewardModel()
    { throw E("FactoredDecPOMDPDiscrete::CreateNewRewardModelForAgent check if this is necessary!");}

    /// Set the reward for state, joint action indices 
    void SetReward(Index sI, Index jaI, double r)
    { throw E("FactoredDecPOMDPDiscrete:: SetReward(Index sI, Index jaI, double r) - do we want to divide the reward r equally over the local reward functions?!");}

    /// Set the reward for state, joint action , suc. state indices 
    void SetReward(Index sI, Index jaI, Index sucSI, double r);

    /// Set the reward for state, joint action, suc.state, joint obs indices
    void SetReward(Index sI, Index jaI, Index sucSI, Index joI, 
                   double r);
    
    //double GetReward(Globals::Index, Globals::Index) const;
    double GetReward(Index sI, Index jaI) const;
    RGet * GetRGet() const;

//implement the DecPOMDPInterface
    double GetReward(State*s, JointAction* ja) const
    {
        return GetReward(
        ((StateDiscrete*)s)->GetIndex(), 
        ((JointActionDiscrete*)ja)->GetIndex());
    }

    void SetReward(State* s, JointAction* ja, double r)
    {
        SetReward(
            ((StateDiscrete*)s)->GetIndex(), 
            ((JointActionDiscrete*)ja)->GetIndex(), r);
    }

//implement the POSGInterface    
    void SetRewardForAgent(Index agI, State* s, JointAction* ja,double r)
    { SetReward( s, ja, r); }
    double GetRewardForAgent(Index agI, State* s, JointAction* ja) const
    { return GetReward( s, ja); }

//var    
    void CacheFlatRewardModel(bool sparse=false);
    virtual void CacheFlatModels(bool sparse);

    /// Returns a pointer to a copy of this class.
    virtual FactoredDecPOMDPDiscrete* Clone() const
    { return new FactoredDecPOMDPDiscrete(*this); }

    void ConvertFiniteToInfiniteHorizon(size_t horizon) {
        throw(E("ConvertFiniteToInfiniteHorizon not implemented for FactoredDecPOMDPDiscrete"));
    }

    ///Export fully-observable subset of this FactoredDecPOMDPDiscrete to spudd file.
    /**This function is virtual since some specific problem instances may
     * want to override variable names or other elements to conform with the spudd 
     * file format.
     */
    virtual void ExportSpuddFile(const std::string& filename) const;

    /**\brief Attempts to remove an extraneous state factor from the problem.
     * Such state factors are typically useful when modeling large systems graphically,
     * but hinder performance when solving with flat algorithms (such as Perseus).
     * Currently, it only supports the marginalization of nodes without
     * NS dependencies, and which do not directly influence any LRF
     * */
    void MarginalizeStateFactor(Index sf, bool sparse);

    ///Adjusts the reward model by ignoring a state factor
    void ClipRewardModel(Index sf, bool sparse);

    ///Marginalizes the ISD over a given state factor.
    void MarginalizeISD(Index sf, std::vector<size_t>& factor_sizes, const FactoredStateDistribution* fsd);
};


#endif /* !_FACTOREDDECPOMDPDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
