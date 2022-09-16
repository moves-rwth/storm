/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * João Messias 
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */

#include "EventObservationModelMappingSparse.h"

using namespace std;

EventObservationModelMappingSparse::EventObservationModelMappingSparse(int nrS,
                                                             int nrJA,
                                                             int nrJO) :
    ObservationModelDiscrete(nrS, nrJA, nrJO)
{
    SparseMatrix *O;
    for(int a=0;a!=nrJA;++a)
    {
        std::vector<SparseMatrix*> S;
        for(int joI=0;joI!=nrJO;++joI)
        {
            O=new SparseMatrix(nrS,nrS);
            O->clear();
            S.push_back(O);
        }
        _m_O.push_back(S);
    }
}

EventObservationModelMappingSparse::
EventObservationModelMappingSparse(const EventObservationModelMappingSparse& OM) :
    ObservationModelDiscrete(OM)
{
    SparseMatrix *O;
    for(unsigned int a=0;a!=OM._m_O.size();++a)
    {
        std::vector<SparseMatrix*> S;
        for(unsigned int joI=0;joI!=OM._m_O.at(0).size();++joI)
        {
            O=new SparseMatrix(*OM._m_O[a][joI]);
            S.push_back(O);
        }
        _m_O.push_back(S);
    }    
}

EventObservationModelMappingSparse::~EventObservationModelMappingSparse()
{
    for(size_t i = 0; i < _m_O.size(); i++)
    {
      for(vector<SparseMatrix*>::iterator it=_m_O.at(i).begin(); it!=_m_O.at(i).end(); ++it)
        delete(*it);
      _m_O.at(i).clear();
    }
}
