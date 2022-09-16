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

#include "JointActionDiscrete.h"
#include "ActionDiscrete.h"

using namespace std;

#define DEBUG_JAD 0

//Default constructor
JointActionDiscrete::JointActionDiscrete(Index index) :
    DiscreteEntity(index)
{
}

JointActionDiscrete::JointActionDiscrete(Index index,
                                         vector<const ActionDiscrete*> a) : 
    DiscreteEntity(index),
    _m_apVector(a)
{
    _m_aIndexVector = ConstructIndividualActionDiscretesIndices();
}

//Copy assignment constructor.        
JointActionDiscrete::JointActionDiscrete(const JointActionDiscrete& o) :
    DiscreteEntity(o)
{    
if(DEBUG_JAD)   cout << " cloning joint action ";

    vector<const ActionDiscrete*>::const_iterator itp = o._m_apVector.begin();
    vector<const ActionDiscrete*>::const_iterator lastp = o._m_apVector.end();
    while(itp != lastp)
    {
        const ActionDiscrete* t = *itp;
        _m_apVector.push_back(t);
        itp++;
    }
    _m_aIndexVector = o._m_aIndexVector;
}

//Destructor
JointActionDiscrete::~JointActionDiscrete()
{
if(DEBUG_JAD) cout << "deleting joint action";

/*  Do not delete the individual actions that are pointed to (there is only
 *  one copy of those, so that will lead to segfaults)
    for(Index i=0; i<_m_apVector.size(); i++) 
        delete _m_apVector[i];*/

    _m_apVector.clear();
}

void JointActionDiscrete::DeleteIndividualActions()
{
    for(vector<const ActionDiscrete*>::size_type i=0; i<_m_apVector.size(); i++) 
        delete _m_apVector[i];
}

void JointActionDiscrete::AddIndividualAction(const ActionDiscrete* a, 
        Index agentI)
{
    if( static_cast< vector<const ActionDiscrete*>::size_type >(agentI) != 
        _m_apVector.size() )
    {
        stringstream ss;
        ss << "WARNING! AddIndividualAction: size of _m_apVector does not match index of agent!\n _m_apVector.size()="
           << _m_apVector.size()<<" - agentI="<<agentI;
        throw(E(ss));
    }
    _m_apVector.push_back(a);
    _m_aIndexVector.push_back(a->GetIndex());
}

string JointActionDiscrete::SoftPrint() const
{
    stringstream ss;
    vector<const ActionDiscrete*>::const_iterator it =  _m_apVector.begin();
    vector<const ActionDiscrete*>::const_iterator last =  _m_apVector.end();

    ss << "JA" << GetIndex();
    
    while(it != last)
    {
        if(*it != 0)
            ss << "_" << (*it)->SoftPrintBrief(); 
        it++;
    }
    return(ss.str());
}

string JointActionDiscrete::SoftPrintBrief() const
{
    stringstream ss;
    vector<const ActionDiscrete*>::const_iterator it =  _m_apVector.begin();
    vector<const ActionDiscrete*>::const_iterator last =  _m_apVector.end();
    
    while(it != last)
    {
        if(*it != 0)
            ss << (*it)->SoftPrintBrief();
        if(it != last-1)
            ss << "_";
        it++;
    }
    return(ss.str());
}

vector<Index> JointActionDiscrete::ConstructIndividualActionDiscretesIndices() const
{
    vector<Index> iv;
    vector<const ActionDiscrete*>::const_iterator it = _m_apVector.begin();
    vector<const ActionDiscrete*>::const_iterator last = _m_apVector.end();
    while(it != last)
    {
        
        Index index = (*it)->GetIndex();
        iv.push_back( index );
        it++;
    }
    return(iv);
}
