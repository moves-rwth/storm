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
#ifndef _MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATES_H_
#define _MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATES_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
//#include "MultiAgentDecisionProcessDiscreteInterface.h"
#include "MultiAgentDecisionProcess.h"
#include "MultiAgentDecisionProcessDiscreteFactoredStatesInterface.h"
#include "MADPComponentFactoredStates.h"
#include "MADPComponentDiscreteActions.h"
#include "MADPComponentDiscreteObservations.h"
#include "TwoStageDynamicBayesianNetwork.h"
#include "FSDist_COF.h"

#define MADP_DFS_WARNINGS 0

/**\brief MultiAgentDecisionProcessDiscreteFactoredStates is a class that 
 * represents the dynamics of a MAS with a factored state space.
 *
 * The agents are inherited from the MultiAgentDecisionProcess.
 * Their actions and observations are discrete and defined and implemented in 
 * MADPComponentXXX
 *
 * The state space is factored and defined and implemented in 
 * MADPComponentFactoredStates.
 *
 * This class implements/maintains the factored transition and observation 
 * models through means of a TwoStageDynamicBayesianNetwork.
 * */
class MultiAgentDecisionProcessDiscreteFactoredStates 
    :
    virtual public MultiAgentDecisionProcessDiscreteFactoredStatesInterface,
    public MultiAgentDecisionProcess
{
private:    

    MADPComponentFactoredStates _m_S;
    MADPComponentDiscreteActions _m_A;
    MADPComponentDiscreteObservations _m_O;

    ///Boolean to indicate whether this MADPDiscrete has been initialized.
    bool _m_initialized;  

    //to add implementation of factored transition and observation model
    //...
    ///Check whether models appear valid probability distributions.  
    bool SanityCheck() const
    {return(SanityCheckTransitions() && SanityCheckObservations());}

    /** \brief Pointer to *the flat (chached)* transition model
     */
    TransitionModelDiscrete* _m_p_tModel;

    /** \brief Pointer to *the flat (chached)* observation model
     */
    ObservationModelDiscrete* _m_p_oModel;

    bool _m_cached_FlatTM;
    bool _m_sparse_FlatTM;
    bool _m_cached_FlatOM;
    bool _m_sparse_FlatOM;

    /**\brief Boolean that controls whether the observation model is defined over events.
     */
    bool _m_eventObservability;

    TwoStageDynamicBayesianNetwork _m_2dbn;

    virtual void SetYScopes() = 0;
    virtual void SetOScopes() = 0;
    virtual void SetScopes()
    {SetYScopes(); SetOScopes();}
    
    virtual double ComputeTransitionProb(
        Index y,
        Index yVal,
        const std::vector< Index>& Xs,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys
        ) const = 0;
    virtual double ComputeObservationProb(
        Index o,
        Index oVal,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys,
        const std::vector< Index>& Os
        ) const = 0;
    virtual double ComputeObservationProb(
        Index o,
        Index oVal,
        const std::vector< Index>& Xs,
        const std::vector< Index>& As,
        const std::vector< Index>& Ys,
        const std::vector< Index>& Os
        ) const 
    {return ComputeObservationProb(o,oVal,As,Ys,Os);}

    ///Boolean to indicate whether all connections in the 2TBN are specified
    /**If this is the case, than we can allocate space for CPDs. So this var
     * is referenced by CreateNewTransitionModel and CreateNewObservationModel.
     */
    bool _m_connectionsSpecified;

protected:
    // subclasses have direct access to 2DBN
    TwoStageDynamicBayesianNetwork* Get2DBN()
    {return &_m_2dbn;}

    virtual bool SanityCheckTransitions() const;
    virtual bool SanityCheckObservations() const;

public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    MultiAgentDecisionProcessDiscreteFactoredStates(
        const std::string &name="received unspec. by MultiAgentDecisionProcessDiscreteFactoredStates", 
        const std::string &descr="received unspec. by MultiAgentDecisionProcessDiscreteFactoredStates", 
        const std::string &pf="received unspec. by MultiAgentDecisionProcessDiscreteFactoredStates");
    /// Copy constructor.
    MultiAgentDecisionProcessDiscreteFactoredStates(const MultiAgentDecisionProcessDiscreteFactoredStates& a);
    /// Destructor.
    ~MultiAgentDecisionProcessDiscreteFactoredStates();
    /// Copy assignment operator
    MultiAgentDecisionProcessDiscreteFactoredStates& operator= (const MultiAgentDecisionProcessDiscreteFactoredStates& o);


    size_t GetNrStates() const { return(_m_S.GetNrStates()); }
    const State* GetState(Index i) const { return(_m_S.GetState(i)); }
    std::string SoftPrintState(Index sI) const { return(_m_S.SoftPrintState(sI)); }
    double GetInitialStateProbability(Index sI) const { return(_m_S.GetInitialStateProbability(sI)); }
    StateDistribution* GetISD() const { return(_m_S.GetISD()); }
    Index SampleInitialState() const { return(_m_S.SampleInitialState()); }
    void SampleInitialState(std::vector<Index> &sIs) const { _m_S.SampleInitialState(sIs); }

    size_t GetNrStateFactors() const { return(_m_S.GetNrStateFactors()); }
    const Scope& GetAllStateFactorScope() const { return(_m_S.GetAllStateFactorScope()); }
    const std::vector<size_t>& GetNrValuesPerFactor() const { return(_m_S.GetNrValuesPerFactor()); }
    const size_t GetNrValuesForFactor(Index sf) const { return(_m_S.GetNrValuesForFactor(sf)); }
    const StateFactorDiscrete* GetStateFactorDiscrete(Index sfacI) const { return(_m_S.GetStateFactorDiscrete(sfacI)); }
    const FactoredStateDistribution* GetFactoredISD() const { return(_m_S.GetFactoredISD()); }
    std::vector<Index> StateIndexToFactorValueIndices(Index stateI) const
        { return(_m_S.StateIndexToFactorValueIndices(stateI)); }
    Index StateIndexToFactorValueIndex(Index factor, Index s) const 
        { return(_m_S.StateIndexToFactorValueIndex(factor,s)); }
    Index FactorValueIndicesToStateIndex(const std::vector<Index> &fv) const 
        { return(_m_S.FactorValueIndicesToStateIndex(fv)); }
    Index FactorValueIndicesToStateIndex(const std::vector<Index>& s_e_vec,
                                         const Scope& sfSC) const
        { return(_m_S.FactorValueIndicesToStateIndex(s_e_vec,sfSC)); }
    std::vector<Index> StateIndexToFactorValueIndices(Index s_e, 
                                                      const Scope& sfSC) const
        { return(_m_S.StateIndexToFactorValueIndices(s_e,sfSC)); }

    const std::vector<size_t>& GetNrActions() const { return(_m_A.GetNrActions()); }
    size_t GetNrActions(Index AgentI) const { return(_m_A.GetNrActions(AgentI)); }
    size_t GetNrJointActions() const { return(_m_A.GetNrJointActions()); }
    size_t GetNrJointActions(const Scope& agScope) const { return(_m_A.GetNrJointActions(agScope)); }
    bool JointAIndicesValid() const { return(_m_A.JointIndicesValid()); }
    const Action* GetAction(Index agentI, Index a) const { return(_m_A.GetAction(agentI,a)); }
    const JointAction* GetJointAction(Index i) const { return(_m_A.GetJointAction(i)); }
    size_t ConstructJointActions() { return(_m_A.ConstructJointActions()); }
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
    bool JointOIndicesValid() const { return(_m_O.JointIndicesValid()); }
    size_t ConstructJointObservations() { return(_m_O.ConstructJointObservations()); }
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

    bool JointIndicesValid() const
        { return (JointAIndicesValid() && JointOIndicesValid()); }
    void SetISD(FactoredStateDistribution* p) { _m_S.SetISD(p); }
//    void SetISD(StateDistributionVector* p) { _m_S.SetISD(p); }
    void SetUniformISD() { _m_S.SetUniformISD(); }
    Index AddStateFactor(const std::string &n="undef. name",
                         const std::string &d="undef. descr.")
        { return(_m_S.AddStateFactor(n,d)); }
    Index AddStateFactorValue(Index sf, const std::string &v="undef")
        { return(_m_S.AddStateFactorValue(sf,v)); }
    
    /**\brief This function removes a state factor from the model's
     * MADPComponentFactoredStates, fixes the factor indices, and
     * adjusts the 2DBN accordingly - all CPTs of nodes which depend on the 
     * removed state factor are recalculated by marginalizing their 
     * respective clique joints.
     * */
    void RemoveStateFactor(Index sf);

    bool SetStatesInitialized(bool b) { return(_m_S.SetInitialized(b)); }

    void SetNrActions(Index AI, size_t nrA) { _m_A.SetNrActions(AI,nrA); }
    void AddAction(Index AI, const std::string &name,
                   const std::string &description="") { _m_A.AddAction(AI,name, description); }
    bool SetActionsInitialized(bool b) { return(_m_A.SetInitialized(b)); }

    void SetNrObservations(Index AI, size_t nrO) { _m_O.SetNrObservations(AI,nrO); }
    void AddObservation(Index AI, const std::string &name,
                        const std::string &description="") { _m_O.AddObservation(AI,name, description); }
    bool SetObservationsInitialized(bool b) { return(_m_O.SetInitialized(b)); }


    Scope StateScopeBackup( const Scope & stateScope, 
                            const Scope & agentScope) const
        { return(_m_2dbn.StateScopeBackup(stateScope,agentScope)); }
    Scope AgentScopeBackup( const Scope & stateScope, 
                            const Scope & agentScope) const
        { return(_m_2dbn.AgentScopeBackup(stateScope,agentScope)); }
    double GetYOProbability(const Scope& X, const std::vector<Index>& Xs,
                            const Scope& A, const std::vector<Index>& As,
                            const Scope& Y, const std::vector<Index>& Ys,
                            const Scope& O, const std::vector<Index>& Os) const
        { return(_m_2dbn.GetYOProbability(X,Xs,A,As,Y,Ys,O,Os)); }
    void SetSoI_Y( Index y, 
                   const Scope& XSoI, 
                   const Scope& ASoI, 
                   const Scope& YSoI) 
        { _m_2dbn.SetSoI_Y(y,XSoI,ASoI,YSoI);}
    void SetSoI_O( Index o, 
                   const Scope& ASoI, 
                   const Scope& YSoI, 
                   const Scope& OSoI)
        { _m_2dbn.SetSoI_O(o,ASoI,YSoI,OSoI); }
    void SetSoI_O( Index o, 
                   const Scope& XSoI,
                   const Scope& ASoI, 
                   const Scope& YSoI, 
                   const Scope& OSoI)
        { _m_2dbn.SetSoI_O(o,XSoI,ASoI,YSoI,OSoI); }
    
    const Scope& GetXSoI_Y(Index y) const
        { return(_m_2dbn.GetXSoI_Y(y)); }
    const Scope& GetASoI_Y(Index y) const
        { return(_m_2dbn.GetASoI_Y(y)); }
    const Scope& GetYSoI_Y(Index y) const
        { return(_m_2dbn.GetYSoI_Y(y)); }
        
    const Scope& GetXSoI_O(Index o) const
        { return(_m_2dbn.GetXSoI_O(o)); }
    const Scope& GetASoI_O(Index o) const
        { return(_m_2dbn.GetASoI_O(o)); }
    const Scope& GetYSoI_O(Index o) const
        { return(_m_2dbn.GetYSoI_O(o)); }
    const Scope& GetOSoI_O(Index o) const
        { return(_m_2dbn.GetOSoI_O(o)); }

    //operators:

    //data manipulation (set) functions:
    bool SetInitialized(bool b);
    void SetConnectionsSpecified(bool b)
    {   _m_connectionsSpecified = b; }


    void SetSparse(bool b)
    {
#if MADP_DFS_WARNINGS
       std::cerr<< "Warning MultiAgentDecisionProcessDiscreteFactoredStates SetSparse not yet implemented" << std::endl;
#endif                 
    }
    void SetEventObservability(bool eventO)
    {_m_eventObservability=eventO;}

    ///Creates a new transition model: initializes new CPDs for the 2BDN
    /**This first checks whether all connections are specified 
     * (_m_connectionsSpecified) and consequently allocates CPDs for
     * the 2DBN.
     *
     * In the future it should be possible to specify what kind of CPDs
     * are used (e.g. CPT, sparse CPT, ADD, rules etc.)
     * 
     */
    void CreateNewTransitionModel();
    ///Creates a new observation model mapping: initializes new CPDs 
    /**This first checks whether all connections are specified 
     * (_m_connectionsSpecified) and consequently allocates CPDs for
     * the 2DBN.
     *
     * In the future it should be possible to specify what kind of CPDs
     * are used (e.g. CPT, sparse CPT, ADD, rules etc.)
     */
    void CreateNewObservationModel();



/* perhaps some forwarding functions here to set probabilities?        
        ///Set the probability of successor state sucSI: P(sucSI|sI,jaI).
        void SetTransitionProbability(Index sI, Index jaI, Index sucSI, 
                double p);
        ///Set the probability of joint observation joI: P(joI|jaI,sucSI).
        void SetObservationProbability(Index jaI, Index sucSI, Index joI, 
                double p);
*/

    //get (data) functions:
    //
    const TwoStageDynamicBayesianNetwork* Get2DBN() const
    {return &_m_2dbn;}

//implement the MultiAgentDecisionProcessDiscreteFactoredStatesInterface
//(i.e., the functions not handled by MADPComponentFactoredStates )

    ///Get the number of joint instantiations for the factors in sfScope
    size_t GetNrStateFactorInstantiations(const Scope& sfScope) const;

    /// Are we using an event observation model?
    bool GetEventObservability() const { return(_m_eventObservability); }

    ///SoftPrints information on the MultiAgentDecisionProcessDiscrete.
    std::string SoftPrint() const;


//implement the MultiAgentDecisionProcessDiscreteInterface.h:
    

    double GetTransitionProbability (Index sI, Index jaI, Index sucSI) const;
    TGet* GetTGet() const;

    double GetObservationProbability  (Index jaI, Index sucSI, Index joI) const;
    /// O(s,ja,s',jo) version. You can access a standard O(ja,s',jo) model both ways
    /// (the PS index is simply ignored in the latter case).
    double GetObservationProbability  (Index sI, Index jaI, Index sucSI, Index joI) const;
    OGet* GetOGet() const;

    Index SampleSuccessorState (Index sI, Index jaI) const;
    void SampleSuccessorState(const std::vector<Index> &sIs,
                              const std::vector<Index> &aIs,
                              std::vector<Index> &sucIs) const;
    Index SampleJointObservation(Index jaI, Index sucI) const;
    Index SampleJointObservation(Index sI, Index jaI, Index sucI) const;
    void SampleJointObservation(const std::vector<Index> &aIs,
                                const std::vector<Index> &sucIs,
                                std::vector<Index> &oIs) const
    { SampleJointObservation(std::vector<Index>(),aIs,sucIs,oIs); }
    void SampleJointObservation(const std::vector<Index> &sIs,
                                const std::vector<Index> &aIs,
                                const std::vector<Index> &sucIs,
                                std::vector<Index> &oIs) const;

    //the following are implemented by MADPComponentFactoredStates 
    //double GetInitialStateProbability(Globals::Index) const;
    //std::vector<double> GetISD() const;
    //Globals::Index SampleInitialState() const;

    void CacheFlatTransitionModel(bool sparse=false);
    void CacheFlatObservationModel(bool sparse=false);

    //the observation and transition model are represented by the 
    //TwoStageDynamicBayesianNetwork so the following functions are 
    //problematic...
    //However, they can simply return 0
    const TransitionModelDiscrete* GetTransitionModelDiscretePtr() const
    {
        if(_m_cached_FlatTM)
            return _m_p_tModel;
        else
            return(0);        
    }
    const ObservationModelDiscrete* GetObservationModelDiscretePtr() const
    { 
        if(_m_cached_FlatOM)
            return _m_p_oModel;
        else
            return(0);        
    }
    
    /**\brief This function marginalizes a state factor out of the flat
     * joint transition and observation models of the system. The function then
     * removes that factor from the process model altogether (through RemoveStateFactor).
     * Currently, it only supports the marginalization of nodes without
     * NS dependencies, and which do not directly influence any LRF
     * */
    void MarginalizeTransitionObservationModel(const Index sf, bool sparse);
    
    /**
     * \brief This is the base class for functors that set the scopes of the 2-DBN.
     */
    class ScopeFunctor
    {
    public:
        virtual void operator()(void) = 0;
    };

    /**
     * \brief This is the base class for functors that return the transition probability for a given (s,a,s') tuple.
     */
    class TransitionProbFunctor
    {
    public:
        virtual double operator()(Index y,
                                  Index yVal,
                                  const std::vector< Index>& Xs,
                                  const std::vector< Index>& As,
                                  const std::vector< Index>& Ys) const = 0;
    };
    
    /**
     * \brief This is the base class for functors that return the observation probability for a given (s,a,s',o) tuple.
     */
    class ObservationProbFunctor
    {
    public:
        ObservationProbFunctor(bool isEmpty = false) :
        _m_isEmpty(isEmpty){}
        
        virtual double operator()(Index o,
                                  Index oVal,
                                  const std::vector< Index>& Xs,
                                  const std::vector< Index>& As,
                                  const std::vector< Index>& Ys,
                                  const std::vector< Index>& Os) const
                                  {return 0;}
                                  
        bool isEmpty()
        {return _m_isEmpty;}
                                
    private:  
        bool _m_isEmpty;
    };
    
    /**
     * \brief The BoundScopeFunctor class binds the "SetScopes" function to a templated object.
     */
    template <class SF> class BoundScopeFunctor : public ScopeFunctor
    {
    private:
        SF* _m_sf;
        void (SF::*_m_func)();
    public:
        BoundScopeFunctor(SF* sf_ptr, void (SF::*func_ptr) (void)):
        _m_sf(sf_ptr),
        _m_func(func_ptr){};
      
        void operator()(void)
        {(*_m_sf.*_m_func) ();};
    };
    
    /**
     * \brief The BoundTransitionProbFunctor class binds the "ComputeTransitionProb" function to a templated object.
     */    
    template <class TF> class BoundTransitionProbFunctor : public TransitionProbFunctor
    {
    private:
        TF* _m_tf;
        double (TF::*_m_func)(Index,
                              Index,
                              const std::vector< Index>&,
                              const std::vector< Index>&,
                              const std::vector< Index>&) const;
    public:
        BoundTransitionProbFunctor(TF* tf_ptr, double (TF::*func_ptr) (Index,
                                                                       Index,
                                                                       const std::vector< Index>&,
                                                                       const std::vector< Index>&,
                                                                       const std::vector< Index>&) const):
                                                                       _m_tf(tf_ptr),
                                                                       _m_func(func_ptr){};
                                                                     
        double operator()(Index y,
                          Index yVal,
                          const std::vector< Index>& Xs,
                          const std::vector< Index>& As,
                          const std::vector< Index>& Ys) const
                          {return (*_m_tf.*_m_func) (y,yVal,Xs,As,Ys);};
    };
    
    /**
     * \brief The BoundObservationProbFunctor class binds the "ComputeObservationProb" function to a templated object.
     */     
    template <class OF> class BoundObservationProbFunctor : public ObservationProbFunctor
    {
    private:
        OF* _m_of;
        double (OF::*_m_func)(Index,
                              Index,
                              const std::vector< Index>&,
                              const std::vector< Index>&,
                              const std::vector< Index>&,
                              const std::vector< Index>&) const;
    public:
        BoundObservationProbFunctor(OF* of_ptr, double (OF::*func_ptr) (Index,
                                                                        Index,
                                                                        const std::vector< Index>&,
                                                                        const std::vector< Index>&,
                                                                        const std::vector< Index>&,
                                                                        const std::vector< Index>&) const):
                                                                        _m_of(of_ptr),
                                                                        _m_func(func_ptr){};
        
        double operator()(Index o,
                          Index oVal,
                          const std::vector< Index>& Xs,
                          const std::vector< Index>& As,
                          const std::vector< Index>& Ys,
                          const std::vector< Index>& Os) const
                          {return (*_m_of.*_m_func) (o,oVal,Xs,As,Ys,Os);};
    };
    
    /**
     * The EmptyObservationProbFunctor class can be used by fully-observable subclasses of
     * MultiAgentDecisionProcessDiscreteFactoredStates, in order to initialize the 2DBN without
     * requiring an actual observation function.
     */
    class EmptyObservationProbFunctor : public ObservationProbFunctor
    {          
    public:     
        EmptyObservationProbFunctor() :
        ObservationProbFunctor(true){};
    };
    
    virtual void Initialize2DBN();
    /**
     * This signature allows us to initialize the 2DBN using externally supplied functors to
     * set the scopes, and compute transition and observation probabilities in a discrete factored
     * model. This is useful, for example, if we want to read these from a file 
     * (e.g. as done by ParserProbModelXML) instead of creating ad-hoc implementations of each
     * of these functions for each specific planning problem.
     */
    virtual void Initialize2DBN(ScopeFunctor& SetScopes,
                                TransitionProbFunctor& ComputeTransitionProb,
                                ObservationProbFunctor& ComputeObservationProb);
};




#endif /* !_MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATES_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
