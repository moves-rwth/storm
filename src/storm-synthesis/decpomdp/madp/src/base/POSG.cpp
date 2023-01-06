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

#include "POSG.h"

using namespace std;

POSG::POSG() 
{
    _m_initialized = false;
    _m_nrAgents = 0;
    //we can not call GetNrAgents from here...
    //instead this object will have to have the number of agents set and be
    //initialized.
    //_m_rewardType = vector<double>(GetNrAgents(), REWARD);
    //_m_discount = vector<double>(GetNrAgents(), 1.0);
}

void POSG::SetDiscount(Index agentI, double d)
{
    if(d>=0 && d<=1)
        _m_discount.at(agentI)=d;
    else
        throw(E("POSG::SetDiscount() discount not valid, should be >=0 and <=1"));
}

string POSG::SoftPrint() const
{
    stringstream ss;
    ss << "Discount factors: " << 
        PrintTools::SoftPrintVector(_m_discount) << endl;
    ss << "Reward type: " << 
        PrintTools::SoftPrintVector(_m_rewardType) << endl;
    return ss.str();
}

void POSG::SetRewardType(Index agentI, reward_t r)
{
    if(r!=REWARD)
        throw(E("POSG::SetRewardType only reward type REWARD is supported"));
    _m_rewardType.at(agentI) = r;
}

///changed initialized status
bool POSG::SetInitialized(bool b)
{
    if(_m_nrAgents == 0)
    {
        throw E("POSG::SetInitialized failed because POSG doesn't know the \
number of agents yet. (use SetNrAgents first!)");
    }

    //do some checks?
    _m_initialized = true;
    return(true);
}

///Sets the number of agents
void POSG::SetNrAgents (size_t nrAgents)
{
    if(_m_initialized)
    {
        //do some de-initialization things ?
        _m_initialized = false;
        _m_nrAgents = nrAgents;
    }    

    _m_discount = vector<double>(_m_nrAgents, 1.0);
    _m_rewardType = vector<reward_t>(_m_nrAgents, REWARD);

}

