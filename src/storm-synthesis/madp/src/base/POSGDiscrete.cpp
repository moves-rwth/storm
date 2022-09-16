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

#include "POSGDiscrete.h"    

using namespace std;

#define DEBUG_DPOMDPD 0
//Debug Fill POSG Discrete - functions to initialize the POSGDiscrete
//(typically from a file)
#define DEBUG_FILLDPD 0

POSGDiscrete::POSGDiscrete(
    const string &name, const string &descr, const string &pf) :
    MultiAgentDecisionProcessDiscrete(name, descr, pf)
{
    _m_initialized = false;
    size_t nrAgents = GetNrAgents();
    POSG::SetNrAgents(nrAgents);
    POSG::SetInitialized(true);
    _m_p_rModel = vector<RewardModelMapping*>(nrAgents, 0);

}

POSGDiscrete::~POSGDiscrete()
{
    if(DEBUG_DPOMDPD)
        cout << "deleting POSGDiscrete (deleting rewards)"<<endl;
    for(vector<RewardModelMapping*>::iterator it = _m_p_rModel.begin();
            it != _m_p_rModel.end(); it++)
        delete *it;
}

bool POSGDiscrete::SetInitialized(bool b)
{
    if( MultiAgentDecisionProcessDiscrete::SetInitialized(true) )
    {
        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}

void POSGDiscrete::CreateNewRewardModel
    (Index agentI, size_t nrS, size_t nrJA)
{
    if(_m_initialized)
    delete(_m_p_rModel.at(agentI));

    _m_p_rModel.at(agentI) = new RewardModelMapping( nrS, nrJA);
}

string POSGDiscrete::SoftPrint() const
{
    stringstream ss;
    ss << MultiAgentDecisionProcessDiscrete::SoftPrint();
    ss << POSG::SoftPrint();

    if(_m_initialized)
    {
        for(Index agentI = 0; agentI < GetNrAgents(); agentI++)
        {
            ss << "Reward model for agent "<<agentI<<": " << endl;
            ss << _m_p_rModel.at(agentI)->SoftPrint() << endl;
        }
    }
    else
        throw E("POSGDiscrete components (reward model) not initialized");

    return(ss.str());
}

double POSGDiscrete::GetReward(Index agentI, State* s, JointAction* ja) 
    const
{
    return GetReward(agentI, 
            ((StateDiscrete*)s)->GetIndex(), 
            ((JointActionDiscrete*)ja)->GetIndex());
}

void POSGDiscrete::SetReward(Index agentI, Index sI, Index jaI,
                             Index sucSI, double r)
{
    double rOld=GetReward(agentI, sI,jaI),
        rExp=GetTransitionProbability(sI,jaI,sucSI)*r;
    SetReward(agentI, sI,jaI,rOld+rExp);
}

void POSGDiscrete::SetReward(Index agentI, Index sI, Index jaI, Index sucSI,
                             Index joI, double r)
{
    throw(E("POSGDiscrete::SetReward(agentI, sI,jaI,sucSI,joI,r) not implemented"));
}

