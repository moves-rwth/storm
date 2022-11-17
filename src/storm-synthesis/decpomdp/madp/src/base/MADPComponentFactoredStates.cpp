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

#include "MADPComponentFactoredStates.h"
#include "StateFactorDiscrete.h"
#include <stdlib.h>
#include "FSDist_COF.h"
#include "State.h"

using namespace std;

#define DEBUG_MCFS 1
//Default constructor
MADPComponentFactoredStates::MADPComponentFactoredStates() 
    :
        _m_initialized(false)
        , _m_nrStates(0)
        , _m_nrStateFactors(0)
        , _m_stepSize(0)
        , _m_initialStateDistribution(0)
{
    _m_jointStatesMap=new map<vector<Index>, State*>;
}
//Copy constructor.    
MADPComponentFactoredStates::MADPComponentFactoredStates(const MADPComponentFactoredStates& o) 
{
}
//Destructor
MADPComponentFactoredStates::~MADPComponentFactoredStates()
{
    for(Index i=0;i!=_m_stateFactors.size();++i)
        delete _m_stateFactors.at(i);
    delete _m_initialStateDistribution;

    while( !_m_jointStatesMap->empty() )
    {
        delete (*_m_jointStatesMap->begin()).second;
        _m_jointStatesMap->erase( _m_jointStatesMap->begin() );
    }
    delete _m_jointStatesMap;
    delete [] _m_stepSize;
}
//Copy assignment operator
MADPComponentFactoredStates& MADPComponentFactoredStates::operator= (const MADPComponentFactoredStates& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    return *this;
}

string MADPComponentFactoredStates::SoftPrintInitialStateDistribution() const
{
    AssureInitialized(":SoftPrintInitialStateDistribution()");
    if(_m_initialStateDistribution == 0)
        throw E("MADPComponentFactoredStates::SoftPrintInitialStateDistribution No ISD set yet (use SetISD first) !");
    return( _m_initialStateDistribution->SoftPrint() );
}

string MADPComponentFactoredStates::SoftPrintStates() const
{    
    AssureInitialized("SoftPrintStates()");
    stringstream ss;
    ss << "Number of state factors: " << _m_nrStateFactors <<endl;
    vector<StateFactorDiscrete*>::const_iterator it = _m_stateFactors.begin();
    vector<StateFactorDiscrete*>::const_iterator last = _m_stateFactors.end();
    Index i=0;
    while(it!=last)
    {
        ss << "SF index "<<i++<<" - ";
        ss << (*it)->SoftPrint();
        ss << endl;
        it++;
    }
    return(ss.str());
}
string MADPComponentFactoredStates::SoftPrintState(Index sI) const
{
    stringstream ss;
    vector<Index> s_vec = StateIndexToFactorValueIndices(sI);
    ss << SoftPrintVector(s_vec);
    return(ss.str());
    
}
string MADPComponentFactoredStates::SoftPrint() const
{
    stringstream ss;
    AssureInitialized("SoftPrint()");
    ss << "- nr. states="<< GetNrStates()<<endl;
    ss << SoftPrintStates();
    ss << "Initial state distr.: ";
    ss << SoftPrintInitialStateDistribution();
    ss << endl;
    return(ss.str());
}


string MADPComponentFactoredStates::SoftPrintPartialState(
        const Scope& sfSc, 
        const std::vector<Index>& sfValues) const
{
    if(sfSc.size() != sfValues.size())
        throw E("MADPComponentFactoredStates::SoftPrintPartialState: sfSc.size() != sfValues.size()");

    stringstream ss;
    ss << "<";

    for(Index scI=0; scI < sfSc.size(); scI++)
    {
        Index sfI = sfSc.at(scI);
        Index sfValI = sfValues.at(scI);
        StateFactorDiscrete* sfac = _m_stateFactors.at(sfI);
        string name = sfac->GetName();
        string value = sfac->GetStateFactorValue(sfValI);
        if(scI > 0)
            ss << ",";
        ss << name << "=" << value;
    }
    ss << ">";
    return(ss.str());
}

void MADPComponentFactoredStates::AssureInitialized(string caller) const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentFactoredStates::"<<caller<< 
        " - Error: not initialized. " <<endl;
        throw E(ss);
    }
}

void MADPComponentFactoredStates::PrintDebugStuff() const
{
    cout << "-----------------------------------------------------"<<endl;
    cout << "MADPComponentFactoredStates::SetInitialized(true) run"<<endl;
    cout << "*this* (at mem address"<<this<<") has "<<endl;
    cout << "_m_initialized="<<_m_initialized<<endl;
    cout << "_m_nrStates="<<_m_nrStates<<endl;
    cout << "_m_nrStateFactors="<<_m_nrStateFactors<<endl;
    cout << "_m_sfacDomainSizes="<<SoftPrintVector(_m_sfacDomainSizes)<<
        endl;
    cout << "-----------------------------------------------------"<<endl;
}

bool MADPComponentFactoredStates::SetInitialized(bool b)
{

    if(b == true)
    {
        if(_m_initialized)
            //already initialized: just return
            return true;

        _m_stepSize = IndexTools::CalculateStepSize(_m_sfacDomainSizes);
        _m_nrStates = 1;
        for(Index i=0;i<_m_nrStateFactors;i++)
            _m_nrStates *= _m_sfacDomainSizes.at(i);



        _m_initialStateDistribution = new FSDist_COF(*this);

#if 0 && DEBUG_MCFS
        PrintDebugStuff();
#endif    

    }
    
    _m_initialized = b;
    return b;
}


void MADPComponentFactoredStates::SetUniformISD()
{
#if 0 && DEBUG_MCFS
        PrintDebugStuff();
#endif    
    AssureInitialized("SetUniformISD()");
    if(_m_initialStateDistribution == 0)
        throw E(" MADPComponentFactoredStates::SetUniformISD() - ISD==0 !?");
    _m_initialStateDistribution->SetUniform();
/*    
    size_t nrS = GetNrStates();
    double uprob = 1.0 / ((double)nrS);
#if 0 && DEBUG_MCFS
    cout << "uprob="<<uprob << ", nrStates="<<nrS<<" / "<< _m_nrStates<<endl;
#endif
    vector<double>::iterator it = _m_initialStateDistribution->begin();
    vector<double>::iterator last = _m_initialStateDistribution->end();
    while(it!=last)
    {
        *it = uprob;
        it++;
    }
#if 0 && DEBUG_MCFS
        PrintDebugStuff();
#endif    
*/
}

/*
void MADPComponentFactoredStates::SetISD(const vector<double>& v)
{
    AssureInitialized("SetISD(vector<double> v)");
//    _m_initialStateDistribution->clear();
    _m_initialStateDistribution = v;
}
*/

double MADPComponentFactoredStates::GetInitialStateProbability(Index sI) const
{
    AssureInitialized("GetInitialStateProbability(Index sI)"); 
    vector<Index> sfacValues = StateIndexToFactorValueIndices(sI);
    return(_m_initialStateDistribution->GetProbability(sfacValues) );
}


Index MADPComponentFactoredStates::SampleInitialState() const
{
  if(!_m_initialized)
  {
    stringstream ss;
    ss << "MADPComponentFactoredStates::SampleInitialState()" <<
      " - Error: not initialized. " << endl;
    throw E(ss);
  }

  vector<Index> sfacValues;
  SampleInitialState(sfacValues);
  Index state = FactorValueIndicesToStateIndex(sfacValues);
  return(state);
}

void MADPComponentFactoredStates::SampleInitialState(vector<Index> &sIs) const
{
    sIs=_m_initialStateDistribution->SampleState();
}

Index MADPComponentFactoredStates::
AddStateFactor(const string &n, const string &d)
{
    if(_m_initialized)
        throw E("Can't add state factor to initialized MADPComponentFactoredStates!");

    _m_stateFactors.push_back( new StateFactorDiscrete(n,d) );
    _m_sfacDomainSizes.push_back(0);
    Index i = _m_nrStateFactors++;
    _m_allStateFactorScope.Insert(i);
    return i;

}

Index MADPComponentFactoredStates::
AddStateFactorValue(Index sf, const string &v)
{
    Index i = _m_stateFactors.at(sf)->AddStateFactorValue(v);
    _m_sfacDomainSizes.at(sf)++;
    return(i);

}

void MADPComponentFactoredStates::
RemoveStateFactor(const Index sf)
{
    delete(_m_stateFactors.at(sf));
    _m_stateFactors.erase(_m_stateFactors.begin()+sf);
    _m_sfacDomainSizes.erase(_m_sfacDomainSizes.begin()+sf);
    _m_nrStateFactors--;
    _m_allStateFactorScope.Remove(sf);

}


Index MADPComponentFactoredStates::
FactorValueIndicesToStateIndex(const vector<Index>& s_e_vec, 
        const Scope& sfSC) const
{
    vector<size_t> nr_sf_e(sfSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        _m_sfacDomainSizes, sfSC, nr_sf_e);
    Index sI_e = IndexTools::IndividualToJointIndices( s_e_vec, nr_sf_e);
    return(sI_e);
}

vector<Index> MADPComponentFactoredStates::
StateIndexToFactorValueIndices(Index s_e, 
        const Scope& sfSC) const
{
    vector<size_t> nr_sf_e(sfSC.size());
    IndexTools::RestrictIndividualIndicesToScope(
        _m_sfacDomainSizes, sfSC, nr_sf_e);
    vector<Index> s_e_vec = IndexTools::JointToIndividualIndices(s_e, nr_sf_e);
    return(s_e_vec);
}

const State* MADPComponentFactoredStates::GetState(Index i) const
{
    vector<Index> sIs=StateIndexToFactorValueIndices(i);

    // we cached the ones already asked for
    if(_m_jointStatesMap->find(sIs)!=_m_jointStatesMap->end())
        return(_m_jointStatesMap->find(sIs)->second);
    else // create new joint state and add it to cache
    {
        State *state=new State; // not a StateDiscrete, since the
                                // index might overflow
        string name="";
        for(Index y = 0; y < _m_nrStateFactors; y++)
        {
            if(y>0)
                name+="_";
            name+=GetStateFactorDiscrete(y)->GetStateFactorValue(sIs[y]);
        }
        state->SetName(name);
        state->SetDescription("");
        _m_jointStatesMap->insert(make_pair(sIs,state));
        return(state);
    }
}

