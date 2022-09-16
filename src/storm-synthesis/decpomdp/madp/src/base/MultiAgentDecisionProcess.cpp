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

#include "MultiAgentDecisionProcess.h"    
#include "Scope.h"

using namespace std;

#define DEBUG_MADP 0

MultiAgentDecisionProcess::MultiAgentDecisionProcess(size_t nrAgents, 
    const string &name, const string &description, const string &pf) :
    _m_name(name, description),
    _m_nrAgents(nrAgents)
    ,_m_allAgentsScope("<>")
    ,_m_problemFile(pf)
{
    InitializeUnixName();
    for(Index i=0; i<nrAgents; i++)
        _m_allAgentsScope.Insert(i);
}

MultiAgentDecisionProcess::MultiAgentDecisionProcess(
    const string &name, const string &description, const string &pf) :
    _m_name(name, description),
    _m_nrAgents(0)
    ,_m_allAgentsScope("<>")
    ,_m_problemFile(pf)
{
    InitializeUnixName();
}

void MultiAgentDecisionProcess::InitializeUnixName()
{
    // strip everything before and including the last /
    string unixName=_m_problemFile.substr(_m_problemFile.find_last_of('/') + 1);

    // and after the last .
    _m_unixName=unixName.substr(0,unixName.find_last_of('.'));
}

MultiAgentDecisionProcess::~MultiAgentDecisionProcess()
{
    _m_problemFile.clear();
}

size_t MultiAgentDecisionProcess::GetNrAgents() const 
{
    return(_m_nrAgents);
}

void MultiAgentDecisionProcess::SetNrAgents(size_t nrAgents) 
{
    _m_nrAgents = nrAgents;
    _m_agents.clear();
    for(Index i = 0; i < nrAgents; i++)
    {
        _m_agents.push_back(Agent(i));
        _m_allAgentsScope.Insert(i);
    }
}

void MultiAgentDecisionProcess::AddAgent(string name)
{
    Index agI = _m_nrAgents++;
    _m_agents.push_back( Agent(agI, name) );
    _m_allAgentsScope.Insert(agI);
}

Index MultiAgentDecisionProcess::GetAgentIndexByName(const string &s) const	       
{
    vector<Agent>::const_iterator it;
    for(it = _m_agents.begin(); it != _m_agents.end(); it++)
    {
        string s2 = (*it).GetName();
        if(s == s2)
            return( (*it).GetIndex() );
    }
    //not found
    stringstream ss;
    ss << "GetAgentIndexByName - agent \"" << s << " not found." << endl;
    throw E(ss.str().c_str());
}

string MultiAgentDecisionProcess::GetAgentNameByIndex(Index i) const
{
  return _m_agents[i].GetName();
}

string MultiAgentDecisionProcess::GetProblemFile() const 
{
    return(_m_problemFile);
}

string MultiAgentDecisionProcess::SoftPrint() const
{
    stringstream ss;
    ss << "Problem:"<< _m_name.GetName()<< endl;
    ss << "descr.:"<< _m_name.GetDescription() << endl;
    ss << "nrAgents=" << _m_nrAgents << endl;
    ss << "problem file=" << _m_problemFile << endl;
    ss << "unixName=" << _m_unixName << endl;
    return(ss.str());
}
