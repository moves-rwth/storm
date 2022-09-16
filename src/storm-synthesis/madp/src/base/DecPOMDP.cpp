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

#include "DecPOMDP.h"

using namespace std;

DecPOMDP::DecPOMDP() 
{
    _m_rewardType = REWARD;
    _m_discount = 1.0;
}

void DecPOMDP::SetDiscount(double d)
{
    if(d>=0 && d<=1)
        _m_discount=d;
    else
        throw(E("DecPOMDP::SetDiscount() discount not valid, should be >=0 and <=1"));
}

string DecPOMDP::SoftPrint() const
{
    stringstream ss;
    ss << "Discount factor: " << _m_discount << endl;
    ss << "Reward type: " << _m_rewardType << endl;
    return ss.str();
}

void DecPOMDP::SetRewardType(reward_t r)
{
    if(r!=REWARD)
        throw(E("DecPOMDP::SetRewardType only reward type REWARD is supported"));
    _m_rewardType = r;
}
