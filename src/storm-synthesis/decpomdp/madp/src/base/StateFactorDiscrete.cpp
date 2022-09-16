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

#include "StateFactorDiscrete.h"

using namespace std;

//Default constructor
StateFactorDiscrete::StateFactorDiscrete(const string &n, 
                                         const string &d)
    : NamedDescribedEntity(n,d)
{
}
StateFactorDiscrete::StateFactorDiscrete(size_t nrVs, const string &n,
                                         const string &d)
    : NamedDescribedEntity(n,d)
{
    _m_domainSize = nrVs;
    //perhaps also add strings 0...nrVs-1 to _m_domainValues ?
    
}
//Copy constructor.    
StateFactorDiscrete::StateFactorDiscrete(const StateFactorDiscrete& o) 
{
}
//Destructor
StateFactorDiscrete::~StateFactorDiscrete()
{
}
//Copy assignment operator
StateFactorDiscrete& StateFactorDiscrete::operator= (const StateFactorDiscrete& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    return *this;
}

string StateFactorDiscrete::SoftPrint() const
{
    stringstream ss;

    ss << "SF '"<< GetName() << "' ("<<GetDescription()<<"), values: {";
    vector<string>::const_iterator it = _m_domainValues.begin();
    vector<string>::const_iterator last = _m_domainValues.end();
    while(it != last)
    {
        if(it != _m_domainValues.begin() )
            ss << ", ";
        ss << *it;
        it++;
    }
    ss << "}";
    return(ss.str() );
}


Index StateFactorDiscrete::
AddStateFactorValue(const string &v)
{
    _m_domainValues.push_back(v);
    Index i = _m_domainSize++;
    return(i);
}
