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

#include "FactoredQFunctionScopeForStage.h"

using namespace std;

//Default constructor
FactoredQFunctionScopeForStage::FactoredQFunctionScopeForStage()
    :
    _m_agentScopes(0),
    _m_sfacScopes(0)
{
}

void 
FactoredQFunctionScopeForStage::AddLocalQ( const Scope& sfacS, const Scope& agS)
{
    _m_sfacScopes.push_back(sfacS);
    _m_agentScopes.push_back(agS);
}

void 
FactoredQFunctionScopeForStage::RemoveLocalQ( Index j)
{
    if(j >= _m_sfacScopes.size())
        throw E("FactoredQFunctionScopeForStage::RemoveLocalQ( Index j), index j out of bounds");

    _m_sfacScopes.erase(_m_sfacScopes.begin() + j );
    _m_agentScopes.erase(_m_agentScopes.begin() + j );
}

string FactoredQFunctionScopeForStage::SoftPrint() const
{
    stringstream ss; 
    for(Index q=0; q < _m_sfacScopes.size(); q++)
    {
        ss << q << "-th local Q function, agentScope="<< _m_agentScopes.at(q) <<
            ", sfacScope=" << _m_sfacScopes.at(q) << endl;
    }
    return (ss.str());
}
