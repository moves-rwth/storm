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

#include "RewardModelMappingSparse.h"

using namespace std;

RewardModelMappingSparse::RewardModelMappingSparse(size_t nrS, size_t nrJA,
                                                   const string &s_str,
                                                   const string &ja_str) : 
    RewardModel(nrS, nrJA),
    _m_R(nrS,nrJA)
{
    _m_s_str = s_str;
    _m_ja_str = ja_str;
}

RewardModelMappingSparse::~RewardModelMappingSparse()
{
}

string RewardModelMappingSparse::SoftPrint() const
{
    stringstream ss;
    double r;
    ss << _m_s_str <<"\t"<< _m_ja_str <<"\t"
       << "R(" << _m_s_str <<","<< _m_ja_str
       <<  ") (rewards of 0 are not printed)"<<endl;
    for(Index s_i = 0; s_i < GetNrStates(); s_i++)
        for(Index ja_i = 0; ja_i < GetNrJointActions(); ja_i++)
        {
            r=Get(s_i, ja_i);
            if(std::abs(r)>0)
                ss << s_i << "\t" << ja_i << "\t" << r << endl;
        }
    return(ss.str());
}

