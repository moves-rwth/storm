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

#include "MADPComponentDiscreteObservations.h"
#include "IndexTools.h"

using namespace std;

#define DEBUG_GETJO_COPYVEC 0
#define DEBUG_CJO 0
#define DEBUG_ADD_DO 0

//Default constructor
MADPComponentDiscreteObservations::MADPComponentDiscreteObservations()
{
    _m_initialized = false;
    _m_observationStepSize = 0;
    _m_cachedAllJointObservations=false;
    _m_jointObservationIndices=0;
    _m_jointIndicesValid=true;
    _m_nrJointObservations=0;
}

MADPComponentDiscreteObservations::MADPComponentDiscreteObservations(
    const MADPComponentDiscreteObservations& a)
{
    _m_initialized=a._m_initialized;
    _m_cachedAllJointObservations=a._m_cachedAllJointObservations;
    _m_jointIndicesValid=a._m_jointIndicesValid;
    _m_nrJointObservations=a._m_nrJointObservations;
    _m_nrObservations=a._m_nrObservations;
    _m_observationStepSize=IndexTools::CalculateStepSize(_m_nrObservations);
    _m_observationVecs=a._m_observationVecs;

    _m_jointObservationVec.clear();
    for(Index ja=0;ja!=a._m_jointObservationVec.size();++ja)
        _m_jointObservationVec.push_back(new JointObservationDiscrete(*a._m_jointObservationVec.at(ja)));

    _m_jointObservationIndices=new map<Index, vector<Index> *>();
    if(a._m_jointObservationIndices)
    {
        map<Index, vector<Index> *>::const_iterator iter;
        for(iter = a._m_jointObservationIndices->begin();
            iter != a._m_jointObservationIndices->end();
            ++iter)
            _m_jointObservationIndices->insert(make_pair( iter->first, new vector<Index>(*iter->second ))); 
    }
}

//Destructor
MADPComponentDiscreteObservations::~MADPComponentDiscreteObservations()
{    
    _m_nrObservations.clear();
    vector<vector<ObservationDiscrete> >::iterator it = 
    _m_observationVecs.begin();
    vector<vector<ObservationDiscrete> >::iterator last = 
    _m_observationVecs.end();
    while(it != last)
    {
        // (*it) isa vector<ObservationDiscrete>
        (*it).clear();
        it++;
    }
    _m_observationVecs.clear();    
    vector<JointObservationDiscrete*>::iterator it2 = 
    _m_jointObservationVec.begin();
    vector<JointObservationDiscrete*>::iterator last2 = 
    _m_jointObservationVec.end();
    while(it2 != last2)
    {
        delete *it2; //removes the joint observation pointed to...
        it2++;
    }

    _m_jointObservationVec.clear();

    if(_m_jointObservationIndices)
    {
        while(!_m_jointObservationIndices->empty())
        {
            delete (*_m_jointObservationIndices->begin()).second;
            _m_jointObservationIndices->erase( _m_jointObservationIndices->begin() );
        }
        delete _m_jointObservationIndices;
    }
#if 0
    if(_m_jointObservationIndices)
    {
        vector<vector<Index>*>::iterator it3 = _m_jointObservationIndices->begin();
        vector<vector<Index>*>::iterator last3 = _m_jointObservationIndices->end();
        while(it3 != last3)
        {
            delete *it3; //removes the joint observation pointed to...
            it3++;
        }
    }
#endif
    delete[] _m_observationStepSize;
}

//data manipulation (set) functions:

/** Calls ConstructJointObservationsRecursively on a new (empty) joint
 * observation.  this function is typically called from the parser
 * (parser/ParserDecPOMDPDiscrete.h) */ 
size_t MADPComponentDiscreteObservations::ConstructJointObservations()
{
if(DEBUG_CJO)    cerr << "ConstructJointObservations() - called";
    JointObservationDiscrete* jo = new JointObservationDiscrete();
    size_t NRJO = ConstructJointObservationsRecursively(0, *jo, 0);
    _m_cachedAllJointObservations=true;
    return NRJO;
}

/** Creates (_m_jointObservationVec) using _m_observationVecs (which
 * need to be initialized before calling this function...) */
size_t MADPComponentDiscreteObservations::ConstructJointObservationsRecursively( 
    Index curAgentI, JointObservationDiscrete& jo, Index joI)
{
if(DEBUG_CJO)    cerr << ">> MADPComponentDiscreteObservations::ConstructJointObservations(Index "<<curAgentI<<", JointObservationDiscrete& jo, Index "<< joI<<") called"<<endl;

    bool lastAgent=false;
    if(curAgentI == _m_nrObservations.size()-1)
    {
        lastAgent = true;    
        if(DEBUG_CJO)     cerr << "\nlast agent\n";
    }    
    if(curAgentI >= _m_observationVecs.size())
    {
        stringstream ss;
        ss << "ConstructJointObservationsRecursively - current Agent index ("<<
            curAgentI<<") out of bounds! (_m_observationVecs contains "<<
            "observations for "<<_m_observationVecs.size() << " agents...)\n";
        throw E(ss);
    }
    ObservationDVec::iterator first = _m_observationVecs[curAgentI].begin();
    ObservationDVec::iterator it = _m_observationVecs[curAgentI].begin();
    ObservationDVec::iterator last = _m_observationVecs[curAgentI].end();
    ObservationDVec::iterator beforelast = _m_observationVecs[curAgentI].end();
    beforelast--;

    if(it == last)
    {
        stringstream ss;
            ss << " empty observation set for agent " << curAgentI;
        throw E(ss);
    }
    //first observation extends the received jo 
    JointObservationDiscrete* p_joReceivedArgCopy = new 
        JointObservationDiscrete(jo);
    JointObservationDiscrete* p_jo;
        
    while( it != last) // other observations extend duplicates of jo
    {
        if(DEBUG_CJO)    
            cerr << "\nnext observation";
        if(it == first) //
        {
            if(DEBUG_CJO)     cerr << "(first observation - not making copy)\n";
            p_jo = &jo;
        }
        else if(it == beforelast)//this is the last valid it ->last observation 
        {
            if(DEBUG_CJO)     cerr << "(last observation - not making copy)\n";
            p_jo = p_joReceivedArgCopy; //don't make a new copy
        }
        else //make a new copy
        {        
            if(DEBUG_CJO)     cerr << "(intermed. observation - making copy)\n";
            p_jo = new JointObservationDiscrete(*p_joReceivedArgCopy);    
        }    
        if(lastAgent)
        {
            p_jo->SetIndex(joI);
            if(DEBUG_CJO)    
                cerr << "setting index of this joint observation to: "<< joI 
                    <<endl;
        }
        ObservationDiscrete* ai = /*(ObservationDiscrete*)*/ &(*it);
        if(DEBUG_CJO)    
            cerr << "Adding agent's indiv. observation to joint observation..."<<endl;
        p_jo->AddIndividualObservation(ai, curAgentI);
        
        if(lastAgent)//jointObservation is now completed:add it to the jointObservation set.
        {
            if(DEBUG_CJO)
            {cerr<<"INSERTING the joint observation:"; p_jo->Print();cerr<<endl;}
            _m_jointObservationVec.push_back(p_jo);
            if(DEBUG_CJO){cerr << "\nINSERTED the joint observation"<<endl<< "_m_jointObservationVec now containts "<< _m_jointObservationVec.size() << " joint observations." << endl;}
            joI++;
        }
        else
            joI = ConstructJointObservationsRecursively(curAgentI+1,*p_jo, joI);
        it++;
    }
    if(DEBUG_CJO)    cerr << ">> MADPComponentDiscreteObservations::ConstructJointObservationsRecursively(Index "<<curAgentI<<", JointObservationDiscrete& jo, Index "<< joI<<") FINISHED"<<endl;
    return joI;
    
}

/** Creates nrO unnamed observations.*/
void MADPComponentDiscreteObservations::SetNrObservations(Index AI,
                                                          size_t nrO)
{
    if(_m_nrObservations.size() != AI)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteObservations::SetNrObservation("<<AI<<","<<
            nrO<<
            ") - error, observations of agents should be specified in order!"<<
            " (the vector _m_nrObservations should contain entries for all "<<
            "preceeding agents.)";
        throw(E(ss));
    }
    else
    {
        _m_nrObservations.push_back(nrO);
        //create nameless observations for this agent...
        vector<ObservationDiscrete> thisAgentsObservations;
        for(Index i=0;i<nrO;i++)
        {
            stringstream ss;
            ss //<< "ag"<<AI
                <<"o"<<i;
            thisAgentsObservations.push_back(ObservationDiscrete(i, ss.str()));
        }
        _m_observationVecs.push_back(thisAgentsObservations);
    }
}

void MADPComponentDiscreteObservations::AddObservation(Index AI,
                                                       const string &name,
                                                       const string &description)
{
    if(DEBUG_ADD_DO) 
        cerr<<"MADPComponentDiscreteObservation::AddObservation("<<AI<<","<<name<<")"
            <<endl;    

    if(_m_nrObservations.size() != AI && _m_nrObservations.size() != AI+1)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteObservations::AddObservation("<<AI<<","<<name<<
            ") - error, observations of agents should be specified in order!"<<
            " first all observations of agent 1, then all of agent 2,...etc..."<<
            " _m_nrObservations.size now is: "<< _m_nrObservations.size() <<
            "\n(the vector _m_nrObservations should contain entries for all "<<
            "preceeding agents.)";
        throw(E(ss));
    }
    if(_m_nrObservations.size() == AI )
    {
        //this is the first observation we add for this agent
        _m_nrObservations.push_back(1);

        vector<ObservationDiscrete> thisAgentsObservations;
        ObservationDiscrete ad(0, name, description);
        thisAgentsObservations.push_back(ad);
        _m_observationVecs.push_back(thisAgentsObservations);
    }
    else
    {
        //we add an observation for this agent - increment his nr_observations
        Index newObservationIndex = _m_nrObservations[AI]++;
        ObservationDiscrete ad(newObservationIndex, name, description);
        _m_observationVecs[AI].push_back(ad);
    }
}

/** When setting to true, a verification of member elements is
 * performed. (i.e. a check whether all vectors have the correct size
 * and non-zero entries) */
bool MADPComponentDiscreteObservations::SetInitialized(bool b)
{
    if(b == false)
    {
        if(_m_initialized == true)
            delete [] _m_observationStepSize;
        _m_initialized = b;
        return true;
    }
    if(_m_initialized == true && b == true)
    {
        //first free mem before re-initialize:
        delete [] _m_observationStepSize;
    }
    if(b == true)
    {
        if(_m_nrObservations.size() == 0)
            throw(E("MADPComponentDiscreteObservations::SetInitialized() no observations specified"));
        _m_observationStepSize=
            IndexTools::CalculateStepSize(_m_nrObservations);

        if(!_m_cachedAllJointObservations)
        {
            size_t nrJO=1;
            size_t prevNrJO=nrJO;
            for(Index i=0;i!=_m_nrObservations.size();++i)
            {
                nrJO*=_m_nrObservations[i];
                // detect overflow
                if(nrJO<prevNrJO)
                    _m_jointIndicesValid=false;
                prevNrJO=nrJO;
            }
            _m_nrJointObservations=nrJO;
            _m_jointObservationIndices=
                new map<Index, vector<Index> *>();
        }
        else
            _m_nrJointObservations=_m_jointObservationVec.size();
        _m_initialized = b;
    }    
    return(true);
}

size_t MADPComponentDiscreteObservations::GetNrObservations(Index agentI) const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteObservations::GetNrObservations("<< 
            agentI<<") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(agentI < _m_nrObservations.size())
        return _m_nrObservations[agentI];
    else
    {
        stringstream ss;
        ss << "Warning: MADPComponentDiscreteObservations::GetNrObservations("<<
            agentI<<") - index out of bounds"<<endl;
        throw E(ss);
    }
}

size_t MADPComponentDiscreteObservations::GetNrJointObservations() const
{
    if(!_m_initialized)
    {
        throw(E("MADPComponentDiscreteObservations::GetNrJointObservations() - Error: not initialized."));
    }
    if(!_m_jointIndicesValid)
    {
        throw(E("MADPComponentDiscreteObservations::GetNrJointObservations() joint indices are not valid (overflow)"));
    }
    return _m_nrJointObservations;
}

/** Throws an exception if there is no Observation with name s.*/
Index MADPComponentDiscreteObservations::GetObservationIndexByName(const string &s,
                                                                   Index agentI) const
{
    if(!_m_initialized)
        throw E("MADPComponentDiscreteObservations::GetObservationIndexByName - not initialized!");
    if(agentI >= _m_observationVecs.size())
    {
        stringstream ss;
        ss << "GetObservationIndexByName -  Agent index ("<<
            agentI<<") out of bounds! (_m_observationVecs contains observations"
            <<" for " << _m_observationVecs.size() << " agents...)\n";
        throw E(ss);
    }
    vector<ObservationDiscrete>::const_iterator it = 
    _m_observationVecs[agentI].begin();
    vector<ObservationDiscrete>::const_iterator last = 
    _m_observationVecs[agentI].end();
    while(it != last)
    {
        string s2 = (*it).GetName();
        if(s == s2)
            return( (*it).GetIndex() );
        it++;
    }
    //not found
    //return(-1);
    stringstream ss;
    ss << "GetObservationIndexByName - observation \"" << s << "\" of agent " <<
           agentI << " not found." << endl;
    throw E(ss);
}


const ObservationDiscrete* MADPComponentDiscreteObservations::GetObservationDiscrete(Index agentI, Index a) const
{    
    if(!_m_initialized)
    {
        stringstream ss;
            ss << "MADPComponentDiscreteObservations::GetObservationDiscrete("<<
            agentI<<","<< a <<") - Error: not initialized. "<<endl;
        throw E(ss);
    }

    if(agentI < _m_nrObservations.size() )
    {
        if(a < GetNrObservations(agentI) )
        {
            return ( &_m_observationVecs[agentI][a]);
        }
        else
            cerr << "WARNING MADPComponentDiscreteObservations::GetObservationDiscrete("<< agentI<<","<< a <<") - observation (2nd) index out of bounds"<<endl;
    }
    else
        cerr << "WARNING MADPComponentDiscreteObservations::GetObservationDiscrete("<< agentI<<","<< a <<") - agent (1st) index out of bounds"<<endl;   
    
    return(0);
}
/*return a ref to the i-th joint observation.*/
const JointObservationDiscrete* MADPComponentDiscreteObservations::
    GetJointObservationDiscrete(Index i) const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteObservations::GetJointObservationDiscrete("
            << i<<") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(!_m_cachedAllJointObservations)
    {
        throw E("MADPComponentDiscreteObservations::GetJointObservationDiscrete: joint observations have not been created");
    }
    if(i < _m_jointObservationVec.size() )
    {
        const JointObservationDiscrete* j = _m_jointObservationVec[i];
        return( j );
    }
    //else        
    stringstream ss;
    ss << "WARNING MADPComponentDiscreteObservations::GetJointObservationDiscrete(Index i) index out of bounds (i="<< i <<")"<<endl;
    throw E(ss);
}
/*return a ref to the i-th joint observation.*/
const JointObservation* MADPComponentDiscreteObservations::GetJointObservation(Index i) const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteObservations::GetJointObservation("<< 
        i<<") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    if(!_m_cachedAllJointObservations)
    {
        throw E("MADPComponentDiscreteObservations::GetJointObservationDiscrete: joint observations have not been created");
    }
    if(i < _m_jointObservationVec.size() )
        return( (const JointObservation*) _m_jointObservationVec[i] );
    //else        
    stringstream ss;
    ss << "WARNING MADPComponentDiscreteObservations::GetJointObservation(Index i) index out of bounds (i="<< i <<")"<<endl;
    throw E(ss);
}
Index MADPComponentDiscreteObservations::IndividualToJointObservationIndices(
    const vector<Index>& indivObservationIndices)const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss <<  "MADPComponentDiscreteObservations::GetJointObservationIndex("<<
           "vector<Index>& indivObservationIndices) - Error: not initialized."
            <<endl;
        throw E(ss);
    }
    Index i = IndexTools::IndividualToJointIndicesStepSize(
            indivObservationIndices, _m_observationStepSize);

    return(i);
}



Index MADPComponentDiscreteObservations::
IndividualToJointObservationIndices(
        const std::vector<Index>& jo_e, const Scope& agSC) const
{ 
    vector<size_t> nr_A_e(agSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        GetNrObservations(), agSC, nr_A_e);
    Index joI = IndexTools::IndividualToJointIndices( jo_e, nr_A_e);
    return(joI);
}
std::vector<Index> MADPComponentDiscreteObservations::
JointToIndividualObservationIndices(
        Index jo_e, const Scope& agSC) const
{
    vector<size_t> nr_A_e(agSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        GetNrObservations(), agSC, nr_A_e);
    vector<Index> jo_e_vec = IndexTools::JointToIndividualIndices(jo_e, nr_A_e);
    return(jo_e_vec);
}
Index MADPComponentDiscreteObservations::
JointToRestrictedJointObservationIndex(
        Index joI, const Scope& agSc_e ) const
{
    const vector<Index>& jo_vec = JointToIndividualObservationIndices(joI);
    vector<Index> jo_vec_e(agSc_e.size());
    IndexTools::RestrictIndividualIndicesToScope(jo_vec, agSc_e, jo_vec_e);
    Index jo_e = IndividualToJointObservationIndices(jo_vec_e, agSc_e);
    return(jo_e);

}





string MADPComponentDiscreteObservations::SoftPrint() const
{
    stringstream ss;
    ss << "MADPComponentDiscreteObservations::Print()" << endl;    
    ss << "#joint observations="<<GetNrJointObservations()<<endl;
    ss << SoftPrintObservationSets();
    ss << "Joint Observations:"<<endl;
    ss << SoftPrintJointObservationSet();    
    return(ss.str());
}
 
string MADPComponentDiscreteObservations::SoftPrintObservationSets() const
{    
    stringstream ss;
    if(!_m_initialized)
    {
    stringstream ss;
        ss << "MADPComponentDiscreteObservations::PrintObservationSets("<< 
        ") - Error: not initialized. "<<endl;
        throw E(ss);
    }
    ss << "Observations:"<<endl;
    for(Index agentIndex=0; agentIndex < _m_nrObservations.size(); agentIndex++)
    {
    ss << "agentI " << agentIndex << " - nrObservations " << 
        GetNrObservations(agentIndex)<<endl;
    vector<ObservationDiscrete>::const_iterator f =
        _m_observationVecs[agentIndex].begin();
    vector<ObservationDiscrete>::const_iterator l =
        _m_observationVecs[agentIndex].end();
    while(f != l)
    {
        ss << (*f).SoftPrint() << endl;
//        GetName() << " - " << (*f).GetDescription()<<endl;
        f++;
    }
    }
    return(ss.str());
}

string MADPComponentDiscreteObservations::SoftPrintJointObservationSet() const
{
    stringstream ss;
    if(!_m_initialized)
    {
    stringstream ss;
        ss << "MADPComponentDiscreteObservations::PrintJointObservationSet("<<
       ") - Error: not initialized. "<<endl;
    throw E(ss);
    }
    vector<JointObservationDiscrete*>::const_iterator jo_it = 
    _m_jointObservationVec.begin();
    vector<JointObservationDiscrete*>::const_iterator jo_last = 
    _m_jointObservationVec.end();
    while(jo_it != jo_last)
    {
        ss << (*jo_it)->SoftPrint()<<endl;
        jo_it++;    
    }
    return(ss.str());
}

