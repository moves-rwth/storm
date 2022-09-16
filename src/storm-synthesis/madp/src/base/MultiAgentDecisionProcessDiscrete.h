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
#ifndef _MULTIAGENTDECISIONPROCESSDISCRETE_H_
#define _MULTIAGENTDECISIONPROCESSDISCRETE_H_ 1

/* the include directives */
#include <vector>
#include <string>

#include "MultiAgentDecisionProcess.h"
#include "MultiAgentDecisionProcessDiscreteInterface.h"
#include "MADPComponentDiscreteActions.h"
#include "MADPComponentDiscreteObservations.h"
#include "MADPComponentDiscreteStates.h"
#include "StateDistributionVector.h"
#include "StateDistribution.h"
class TGet;

/**\brief MultiAgentDecisionProcessDiscrete is defines the primary properties
 * of a discrete decision process.
 *
 * MultiAgentDecisionProcessDiscrete is defines the primary properties
 * of a discrete decision process.
 * 
 * It extends MultiAgentDecisionProcess, MADPComponentDiscreteActions and 
 * MADPComponentDiscreteObservations,
 * such that, for each of the MultiAgentDecisionProcess::GetNrAgents() it stores
 * -the discrete action sets 
 * -the discrete observation sets
 *
 * It also extends MADPComponentDiscreteStates such that a discrete state space 
 * is implemented. 
 * 
 * Finally, this class itself stores (pointers to)
 * -the transition model
 * -the observation model */
class MultiAgentDecisionProcessDiscrete : 
    virtual public MultiAgentDecisionProcessDiscreteInterface,
    virtual public MultiAgentDecisionProcessInterface,
    public MultiAgentDecisionProcess
{
private:    

    MADPComponentDiscreteStates _m_S;
    MADPComponentDiscreteActions _m_A;
    MADPComponentDiscreteObservations _m_O;

    ///Boolean to indicate whether this MADPDiscrete has been initialized.
    bool _m_initialized;    

    /**\brief Boolean that controls whether sparse transition and 
     * observation models are used.
     */
    bool _m_sparse;

    /**\brief Boolean that controls whether the observation model is defined over events.
     */
    bool _m_eventObservability;
        
    /**\brief Check whether models appear valid probability
     * distributions.  
     *
     * This is a private function as to avoid
     * doubts as to what is and is not checked. (when called from
     * a derived class, no derived features are checked).*/
    bool SanityCheck(void);

    /// Pointer to transition model
    TransitionModelDiscrete* _m_p_tModel;

    /// Pointer to observation model
    ObservationModelDiscrete* _m_p_oModel;

    /**\brief An index representing false negative observations, which are seen by
     * planners, but not by agents. This allows us to model and simulate unobservable transitions.
     * A negative value indicates that this property has not been set.
     */
    int _m_falseNegativeObs;

protected:    

    //data manipulation (set) functions:
    /**\brief Sets _m_initialized to b. 
     *
     * When setting to true, a verification of
     * member elements is performed. (i.e. a check whether all vectors
     * have the correct size and non-zero entries) */
    bool SetInitialized(bool b);
public:
    // Constructor, destructor and copy assignment.
    
    /**\brief Constructor that  sets the 
     * \li name
     * \li description 
     * \li problem file for the MADP.*/
    MultiAgentDecisionProcessDiscrete(
        const std::string &name="received unspec. by MultiAgentDecisionProcessDiscrete",
        const std::string &descr="received unspec.by MultiAgentDecisionProcessDiscrete",
        const std::string &pf="received unspec. by MultiAgentDecisionProcessDiscrete");
    /**\brief Constructor that  sets the 
     * \li nrAgents
     * \li nrStates
     * \li name
     * \li description 
     * \li problem file for the MADP.*/
    MultiAgentDecisionProcessDiscrete(
        size_t nrAgents, size_t nrS,
        const std::string &name="received unspec. by MultiAgentDecisionProcessDiscrete",
        const std::string &descr="received unspec.by MultiAgentDecisionProcessDiscrete",
        const std::string &pf="received unspec. by MultiAgentDecisionProcessDiscrete");
    
    /// Copy constructor.
    MultiAgentDecisionProcessDiscrete(const MultiAgentDecisionProcessDiscrete& a);
    
    ///Destructor.
    ~MultiAgentDecisionProcessDiscrete();

    size_t GetNrStates() const { return(_m_S.GetNrStates()); }
    const State* GetState(Index i) const { return(_m_S.GetState(i)); }
    std::string SoftPrintState(Index sI) const { return(_m_S.SoftPrintState(sI)); }
    double GetInitialStateProbability(Index sI) const { return(_m_S.GetInitialStateProbability(sI)); }
    StateDistribution* GetISD() const { return(_m_S.GetISD()); }
    Index SampleInitialState() const { return(_m_S.SampleInitialState()); }

    const std::vector<size_t>& GetNrActions() const { return(_m_A.GetNrActions()); }
    size_t GetNrActions(Index AgentI) const { return(_m_A.GetNrActions(AgentI)); }
    size_t GetNrJointActions() const { return(_m_A.GetNrJointActions()); }
    size_t GetNrJointActions(const Scope& agScope) const { return(_m_A.GetNrJointActions(agScope)); }
    const Action* GetAction(Index agentI, Index a) const { return(_m_A.GetAction(agentI,a)); }
    const JointAction* GetJointAction(Index i) const { return(_m_A.GetJointAction(i)); }
    Index IndividualToJointActionIndices(const Index* AI_ar) const
        { return(_m_A.IndividualToJointActionIndices(AI_ar)); }
    Index IndividualToJointActionIndices(const std::vector<Index>& indivActionIndices) const
        { return(_m_A.IndividualToJointActionIndices(indivActionIndices)); }
    const std::vector<Index>& JointToIndividualActionIndices(Index jaI) const
        { return(_m_A.JointToIndividualActionIndices(jaI)); }
    Index IndividualToJointActionIndices(const std::vector<Index>& ja_e, const Scope& agSC) const
        { return(_m_A.IndividualToJointActionIndices(ja_e, agSC)); }
    std::vector<Index> JointToIndividualActionIndices(Index ja_e, const Scope& agSC) const
        { return(_m_A.JointToIndividualActionIndices(ja_e, agSC)); }
    Index JointToRestrictedJointActionIndex(Index jaI, const Scope& agSc_e ) const
        { return(_m_A.JointToRestrictedJointActionIndex(jaI, agSc_e)); }

    const std::vector<size_t>& GetNrObservations() const { return(_m_O.GetNrObservations()); }
    size_t GetNrObservations(Index AgentI) const { return(_m_O.GetNrObservations(AgentI)); }
    size_t GetNrJointObservations() const { return(_m_O.GetNrJointObservations()); }
    const Observation* GetObservation(Index agentI, Index a) const
        { return(_m_O.GetObservation(agentI,a)); }
    const JointObservation* GetJointObservation(Index i) const
        { return(_m_O.GetJointObservation(i)); }
    Index IndividualToJointObservationIndices(const std::vector<Index>& indivObservationIndices) const
        { return(_m_O.IndividualToJointObservationIndices(indivObservationIndices)); }
    const std::vector<Index>& JointToIndividualObservationIndices(Index joI) const
        { return(_m_O.JointToIndividualObservationIndices(joI)); }
    Index IndividualToJointObservationIndices(
        const std::vector<Index>& jo_e, const Scope& agSC) const
        { return(_m_O.IndividualToJointObservationIndices(jo_e,agSC)); }
    std::vector<Index> JointToIndividualObservationIndices(Index jo_e, const Scope& agSC) const 
        { return(_m_O.JointToIndividualObservationIndices(jo_e,agSC)); }
    Index JointToRestrictedJointObservationIndex(Index joI, const Scope& agSc_e ) const
        { return(_m_O.JointToRestrictedJointObservationIndex(joI,agSc_e)); }


    void SetNrStates(size_t nrS) { _m_S.SetNrStates(nrS); }
    void AddState(const std::string &StateName) { _m_S.AddState(StateName); }
    void SetISD(StateDistribution* p) { _m_S.SetISD(p); }
//    void SetISD(StateDistributionVector* p) { _m_S.SetISD(p); }
    void SetUniformISD() { _m_S.SetUniformISD(); }
    Index GetStateIndexByName(const std::string &s) const
        { return(_m_S.GetStateIndexByName(s)); }
    bool SetStatesInitialized(bool b) { return(_m_S.SetInitialized(b)); }

    void SetNrObservations(Index AI, size_t nrO) { _m_O.SetNrObservations(AI,nrO); }
    void AddObservation(Index AI, const std::string &name, const std::string &description="") 
        { _m_O.AddObservation(AI,name, description); }
    const ObservationDiscrete* GetObservationDiscrete(Index agentI, 
                                                      Index a) const
        { return(_m_O.GetObservationDiscrete(agentI,a)); }
    size_t ConstructJointObservations() { return(_m_O.ConstructJointObservations()); }
    Index GetObservationIndexByName(const std::string &o, Index agentI) const
        { return(_m_O.GetObservationIndexByName(o,agentI)); }
    bool SetObservationsInitialized(bool b) { return(_m_O.SetInitialized(b)); }

    void SetNrActions(Index AI, size_t nrA) { _m_A.SetNrActions(AI,nrA); }
    void AddAction(Index AI, const std::string &name, const std::string &description="") 
        { _m_A.AddAction(AI,name, description); }
    const ActionDiscrete* GetActionDiscrete(Index agentI, Index a) const
        { return(_m_A.GetActionDiscrete(agentI,a)); }
    size_t ConstructJointActions() { return(_m_A.ConstructJointActions()); }
    Index GetActionIndexByName(const std::string &a, Index agentI) const
        { return(_m_A.GetActionIndexByName(a,agentI)); }
    bool SetActionsInitialized(bool b) { return(_m_A.SetInitialized(b)); }

    /**\brief A function that can be called by other classes in order to
     * request a MultiAgentDecisionProcessDiscrete to (try to)
     * initialize.*/
    bool Initialize()
        {return SetInitialized(true);}

    ///Creates a new transition model mapping.
    void CreateNewTransitionModel();
    ///Creates a new observation model mapping.
    void CreateNewObservationModel();

    ///Set the probability of successor state sucSI: P(sucSI|sI,jaI).
    void SetTransitionProbability(Index sI, Index jaI, Index sucSI, 
                                  double p);

    ///Set the probability of joint observation joI: P(joI|jaI,sucSI).
    void SetObservationProbability(Index jaI, Index sucSI, Index joI, 
                                   double p);
    void SetObservationProbability(Index sI, Index jaI, Index sucSI, Index joI, 
                                   double p);        
    // 'get' functions:     
    ///Return the probability of successor state sucSI: P(sucSI|sI,jaI).
    double GetTransitionProbability(Index sI, Index jaI, Index sucSI) 
        const;

    TGet* GetTGet() const;
    OGet* GetOGet() const;

    ///Return the probability of joint observation joI: P(joI|jaI,sucSI).
    double GetObservationProbability(Index jaI, Index sucSI, Index joI) 
        const;
    double GetObservationProbability(Index sI, Index jaI, Index sucSI, Index joI) 
        const;

    /// Sample a successor state.
    Index SampleSuccessorState(Index sI, Index jaI) const;
    
    /// Sample an observation.
    Index SampleJointObservation(Index jaI, Index sucI) const;
    Index SampleJointObservation(Index sI, Index jaI, Index sucI) const;
    

    ///SoftPrints information on the MultiAgentDecisionProcessDiscrete.
    std::string SoftPrint() const;
    ///Prints some information on the MultiAgentDecisionProcessDiscrete.
    void Print() const
        { std::cout << SoftPrint();}
        
    /**\brief Indicate whether sparse transition and observation models
     * should be used. 
     *
     * Default is to not use sparse models. Only
     * has effect before the class has been initialized. */
    void SetSparse(bool sparse);
    
    /// Are we using sparse transition and observation models?
    bool GetSparse() const { return(_m_sparse); }

    /**\brief Indicate whether the observation model
     * is defined over (s',a,s) (an event-driven model)
     * or the standard (s',a)
     *
     * Default is to not use event-driven models.*/
    void SetEventObservability(bool eventO);

    ///Sets the index for false negative observations (see above)
    void SetFalseNegativeObs(Index falseNegativeObs)
    {_m_falseNegativeObs = falseNegativeObs;}

    /// Are we using an event observation model?
    bool GetEventObservability() const { return(_m_eventObservability); }

    ///Gets the index for false negative observations (if any). 
    ///A negative value means that are none (which is the default case).
    int GetFalseNegativeObs() const { return(_m_falseNegativeObs); }

    const TransitionModelDiscrete* GetTransitionModelDiscretePtr() const
        { return(_m_p_tModel); }
    
    const ObservationModelDiscrete* GetObservationModelDiscretePtr() const
        { return(_m_p_oModel); }
    
    /// Set the transition model.
    void SetTransitionModelPtr(TransitionModelDiscrete* ptr)
        { _m_p_tModel=ptr; }

    /// Set the obversation model.
    void SetObservationModelPtr(ObservationModelDiscrete* ptr)
        { _m_p_oModel=ptr; }
    
    /// Returns a pointer to a copy of this class.
    virtual MultiAgentDecisionProcessDiscrete* Clone() const
        { return new MultiAgentDecisionProcessDiscrete(*this); }

};

#include "TransitionModelDiscrete.h"
#include "ObservationModelDiscrete.h"

inline void MultiAgentDecisionProcessDiscrete::SetTransitionProbability(Index
        sI, Index jaI, Index sucSI, double p)
{ _m_p_tModel->Set(sI, jaI, sucSI, p);}
inline void MultiAgentDecisionProcessDiscrete::SetObservationProbability(Index
        jaI, Index sucSI, Index joI, double p)
{ _m_p_oModel->Set(jaI, sucSI, joI,p);}
inline void MultiAgentDecisionProcessDiscrete::SetObservationProbability(Index sI, Index
        jaI, Index sucSI, Index joI, double p)
{ _m_p_oModel->Set(sI, jaI, sucSI, joI,p);}
inline double
MultiAgentDecisionProcessDiscrete::GetTransitionProbability(Index sI, Index
        jaI, Index sucSI) const
{ return(_m_p_tModel->Get(sI, jaI, sucSI));}
inline double
MultiAgentDecisionProcessDiscrete::GetObservationProbability(Index jaI, Index
        sucSI, Index joI) const
{ return(_m_p_oModel->Get(jaI, sucSI, joI));}
inline double
MultiAgentDecisionProcessDiscrete::GetObservationProbability(Index sI, Index jaI, Index
        sucSI, Index joI) const
{ return(_m_p_oModel->Get(sI, jaI, sucSI, joI));}
inline Index MultiAgentDecisionProcessDiscrete::SampleSuccessorState(Index
        sI, Index jaI) const
{ return(_m_p_tModel->SampleSuccessorState(sI,jaI));}
inline Index MultiAgentDecisionProcessDiscrete::SampleJointObservation(Index
        jaI, Index sucI) const
{ return(_m_p_oModel->SampleJointObservation(jaI,sucI)); }
inline Index MultiAgentDecisionProcessDiscrete::SampleJointObservation(Index sI, Index
        jaI, Index sucI) const
{ return(_m_p_oModel->SampleJointObservation(sI,jaI,sucI)); }

#endif /* !_MULTIAGENTDECISIONPROCESS_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
