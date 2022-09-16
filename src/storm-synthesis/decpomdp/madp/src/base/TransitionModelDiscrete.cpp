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

#include "TransitionModelDiscrete.h"

using namespace std;

TransitionModelDiscrete::TransitionModelDiscrete(int nrS, int nrJA) :
    _m_nrStates(nrS),
    _m_nrJointActions(nrJA)
{
}

TransitionModelDiscrete::~TransitionModelDiscrete()
{    
}

string TransitionModelDiscrete::SoftPrint() const 
{
    stringstream ss;
    double p = 0.0;
    ss << "s\tja\ts'\tP (tuples with P==0 are not printed)"<<endl;
    for(int sI = 0; sI < _m_nrStates; sI++)
        for(int jaI = 0; jaI < _m_nrJointActions; jaI++)
            for(int sIp = 0; sIp < _m_nrStates; sIp++)
            {
                p = Get(sI, jaI, sIp);
                if(p>0)
                    ss << sI << "\t" << jaI << "\t" << sIp << "\t" << p << endl;
            }
    return(ss.str());
}

Index TransitionModelDiscrete::SampleSuccessorState(Index state, Index action)
{
    double randNr=rand() / (RAND_MAX + 1.0);

    double sum=0;
    Index sucState=0;
    int i;

    for(i=0;i<_m_nrStates;i++)
    {
        sum+=Get(state,action,i);
        if(randNr<=sum)
        {
            sucState=i;
            break;
        }
    }
    return(sucState);
}
