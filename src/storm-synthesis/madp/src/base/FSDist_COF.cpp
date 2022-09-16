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

#include <cstdlib>
#include "FSDist_COF.h"
#include "StateFactorDiscrete.h"
#include "MADPComponentFactoredStates.h"
#include "MultiAgentDecisionProcessDiscreteFactoredStatesInterface.h"
using namespace std;

//Default constructor
//FSDist_COF::FSDist_COF() {}

FSDist_COF::FSDist_COF() :
    _m_nrStateFactors(0),
    _m_sfacDomainSizes(0),
    _m_stepSize(0),
    _m_probs(0)
{
}

FSDist_COF::FSDist_COF(const MADPComponentFactoredStates& a) :
    _m_nrStateFactors(a.GetNrStateFactors()),
    _m_sfacDomainSizes(a.GetNrStateFactors()),
    _m_probs(a.GetNrStateFactors())
{
    for(Index i=0; i < a.GetNrStateFactors(); i++)
    {
        _m_sfacDomainSizes[i] = a.GetNrValuesForFactor(i);
        _m_probs[i] = vector<double>( a.GetNrValuesForFactor( i ), 0.0 );
    }
    _m_stepSize = IndexTools::CalculateStepSize(_m_sfacDomainSizes);
}

FSDist_COF::FSDist_COF(const MultiAgentDecisionProcessDiscreteFactoredStatesInterface& a) :
    _m_nrStateFactors(a.GetNrStateFactors()),
    _m_sfacDomainSizes(a.GetNrStateFactors()),
    _m_probs(a.GetNrStateFactors())
{
    for(Index i=0; i < a.GetNrStateFactors(); i++)
    {
        _m_sfacDomainSizes[i] = a.GetNrValuesForFactor(i);
        _m_probs[i] = vector<double>( a.GetNrValuesForFactor( i ), 0.0 );
    }
    _m_stepSize = IndexTools::CalculateStepSize(_m_sfacDomainSizes);
}

//Copy constructor.    
FSDist_COF::FSDist_COF(const FSDist_COF& o) :
    _m_nrStateFactors(o._m_nrStateFactors),
    _m_sfacDomainSizes(o._m_sfacDomainSizes),
    _m_probs(o._m_probs)
{
    _m_stepSize = IndexTools::CalculateStepSize(_m_sfacDomainSizes);
}

//Destructor
FSDist_COF::~FSDist_COF()
{
    delete [] _m_stepSize;
}

//Copy assignment operator
FSDist_COF& FSDist_COF::operator= (const FSDist_COF& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    _m_nrStateFactors = o._m_nrStateFactors;
    _m_sfacDomainSizes = o._m_sfacDomainSizes;
    _m_probs = o._m_probs;
    _m_stepSize = IndexTools::CalculateStepSize(_m_sfacDomainSizes);

    return *this;
}

void FSDist_COF::SetUniform()
{
    for(Index i=0; i < _m_nrStateFactors; i++)
    { 
        size_t nrVals =  _m_sfacDomainSizes[i]; 
        double p = 1.0 / (double) nrVals;
        for(Index j=0; j < nrVals; j++)
            _m_probs[i][j] = p;
    }
}

void FSDist_COF::SetZero()
{
    for(Index i=0; i < _m_nrStateFactors; ++i)
    { 
        size_t nrVals =  _m_sfacDomainSizes[i]; 
        double p = 0.0;// / (double) nrVals;
        for(Index j=0; j < nrVals; j++)
            _m_probs[i][j] = p;
    }
}

double FSDist_COF::GetProbability(Index sI) const
{
    vector<Index> sfacValues=IndexTools::JointToIndividualIndicesStepSize(
        sI, _m_stepSize, _m_nrStateFactors);
    return GetProbability(sfacValues);
}

double FSDist_COF::GetProbability(const vector<Index>& sfacValues) const
{
    double p = 1.0;
    for(Index i=0; i < _m_nrStateFactors; i++)    
        p *= _m_probs.at(i).at( sfacValues.at(i) );
    return p;
}

double FSDist_COF::GetProbability(const Scope& sfSc,
                                  const std::vector<Index>& sfacValues) const
{
    double p = 1.0;
    for(Index i=0; i < sfSc.size(); i++)
    {
        Index sfI = sfSc.at(i);
        Index sfValI_sfI = sfacValues.at(i);
        double p_this_sf = _m_probs.at(sfI).at(sfValI_sfI);
        p *= p_this_sf;
    }
    
    return p;
}

vector<double> FSDist_COF::ToVectorOfDoubles() const
{
    size_t nrStates=GetNrStates();
    vector<Index> sfacValues(_m_nrStateFactors, 0);
    Index sI = 0;
    vector<double> flatDist( nrStates, 0.0 );
    do {
        double p = GetProbability(sfacValues);
        flatDist.at(sI) = p;
        sI++;
    }while ( ! IndexTools::Increment(sfacValues, _m_sfacDomainSizes) );
    return flatDist;
}

size_t FSDist_COF::GetNrStates() const
{
    size_t nrStates = 1;
    for(Index i=0; i!=_m_nrStateFactors; ++i)
        nrStates*=_m_sfacDomainSizes.at(i);
    return(nrStates);
}
   
vector<Index> FSDist_COF::SampleState() const
{
    vector<Index> state(_m_nrStateFactors);

    for(Index i=0; i < _m_nrStateFactors; i++)    
    {  
        double randNr=rand() / (RAND_MAX + 1.0);
        double sum=0;
        for(Index valI = 0; valI < _m_sfacDomainSizes[i]; valI++)
        {
            sum+=_m_probs[i][valI];
            if(randNr<=sum)
            {
                state.at(i)=valI;
                break;
            }
        }
    }
    return state;
}

string FSDist_COF::SoftPrint() const
{
    stringstream ss; 
    for(Index i=0; i < _m_probs.size(); i++)
    {
        ss << "SF" << i;
        ss << " - ";
        ss << SoftPrintVector(_m_probs.at(i) ) << endl;
    }
    return (ss.str());
}

void FSDist_COF::SanityCheck()
{
    //cout << "Starting FSDist_COF<M>::SanityCheck()"<<endl;
    for(Index sfacI=0; sfacI < _m_probs.size(); sfacI++)
    {
        double psum = 0.0;
        for(Index valI=0; valI < _m_probs.at(sfacI).size(); valI++)
            psum += _m_probs.at(sfacI).at(valI);

        if(!Globals::EqualProbability(psum, 1.0))
        {
            stringstream ss;
            ss << "Warning! - FSDist_COF::SanityCheck sfacI="<<sfacI<<" does not sum to 1.0, but to " << psum << endl;
            if(  abs(psum - 1.0) < 1e-9  )
            {
                ss << "Renormalizing, since the difference is < 1e-9."<<endl;
		Normalize(sfacI, psum);

                cerr << ss.str();
            }
            else
                throw(E(ss));
        }
    }
}

void FSDist_COF::Normalize(Index sfacI)
{
    double sum = 0;
    for(Index valI=0; valI < _m_probs[sfacI].size(); valI++)
        sum += _m_probs[sfacI][valI];
    Normalize(sfacI, sum);
}

void FSDist_COF::Normalize(Index sfacI, double sum)
{
    if(sum > 0)
    {
        for(Index valI=0; valI < _m_probs.at(sfacI).size(); valI++)
	    _m_probs[sfacI][valI] /= sum;
    }
    else
    {
        for(Index valI=0; valI < _m_probs.at(sfacI).size(); valI++)
	    _m_probs[sfacI][valI] = 1.0/_m_probs[sfacI].size();
    }
}
