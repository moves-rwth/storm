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

#include "TransitionModelMappingSparse.h"

using namespace std;

TransitionModelMappingSparse::TransitionModelMappingSparse(int nrS, int nrJA) :
    TransitionModelDiscrete(nrS, nrJA)
{    
    SparseMatrix *T;
    for(int a=0;a!=nrJA;++a)
    {
        T=new SparseMatrix(nrS,nrS);
        _m_T.push_back(T);
    }
}

TransitionModelMappingSparse::
TransitionModelMappingSparse(const TransitionModelMappingSparse& TM) :
    TransitionModelDiscrete(TM)
{
    SparseMatrix *T;
    for(unsigned int a=0;a!=TM._m_T.size();++a)
    {
        T=new SparseMatrix(*TM._m_T[a]);
        _m_T.push_back(T);
    }
}

TransitionModelMappingSparse::~TransitionModelMappingSparse()
{    
    for(vector<SparseMatrix*>::iterator it=_m_T.begin();
        it!=_m_T.end(); ++it)
        delete(*it);
    _m_T.clear();
}
