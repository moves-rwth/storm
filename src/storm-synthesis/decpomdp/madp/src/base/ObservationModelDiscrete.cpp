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

#include "ObservationModelDiscrete.h"
#include <stdlib.h>

using namespace std;

//Default constructor
ObservationModelDiscrete::ObservationModelDiscrete(int nrS,
                                                   int nrJA,
                                                   int nrJO) :
    _m_nrStates(nrS),
    _m_nrJointActions(nrJA),
    _m_nrJointObservations(nrJO)
{
}

//Destructor
ObservationModelDiscrete::~ObservationModelDiscrete()
{
}

string ObservationModelDiscrete::SoftPrint() const
{
    stringstream ss;
    double p;
    ss << "jo\tja\ts'\tP (tuples with P==0 are not printed)"<<endl;
    for(int jo_i = 0; jo_i < _m_nrJointObservations; jo_i++)
        for(int ja_i = 0; ja_i < _m_nrJointActions; ja_i++)
            for(int s_ip = 0; s_ip < _m_nrStates; s_ip++)
            {
                p=Get(ja_i, s_ip, jo_i);
                if(p>0)
                    ss << jo_i << "\t" << ja_i << "\t" << s_ip << "\t" << p
                         << endl;
            }
    return(ss.str());
}
    
Index ObservationModelDiscrete::SampleJointObservation(Index jaI, Index sucI)
{
    double randNr=rand() / (RAND_MAX + 1.0);

    double sum=0;
    Index jo=0;
    int i;
    
    for(i=0;i<_m_nrJointObservations;i++)
    {
        sum+=Get(jaI,sucI,i);
        if(randNr<=sum)
        {
            jo=i;
            break;
        }
    }
    
    return(jo);
}

Index ObservationModelDiscrete::SampleJointObservation(Index sI, Index jaI, Index sucI)
{
    double randNr=rand() / (RAND_MAX + 1.0);

    double sum=0;
    Index jo=0;
    int i;
    
    for(i=0;i<_m_nrJointObservations;i++)
    {
        sum+=Get(sI,jaI,sucI,i);
        if(randNr<=sum)
        {
            jo=i;
            break;
        }
    }
    
    return(jo);
}

