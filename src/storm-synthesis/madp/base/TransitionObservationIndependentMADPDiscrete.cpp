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

#include "TransitionObservationIndependentMADPDiscrete.h"    
#include "TransitionModelMappingSparse.h"
#include "TransitionModelMapping.h"
#include "ObservationModelMappingSparse.h"
#include "ObservationModelMapping.h"

#include "VectorTools.h"

using namespace std;

#define DEBUG_TOIMADPD 0
#define DEBUG_CJA 0
#define DEBUG_CJO 0
#define DEBUG_CENTRALIZEDSPARSEMODELS 0

TransitionObservationIndependentMADPDiscrete::
TransitionObservationIndependentMADPDiscrete(
    const string &name, const string &descr, const string &pf,
    bool cacheFlatModels) :
    MultiAgentDecisionProcess(name, descr, pf)
{
    _m_initialized = false;
    _m_sparse = false;
    if(cacheFlatModels)
    {
        _m_jointIndicesCached = true;
        _m_jointModelsGenerated = true;
    }
    else
    {
        _m_jointIndicesCached = false;
        _m_jointModelsGenerated = false;
    }
    _m_nrJointActions=0;
    _m_nrJointStates=0;
    _m_nrJointObservations=0;
    _m_p_tModel = 0;
    _m_p_oModel = 0;
    // these are pointers to get around the const-ness of their Get functions
    _m_jointStatesMap=new map<vector<Index>, State*>;
    _m_indivStateIndicesMap = new map< Index, vector<Index> >;
    _m_jointActionMap=new map<Index, JointActionDiscrete*>;
    _m_jointObsMap=new map<Index, JointObservationDiscrete*>;
    _m_initialStateDistribution=0;
}

TransitionObservationIndependentMADPDiscrete::
TransitionObservationIndependentMADPDiscrete(
    const TransitionObservationIndependentMADPDiscrete&)
{
    throw(E("TransitionObservationIndependentMADPDiscrete: copy ctor not implemented"));
}

TransitionObservationIndependentMADPDiscrete::
~TransitionObservationIndependentMADPDiscrete()
{
    // this destructor leaks memory

    vector<MultiAgentDecisionProcessDiscrete*>::iterator it = 
        _m_individualMADPDs.begin();
    vector<MultiAgentDecisionProcessDiscrete*>::iterator last = 
        _m_individualMADPDs.end();
    while(it != last)
    {
        delete *it;//pointer to MultiAgentDecisionProcessDiscrete
        it++;
    }
    _m_individualMADPDs.clear();
    
    vector<JointActionDiscrete*>::iterator it2 = 
        _m_jointActionVec.begin();
    vector<JointActionDiscrete*>::iterator last2 = 
        _m_jointActionVec.end();
    while(it2 != last2)
    {
        delete *it2; //pointer to MultiAgentDecisionProcessDiscrete
        it2++;
    }
    _m_jointActionVec.clear();
}


void TransitionObservationIndependentMADPDiscrete::SetNrAgents(size_t n)
{
    MultiAgentDecisionProcess::SetNrAgents(n);
    vector<MultiAgentDecisionProcessDiscrete*>::iterator it =  
        _m_individualMADPDs.begin();
    vector<MultiAgentDecisionProcessDiscrete*>::iterator last =  
        _m_individualMADPDs.end();
    while(it != last)
    {
        delete (*it);
        it++;
    }

    _m_individualMADPDs.clear();
    for(Index i=0; i < n; i++)
    {
        stringstream name, descr, pf;
        name << "Indiv.MADPDiscr.Agent"<<i;
        descr << "The individual MADPDiscrete used to model the transition \
and observation dynamics for agent "<<i;
        pf << GetProblemFile() << ".agent" << i;

        MultiAgentDecisionProcessDiscrete* m = new 
            MultiAgentDecisionProcessDiscrete(1, 0, name.str(), descr.str(),
                   pf.str() );
        m->SetSparse(_m_sparse);
        _m_individualMADPDs.push_back( m );
    }

    SetInitialized(false);
}

void TransitionObservationIndependentMADPDiscrete::AddAgent(const string &namestring)
{
    Index i = GetNrAgents();//the old number of agents
    MultiAgentDecisionProcess::AddAgent(namestring);

    stringstream name, descr, pf;
    name << "Indiv.MADPDiscr.Agent"<<i;
    descr << "The individual MADPDiscrete used to model the transition \
and observation dynamics for agent "<<namestring <<"(agent "<<i<<")";
    pf << GetProblemFile() << ".agent" << i;

    MultiAgentDecisionProcessDiscrete* m = new 
        MultiAgentDecisionProcessDiscrete(1, 0, name.str(), descr.str(),
               pf.str() );
    m->SetSparse(_m_sparse);
    _m_individualMADPDs.push_back( m );

    SetInitialized(false);
}

void TransitionObservationIndependentMADPDiscrete::SetNrStates(Index agentI, size_t nr)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::SetNrStates - agentI out of bounds...");
    _m_individualMADPDs[agentI]->SetNrStates(nr);    
}
void TransitionObservationIndependentMADPDiscrete::AddState(Index agentI, const string &name)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::AddState - agentI out of bounds...");
    _m_individualMADPDs[agentI]->AddState(name);    
}

void TransitionObservationIndependentMADPDiscrete::SetNrActions(Index agentI, size_t nr)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::SetNrActions - agentI out of bounds...");
    _m_individualMADPDs[agentI]->SetNrActions(0, nr);    
}

void TransitionObservationIndependentMADPDiscrete::AddAction(Index agentI, const string &name)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::AddAction - agentI out of bounds...");
    _m_individualMADPDs[agentI]->AddAction(0, name);    
}

void TransitionObservationIndependentMADPDiscrete::SetNrObservations(Index agentI, size_t nr)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::SetNrObservations - agentI out of bounds...");
    _m_individualMADPDs[agentI]->SetNrObservations(0, nr);    
}

void TransitionObservationIndependentMADPDiscrete::AddObservation(Index agentI, const string &name)
{
    if(agentI >= _m_individualMADPDs.size())
        throw EInvalidIndex("TransitionObservationIndependentMADPDiscrete::AddObservation - agentI out of bounds...");
    _m_individualMADPDs[agentI]->AddObservation(0, name);    
}

#if 0 // will be computed from individual ISDs
void TransitionObservationIndependentMADPDiscrete::SetUniformISD()
{
    size_t nrJS =  GetNrJointStates();
    if(_m_initialStateDistribution->size() != nrJS)
        _m_initialStateDistribution->resize(nrJS);
    double uprob = 1.0 / nrJS;
    vector<double>::iterator it = _m_initialStateDistribution->begin();
    vector<double>::iterator last = _m_initialStateDistribution->end();
    while(it!=last)
    {
        *it = uprob;
        it++;
    }
}
#endif

void  TransitionObservationIndependentMADPDiscrete::SetISD(const vector<double> &v)
{
    if(_m_nrJointStates==0)
        throw(E("TransitionObservationIndependentMADPDiscrete::CreateISD() joint states should have been created already"));
    
    if(v.size() != _m_nrJointStates)
        throw E("TransitionObservationIndependentMADPDiscrete::SetISD - ERROR: nrStates don't match!");

    delete _m_initialStateDistribution;
    _m_initialStateDistribution = new StateDistributionVector(v);
}

void TransitionObservationIndependentMADPDiscrete::CreateJointActions()
{
    _m_jointActionVec.clear();
    if(!_m_jointModelsGenerated)
    {
        _m_nrJointActions=1;
        for(Index agI = 0; agI < GetNrAgents(); agI++)
            _m_nrJointActions*=(GetIndividualMADPD(agI)->GetNrActions(0));
    }
    else
    {
        JointActionDiscrete* ja = new JointActionDiscrete();
        _m_nrJointActions = CreateJointActionsRecursively(0, *ja, 0);

        if(_m_nrJointActions!=_m_jointActionVec.size())
            throw(E("TransitionObservationIndependentMADPDiscrete::CreateJointActions() sizes do not match"));
    }

    _m_nrIndivActions.clear();
    for(Index agI = 0; agI < GetNrAgents(); agI++)
        _m_nrIndivActions.push_back(GetIndividualMADPD(agI)->GetNrActions(0));

    _m_jointToIndActionCache.clear();
    if(_m_jointIndicesCached)
        for(Index ja=0;ja!=_m_nrJointActions;++ja)
            _m_jointToIndActionCache.push_back(
                JointToIndividualActionIndicesNoCache(ja));
}

size_t TransitionObservationIndependentMADPDiscrete::
CreateJointActionsRecursively(Index curAgentI, JointActionDiscrete& ja, 
        Index jaI)
{
if(DEBUG_CJA)    cerr << "TransitionObservationIndependentMADPDiscrete::CreateJointActions(Index "<<curAgentI<<", JointActionDiscrete& ja, Index "<< jaI<<") called"<<endl;

    bool lastAgent=false;
    if(curAgentI == _m_nrAgents-1)
    {
        lastAgent = true;    
        if(DEBUG_CJA)     cerr << "\nlast agent\n";
    }

    //we are going to walk through this agents action vector
    size_t nrA = GetNrActions(curAgentI);
    
    //first action extends the received ja 
    JointActionDiscrete* p_jaReceivedArgCopy = new JointActionDiscrete(ja);
    JointActionDiscrete* p_ja;
    
    for(Index a=0; a < nrA; a++)
    {
        const ActionDiscrete * adp = GetIndividualMADPD(curAgentI)->
            GetActionDiscrete(0, a);
        if(DEBUG_CJA) cerr << "\nnext action";
//        if(it == first) //
        if(a == 0)      
        {
            if(DEBUG_CJA) cerr << "(first action - not making copy)\n";
            p_ja = &ja;
        }
        else if ( a == nrA-1 )//it == beforelast)
            //this is the last last action   
        {
            if(DEBUG_CJA) cerr << "(last action - not making copy)\n";
            p_ja = p_jaReceivedArgCopy; //don't make a new copy
        }
        else //make a new copy
        {        
            if(DEBUG_CJA) cerr << "(intermed. action - making copy)\n";
            p_ja = new JointActionDiscrete(*p_jaReceivedArgCopy);    
        }    
        if(lastAgent)
        {
            p_ja->SetIndex(jaI);
            if(DEBUG_CJA)cerr << "setting index of this joint action to: "<< 
                jaI <<endl;
        }
//        ActionDiscrete* adp = /*(ActionDiscrete*)*/ &(*it);
        if(DEBUG_CJA)
            cerr <<"Adding agent's indiv. action to joint action..."<<endl;
        p_ja->AddIndividualAction(adp, curAgentI);        
        if(lastAgent) //jointAction is now completed: add it to jointAction set.
        {
            if(DEBUG_CJA){cerr << "INSERTING the joint action:"; 
                p_ja->Print();cerr<<endl;}
            _m_jointActionVec.push_back(p_ja);
            if(DEBUG_CJA){cerr << "\nINSERTED the joint action."<<endl;
                cerr << "_m_jointActionVec now containts "<< 
                _m_jointActionVec.size() << " joint actions." << endl;}
            jaI++;
        }
        else
            jaI = CreateJointActionsRecursively(curAgentI+1,*p_ja, jaI);
        
//        it++;
    }
    if(DEBUG_CJA)    cerr << ">>ProblemDecTiger::CreateJointActionsRecursively(Index "<<curAgentI<<", JointActionDiscrete& ja, Index "<< jaI<<") FINISHED"<<endl;
    return jaI;
}


void TransitionObservationIndependentMADPDiscrete::CreateJointStates()
{
    _m_jointStates.clear();
    size_t nrAg = GetNrAgents();
    _m_nrIndivStates.clear();
    _m_nrJointStates=1;
    for(Index agI = 0; agI < nrAg; agI++)
    {
        size_t nrS = GetIndividualMADPD(agI)->GetNrStates();
        _m_nrIndivStates.push_back(nrS);
        _m_nrJointStates*=nrS;
    }
    if(_m_jointIndicesCached)
    {
        vector<Index> ind_sI(nrAg, 0);
        size_t i=0;
        do
        {
            _m_indivStateIndices.push_back(ind_sI);
            State *state=new StateDiscrete(i);
            string name="";
            for(Index agI = 0; agI < nrAg; agI++)
            {
                if(agI>0)
                    name+="_";
                name+=GetIndividualMADPD(agI)->GetState(ind_sI[agI])->GetName();
            }
            state->SetName(name);
            state->SetDescription("");
            _m_jointStates.push_back(state);
            i++;
        } while(! IndexTools::Increment(ind_sI, _m_nrIndivStates) );
        
        if(_m_nrJointStates!=_m_jointStates.size())
            throw(E("TransitionObservationIndependentMADPDiscrete::CreateJointStates() sizes do not match"));
    }
}

const State* TransitionObservationIndependentMADPDiscrete::GetState(
    const std::vector<Index> &sIs) const
{
    // we cached the ones already asked for
    if(_m_jointStatesMap->find(sIs)!=_m_jointStatesMap->end())
        return(_m_jointStatesMap->find(sIs)->second);
    else // create new joint state and add it to cache
    {
        State *state=new State; // not a StateDiscrete, since the
                                // index might overflow
        string name="";
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            if(agI>0)
                name+="_";
            name+=GetIndividualMADPD(agI)->GetState(sIs[agI])->GetName();
        }
        state->SetName(name);
        state->SetDescription("");
        _m_jointStatesMap->insert(make_pair(sIs,state));
        return(state);
    }
}

const JointActionDiscrete* 
TransitionObservationIndependentMADPDiscrete::GetJointActionDiscrete(Index i) const
{
    if(_m_jointIndicesCached) // we cached all joint actions
        return(_m_jointActionVec.at(i));
    // we cached the ones already asked for
    else if(_m_jointActionMap->find(i)!=_m_jointActionMap->end())
        return(_m_jointActionMap->find(i)->second);
    else // create new joint action and add it to cache
    {
        JointActionDiscrete *action=new JointActionDiscrete(i);
        vector<Index> ind_sI=
            IndexTools::JointToIndividualIndices(i,_m_nrIndivActions);
        for(Index agI = 0; agI < GetNrAgents(); agI++)
            action->AddIndividualAction(
                GetIndividualMADPD(agI)->GetActionDiscrete(0,ind_sI[agI]),
                agI);
        
        _m_jointActionMap->insert(make_pair(i,action));
        return(action);
    }
}

const JointObservation* 
TransitionObservationIndependentMADPDiscrete::GetJointObservation(Index i) const
{
    if(_m_jointIndicesCached) // we cached all joint obs
        return(_m_jointObs.at(i));
    // we cached the ones already asked for
    else if(_m_jointObsMap->find(i)!=_m_jointObsMap->end())
        return(_m_jointObsMap->find(i)->second);
    else // create new joint obs and add it to cache
    {
        JointObservationDiscrete *observation=new JointObservationDiscrete(i);
        vector<Index> ind_sI=
            IndexTools::JointToIndividualIndices(i,_m_nrIndivObs);
        for(Index agI = 0; agI < GetNrAgents(); agI++)
            observation->AddIndividualObservation(
                GetIndividualMADPD(agI)->GetObservationDiscrete(0,ind_sI[agI]),
                agI);
        
        _m_jointObsMap->insert(make_pair(i,observation));
        return(observation);
    }
}

void TransitionObservationIndependentMADPDiscrete::CreateJointObservations()
{
    _m_jointObs.clear();
    size_t nrAg = GetNrAgents();
    _m_nrIndivObs.clear();
    for(Index agI = 0; agI < nrAg; agI++)
    {
        size_t nrO = GetIndividualMADPD(agI)->GetNrObservations(0);
        _m_nrIndivObs.push_back(nrO);
    }

    for(Index agI = 0; agI < nrAg; agI++)
    {
        vector<ObservationDiscrete> indObs;
        for(Index o=0; o!=GetIndividualMADPD(agI)->GetNrObservations(0); ++o)
            indObs.push_back(*GetIndividualMADPD(agI)->
                             GetObservationDiscrete(0,o));
        _m_indivObs.push_back(indObs);
    }

    if(!_m_jointIndicesCached)
    {
        _m_nrJointObservations=1;
        for(Index agI = 0; agI < nrAg; agI++)
            _m_nrJointObservations*=GetIndividualMADPD(agI)->
                GetNrObservations(0);
    }
    else
    {
        JointObservationDiscrete* jo = new JointObservationDiscrete();
        _m_nrJointObservations=ConstructJointObservationsRecursively(0, *jo, 0);
        if(_m_nrJointObservations!=_m_jointObs.size())
            throw(E("TransitionObservationIndependentMADPDiscrete::CreateJointObservations() sizes do not match"));
        
        for(Index jo=0;jo!=_m_nrJointObservations;++jo)
            _m_jointToIndObsCache.push_back(
                JointToIndividualObservationIndicesNoCache(jo));
    }
}

size_t TransitionObservationIndependentMADPDiscrete::
ConstructJointObservationsRecursively( 
    Index curAgentI, JointObservationDiscrete& jo, Index joI)
{
    bool lastAgent=false;
    if(curAgentI == GetNrAgents()-1)
    {
        lastAgent = true;    
    }    
    if(curAgentI >= _m_indivObs.size())
    {
        stringstream ss;
        ss << "ConstructJointObservationsRecursively - current Agent index ("<<
            curAgentI<<") out of bounds! (_m_indivObs contains "<<
            "observations for "<<_m_indivObs.size() << " agents...)\n";
        throw E(ss.str().c_str());
    }
    ObservationDVec::iterator first = _m_indivObs[curAgentI].begin();
    ObservationDVec::iterator it = _m_indivObs[curAgentI].begin();
    ObservationDVec::iterator last = _m_indivObs[curAgentI].end();
    ObservationDVec::iterator beforelast = _m_indivObs[curAgentI].end();
    beforelast--;

    if(it == last)
    {
        stringstream ss;
            ss << " empty observation set for agent " << curAgentI;
        throw E(ss);
    }
    //first observation extends the received jo 
    JointObservationDiscrete* p_joReceivedArgCopy =
        new JointObservationDiscrete(jo);
    JointObservationDiscrete* p_jo;
        
    while( it != last) // other observations extend duplicates of jo
    {
        if(it == first) //
        {
            p_jo = &jo;
        }
        else if (it == beforelast)//this is the last valid it -> last observation   
        {
            p_jo = p_joReceivedArgCopy; //don't make a new copy
        }
        else //make a new copy
        {        
            p_jo = new JointObservationDiscrete(*p_joReceivedArgCopy);    
        }    
        if(lastAgent)
        {
            p_jo->SetIndex(joI);
            if(DEBUG_CJO)    
                cerr << "setting index of this observation to: "<< joI <<endl;
        }
        ObservationDiscrete* ai = /*(ObservationDiscrete*)*/ &(*it);
        if(DEBUG_CJO)    
            cerr << "Adding agent's indiv. observation to joint observation..."<<endl;
        p_jo->AddIndividualObservation(ai, curAgentI);
        
        if(lastAgent)//jointObservation is now completed:add it to the jointObservation set.
        {
            if(DEBUG_CJO)
            {cerr<<"INSERTING the joint observation:"; p_jo->Print();cerr<<endl;}
            _m_jointObs.push_back(p_jo);
            if(DEBUG_CJO){cerr << "\nINSERTED the joint observation"<<endl<< "_m_jointObservationVec now containts "<< _m_jointObs.size() << " joint actions." << endl;}
            joI++;
        }
        else
            joI = ConstructJointObservationsRecursively(curAgentI+1,*p_jo, joI);
        it++;
    }
    if(DEBUG_CJO)    cerr << ">> TransitionObservationIndependentMADPDiscrete::ConstructJointObservationsRecursively(Index "<<curAgentI<<", JointObservationDiscrete& jo, Index "<< joI<<") FINISHED"<<endl;
    return joI;
    
}

void TransitionObservationIndependentMADPDiscrete::CreateISD()
{
    size_t nrJS = _m_nrJointStates;
    if(nrJS==0)
        throw(E("TransitionObservationIndependentMADPDiscrete::CreateISD() joint states should have been created already"));

    //vector<vector<double> > isdIs;
    //vector< const StateDistributionVector* > isdIs;
    vector< const StateDistribution* > isdIs;

    for(Index i=0;i!=GetNrAgents();++i)
        isdIs.push_back(GetIndividualMADPD(i)->GetISD());

    if(_m_jointIndicesCached)
    {
        vector<double> ISD(nrJS,1);
        for(Index s=0;s!=nrJS;++s)
            for(Index i=0;i!=GetNrAgents();++i)
                ISD[s]*=  isdIs[i]->GetProbability(_m_indivStateIndices[s][i] );

        SetISD(ISD);
    }
}

bool TransitionObservationIndependentMADPDiscrete::SetInitialized(bool b)
{
    if(b == false)
    {
        if(_m_initialized == true)
            delete [] _m_actionStepSizeArray;
        _m_initialized = false;
        return(true);
    }
   if(_m_initialized == true && b == true)
   {
       //first free mem before re-initialize:
       delete [] _m_actionStepSizeArray;
   }

    if(DEBUG_TOIMADPD)
        cerr << "TransitionObservationIndependentMADPDiscrete::SetInitialized"<<
        " called - GetNrAgents()="<< GetNrAgents()<<endl;
    
    if(GetNrAgents() < 1 )
        throw E("TransitionObservationIndependentMADPDiscrete::SetInitialized(bool b) called, but the number of agents has not been set yet.. (is 0 at the moment...)");

    if(b == true)
    {
        //first initialize the individual models... 
        vector<MultiAgentDecisionProcessDiscrete*>::iterator it = 
            _m_individualMADPDs.begin();
        vector<MultiAgentDecisionProcessDiscrete*>::iterator last = 
            _m_individualMADPDs.end();
        while(it != last)
        {
            (*it)->Initialize();
            it++;
        }

        // for three agents or less we cache the joint indices
        if(GetNrAgents()<=3)
            _m_jointIndicesCached=true;

        //now perform the necessary actions for this class...
        CreateJointActions();
        CreateJointStates();
        CreateJointObservations();
        
        CreateISD();

        _m_nr_agents=GetNrAgents();
        _m_actionStepSizeArray=IndexTools::CalculateStepSize(_m_nrIndivActions);
        _m_actionStepSize=IndexTools::CalculateStepSizeVector(_m_nrIndivActions);
        _m_observationStepSize=IndexTools::CalculateStepSizeVector(_m_nrIndivObs);
        _m_stateStepSize=IndexTools::CalculateStepSizeVector(_m_nrIndivStates);

        _m_initialized = b;

        if(_m_jointModelsGenerated)
        {
            if(_m_sparse)
                CreateCentralizedSparseModels();
            else
                CreateCentralizedFullModels();
        }


    }
    return(true);
}

vector<Index> TransitionObservationIndependentMADPDiscrete::
JointToIndividualActionIndicesNoCache(Index jaI) const
{
#if 0
    if(!_m_initialized)
    {
    stringstream ss;
        ss << "TransitionObservationIndependentMADPDiscrete::"<<
            "JointToIndividualActionIndices("<< jaI<<
            ") - Error: not initialized. "<<endl;
    throw E(ss);
    }
#endif
#if 0
    const JointActionDiscrete* jai = GetJointActionDiscrete(jaI);
    vector<Index> vai = jai->GetIndividualActionDiscretesIndices();
#endif
    vector<Index> vai=IndexTools::JointToIndividualIndices(jaI,
                                                           _m_nrIndivActions);

    if(DEBUG_TOIMADPD)
    {    
        cerr << "TransitionObservationIndependentMADPDiscrete::"<<
            "JointToIndividualActionIndices"<< "(Index "<<jaI<<")" << endl <<
            "vai.size() = " <<vai.size() << " - elements: "<<endl;
        vector<Index>::iterator vai_it = vai.begin();
        vector<Index>::iterator vai_last = vai.end();
        while(vai_it != vai_last)
        {
            cerr << ", " << *vai_it;
            vai_it++;
        }
        cerr << endl;
    }
    return(vai);
}

vector<Index> TransitionObservationIndependentMADPDiscrete::
JointToIndividualObservationIndicesNoCache(Index joI) const
{
#if 0
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "TransitionObservationIndependentMADPDiscrete::"<<
            "JointToIndividualObservationIndices("<< joI<<
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }
#endif
    vector<size_t> nrO;
    for(Index agI=0; agI < GetNrAgents(); agI++)
        nrO.push_back(GetNrObservations(agI));
    vector<Index> voi =
        IndexTools::JointToIndividualIndices(joI,nrO);
    if(DEBUG_TOIMADPD)
    {    
        cerr << "TransitionObservationIndependentMADPDiscrete::"<<
            "JointToIndividualActionIndices"<< "(Index "<<joI<<")" << endl <<
            "voi.size() = " <<voi.size() << " - elements: "<<endl;
        vector<Index>::iterator voi_it = voi.begin();
        vector<Index>::iterator voi_last = voi.end();
        while(voi_it != voi_last)
        {
            cerr << ", " << *voi_it;
            voi_it++;
        }
        cerr << endl;
    }
    return(voi);
}

string TransitionObservationIndependentMADPDiscrete::SoftPrint() const
{    
    stringstream str;
    str << MultiAgentDecisionProcess::SoftPrint();
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "TransitionObservationIndependentMADPDiscrete::Print("<< 
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    str << "#joint states=" << GetNrJointStates() << endl;
    str << "#joint actions="<<GetNrJointActions()<<endl;
    str << "#joint observations="<<GetNrJointObservations()<<endl;
    str << SoftPrintActionSets();
    str << "Joint Actions:"<<endl;
    str << SoftPrintJointActionSet();
    //print the individual MADPs
    str << "Individual MADPs for each agent:"<<endl;
    for(Index agI=0; agI < GetNrAgents(); agI++)
        str << "Agent "<< agI << ":" <<endl << 
            GetIndividualMADPD(agI)->SoftPrint();
    
    return(str.str());
}

string TransitionObservationIndependentMADPDiscrete::SoftPrintActionSets() const
{
    stringstream str;
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "TransitionObservationIndependentMADPDiscrete::PrintAction"<<
                "Sets() - Error: not initialized. "<<endl;
        throw E(ss);
    }    
    str << "Actions:"<<endl;
    for(Index agentIndex=0; agentIndex < _m_nrAgents; agentIndex++)
    {
        str << "agentI " << agentIndex << " - nrActions " << 
            GetNrActions(agentIndex)<<endl; 
        for(Index actionI=0; actionI < GetNrActions(agentIndex); actionI++)
        {

            const ActionDiscrete * adp = GetIndividualMADPD(agentIndex)->
                GetActionDiscrete(0, actionI);
            str << adp->SoftPrint();
            str << endl;
        }
    }
    return(str.str());
}

string TransitionObservationIndependentMADPDiscrete::SoftPrintJointActionSet() 
    const
{
    stringstream str;
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "TransitionObservationIndependentMADPDiscrete::PrintJoint"<<
                "ActionSets() - Error: not initialized. "<<endl;
        throw E(ss);
    }   
    vector<JointActionDiscrete*>::const_iterator ja_it = 
        _m_jointActionVec.begin();
    vector<JointActionDiscrete*>::const_iterator ja_last = 
        _m_jointActionVec.end();
    while(ja_it != ja_last)
    {
        str << (*ja_it)->SoftPrint();
        str<<endl;
        ja_it++;    
    }
    return(str.str());
}

string
TransitionObservationIndependentMADPDiscrete::SoftPrintState(Index sI) const
{
    vector<Index> sIs=JointToIndividualStateIndices(sI);
    stringstream ss;
    for(Index agI = 0; agI < sIs.size(); agI++)
        ss << GetIndividualMADPD(agI)->SoftPrintState(sIs[agI]);
    return(ss.str());
}


double TransitionObservationIndependentMADPDiscrete::
GetTransitionProbability(Index sI,
                         Index jaI,
                         Index sucSI) const
{
    double p=1;
    if(!_m_jointModelsGenerated)
    {
        vector<Index> sIs=JointToIndividualStateIndices(sI),
            sucSIs=JointToIndividualStateIndices(sucSI);
        vector<Index> aIs=JointToIndividualActionIndices(jaI);
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            p*=GetIndividualMADPD(agI)->GetTransitionProbability(
                sIs[agI],
                aIs[agI],
                sucSIs[agI]);
            if(p==0)
                break;
        }
    }
    else
    {
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            p*=GetIndividualMADPD(agI)->GetTransitionProbability(
                _m_indivStateIndices[sI][agI],
                _m_jointToIndActionCache[jaI][agI],
                _m_indivStateIndices[sucSI][agI]);
            if(p==0)
                break;
        }
    }
    return(p);
}

double TransitionObservationIndependentMADPDiscrete::
GetObservationProbability(Index jaI,
                          Index sucSI, 
                          Index joI) const
{
    double p=1;
    if(!_m_jointModelsGenerated)
    {
        vector<Index> sucSIs=JointToIndividualStateIndices(sucSI);
        vector<Index> aIs=JointToIndividualActionIndices(jaI);
        vector<Index> oIs=JointToIndividualObservationIndices(joI);
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            p*=GetIndividualMADPD(agI)->GetObservationProbability(
                aIs[agI],
                sucSIs[agI],
                oIs[agI]);
            if(p==0)
                break;
        }
    }
    else
    {
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            p*=GetIndividualMADPD(agI)->GetObservationProbability(
                _m_jointToIndActionCache[jaI][agI],
                _m_indivStateIndices[sucSI][agI],
                _m_jointToIndObsCache[joI][agI]);
            if(p==0)
                break;
        }
    }
    return(p);
}

StateDistributionVector* TransitionObservationIndependentMADPDiscrete::GetISD() const
{
    if(!_m_jointModelsGenerated)
    {
        throw(E("TransitionObservationIndependentMADPDiscrete::GetISD initial state distribution has not been generated as a double vector."));
    }
    else
        return(_m_initialStateDistribution);
}

double TransitionObservationIndependentMADPDiscrete::GetInitialStateProbability(Index sI) const
{
    if(_m_initialStateDistribution)
        return(_m_initialStateDistribution->at(sI));
    else
    {
        double p=1;
        vector<Index> sIs=JointToIndividualStateIndices(sI);
        for(Index agI = 0; agI < GetNrAgents(); agI++)
        {
            p*=GetIndividualMADPD(agI)->GetInitialStateProbability(
                sIs[agI]);
            if(p==0)
                break;
        }
        return(p);
    }
}

void TransitionObservationIndependentMADPDiscrete::SetSparse(bool sparse)
{
    _m_sparse=sparse;

    vector<MultiAgentDecisionProcessDiscrete*>::iterator it =  
        _m_individualMADPDs.begin();
    vector<MultiAgentDecisionProcessDiscrete*>::iterator last =  
        _m_individualMADPDs.end();
    while(it != last)
    {
        (*it)->SetSparse(sparse);
        it++;
    }
}

///Get the number of joint actions the agents in agScope can form
size_t TransitionObservationIndependentMADPDiscrete::GetNrJointActions(const Scope& agScope) const
{
    if(agScope.size()>0)
    {
        const vector<size_t>& nrActions = GetNrActions();
        vector<size_t> restr_nrAs(agScope.size());
        IndexTools::RestrictIndividualIndicesToScope(
            nrActions, agScope, restr_nrAs);
        size_t restr_nrJA = VectorTools::VectorProduct(restr_nrAs);
        return restr_nrJA;
    }
    else
        return(0);
}


#if 0
 /// Kronecker tensor product of matrices - as per Matlab kron
  template< class Matrix_T >
  void
  kron(const Matrix_T& x, const Matrix_T& y, Matrix_T& z)
  {
    const int rx = x.size1();
    const int cx = x.size2();
    const int ry = y.size1();
    const int cy = y.size2();
    z.resize (rx*ry, cx*cy);
    z.clear ();
    for ( typename Matrix_T::const_iterator1 i = x.begin1();
               i != x.end1(); ++i)
      for ( typename Matrix_T::const_iterator2 j = i.begin();
               j != i.end(); ++j)
        for ( typename Matrix_T::const_iterator1 k = y.begin1();
               k != y.end1(); ++k)
          for (typename Matrix_T::const_iterator2 l = k.begin();
               l != k.end(); ++l)
              z(j.index1()*ry + l.index1(), j.index2()*cy + l.index2()) =
                      (*j) * (*l);
  } 

#endif


void TransitionObservationIndependentMADPDiscrete::CreateCentralizedSparseTransitionModel()
{
    if(!_m_initialized)
    {
        throw(E("TransitionObservationIndependentMADPDiscrete::CreateCentralizedSparseTransitionModel not initialized yet"));
        return;
    }

#if 0
    boost::numeric::ublas::coordinate_matrix<double> a;
    boost::numeric::ublas::compressed_matrix<double> b;
    a=b;
#endif

    _m_p_tModel = new TransitionModelMappingSparse(_m_nrJointStates,
                                                   _m_nrJointActions);

    vector<Index> stateIndices(GetNrAgents(),0),
        sucStateIndices(GetNrAgents(),0),
        actionIndices(GetNrAgents(),0);

    double p=0;
    switch(GetNrAgents())
    {
    case 2:
    case 3:
    {

#if DEBUG_CENTRALIZEDSPARSEMODELS

        for(Index a=0;a!=_m_nrJointActions;++a)
            for(Index s=0;s!=_m_nrJointStates;++s)
                for(Index s1=0;s1!=_m_nrJointStates;++s1)
                {
                    p=GetTransitionProbability(s,a,s1);
                    if(p>0)
                        _m_p_tModel->Set(s,a,s1,p);
                }
        string okModel=_m_p_tModel->SoftPrint();

#endif
        vector<vector<const TransitionModelMappingSparse::SparseMatrix *> > Ts;
        for(Index i=0;i!=GetNrAgents();++i)
        {
            Ts.push_back(vector<const TransitionModelMappingSparse::SparseMatrix *>());
            const TransitionModelMappingSparse *tms=
                dynamic_cast<const TransitionModelMappingSparse *>(_m_individualMADPDs[i]->
                                                                   GetTransitionModelDiscretePtr());
            for(Index a=0;a!=GetNrActions(i);++a)
                Ts.at(i).push_back(tms->GetMatrixPtr(a));
        }

#if 0
        vector<boost::numeric::ublas::coordinate_matrix<double> > Tjoint;
        // this uses a lot of memory, don't know why...
        for(Index ja=0;ja!=GetNrJointActions();++ja)
        {
            cout << _m_nrJointStates << ja << endl;
            Tjoint.push_back(boost::numeric::ublas::coordinate_matrix<double>(_m_nrJointStates,
                                                                              _m_nrJointStates));
        }
#endif
            
        for(Index a0=0;a0!=_m_nrIndivActions[0];++a0)
        {
            actionIndices[0]=a0;
            for(Index a1=0;a1!=_m_nrIndivActions[1];++a1)
            {
                actionIndices[1]=a1;
                Index jaI=IndividualToJointActionIndices(actionIndices);
                
                cout << "trans ja " << jaI << endl;

                for(TransitionModelMappingSparse::SparseMatrix::const_iterator1
                        ri0=Ts[0][a0]->begin1();
                    ri0!=Ts[0][a0]->end1();
                    ++ri0)
                {
                    stateIndices[0]=ri0.index1();
                    for (TransitionModelMappingSparse::SparseMatrix::const_iterator2
                             ci0 = ri0.begin();
                         ci0 != ri0.end();
                         ++ci0)
                    {
                        sucStateIndices[0]=ci0.index2();

                        for(TransitionModelMappingSparse::SparseMatrix::const_iterator1
                                ri1=Ts[1][a1]->begin1();
                            ri1!=Ts[1][a1]->end1();
                            ++ri1)
                        {
                            stateIndices[1]=ri1.index1();
                            for (TransitionModelMappingSparse::SparseMatrix::const_iterator2
                                     ci1 = ri1.begin();
                                 ci1 != ri1.end();
                                 ++ci1)
                            {
                                sucStateIndices[1]=ci1.index2();

                                if(GetNrAgents()==3)
                                {
                                    for(Index a2=0;a2!=_m_nrIndivActions[2];++a2)
                                    {
                                        actionIndices[2]=a2;
                                        Index jaI=IndividualToJointActionIndices(actionIndices);

                                        for(TransitionModelMappingSparse::SparseMatrix::const_iterator1
                                                ri2=Ts[2][a2]->begin1();
                                            ri2!=Ts[2][a2]->end1();
                                            ++ri2)
                                        {
                                            stateIndices[2]=ri2.index1();
                                            for (TransitionModelMappingSparse::SparseMatrix::const_iterator2
                                                     ci2 = ri2.begin();
                                                 ci2 != ri2.end();
                                                 ++ci2)
                                            {
                                                sucStateIndices[2]=ci2.index2();
                                                Index sI=IndividualToJointStateIndices(stateIndices);
                                                Index sucSI=IndividualToJointStateIndices(sucStateIndices);
                                                p=(*ci0)*(*ci1)*(*ci2);
                                                _m_p_tModel->Set(sI, jaI, sucSI, p);
                                            }
                                        }
                                    }
                                }
                                else
                                {                                
                                    Index sI=IndividualToJointStateIndices(stateIndices);
                                    Index sucSI=IndividualToJointStateIndices(sucStateIndices);
                                    p=(*ci0)*(*ci1);
                                    _m_p_tModel->Set(sI, jaI, sucSI, p);
                                }
                            }
                        }
                    }
                }
            }
        }


#if 0
        vector<boost::numeric::ublas::compressed_matrix<double> > Tjoint1;
        for(Index ja=0;ja!=GetNrJointActions();++ja)
            Tjoint1.push_back(boost::numeric::ublas::compressed_matrix<double>(Tjoint[ja]));
#endif

#if DEBUG_CENTRALIZEDSPARSEMODELS
        string newModel=_m_p_tModel->SoftPrint();

        if(okModel!=newModel)
            abort();
#endif

        break;
    }
    default:
        for(Index a=0;a!=_m_nrJointActions;++a)
            for(Index s=0;s!=_m_nrJointStates;++s)
            {
            for(Index s1=0;s1!=_m_nrJointStates;++s1)
            {
                p=GetTransitionProbability(s,a,s1);
                if(p>0)
                    _m_p_tModel->Set(s,a,s1,p);
            }
        }
    }
}

void TransitionObservationIndependentMADPDiscrete::CreateCentralizedObservationTransitionModel()
{
    if(!_m_initialized)
    {
        throw(E("TransitionObservationIndependentMADPDiscrete::CreateCentralizedSparseObservationModel not initialized yet"));
        return;
    }

    _m_p_oModel = new ObservationModelMappingSparse(_m_nrJointStates,
                                                    _m_nrJointActions,
                                                    _m_nrJointObservations);

    double p=0;
    switch(GetNrAgents())
    {
    case 2:
    {
        vector<Index> sucStateIndices(GetNrAgents(),0),
            actionIndices(GetNrAgents(),0),
            observationIndices(GetNrAgents(),0);

#if DEBUG_CENTRALIZEDSPARSEMODELS
        for(Index a=0;a!=_m_nrJointActions;++a)
            for(Index s=0;s!=_m_nrJointStates;++s)
                for(Index o=0;o!=_m_nrJointObservations;++o)
             {
                 p=GetObservationProbability(a,s,o);
                 if(p>0)
                     _m_p_oModel->Set(a,s,o,p);
             }           
        string okModelObs=_m_p_oModel->SoftPrint();
#endif

        vector<vector<const ObservationModelMappingSparse::SparseMatrix *> > Os;
        for(Index i=0;i!=GetNrAgents();++i)
        {
            Os.push_back(vector<const ObservationModelMappingSparse::SparseMatrix *>());
            const ObservationModelMappingSparse *oms=
                dynamic_cast<const ObservationModelMappingSparse *>(_m_individualMADPDs[i]->
                                                                    GetObservationModelDiscretePtr());
            for(Index a=0;a!=GetNrActions(i);++a)
                Os.at(i).push_back(oms->GetMatrixPtr(a));
        }

        for(Index a0=0;a0!=_m_nrIndivActions[0];++a0)
        {
            actionIndices[0]=a0;
            for(Index a1=0;a1!=_m_nrIndivActions[1];++a1)
            {
                actionIndices[1]=a1;
                Index jaI=IndividualToJointActionIndices(actionIndices);
                cout << "obs ja " << jaI << endl;
                for(ObservationModelMappingSparse::SparseMatrix::const_iterator1
                        ri0=Os[0][a0]->begin1();
                    ri0!=Os[0][a0]->end1();
                    ++ri0)
                {
                    sucStateIndices[0]=ri0.index1();
                    for(ObservationModelMappingSparse::SparseMatrix::const_iterator2
                             ci0 = ri0.begin();
                         ci0 != ri0.end();
                         ++ci0)
                    {
                        observationIndices[0]=ci0.index2();

                        for(ObservationModelMappingSparse::SparseMatrix::const_iterator1
                                ri1=Os[1][a1]->begin1();
                            ri1!=Os[1][a1]->end1();
                            ++ri1)
                        {
                            sucStateIndices[1]=ri1.index1();
                            for (ObservationModelMappingSparse::SparseMatrix::const_iterator2
                                     ci1 = ri1.begin();
                                 ci1 != ri1.end();
                                 ++ci1)
                            {
                                observationIndices[1]=ci1.index2();
                                
                                Index sucSI=IndividualToJointStateIndices(sucStateIndices);
                                Index oI=IndividualToJointObservationIndices(observationIndices);
                                p=(*ci0)*(*ci1);
                                _m_p_oModel->Set(jaI, sucSI, oI, p);
                            }
                        }
                    }
                }
            }
        }

#if DEBUG_CENTRALIZEDSPARSEMODELS
        string newModelObs=_m_p_oModel->SoftPrint();

        if(okModelObs!=newModelObs)
            abort();
#endif
        break;
    }
    default:
        for(Index a=0;a!=_m_nrJointActions;++a)
            for(Index s=0;s!=_m_nrJointStates;++s)
            {
            for(Index s1=0;s1!=_m_nrJointStates;++s1)
            {
                p=GetTransitionProbability(s,a,s1);
                if(p>0)
                    _m_p_tModel->Set(s,a,s1,p);
            }

            for(Index o=0;o!=_m_nrJointObservations;++o)
            {
                p=GetObservationProbability(a,s,o);
                if(p>0)
                    _m_p_oModel->Set(a,s,o,p);
            }           
        }
    }
}

void
TransitionObservationIndependentMADPDiscrete::CreateCentralizedSparseModels()
{
    CreateCentralizedSparseTransitionModel();
    CreateCentralizedObservationTransitionModel();
}

void
TransitionObservationIndependentMADPDiscrete::CreateCentralizedFullModels()
{
    if(!_m_initialized)
    {
        throw(E("TransitionObservationIndependentMADPDiscrete::CreateCentralizedFullModels not initialized yet"));
        return;
    }

    _m_p_tModel = new TransitionModelMapping(_m_nrJointStates,
                                             _m_nrJointActions);
    _m_p_oModel = new ObservationModelMapping(_m_nrJointStates,
                                              _m_nrJointActions,
                                              _m_nrJointObservations);
    double p=0;
    for(Index a=0;a!=_m_nrJointActions;++a)
        for(Index s=0;s!=_m_nrJointStates;++s)
        {
            for(Index s1=0;s1!=_m_nrJointStates;++s1)
            {
                p=GetTransitionProbability(s,a,s1);
                if(p>0)
                    _m_p_tModel->Set(s,a,s1,p);
            }

            for(Index o=0;o!=_m_nrJointObservations;++o)
            {
                p=GetObservationProbability(a,s,o);
                if(p>0)
                    _m_p_oModel->Set(a,s,o,p);
            }           
        }
}
