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

#include "MADPComponentDiscreteStates.h"
#include <stdlib.h>

using namespace std;

#define DEBUG_MADP_DSTATES 0

//Default constructor
MADPComponentDiscreteStates::MADPComponentDiscreteStates() 
    :
        _m_initialized(false)
        ,_m_nrStates(0)
        ,_m_initialStateDistribution(0)
{
}

MADPComponentDiscreteStates::MADPComponentDiscreteStates(size_t nrS) 
    :
        _m_initialized(false)
        ,_m_nrStates(nrS)
        ,_m_initialStateDistribution(0)
{
    SetNrStates(nrS);
    _m_initialized = true;
}

MADPComponentDiscreteStates::MADPComponentDiscreteStates(
    const MADPComponentDiscreteStates& a)
{
    _m_initialized=a._m_initialized;
    _m_nrStates=a._m_nrStates;
    _m_stateVec=a._m_stateVec;
    _m_initialStateDistribution=
        new StateDistributionVector(*a._m_initialStateDistribution);
}


//Destructor
MADPComponentDiscreteStates::~MADPComponentDiscreteStates()
{
    delete _m_initialStateDistribution;
}

bool MADPComponentDiscreteStates::SetInitialized(bool b)
{
    if(b)
    {
        if(_m_nrStates==0)
            throw(E("MADPComponentDiscreteStates::SetInitialized problem has 0 states"));

        // alike to Belief::SanityCheck()

        bool validISD=true;

        // check for negative and entries>1
        double sum=0;
        for(vector<double>::const_iterator it=
                _m_initialStateDistribution->begin();
            it!=_m_initialStateDistribution->end(); ++it)
        {
            if(*it<0)
                validISD=false;
            if(*it>1)
                validISD=false;
            if(std::isnan(*it))
                validISD=false;
            sum+=*it;
        }

        // check if sums to 1
        if(abs(sum-1)>PROB_PRECISION)
            validISD=false;

        // but if it sums to 0, this means the ISD simply has not been set.
        // in this case, we set to uniform:
        if(abs(sum) < PROB_PRECISION)
        {
            cerr << "Warning, initial state distribution was not set, assuming uniform..." << endl;
            this->SetUniformISD();
            validISD = true;
        }

        // check whether the size is correct
        if(_m_initialStateDistribution->size()!=_m_nrStates)
            validISD=false;
        
        if(!validISD)
            throw(E("MADPComponentDiscreteStates::SetInitialized initial state distribution is not a valid probability distribution"));
    }

    _m_initialized = b;
    return(b);
}

void MADPComponentDiscreteStates::AddState(const string &StateName)
{
    if(_m_initialStateDistribution == 0)
        _m_initialStateDistribution = new StateDistributionVector();
    
    if(!_m_initialized)
    {
        _m_nrStates=0;    
        _m_initialStateDistribution->clear();
        _m_stateVec.clear();

        _m_initialized = true;
    }        
    _m_stateVec.push_back(StateDiscrete( _m_nrStates++, StateName));
    _m_initialStateDistribution->push_back(0.0);
//    _m_nrStates++; - already done...
}
void MADPComponentDiscreteStates::SetNrStates(size_t nrS)
{
    _m_nrStates = nrS;
    _m_stateVec.clear();
    if(_m_initialStateDistribution == 0)
        _m_initialStateDistribution = new StateDistributionVector();

    _m_initialStateDistribution->clear();
    for(Index i=0; i < nrS; i++)
    {
        _m_stateVec.push_back(StateDiscrete(i));
        _m_initialStateDistribution->push_back(0.0);
    }
    _m_initialized = true;
}

void MADPComponentDiscreteStates::SetUniformISD()
{
    //cout << " MADPComponentDiscreteStates::SetUniformISD called"<<endl;
    if(!_m_initialized)
    {
        cout << " MADPComponentDiscreteStates::SetUniformISD - ERROR can't"<<
            "set uniform init. state distr., when not initialized! (don't know how many state yet...)";
        return;
    }
    double uprob = 1.0 / GetNrStates();
    vector<double>::iterator it = _m_initialStateDistribution->begin();
    vector<double>::iterator last = _m_initialStateDistribution->end();
    while(it!=last)
    {
        *it = uprob;
        it++;
    }
}

void MADPComponentDiscreteStates::SetISD(StateDistribution* p)
{
    StateDistributionVector* p2 = dynamic_cast<StateDistributionVector*>(p);
    if (p2 == 0)
        throw E("MADPComponentDiscreteStates::SetISD(StateDistribution* p) dynamic cast failed");
    SetISD(p2);
}

void MADPComponentDiscreteStates::SetISD(vector<double> v)
{
    if(v.size() != _m_nrStates)
    throw E("MADPComponentDiscreteStates::SetISD - ERROR: nrStates don't match!");

    _m_initialStateDistribution->clear();
    *_m_initialStateDistribution = v;
}

Index MADPComponentDiscreteStates::GetStateIndex(StateDiscrete s) const
{
    return(s.GetIndex());
}

/** Throws an exception if there is no state with name s.*/
Index MADPComponentDiscreteStates::GetStateIndexByName(const string &s) const
{
    if(!_m_initialized)
        throw E("MADPComponentDiscreteStates::GetStateIndexByName - not initialized!");

    vector<StateDiscrete>::const_iterator it = _m_stateVec.begin();
    vector<StateDiscrete>::const_iterator last = _m_stateVec.end();
    while(it != last)
    {
        string s2 = (*it).GetName();
        if(s == s2)
        //if(strcmp(s,s2) == 0)//match
            return( (*it).GetIndex() );
        it++;
    }
    //not found
    //return(-1);
    stringstream ss;
    ss << "GetStateIndexByName - state \"" << s << "\" not found." << endl;
    throw E(ss);

}



double MADPComponentDiscreteStates::GetInitialStateProbability(Index sI) const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteStates::GetInitialStateProbability("<< 
        sI<<") - Error: not initialized. " <<endl;
        throw E(ss);
    }
    
    if(sI < _m_initialStateDistribution->size() )
    {
        if(DEBUG_MADP_DSTATES)    cout << "MultiAgentDecisionProcess::GetInitialStateProbability - index "<<sI<<" is okay, size is "<<_m_initialStateDistribution->size() <<endl;
        return(_m_initialStateDistribution->at(sI));
    }
    //else
    stringstream ss;
    ss << "MultiAgentDecisionProcess::GetInitialStateProbability - ERROR GetInitialStateProbability: index out of bounds";
    throw E(ss);
}


string MADPComponentDiscreteStates::SoftPrintInitialStateDistribution() const
{
    if(!_m_initialized)
    {
        stringstream ss;
        ss << "MADPComponentDiscreteStates::PrintInitialStateDistribution("<< 
        ") - Error: not initialized. " <<endl;
        throw E(ss);
    }
    return(_m_initialStateDistribution->SoftPrint());
}

string MADPComponentDiscreteStates::SoftPrintStates() const
{    
    stringstream ss;
    vector<StateDiscrete>::const_iterator it = _m_stateVec.begin();
    vector<StateDiscrete>::const_iterator last = _m_stateVec.end();
    while(it!=last)
    {
        ss << (*it).SoftPrint();
        ss << endl;
        it++;
    }
    return(ss.str());
}
string MADPComponentDiscreteStates::SoftPrint() const
{
    stringstream ss;
    if(!_m_initialized)
    {
        stringstream ss2;
        ss2 << "MADPComponentDiscreteStates::SoftPrint("<< 
        ") - Error: not initialized. " <<endl;
        throw E(ss2);
    }
    ss << "- nr. states="<< GetNrStates()<<endl;
    ss << SoftPrintStates();
    ss << "Initial state distr.: ";
    ss << SoftPrintInitialStateDistribution();
    ss << endl;
    return(ss.str());
}

Index MADPComponentDiscreteStates::SampleInitialState() const
{
  if(!_m_initialized)
  {
    stringstream ss;
    ss << "MADPComponentDiscreteStates::SampleInitialState()" <<
      " - Error: not initialized. " << endl;
    throw E(ss);
  }
  
  double randNr=rand() / (RAND_MAX + 1.0);

  double sum=0;
  Index state=0,i;
  for(i=0;i<_m_initialStateDistribution->size();i++)
  {
    sum+= _m_initialStateDistribution->at(i);
    if(randNr<=sum)
    {
      state=i;
      break;
    }
  }

  return(state);
}
