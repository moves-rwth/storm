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

#include "MADPComponentDiscreteActions.h"
#include "VectorTools.h"

using namespace std;

#define DEBUG_GETJA_COPYVEC 0
#define DEBUG_CJA 0
#define DEBUG_ADD_DA 0
#define DEBUG_MADP_DA 0

//the following variable enables some runtime checks - enable it for debugging
#define RUNTIME_CHECKS 0



//Default constructor
MADPComponentDiscreteActions::MADPComponentDiscreteActions()
{
    _m_initialized = false;
    _m_cachedAllJointActions = false;
    _m_actionStepSize = 0;
    _m_jointActionIndices=0;
    _m_jointIndicesValid=true;
}

MADPComponentDiscreteActions::MADPComponentDiscreteActions(
    const MADPComponentDiscreteActions& a)
{
    _m_initialized=a._m_initialized;
    _m_cachedAllJointActions=a._m_cachedAllJointActions;
    _m_jointIndicesValid=a._m_jointIndicesValid;
    _m_nrJointActions=a._m_nrJointActions;
    _m_nrActions=a._m_nrActions;
    _m_actionStepSize=IndexTools::CalculateStepSize(_m_nrActions);
    _m_actionVecs=a._m_actionVecs;

    _m_jointActionVec.clear();
    for(Index ja=0;ja!=a._m_jointActionVec.size();++ja)
        _m_jointActionVec.push_back(new JointActionDiscrete(*a._m_jointActionVec.at(ja)));

    _m_jointActionIndices=new map<Index, vector<Index> *>();
    if(a._m_jointActionIndices)
    {
        map<Index, vector<Index> *>::const_iterator iter;
        for(iter = a._m_jointActionIndices->begin();
            iter != a._m_jointActionIndices->end();
            ++iter)
            _m_jointActionIndices->insert(make_pair( iter->first, new vector<Index>(*iter->second ))); 
    }
}

//Destructor
MADPComponentDiscreteActions::~MADPComponentDiscreteActions()
{
    _m_nrActions.clear();
    vector<vector<ActionDiscrete> >::iterator it = _m_actionVecs.begin();
    vector<vector<ActionDiscrete> >::iterator last = _m_actionVecs.end();
    while(it != last)
    {
        // (*it) isa vector<ActionDiscrete>
        (*it).clear();
        it++;
    }
    _m_actionVecs.clear();
    vector<JointActionDiscrete*>::iterator it2 = _m_jointActionVec.begin();
    vector<JointActionDiscrete*>::iterator last2 = _m_jointActionVec.end();
    while(it2 != last2)
    {
        delete *it2; //removes the joint action pointed to...
        it2++;
    }
    _m_jointActionVec.clear();
    if(_m_jointActionIndices)
    {
        while(!_m_jointActionIndices->empty())
        {
            delete (*_m_jointActionIndices->begin()).second;
            _m_jointActionIndices->erase( _m_jointActionIndices->begin() );
        }
        delete _m_jointActionIndices;
    }
//     if(_m_jointActionIndices)
//     {
//         vector<vector<Index>*>::iterator it3 = _m_jointActionIndices->begin();
//         vector<vector<Index>*>::iterator last3 = _m_jointActionIndices->end();
//         while(it3 != last3)
//         {
//             delete *it3;
//             it3++;
//         }
//     }

    delete[] _m_actionStepSize;
}

//data manipulation (set) functions:

/** This creates nrA unnamed actions.*/
void MADPComponentDiscreteActions::SetNrActions(Index AI, size_t nrA)
{
    if(_m_nrActions.size() != AI)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteActions::SetNrAction("<<AI<<","<<
            nrA<<
            ") - error, actions of agents should be specified in order!"<<
            " (the vector _m_nrActions should contain entries for all "<<
            "preceeding agents.)";
        throw(E(ss));
    }
    else
    {
        _m_nrActions.push_back(nrA);
        //create nameless actions for this agent...    
        vector<ActionDiscrete> thisAgentsActions;
        for(Index i=0;i<nrA;i++)
        {
            stringstream ss;
            ss //<< "ag"<<AI
                <<"a"<<i;
            thisAgentsActions.push_back(ActionDiscrete(i, ss.str() ));
        }
        _m_actionVecs.push_back(thisAgentsActions);
    }
}

void MADPComponentDiscreteActions::AddAction(Index AI,
                                             const string &name,
                                             const string &description)
{
    if(DEBUG_ADD_DA)
        cerr <<  "MADPComponentDiscreteActions::AddAction("<<AI<<","<<name<<")"<<endl;

    if(_m_nrActions.size() != AI && _m_nrActions.size() != AI+1)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteActions::AddAction("<<AI<<","<<name<<
            ") - error, actions of agents should be specified in order!"<<
            " first all actions of agent 1, then all of agent 2,...etc..."<<
            " _m_nrActions.size now is: "<< _m_nrActions.size() <<
            "\n(the vector _m_nrActions should contain entries for all "<<
            "preceeding agents.)";
        throw(E(ss));
    }
    if(_m_nrActions.size() == AI )
    {
        //this is the first action we add for this agent
        _m_nrActions.push_back(1);

        vector<ActionDiscrete> thisAgentsActions;
        ActionDiscrete ad(0, name, description);
        thisAgentsActions.push_back(ad);
        _m_actionVecs.push_back(thisAgentsActions);
    }
    else
    {
        //we add an action for this agent - increment his nr_actions
        Index newActionIndex = _m_nrActions[AI]++;
        ActionDiscrete ad(newActionIndex, name, description);
        _m_actionVecs[AI].push_back(ad);
    }
}

/** Calls ConstructJointActionsRecursively() on a new (empty) joint
 * action.*/
size_t MADPComponentDiscreteActions::ConstructJointActions()
{
    JointActionDiscrete* ja = new JointActionDiscrete();
    size_t NRJA = ConstructJointActionsRecursively(0, *ja, 0);
    _m_cachedAllJointActions=true;
    return NRJA; 
}

/** Creates (_m_jointActionVec) using _m_actionVecs (which need to be
 * initialized before calling this function...) */    
size_t MADPComponentDiscreteActions::ConstructJointActionsRecursively(
    Index curAgentI, JointActionDiscrete& ja, Index jaI)
{

    bool lastAgent=false;
    if(curAgentI == _m_nrActions.size()-1)
    {
        lastAgent = true;    
    }
    if(curAgentI >= _m_actionVecs.size())
    {
        stringstream ss;
        ss << "ConstructJointActionsRecursively - current Agent index ("<<
            curAgentI<<") out of bounds! (_m_actionVecs contains actions for "<<
            _m_actionVecs.size() << " agents...)\n";
        throw E(ss);
    }

    ActionDVec::iterator first = _m_actionVecs[curAgentI].begin();
    ActionDVec::iterator it = _m_actionVecs[curAgentI].begin();
    ActionDVec::iterator last = _m_actionVecs[curAgentI].end();
    ActionDVec::iterator beforelast = _m_actionVecs[curAgentI].end();
    beforelast--;

    if(it == last)
    {
        stringstream ss; ss << "ERROR empty action set for agent " << curAgentI;
        throw E(ss);
    }
    //first action extends the received ja 
    JointActionDiscrete* p_jaReceivedArgCopy = new JointActionDiscrete(ja);
    JointActionDiscrete* p_ja;
        
    while( it != last) // other actions extend duplicates of ja
    {
        if(DEBUG_CJA)    cerr << "\nnext action";
            if(it == first) //
            {
                if(DEBUG_CJA)        
                    cerr << "(first action - not making copy)\n";
                p_ja = &ja;
            }
        else if (it == beforelast)//this is the last valid it -> last action   
        {
            if(DEBUG_CJA)        cerr << "(last action - not making copy)\n";
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
            if(DEBUG_CJA)cerr << "setting index of this joint action to: "
                << jaI <<endl;
        }
        ActionDiscrete* ai = /*(ActionDiscrete*)*/ &(*it);
        if(DEBUG_CJA)cerr <<"Adding agent's indiv. action to joint action..."
            <<endl;
        p_ja->AddIndividualAction(ai, curAgentI);
        
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
            jaI = ConstructJointActionsRecursively(curAgentI+1,*p_ja, jaI);
        
        it++;
    }
    if(DEBUG_CJA)    
        cerr << ">>ProblemDecTiger::ConstructJointActionsRecursively(Index "<<
            curAgentI<<", JointActionDiscrete& ja, Index "<< jaI<<") FINISHED"
            <<endl;
    return jaI;
}


/** When setting to true, a verification of member elements is
 * performed. (i.e. a check whether all vectors have the correct size
 * and non-zero entries) */
bool MADPComponentDiscreteActions::SetInitialized(bool b)
{
    if(b == false)
    {
        if(_m_initialized == true)
            delete [] _m_actionStepSize;
        _m_initialized = b;
        return true;
    }
    if(_m_initialized == true && b == true)
    {
        //first free mem before re-initialize:
        delete [] _m_actionStepSize;
    }
    if(b == true)
    {
        _m_actionStepSize=IndexTools::CalculateStepSize(_m_nrActions);

        if(!_m_cachedAllJointActions)
        {
            size_t nrJA=1;
            size_t prevNrJA=nrJA;
            for(Index i=0;i!=_m_nrActions.size();++i)
            {
                nrJA*=_m_nrActions[i];
                // detect overflow
                if(nrJA<prevNrJA)
                    _m_jointIndicesValid=false;
                prevNrJA=nrJA;
            }
            _m_nrJointActions=nrJA;
            _m_jointActionIndices=
                new map<Index, vector<Index> *>();
        }
        else
            _m_nrJointActions=_m_jointActionVec.size();
        _m_initialized = b;
    }
    return(true);
}

size_t MADPComponentDiscreteActions::GetNrActions(Index agentI) const
{    
    if(agentI < _m_nrActions.size())
        return _m_nrActions[agentI];
    else
    {
        stringstream ss;
            ss << "Warning: MADPComponentDiscreteActions::GetNrActions(Index agentI) - index out of bounds"<<endl;
        throw E(ss);
    }
}

size_t MADPComponentDiscreteActions::GetNrJointActions() const
{
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::GetNrJointActions("<< 
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(!_m_jointIndicesValid)
    {
        throw(E("MADPComponentDiscreteActions::GetNrJointActions() joint indices are not available, overflow detected"));
    }
    return(_m_nrJointActions);
}
///Get the number of joint actions the agents in agScope can form
size_t MADPComponentDiscreteActions::GetNrJointActions(const Scope& agScope) const
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

/** Throws an exception if there is no action with name s.*/
Index MADPComponentDiscreteActions::GetActionIndexByName(const string &s,
                                                         Index agentI) const
{
    if(!_m_initialized)
        throw E("MADPComponentDiscreteActions::GetActionIndexByName - not initialized!");

    if(agentI >= _m_actionVecs.size())
    {
        stringstream ss;
        ss << "GetActionIndexByName -  Agent index ("<<
            agentI<<") out of bounds! (_m_actionVecs contains actions for "<<
            _m_actionVecs.size() << " agents...)\n";
        throw E(ss);
    }
    vector<ActionDiscrete>::const_iterator it = _m_actionVecs[agentI].begin();
    vector<ActionDiscrete>::const_iterator last = _m_actionVecs[agentI].end();
    while(it != last)
    {
        string s2 = (*it).GetName();
        if(s == s2)
        //if(strcmp(s,s2) == 0)//match
            return( (*it).GetIndex() );
        it++;
    }
    //not found
    stringstream ss;
    ss << "GetActionIndexByName - action \"" << s << "\" of agent " << agentI <<
    " not found." << endl;
    throw E(ss.str().c_str());

}

const Action* MADPComponentDiscreteActions::GetAction(Index agentI, Index a) const
{
    return ((Action*) GetActionDiscrete( agentI,  a) );
}
const ActionDiscrete* MADPComponentDiscreteActions::GetActionDiscrete(Index agentI, Index a) const
{
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::GetAction("<< 
            agentI<<") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(agentI < _m_nrActions.size() )
    {
        if(a < GetNrActions(agentI) )
        {
            return (&_m_actionVecs[agentI][a]);
        }
        //else
        stringstream ss;
            ss << "WARNING MADPComponentDiscreteActions::GetAction(Index agentI, Index a) index a out of bounds"<<endl;
        throw E(ss);
    }
    //else
    stringstream ss;
    ss << "WARNING MADPComponentDiscreteActions::GetAction(Index agentI, Index a) index agentI out of bounds"<<endl;   
    throw E(ss);
}
/*return a ref to the i-th joint action.*/
const JointActionDiscrete* MADPComponentDiscreteActions::GetJointActionDiscrete(Index i) const
{
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::GetJointActionDiscrete("<< 
            i<<") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(!_m_cachedAllJointActions)
    {
        throw E("MADPComponentDiscreteActions::GetJointActionDiscrete: joint actions have not been created");
    }
    if(i < _m_jointActionVec.size() )
    {
        const JointActionDiscrete* j = _m_jointActionVec[i];
        return( j );
    }
    //else        
    stringstream ss;
    ss << "WARNING MADPComponentDiscreteActions::GetJointActionDiscrete(Index i) index a out of bounds"<<endl;
    throw E(ss);
}
/*return a ref to the i-th joint action.*/
const JointAction* MADPComponentDiscreteActions::GetJointAction(Index i) const
{
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::GetJointAction("<< 
            i<<") - Error: not initialized. "<<endl;
            throw E(ss);
    }
    if(!_m_cachedAllJointActions)
    {
        throw E("MADPComponentDiscreteActions::GetJointActionDiscrete: joint actions have not been created");
    }
    if(i < _m_jointActionVec.size() )
        return( (const JointAction*) _m_jointActionVec[i] );
    //else        
    stringstream ss;
    ss << "WARNING MADPComponentDiscreteActions::GetJointAction(Index i) index a out of bounds"<<endl;
    throw E(ss);
}

Index MADPComponentDiscreteActions::IndividualToJointActionIndices(
        const vector<Index>& indivActionIndices)const
{
#if RUNTIME_CHECKS
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::GetJointActionIndex("<< 
            "vector<Index>& indivActionIndices) -Error: not initialized."<<endl;
        throw E(ss);
    }
#endif                 
    Index i = IndexTools::IndividualToJointIndicesStepSize(indivActionIndices,
            _m_actionStepSize);

    return(i);
}

Index MADPComponentDiscreteActions::
IndividualToJointActionIndices(
        const std::vector<Index>& ja_e, const Scope& agSC) const
{ 
    vector<size_t> nr_A_e(agSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        GetNrActions(), agSC, nr_A_e);
    Index jaI = IndexTools::IndividualToJointIndices( ja_e, nr_A_e);
    return(jaI);
}
std::vector<Index> MADPComponentDiscreteActions::
JointToIndividualActionIndices(
        Index ja_e, const Scope& agSC) const
{
    vector<size_t> nr_A_e(agSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        GetNrActions(), agSC, nr_A_e);
    vector<Index> ja_e_vec = IndexTools::JointToIndividualIndices(ja_e, nr_A_e);
    return(ja_e_vec);
}
Index MADPComponentDiscreteActions::
JointToRestrictedJointActionIndex(
        Index jaI, const Scope& agSc_e ) const
{
    const vector<Index>& ja_vec = JointToIndividualActionIndices(jaI);
    vector<Index> ja_vec_e(agSc_e.size());
    IndexTools::RestrictIndividualIndicesToScope(ja_vec, agSc_e, ja_vec_e);
    Index ja_e = IndividualToJointActionIndices(ja_vec_e, agSc_e);
    return(ja_e);

}
string MADPComponentDiscreteActions::SoftPrint() const
{
    stringstream ss;
    if(DEBUG_MADP_DA){ss << "MADPComponentDiscreteActions::Print()" << endl;}
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::Print("<< 
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }    
    ss << "#joint actions="<<GetNrJointActions()<<endl;
    ss << SoftPrintActionSets();
    ss << "Joint Actions:"<<endl;
    ss << SoftPrintJointActionSet();
    return(ss.str());
}

string MADPComponentDiscreteActions::SoftPrintActionSets() const
{
    stringstream ss;
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::PrintActionSets("<< 
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }    
    ss << "Actions:"<<endl;
    for(Index agentIndex=0; agentIndex < _m_nrActions.size(); agentIndex++)
    {
        ss << "agentI " << agentIndex << " - nrActions " << 
            GetNrActions(agentIndex)<<endl;
        vector<ActionDiscrete>::const_iterator f = 
            _m_actionVecs[agentIndex].begin();
        vector<ActionDiscrete>::const_iterator l = 
            _m_actionVecs[agentIndex].end();
        while(f != l)
        {
            ss << (*f).SoftPrint() << endl;
            // (*f).GetName() << " - " << (*f).GetDescription()<<endl;
            f++;
        }
    }
    return(ss.str());
}

string MADPComponentDiscreteActions::SoftPrintJointActionSet() const
{
    stringstream ss;
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteActions::PrintJointActionSets("<< 
            ") - Error: not initialized. "<<endl;
        throw E(ss);
    }   
    vector<JointActionDiscrete*>::const_iterator ja_it = 
    _m_jointActionVec.begin();
    vector<JointActionDiscrete*>::const_iterator ja_last = 
    _m_jointActionVec.end();
    while(ja_it != ja_last)
    {
        ss << (*ja_it)->SoftPrint()<<endl;
        ja_it++;    
    }
    return(ss.str());
}
