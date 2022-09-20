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

#include "CPT.h"

using namespace std;

CPT::CPT() :
    _m_probTable(0,0)
{
}

CPT::CPT(size_t X, size_t Y)
    :
    _m_probTable(X,Y)
{   
   for(size_t x=0; x < X; x++) 
       for(size_t y=0; y < Y; y++) 
           _m_probTable(x,y)=0;
}

CPT::
CPT(const CPT& cpt)
   :
    _m_probTable( cpt._m_probTable)
{
}

CPT::~CPT()
{    
    _m_probTable.clear();
}

void CPT::SanityCheck() const
{
    //cout << "Starting CPT::SanityCheck()"<<endl;
    size_t X = _m_probTable.size1();
    size_t Y = _m_probTable.size2();
    for(size_t y=0; y < Y; y++) 
    {
        double psum = 0.0;
        for(size_t x=0; x < X; x++) 
            psum += _m_probTable(x,y);
        if (! Globals::EqualProbability( psum, 1.0 ) )
            cerr << "Warning! CPT::SanityCheck for y="<<y<< 
                " doesn't sum to 1.0!" << endl;
    }

}

Index CPT::Sample(Index y) const
{
    double randNr=rand() / (RAND_MAX + 1.0);
    double cumprob = 0.0;
    Index x;
    for(x=0; x < nrX(); x++)
    {
        cumprob += _m_probTable(x,y);
        if(randNr<=cumprob)
            break;
    }
    return(x);    
}

string CPT::SoftPrint() const
{
    stringstream ss;
    size_t X = _m_probTable.size1();
    size_t Y = _m_probTable.size2();
    for(size_t y=0; y < Y; y++) 
    {
        ss << "CPT y=" << y << " x:";
        for(size_t x=0; x < X; x++) 
            ss << " " << _m_probTable(x,y);
        ss << endl;
    }
    return(ss.str());
}

void CPT::SetRandom()
{
    //cout << "Starting CPT::SanityCheck()"<<endl;
    size_t X = _m_probTable.size1();
    size_t Y = _m_probTable.size2();
    for(size_t y=0; y < Y; y++) 
    {
        double psum = 0.0;
        for(size_t x=0; x < X; x++) 
        {
            double p = rand();
            psum += p;
            _m_probTable(x,y) = p;
        }
        //normalize:
        for(size_t x=0; x < X; x++) 
            _m_probTable(x,y) /= psum;
    }

}
