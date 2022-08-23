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

#include "ObservationModelMappingSparse.h"

using namespace std;

ObservationModelMappingSparse::ObservationModelMappingSparse(int nrS,
                                                             int nrJA,
                                                             int nrJO) :
    ObservationModelDiscrete(nrS, nrJA, nrJO)
{
    SparseMatrix *O;
    for(int a=0;a!=nrJA;++a)
    {
        O=new SparseMatrix(nrS,nrJO);
        _m_O.push_back(O);
    }
}

ObservationModelMappingSparse::
ObservationModelMappingSparse(const ObservationModelMappingSparse& OM) :
    ObservationModelDiscrete(OM)
{
    SparseMatrix *O;
    for(unsigned int a=0;a!=OM._m_O.size();++a)
    {
        O=new SparseMatrix(*OM._m_O[a]);
        _m_O.push_back(O);
    }
}

ObservationModelMappingSparse::~ObservationModelMappingSparse()
{    
    for(vector<SparseMatrix*>::iterator it=_m_O.begin();
        it!=_m_O.end(); ++it)
        delete(*it);
    _m_O.clear();
}
