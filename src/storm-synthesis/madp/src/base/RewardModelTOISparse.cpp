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

#include "RewardModelTOISparse.h"

using namespace std;

RewardModelTOISparse::RewardModelTOISparse(const string &s_str,
                                           const string &ja_str)
{
    _m_s_str = s_str;
    _m_ja_str = ja_str;
}

RewardModelTOISparse::~RewardModelTOISparse()
{
}

double RewardModelTOISparse::Get(const std::vector<Index> &sIs,
                                 const std::vector<Index> &aIs) const
{
    if(_m_R.find(make_pair(sIs,aIs))!=_m_R.end())
        return(_m_R.find(make_pair(sIs,aIs))->second);
    else
        return(0);
}

void RewardModelTOISparse::Set(const std::vector<Index> &sIs,
                               const std::vector<Index> &aIs,
                               double reward)
{
    _m_R.insert(make_pair(make_pair(sIs,aIs),reward));
}

string RewardModelTOISparse::SoftPrint() const
{
#if 0
    stringstream ss;
    double r;
    ss << _m_s_str <<"\t"<< _m_ja_str <<"\t"
       << "R(" << _m_s_str <<","<< _m_ja_str
       <<  ") (rewards of 0 are not printed)"<<endl;
    for(Index s_i = 0; s_i < _m_nrStates; s_i++)
        for(Index ja_i = 0; ja_i < _m_nrJointActions; ja_i++)
        {
            r=Get(s_i, ja_i);
            if(std::abs(r)>0)
                ss << s_i << "\t" << ja_i << "\t" << r << endl;
        }
    return(ss.str());
#else
#endif
    return("RewardModelTOISparse::SoftPrint: not yet implemented");
}

