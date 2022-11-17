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

#include "TOIFactoredRewardDecPOMDPDiscrete.h"

using namespace std;

#define DEBUG_TOIFactoredRewardDecPOMDPDiscrete 0

TOIFactoredRewardDecPOMDPDiscrete::
TOIFactoredRewardDecPOMDPDiscrete(
    const string &name, const string &descr, const string &pf,
    bool cacheFlatModels) :
    TOIDecPOMDPDiscrete(name, descr, pf, cacheFlatModels)
{
    _m_initialized = false;
}

TOIFactoredRewardDecPOMDPDiscrete::
TOIFactoredRewardDecPOMDPDiscrete(const TOIFactoredRewardDecPOMDPDiscrete& o) 
{
    throw(E("TOIFactoredRewardDecPOMDPDiscrete: copy ctor not yet implemented"));
}

TOIFactoredRewardDecPOMDPDiscrete::~TOIFactoredRewardDecPOMDPDiscrete()
{
    for(unsigned int i=0;i!=_m_p_rModels.size();++i)
        delete(_m_p_rModels[i]);
}

//Copy assignment operator
TOIFactoredRewardDecPOMDPDiscrete&
TOIFactoredRewardDecPOMDPDiscrete::operator=
(const TOIFactoredRewardDecPOMDPDiscrete& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    throw(E("TOIFactoredRewardDecPOMDPDiscrete: assignment not yet implemented"));

    return *this;
}

bool TOIFactoredRewardDecPOMDPDiscrete::SetInitialized(bool b)
{
    if(b == false)
    {
        _m_initialized = false;
        return(true);
    }

    if( TOIDecPOMDPDiscrete::SetInitialized(true))
    {
        if( b == true )
        {
            if (_m_p_rModels.size()!=GetNrAgents())
                throw E("TOIFactoredRewardDecPOMDPDiscrete::SetInitialized(true) : no reward models specified yet!");
                
            for(unsigned int i=0;i!=GetNrAgents();++i)
                if (_m_p_rModels[i] == 0)
                    throw E("TOIFactoredRewardDecPOMDPDiscrete::SetInitialized(true) : no reward model specified yet! ( _m_p_rModels[i] == 0 )");

        }
        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}

double TOIFactoredRewardDecPOMDPDiscrete::GetReward(Index sI, Index jaI) const
{
    double reward=0;
    vector<Index> indSIs=JointToIndividualStateIndices(sI),
        indAIs=JointToIndividualActionIndices(jaI);

    for(unsigned int i=0;i!=GetNrAgents();++i)
        reward+=GetIndividualReward(indSIs[i],indAIs[i],i);

    reward+=_m_p_rModel->Get(indSIs,indAIs);
//    reward+=_m_p_rModel->Get(sI,jaI);

#if DEBUG_TOIFactoredRewardDecPOMDPDiscrete
    cout << "GetReward(" << sI << "," << jaI << ") = " << reward << endl;
#endif
    return(reward);
}

double
TOIFactoredRewardDecPOMDPDiscrete::GetReward(const std::vector<Index> &sIs,
                                             const std::vector<Index> &aIs) const
{
    double reward=0;

    for(unsigned int i=0;i!=GetNrAgents();++i)
        reward+=GetIndividualReward(sIs[i],aIs[i],i);

    reward+=_m_p_rModel->Get(sIs,aIs);

#if DEBUG_TOIFactoredRewardDecPOMDPDiscrete
    cout << "GetReward(" << sI << "," << jaI << ") = " << reward << endl;
#endif

    return(reward);
}

void TOIFactoredRewardDecPOMDPDiscrete::
SetIndividualRewardModel(RewardModel* rewardModel,
                         Index agentID)
{
    if(_m_p_rModels.size()<=agentID)
        _m_p_rModels.resize(agentID+1);

    _m_p_rModels[agentID]=rewardModel;
}

double TOIFactoredRewardDecPOMDPDiscrete::
GetIndividualReward(Index indSI, Index indAI, Index agentID) const
{
    double reward=_m_p_rModels[agentID]->Get(indSI,indAI);
#if DEBUG_TOIFactoredRewardDecPOMDPDiscrete
    cout << "GetIndividualReward[" << agentID << "](" << indSI << "," << indAI
         << ") = " << reward << endl;
#endif
    return(reward);
}

string TOIFactoredRewardDecPOMDPDiscrete::SoftPrint() const
{
    stringstream ss;
    ss << TOIDecPOMDPDiscrete::SoftPrint();

    if(_m_initialized)
    {
        ss << "Reward models: " << endl;
        for(unsigned int i=0;i!=GetNrAgents();++i)
        {
            ss << "Individual rewards for agent " << i << endl;
            ss << _m_p_rModels[i]->SoftPrint();
        }
    }
    else
        throw E("TOIFactoredRewardDecPOMDPDiscrete components (reward model) not initialized");

    return(ss.str());
}
