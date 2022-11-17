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

#include "TOICompactRewardDecPOMDPDiscrete.h"

using namespace std;

#define DEBUG_TOICompactRewardDecPOMDPDiscrete 0

TOICompactRewardDecPOMDPDiscrete::
TOICompactRewardDecPOMDPDiscrete(
    const string &name, const string &descr, const string &pf,
    bool cacheFlatModels) :
    TOIDecPOMDPDiscrete(name, descr, pf, cacheFlatModels),
    _m_nrTwoAgentStates(2,0),
    _m_nrTwoAgentActions(2,0)
{
    _m_initialized = false;
}

TOICompactRewardDecPOMDPDiscrete::
TOICompactRewardDecPOMDPDiscrete(const TOICompactRewardDecPOMDPDiscrete& o) 
{
    throw(E("TOICompactRewardDecPOMDPDiscrete: copy ctor not yet implemented"));
}

TOICompactRewardDecPOMDPDiscrete::~TOICompactRewardDecPOMDPDiscrete()
{
    for(unsigned int i=0;i!=_m_p_rModels.size();++i)
        delete(_m_p_rModels[i]);
}

//Copy assignment operator
TOICompactRewardDecPOMDPDiscrete&
TOICompactRewardDecPOMDPDiscrete::operator=
(const TOICompactRewardDecPOMDPDiscrete& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    throw(E("TOICompactRewardDecPOMDPDiscrete: assignment not yet implemented"));

    return *this;
}

bool TOICompactRewardDecPOMDPDiscrete::SetInitialized(bool b)
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
                throw E("TOICompactRewardDecPOMDPDiscrete::SetInitialized(true) : no reward models specified yet!");
                
            for(unsigned int i=0;i!=GetNrAgents();++i)
                if (_m_p_rModels[i] == 0)
                    throw E("TOICompactRewardDecPOMDPDiscrete::SetInitialized(true) : no reward model specified yet! ( _m_p_rModels[i] == 0 )");

            size_t nrStates=GetIndividualDecPOMDPD(0)->GetNrStates();
            for(unsigned int i=0;i!=GetNrAgents();++i)
                if(GetIndividualDecPOMDPD(i)->GetNrStates()!=nrStates)
                    throw E("TOICompactRewardDecPOMDPDiscrete::SetInitialized(true) : all agents are required to have the same state space (same number of individual states)");
        }

        for(Index i=0;i!=2;++i)
        {
            _m_nrTwoAgentStates[i]=GetIndividualMADPD(i)->GetNrStates();
            _m_nrTwoAgentActions[i]=GetIndividualMADPD(i)->GetNrJointActions();
        }

        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}

double TOICompactRewardDecPOMDPDiscrete::GetReward(Index sI, Index jaI) const
{
    return(GetReward(JointToIndividualStateIndices(sI),
                     JointToIndividualActionIndices(jaI)));
}

double TOICompactRewardDecPOMDPDiscrete::GetReward(
    const std::vector<Index> &indSIs,
    const std::vector<Index> &indAIs) const
{
    double reward=0;

    for(unsigned int i=0;i!=GetNrAgents();++i)
        reward+=GetIndividualReward(indSIs[i],indAIs[i],i);

    switch(GetNrAgents())
    {
    case 2:
        reward+=_m_p_rModel->Get(indSIs,indAIs);
        break;
    default:
    {
        vector<Index> indexVec(2,0);
        vector<size_t> nrElems(2,GetNrAgents());

        do
        {
#if 0
            if(indexVec[0]!=indexVec[1] &&
               GetTwoAgentReward(indexVec[0],indexVec[1],indSIs,indAIs)!=0)
                cout << "adding  i " << indexVec[0]
                     << " si " << indSIs[indexVec[0]] 
                     << GetIndividualMADPD(indexVec[0])->GetState(indSIs[indexVec[0]])->SoftPrint()
                     << " j " << indexVec[1] 
                     << " sj " << indSIs[indexVec[1]]
                     << GetIndividualMADPD(indexVec[1])->GetState(indSIs[indexVec[1]])->SoftPrint()
                     << " r "
                     << GetTwoAgentReward(indexVec[0],indexVec[1],
                                          indSIs,indAIs) << endl;
#endif
            if(indexVec[0]!=indexVec[1])
                reward+=GetTwoAgentReward(indexVec[0],indexVec[1],
                                          indSIs,indAIs);
        }
        while(!IndexTools::Increment(indexVec,nrElems));

#if 0
        reward2+=GetTwoAgentReward(0,1,indSIs,indAIs);
        reward2+=GetTwoAgentReward(1,0,indSIs,indAIs);

        reward2+=GetTwoAgentReward(0,2,indSIs,indAIs);
        reward2+=GetTwoAgentReward(2,0,indSIs,indAIs);

        reward2+=GetTwoAgentReward(1,2,indSIs,indAIs);
        reward2+=GetTwoAgentReward(2,1,indSIs,indAIs);
#endif
        break;
    }
    }

#if DEBUG_TOICompactRewardDecPOMDPDiscrete
    cout << "GetReward(" << sI << "," << jaI << ") = " << reward << endl;
#endif
    return(reward);
}

double
TOICompactRewardDecPOMDPDiscrete::
GetTwoAgentReward(Index i, Index j,
                  const vector<Index> &indSIs,
                  const vector<Index> &indAIs) const
{
    vector<Index> sIs(2,0), aIs(2,0);

    sIs[0]=indSIs[i]; aIs[0]=indAIs[i];
    sIs[1]=indSIs[j]; aIs[1]=indAIs[j];
#if 0
    double reward=_m_p_rModel->
        Get(IndexTools::IndividualToJointIndices(sIs,_m_nrTwoAgentStates),
            IndexTools::IndividualToJointIndices(aIs,_m_nrTwoAgentActions));
#endif

    double reward=_m_p_rModel->Get(sIs,aIs);

#if 0
    cout << reward << "indSIs " << SoftPrintVector(indSIs) 
         <<  SoftPrintVector(sIs) << " indAIs " 
         << SoftPrintVector(indAIs)
         << IndexTools::IndividualToJointIndices(sIs,_m_nrTwoAgentStates)
         <<  SoftPrintVector(aIs) 
         << IndexTools::IndividualToJointIndices(aIs,_m_nrTwoAgentActions)
         << " i " << i << " j " << j << endl; 
#endif
    return(reward);
}

void TOICompactRewardDecPOMDPDiscrete::
SetIndividualRewardModel(RewardModel* rewardModel,
                         Index agentID)
{
    if(_m_p_rModels.size()<=agentID)
        _m_p_rModels.resize(agentID+1);

    _m_p_rModels[agentID]=rewardModel;
}

double TOICompactRewardDecPOMDPDiscrete::
GetIndividualReward(Index indSI, Index indAI, Index agentID) const
{
    double reward=_m_p_rModels[agentID]->Get(indSI,indAI);
#if DEBUG_TOICompactRewardDecPOMDPDiscrete
    cout << "GetIndividualReward[" << agentID << "](" << indSI << "," << indAI
         << ") = " << reward << endl;
#endif
    return(reward);
}

string TOICompactRewardDecPOMDPDiscrete::SoftPrint() const
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
        throw E("TOICompactRewardDecPOMDPDiscrete components (reward model) not initialized");

    return(ss.str());
}
