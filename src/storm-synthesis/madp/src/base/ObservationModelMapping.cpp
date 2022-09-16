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

#include "ObservationModelMapping.h"

using namespace std;

ObservationModelMapping::ObservationModelMapping(int nrS, int nrJA,
                                                 int nrJO) : 
    ObservationModelDiscrete(nrS, nrJA, nrJO)
{
    Matrix *O;
    for(int a=0;a!=nrJA;++a)
    {
        O=new Matrix(nrS,nrJO);
        O->clear();
        _m_O.push_back(O);
    }
}

ObservationModelMapping::
ObservationModelMapping(const ObservationModelMapping& OM) :
    ObservationModelDiscrete(OM)
{
    Matrix *O;
    for(unsigned int a=0;a!=OM._m_O.size();++a)
    {
        O=new Matrix(*OM._m_O[a]);
        _m_O.push_back(O);
    }
}

ObservationModelMapping::~ObservationModelMapping()
{    
    for(vector<Matrix*>::iterator it=_m_O.begin();
        it!=_m_O.end(); ++it)
        delete(*it);
    _m_O.clear();
}
