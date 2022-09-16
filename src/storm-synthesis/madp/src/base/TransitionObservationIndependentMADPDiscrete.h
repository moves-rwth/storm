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
#ifndef _TRANSITIONOBSERVATIONINDEPENDENTMADPDISCRETE_H_
#define _TRANSITIONOBSERVATIONINDEPENDENTMADPDISCRETE_H_ 1

/* the include directives */
#include "Globals.h"
#include "MultiAgentDecisionProcess.h"
#include "MultiAgentDecisionProcessDiscreteInterface.h"
#include "TransitionModelDiscrete.h"
#include "ObservationModelDiscrete.h"
#include "MultiAgentDecisionProcessDiscrete.h"
#include "TGet.h"
#include "OGet.h"
#include <map>
#include "StateDistributionVector.h"

/** 
 * \brief TransitionObservationIndependentMADPDiscrete is an base class that 
 * defines the primary properties of a Transition and Observation independent
 * decision process. 
 *
 * It implements the transition and observation model by assigning a regular
 * (1-agent) MultiAgentDecisionProcessDiscrete for each agent: the agents
 * local model.
 * This class is responsible for constructing the joint actions and
 * observations (upon which rewards will typically be based).
 *
 *
 * */
class TransitionObservationIndependentMADPDiscrete : 
    public MultiAgentDecisionProcess,
    virtual public MultiAgentDecisionProcessDiscreteInterface
{
private:    
    /// Boolean to indicate whether this MADPDiscrete has been initialized.
    bool _m_initialized;
    /// Boolean that indicates whether models should be stored sparsely.
    bool _m_sparse;

    /// Boolean indicating whether joint indices have been cached.
    bool _m_jointIndicesCached;
    /// Boolean indicating whether joint models have been generated.
    bool _m_jointModelsGenerated;


    //cache this here (set by SetInitialized)
    size_t _m_nr_agents;

    /**In a transition-observation independent MADP, each agent has a set
     * of local states and observations together with an individual 
     * transition and observation model. We model this as each agent having
     * its own MultiAgentDecisionProcessDiscrete. This is the vector that 
     * contains a pointer to each agent's MultiAgentDecisionProcessDiscrete.
     */
    std::vector<MultiAgentDecisionProcessDiscrete*> _m_individualMADPDs;

    /**The vector storing the joint actions */    
    std::vector<JointActionDiscrete*> _m_jointActionVec;
    std::map<Index, JointActionDiscrete*> *_m_jointActionMap;
    
    std::vector<std::vector<Index> > _m_jointToIndActionCache;

    size_t * _m_actionStepSizeArray;
    std::vector<size_t> _m_actionStepSize;
    std::vector<size_t> _m_observationStepSize;
    std::vector<size_t> _m_stateStepSize;

    /**Vector that stores the number of individual states. (Created by
     * CreateJointStates() )*/
    std::vector<size_t> _m_nrIndivStates;

    /**The vector storing the individual state indices for each joint
     * index: _m_indivStateIndices[jointStateIndex] = vector<Index> */
    std::vector< std::vector<Index> > _m_indivStateIndices;
    std::map< Index, std::vector<Index> > *_m_indivStateIndicesMap;
    
    std::vector<State*> _m_jointStates;
    std::map<std::vector<Index>, State*> *_m_jointStatesMap;
    
    size_t _m_nrJointStates;

    /**The vector containing the initial state distr over joint states.*/
    //std::vector<double> _m_initialStateDistribution;
    StateDistributionVector* _m_initialStateDistribution;
    /**the number of joint actions.*/
    size_t _m_nrJointActions;
    
    std::vector<size_t> _m_nrIndivActions;
    
    std::vector<size_t> _m_nrIndivObs;
    
    std::vector<std::vector<ObservationDiscrete> > _m_indivObs;
    std::vector<JointObservationDiscrete*> _m_jointObs;
    std::map<Index, JointObservationDiscrete*> *_m_jointObsMap;
    
    size_t _m_nrJointObservations;
    
    std::vector<std::vector<Index> > _m_jointToIndObsCache;

    TransitionModelDiscrete* _m_p_tModel;

    ObservationModelDiscrete* _m_p_oModel;

// initialization functions
        
    /**Recursively constructs all the joint actions.
     * Works by calling
     * CreateJointActionsRecursively on a new (empty) joint action.*/
    void CreateJointActions();
    /* Recursively creates the joint actions (_m_jointActionVec) 
     * using _m_actionVecs (which need to be initialized before calling
     * this function...) */    
    size_t CreateJointActionsRecursively( Index curAgentI, 
                                          JointActionDiscrete& ja, Index jaI);
    
    /**This function generates the joint -> individual state index cache.
     * This function assumes that all the agents and their local states 
     * have been added.*/
    void CreateJointStates();
    
    void CreateJointObservations();

    void CreateISD();

    size_t ConstructJointObservationsRecursively( 
        Index curAgentI, JointObservationDiscrete& jo, Index joI);
    
    std::vector<Index> JointToIndividualActionIndicesNoCache(Index jaI) const;
    
    std::vector<Index> JointToIndividualObservationIndicesNoCache(Index joI) 
        const;
    
protected:    
    //data manipulation (set) functions:
    /** Sets _m_initialized to b. When setting to true, a verification of
     * member elements is performed. (i.e. a check whether all vectors
     * have the correct size and non-zero entries) */
    virtual bool SetInitialized(bool b);

    bool GetSparse() const { return(_m_sparse); }

    void CreateCentralizedSparseModels();
    void CreateCentralizedObservationTransitionModel();
    void CreateCentralizedFullModels();

public:
    bool Initialize()
        {return(SetInitialized(true));}
    
    // Constructor, destructor and copy assignment.
    // Default constructor.
    TransitionObservationIndependentMADPDiscrete(
        const std::string &name="received unspec. by TransitionObservationIndependentMADPDiscrete",
        const std::string &descr="received unspec.by TransitionObservationIndependentMADPDiscrete",
        const std::string &pf="received unspec. by TransitionObservationIndependentMADPDiscrete",
        bool cacheFlatModels=false);
    // Copy assignment constructor.
    TransitionObservationIndependentMADPDiscrete(
        const TransitionObservationIndependentMADPDiscrete&);
    
    ///Destructor.
    virtual ~TransitionObservationIndependentMADPDiscrete();
    
// pre-initialization functions, functions that are used to construct the models
// etc.
    /**\brief Sets the number of agents to n.*/
    void SetNrAgents(size_t n);
    /**\brief Adds one agent with an optional name.*/
    void AddAgent(const std::string &name="unspec.");
    /**\brief Sets the number of states for the specified agent.*/
    void SetNrStates(Index agentI, size_t nr);
    /**\brief Adds a state with a particular name for the specified agent.*/
    void AddState(Index agentI, const std::string &name);
    /**\brief Sets the number of actions for the specified agent.*/
    void SetNrActions(Index agentI, size_t nr);
    /**\brief Adds an action with a particular name for the specified agent.
         * */
    void AddAction(Index agentI, const std::string &name);
    /**\brief Sets the number of Observations for the specified agent.*/
    void SetNrObservations(Index agentI, size_t nr);
    /**\brief Adds an Observation with a particular name for the specified 
     * agent.*/
    void AddObservation(Index agentI, const std::string &name);
#if 0 // will be computed from individual ISDs
    /**Sets the initial state distribution to a uniform one.*/
    void SetUniformISD();
#endif
    /**\brief Sets the initial state distribution to v.*/
    void SetISD(const std::vector<double> &v);

    //get (data) functions:   
    /**Returns a pointer to agentsI's individual model.
     */
    MultiAgentDecisionProcessDiscrete* GetIndividualMADPD(Index agentI)
        const
        {return _m_individualMADPDs[agentI];}
    /**\brief return the number of joint actions.*/
    size_t GetNrJointActions() const
        {
            if(!_m_initialized)
                throw(E("TransitionObservationIndependentMADPDiscrete::GetNrJointActions() not yet initialized"));

            return(_m_nrJointActions);
        }
    size_t GetNrJointActions(const Scope& agScope) const;

    /**\brief return a ref to the i-th joint action (a JointActionDiscrete).*/
    const JointActionDiscrete* GetJointActionDiscrete(Index i) const;

    size_t GetNrJointStates() const
        {
            if(!_m_initialized)
                throw(E("TransitionObservationIndependentMADPDiscrete::GetNrJointStates() not yet initialized"));
            
            return(_m_nrJointStates);
        }
    
//some shortcut functions        

    /**\brief return the number of actions of agent agentI*/    
    size_t GetNrActions(Index agentI) const
        {return GetIndividualMADPD(agentI)->GetNrActions(0);}
    /**\brief Returns the number of local states for agent i.*/
    size_t GetNrStates(Index agI) const
        {return _m_individualMADPDs[agI]->GetNrStates();}
    
    /**\brief return the number of observations of agent agentI*/    
    size_t GetNrObservations(Index agentI) const
        {return GetIndividualMADPD(agentI)->GetNrObservations(0);}
    
//joint <-> individual action conversion

    /**\brief Returns the joint action index that corresponds to the vector 
     * of specified individual action indices.*/
    Index IndividualToJointActionIndices(const std::vector<Index>& 
                                          indivActionIndices) const
        {
            if(!_m_initialized)
                throw(E("TransitionObservationIndependentMADPDiscrete::IndividualToJointActionIndices(vector<Index>&) - Error: not initialized."));
            return(IndexTools::IndividualToJointIndicesStepSize(indivActionIndices,
                                                                _m_actionStepSize));
        }

    /**\brief returns a vector of indices to indiv. action indicies corr.
     * to joint action index jaI.*/
    const std::vector<Index>& JointToIndividualActionIndices(Index jaI) const {
        if(_m_jointIndicesCached)
            return(_m_jointToIndActionCache.at(jaI));
        else
        {
            throw(E("TransitionObservationIndependentMADPDiscrete::JointToIndividualActionIndices did not cache conversion"));
//            return(JointToIndividualActionIndicesNoCache(jaI));
        }
    }
    Index IndividualToJointActionIndices(const Index* AI_ar) const
        {return IndexTools::IndividualToJointIndicesArrayStepSize(
             AI_ar, _m_actionStepSizeArray, _m_nr_agents);}
    Index IndividualToJointActionIndices(
        const std::vector<Index>& ja_e, const Scope& agSC) const
        {
            // identical to MADPComponentDiscreteActions
            std::vector<size_t> nr_A_e(agSC.size());
            IndexTools::RestrictIndividualIndicesToScope(
                GetNrActions(), agSC, nr_A_e);
            Index jaI = IndexTools::IndividualToJointIndices( ja_e, nr_A_e);
            return(jaI);
        }
    std::vector<Index> JointToIndividualActionIndices(
        Index ja_e, const Scope& agSC) const
        { 
            // identical to MADPComponentDiscreteActions
            std::vector<size_t> nr_A_e(agSC.size());
            IndexTools::RestrictIndividualIndicesToScope(
                GetNrActions(), agSC, nr_A_e);
            std::vector<Index> ja_e_vec = IndexTools::JointToIndividualIndices(ja_e, nr_A_e);
            return(ja_e_vec);
        }

    Index JointToRestrictedJointActionIndex(Index jaI, const Scope& agSc_e ) const
        {
            const std::vector<Index>& ja_vec = JointToIndividualActionIndices(jaI);
            std::vector<Index> ja_vec_e(agSc_e.size());
            IndexTools::RestrictIndividualIndicesToScope(ja_vec, agSc_e, ja_vec_e);
            Index ja_e = IndividualToJointActionIndices(ja_vec_e, agSc_e);
            return(ja_e);
        }

    /**\brief returns a vector of individual (local) state indices
     * corresponding to joint state index jointSI.*/
    const std::vector<Index>& JointToIndividualStateIndices(Index jointSI) const
        {
            if(_m_jointIndicesCached)
                return _m_indivStateIndices[jointSI];
            else if (_m_indivStateIndicesMap->find(jointSI)!=
                     _m_indivStateIndicesMap->end())
                return(_m_indivStateIndicesMap->find(jointSI)->second);
            else
            {
                std::vector<Index> ind_sI=
                    IndexTools::JointToIndividualIndicesStepSize(jointSI,_m_stateStepSize);
                _m_indivStateIndicesMap->insert(make_pair(jointSI,ind_sI));
                return(_m_indivStateIndicesMap->find(jointSI)->second);
            }
        }

    /**\brief returns the joint index for indivStateIndices*/
    Index IndividualToJointStateIndices(const std::vector<Index>&
                                        indivStateIndices) const
        {
            return(IndexTools::IndividualToJointIndicesStepSize(indivStateIndices,
                                                                _m_stateStepSize));
        }
    
    /**\brief returns the joint index for indivObsIndices*/
    Index IndividualToJointObservationIndices(const std::vector<Index>& 
                                               indivObsIndices) const
        {
            if(!_m_initialized)
                throw(E("TransitionObservationIndependentMADPDiscrete::IndividualToJointObservationIndices(const vector<Index>) - Error: not initialized. "));
            return(IndexTools::IndividualToJointIndicesStepSize(indivObsIndices,_m_observationStepSize));
        }

    /**\brief returns the individual indices for joint observation joI.*/
    const std::vector<Index>& JointToIndividualObservationIndices(Index joI) 
        const
        {
            if(_m_jointIndicesCached)
                return(_m_jointToIndObsCache.at(joI));
            else
            {
                throw(E("TransitionObservationIndependentMADPDiscrete::JointToIndividualActionIndices did not cache conversion"));
//                return(JointToIndividualObservationIndicesNoCache(joI));
            }
        }

    /**\brief SoftPrints the action set for each agent.*/ 
    std::string SoftPrintActionSets() const;
    /**\brief Prints the action set for each agent.*/ 
    void PrintActionSets() const
        {std::cout << SoftPrintActionSets();}
    /**\brief SoftPrints the set of joint actions.*/
    std::string SoftPrintJointActionSet() const;
    /**\brief Prints the set of joint actions.*/
    void PrintJointActionSet() const
        {std::cout << SoftPrintJointActionSet();}
    /**\brief SoftPrints information regarding this 
     * TransitionObservationIndependentMADPDiscrete.*/
    std::string SoftPrint() const;
    /**\brief Prints information regarding this 
     * TransitionObservationIndependentMADPDiscrete.*/
    void Print() const
        {std::cout << SoftPrint();}
    std::string SoftPrintState(Index sI) const;

    void SetSparse(bool sparse);

    // stuff to implement MultiAgentDecisionProcessDiscreteInterface
    
    /**\brief returns probability of joint transition (the product of
     * the probabilities of the individual transitions)
     */
    double GetTransitionProbability(Index sI, Index jaI, Index 
                                    sucSI) const;
    double GetTransitionProbability(const std::vector<Index> &sIs,
                                    const std::vector<Index> &aIs,
                                    const std::vector<Index> &sucSIs) const
        { 
            double p=1;
            for(Index agI = 0; agI < GetNrAgents(); agI++)
            {
                p*=GetIndividualMADPD(agI)->GetTransitionProbability(
                    sIs[agI],
                    aIs[agI],
                    sucSIs[agI]);
                if(p==0)
                    break;
            }
            return(p);
        }

    /**\brief Returns the probability of the joint observation joI (the
     * product of the individual observation probabilities, which depend
     * only on local states).
     */
    double GetObservationProbability(Index jaI, Index sucSI, 
                                     Index joI) const;
    double GetObservationProbability(const std::vector<Index> &aIs,
                                     const std::vector<Index> &sucSIs, 
                                     const std::vector<Index> &oIs) const
        {
            double p=1;
            for(Index agI = 0; agI < GetNrAgents(); agI++)
            {
                p*=GetIndividualMADPD(agI)->GetObservationProbability(
                    aIs[agI],
                    sucSIs[agI],
                    oIs[agI]);
                if(p==0)
                    break;
            }
            return(p);
        }

    /**\brief returns a successor state index sampled according to the 
     * transition probabilities.
     */
    Index SampleSuccessorState(Index sI, Index jaI) const
        {
            std::vector<Index> sIs=JointToIndividualStateIndices(sI);
            std::vector<Index> aIs=JointToIndividualActionIndices(jaI);
            return(IndividualToJointStateIndices(SampleSuccessorState(sIs,aIs)));
        }

    std::vector<Index> SampleSuccessorState(const std::vector<Index> &sIs,
                                             const std::vector<Index> &aIs)
        const
        {
            std::vector<Index> sucSIs(GetNrAgents());
            for(Index agI = 0; agI < GetNrAgents(); agI++)
                sucSIs[agI]=GetIndividualMADPD(agI)->SampleSuccessorState(sIs[agI],
                                                                          aIs[agI]);
            return(sucSIs);
        }
                                             
    /**\brief Returns a joint observation, sampled according to the 
     * observation probabilities.*/
    Index SampleJointObservation(Index jaI, Index sucI) const
        {
            std::vector<Index> sucIs=JointToIndividualStateIndices(sucI);
            std::vector<Index> aIs=JointToIndividualActionIndices(jaI);
            return(IndividualToJointObservationIndices(SampleJointObservation(aIs,
                                                                              sucIs)));
        }

    std::vector<Index> SampleJointObservation(const std::vector<Index> &aIs,
                                               const std::vector<Index> &sucIs)
        const
        {
            std::vector<Index> oIs(GetNrAgents());
            
            for(Index agI = 0; agI < GetNrAgents(); agI++)
                oIs[agI]=GetIndividualMADPD(agI)->SampleJointObservation(aIs[agI],
                                                                         sucIs[agI]);
            return(oIs);
        }


    /**\brief Samples an initial state.
     */
    Index SampleInitialState() const
        {
            return(IndividualToJointStateIndices(SampleInitialStates()));
        }

    std::vector<Index> SampleInitialStates() const
        {
            std::vector<Index> sIs(GetNrAgents());
            
            for(Index agI = 0; agI < GetNrAgents(); agI++)
                sIs[agI]=GetIndividualMADPD(agI)->SampleInitialState();
            
            return(sIs);
        }


    /**\brief returns the number of (joint) states.*/
    size_t GetNrStates() const
        { return(GetNrJointStates()); }
    
    /**\brief Returns a pointer to state i.*/
    const State* GetState(Index i) const
        {
            if(_m_jointIndicesCached) // we cached all joint states
                return(_m_jointStates.at(i));
            else
                return(GetState(JointToIndividualStateIndices(i)));
        }

    const State* GetState(const std::vector<Index> &sIs) const;
    
    /**\brief returns the prob. of state sI according to the initial state
     * distribution. */
    double GetInitialStateProbability(Index sI) const;

    /**\brief returns the initial state distribution.*/
    //std::vector<double> GetISD() const;
    virtual StateDistributionVector* GetISD() const;
    
    /**\brief returns a vector with the number of actions for each agent.*/
    const std::vector<size_t>& GetNrActions() const
        { return(_m_nrIndivActions); }
    
    /**\brief Get a pointer to action a of agentI.*/
    const Action* GetAction(Index agentI, Index a) const
        { return(GetIndividualMADPD(agentI)->GetAction(0,a)); }
    
    /**\brief Returns a pointer to joint action i.*/
    const JointAction* GetJointAction(Index i) const
        { return(GetJointActionDiscrete(i)); }
    
    /**\brief converts individual to joint actions.*/
    Index IndividualToJointActionIndices(Index* IndexArray) const
        {
            return(IndexTools::IndividualToJointIndicesArray(IndexArray,
                                                             _m_nrIndivActions));
        }
    
    /**\brief Returns a vector with the number of observations for each
     * agent.*/
    const std::vector<size_t>& GetNrObservations() const
        { return(_m_nrIndivObs); }
    
    /**\brief Get the number of joint observations*/
    size_t GetNrJointObservations() const
        {
            if(!_m_initialized)
                throw(E("TransitionObservationIndependentMADPDiscrete::GetNrJointObservations() not yet initialized"));
            
            return(_m_nrJointObservations);
        }
    
    /**\brief Get a pointer to observation o of agentI.*/
    const Observation* GetObservation(Index agentI, Index o) const
        { return(GetIndividualMADPD(agentI)->GetObservation(0,o)); }
    
    /**\brief Get a pointer to the i-th joint observation.*/
    const JointObservation* GetJointObservation(Index i) const;

    TGet* GetTGet() const
        {    
            if(!_m_jointModelsGenerated)
                return 0;
            
            if(_m_sparse)
                return new TGet_TransitionModelMappingSparse(
                    ((TransitionModelMappingSparse*)_m_p_tModel)  ); 
            else
                return new TGet_TransitionModelMapping(
                    ((TransitionModelMapping*)_m_p_tModel)  );
        }

    OGet* GetOGet() const
        {
            if(!_m_jointModelsGenerated)
                return 0;

            if(_m_sparse)
                return new OGet_ObservationModelMappingSparse(
                    ((ObservationModelMappingSparse*)_m_p_oModel)  ); 
            else
                return new OGet_ObservationModelMapping(
                    ((ObservationModelMapping*)_m_p_oModel)  );
        }

    TransitionModelDiscrete* GetTransitionModelDiscretePtr() const
        { return(_m_p_tModel); }
    
    ObservationModelDiscrete* GetObservationModelDiscretePtr() const
        { return(_m_p_oModel); }

    // this one is called externally sometimes
    void CreateCentralizedSparseTransitionModel();
    
};

#endif /* !_TRANSITIONOBSERVATIONINDEPENDENTMADPDISCRETE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
