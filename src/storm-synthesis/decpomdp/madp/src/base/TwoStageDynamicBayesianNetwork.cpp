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

#include "TwoStageDynamicBayesianNetwork.h"
#include "IndexTools.h"
#include <stdlib.h>

using namespace std;

//Default constructor
TwoStageDynamicBayesianNetwork::TwoStageDynamicBayesianNetwork(
                MultiAgentDecisionProcessDiscreteFactoredStatesInterface& madp)
            :
                _m_madp(&madp)
                ,_m_nrY(0)
                ,_m_nrO(0)
{
    _m_SoIStorageInitialized = false;
    _m_IndividualToJointYiiIndices_catVector = 0;
    _m_IndividualToJointOiiIndices_catVector = 0;
    _m_SampleY = 0;
    _m_SampleO = 0;
    _m_SampleNrO = 0;

}
/*
//Copy constructor.    
TwoStageDynamicBayesianNetwork::TwoStageDynamicBayesianNetwork(const TwoStageDynamicBayesianNetwork& o) 
{
}
*/
//Destructor
TwoStageDynamicBayesianNetwork::~TwoStageDynamicBayesianNetwork()
{
    for(Index i=0;i!=_m_Y_CPDs.size();++i)
        delete _m_Y_CPDs.at(i);
    for(Index i=0;i!=_m_O_CPDs.size();++i)
        delete _m_O_CPDs.at(i);

    // delete by iterator, as temporary data may not be allocated when
    // reading from disk
    { std::vector<std::vector<Index>* >::iterator it;
        for (it=_m_X_restr_perY.begin();it!=_m_X_restr_perY.end();it++)
            delete *it;
        for (it=_m_A_restr_perY.begin();it!=_m_A_restr_perY.end();it++)
            delete *it;
        for (it=_m_Y_restr_perY.begin();it!=_m_Y_restr_perY.end();it++)
            delete *it;

        for (it=_m_X_restr_perO.begin();it!=_m_X_restr_perO.end();it++)
            delete *it;
        for (it=_m_A_restr_perO.begin();it!=_m_A_restr_perO.end();it++)
            delete *it;
        for (it=_m_Y_restr_perO.begin();it!=_m_Y_restr_perO.end();it++)
            delete *it;
        for (it=_m_O_restr_perO.begin();it!=_m_O_restr_perO.end();it++)
            delete *it;
    }
    { std::vector<size_t*>::iterator it;
        for (it=_m_nrVals_SoI_Y_stepsize.begin();it!=_m_nrVals_SoI_Y_stepsize.end();it++)
            delete [] *it;
        for (it=_m_nrVals_SoI_O_stepsize.begin();it!=_m_nrVals_SoI_O_stepsize.end();it++)
            delete [] *it;
    }

    delete _m_IndividualToJointYiiIndices_catVector;
    delete _m_IndividualToJointOiiIndices_catVector;
    delete _m_SampleY;
    delete _m_SampleO;
    delete _m_SampleNrO;
}
/*
//Copy assignment operator
TwoStageDynamicBayesianNetwork& TwoStageDynamicBayesianNetwork::operator= (const TwoStageDynamicBayesianNetwork& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    return *this;
}
*/


void TwoStageDynamicBayesianNetwork::
ScopeBackup(    const Scope & stateScope, 
                const Scope & agentScope,
                Scope & X,
                Scope & A) const
{
    //first we compute the 'closure' of the NS state factors Y and
    //observations O. I.e., within stage connections can grow the set
    //of Y and O that need to be considered:
    Scope Y = stateScope;
    Scope O = agentScope;
    ComputeWithinNextStageClosure(Y,O);    

    X.clear();
    A.clear();
    //Next we do the backup of the Ys and Os
    for( Scope::iterator y_it = Y.begin(); y_it != Y.end(); y_it++)
    {
        Index yI = *y_it;
        X.Insert( _m_XSoI_Y.at(yI) );
        A.Insert( _m_ASoI_Y.at(yI) );
    }
    for( Scope::iterator o_it = O.begin(); o_it != O.end(); o_it++)
    {
        X.Insert( _m_XSoI_O.at(*o_it) );
        A.Insert( _m_ASoI_O.at(*o_it) );
    }
    X.Sort();
    A.Sort();
    return;
}


void TwoStageDynamicBayesianNetwork::
ComputeWithinNextStageClosure(Scope& Y, Scope& O) const
{    
    bool converged = true;
    do{
        converged = true;
        //check all Y for non-included Y dependencies
        Scope::const_iterator s_it = Y.begin();
        Scope::const_iterator s_last = Y.end();
        while(s_it != s_last)
        {
            Index yI = *s_it;
            const Scope& y_YSoI = _m_YSoI_Y.at(yI);
            for(Scope::const_iterator oy_it = y_YSoI.begin(); 
                    oy_it != y_YSoI.end(); oy_it++)
                //yI has other Y (oyI = *oy_it) that point to it...
                //let's see if they are in Y already.
                if(! Y.Contains( *oy_it ) )
                {
                    converged = false;
                    Y.Insert(*oy_it);
                }
            s_it++;
        }
        
        //check all O for non-included O and Y dependencies
        Scope::const_iterator o_it = O.begin();
        Scope::const_iterator o_last = O.end();
        while(o_it != o_last)
        {
            Index oI = *o_it;
            const Scope& o_YSoI = _m_YSoI_O.at(oI);
            for(Scope::const_iterator oy_it = o_YSoI.begin(); 
                oy_it != o_YSoI.end(); oy_it++)
                //oI has other Y (oyI = *oy_it) that point to it...
                //let's see if they are in Y already.
                if(! Y.Contains( *oy_it ) )
                {
                    converged = false;
                    Y.Insert(*oy_it);
                }

            const Scope& o_OSoI = _m_OSoI_O.at(oI);
            for(Scope::const_iterator oo_it = o_OSoI.begin(); 
                oo_it != o_OSoI.end(); oo_it++)
                //oI has other O (ooI = *oo_it) that point to it...
                //let's see if they are in O already.
                if(! O.Contains( *oo_it ) )
                {
                    converged = false;
                    O.Insert(*oo_it);
                }


            o_it++;
        }

    }while (! converged );
}

Scope TwoStageDynamicBayesianNetwork::
StateScopeBackup( const Scope & stateScope, 
                  const Scope & agentScope ) const
{
    Scope X, A;
    ScopeBackup(stateScope, agentScope, X,A);
    return(X);
}
Scope TwoStageDynamicBayesianNetwork::
AgentScopeBackup( const Scope & stateScope, 
                        const Scope & agentScope) const
{
    Scope X, A;
    ScopeBackup(stateScope, agentScope, X,A);
    return(A);
}

double TwoStageDynamicBayesianNetwork::
GetYProbability( const vector<Index>& X,
                    const vector<Index>& A,
                    const vector<Index>& Y) const
{
    size_t nrSF = _m_madp->GetNrStateFactors();
    if(Y.size() != nrSF || X.size() != nrSF)
        throw E("TwoStageDynamicBayesianNetwork::GetYProbability only implemented for full state vectors");
    if(A.size() != _m_madp->GetNrAgents())
        throw E("TwoStageDynamicBayesianNetwork::GetYProbability only implemented for full joint actions");
    
    double p = 1.0;
    for(Index y=0; y < Y.size(); y++)
    {
        vector<Index> X_restr(GetXSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToScope(X, GetXSoI_Y(y), X_restr );
        vector<Index> A_restr(GetASoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_Y(y), A_restr );
        vector<Index> Y_restr(GetYSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToScope(Y, GetYSoI_Y(y), Y_restr );
        Index iiI = IndividualToJointYiiIndices(y, X_restr, A_restr, Y_restr);
        Index yVal = Y[y];
        double p_y = _m_Y_CPDs[y]->Get(yVal, iiI);  
        p *= p_y;
    }
    return(p);
}

double TwoStageDynamicBayesianNetwork::
GetYProbabilityGeneral( 
                    const Scope& Xscope,
                    const vector<Index>& X,
                    const Scope& Ascope,
                    const vector<Index>& A,
                    const Scope& YIIscope,
                    const vector<Index>& YII,
                    const Scope& Yscope,
                    const vector<Index>& Y
                    ) const
{

    double p = 1.0;
    for(Index Y_index=0; Y_index < Y.size(); Y_index++)
    {
        //Y_index is index in vector Y
        Index y = Yscope.at(Y_index); // the index to the variable we look at
        Index yVal = Y.at(Y_index); // the value of that variable 

        vector<Index> X_restr(GetXSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(X, Xscope, GetXSoI_Y(y), X_restr );
        vector<Index> A_restr(GetASoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(A, Ascope, GetASoI_Y(y), A_restr );
        vector<Index> YII_restr(GetYSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(YII, YIIscope, GetYSoI_Y(y), YII_restr );
        Index iiI = IndividualToJointYiiIndices(y, X_restr, A_restr, YII_restr);
        double p_y = _m_Y_CPDs[y]->Get(yVal, iiI);
        p *= p_y;
    }
    return(p);
}


vector<double> TwoStageDynamicBayesianNetwork::
GetYProbabilitiesExactScopes( 
                    const vector<Index>& Xii,
                    const vector<Index>& Aii,
                    const vector<Index>& Yii,
                    const Index& yIndex
                ) const
{
    size_t nrVals = _m_madp->GetNrValuesForFactor(yIndex);
    vector<double> probs(nrVals, 0.0);
    Index iiI = IndividualToJointYiiIndices(yIndex, Xii, Aii, Yii);
    for(Index valI=0; valI < nrVals; valI++)
    {
        probs[valI] = _m_Y_CPDs[yIndex]->Get(valI, iiI);
    }
    return(probs);
}
vector<double> TwoStageDynamicBayesianNetwork::
GetOProbabilitiesExactScopes( 
                    const vector<Index>& Aii,
                    const vector<Index>& Yii,
                    const vector<Index>& Oii,
                    const Index& oIndex //agentI
                ) const
{
    if(GetXSoI_O(oIndex).size() > 0)
    {
      stringstream errormsg;
      errormsg << "Observation Factor " << oIndex << " has a non-null PS SF scope, which was ignored.";
      throw E(errormsg.str());      
    }
    size_t nrVals = _m_madp->GetNrObservations(oIndex);
    vector<double> probs(nrVals, 0.0);
    Index iiI = IndividualToJointOiiIndices(oIndex, Aii, Yii, Oii);
    for(Index valI=0; valI < nrVals; valI++)
    {
        probs[valI] = _m_O_CPDs[oIndex]->Get(valI, iiI);
    }
    return(probs);
}
vector<double> TwoStageDynamicBayesianNetwork::
GetOProbabilitiesExactScopes( 
                    const vector<Index>& Xii,
                    const vector<Index>& Aii,
                    const vector<Index>& Yii,
                    const vector<Index>& Oii,
                    const Index& oIndex //agentI
                ) const
{
    size_t nrVals = _m_madp->GetNrObservations(oIndex);
    vector<double> probs(nrVals, 0.0);
    Index iiI = IndividualToJointOiiIndices(oIndex, Xii ,Aii, Yii, Oii);
    for(Index valI=0; valI < nrVals; valI++)
    {
        probs[valI] = _m_O_CPDs[oIndex]->Get(valI, iiI);
    }
    return(probs);
}
double TwoStageDynamicBayesianNetwork::
GetOProbability( const vector<Index>& A,
                    const vector<Index>& Y,
                    const vector<Index>& O) const
{
    double p = 1.0;
    for(Index o=0; o < O.size(); o++)
    {
        if(GetXSoI_O(o).size() > 0)
        {
          stringstream errormsg;
          errormsg << "Observation Factor " << o << " has a non-null PS SF scope, which was ignored.";
          throw E(errormsg.str());      
        }
        vector<Index> A_restr(GetASoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_O(o), A_restr );
        vector<Index> Y_restr(GetYSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(Y, GetYSoI_O(o), Y_restr );
        vector<Index> O_restr(GetOSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(O, GetOSoI_O(o), O_restr );
        Index iiI = IndividualToJointOiiIndices(o, A_restr, Y_restr, O_restr);
        Index oVal = O[o];
        double p_o = _m_O_CPDs[o]->Get(oVal, iiI);
        p *= p_o;
    }
    return(p);
}
double TwoStageDynamicBayesianNetwork::
GetOProbability( const vector<Index>& X, 
                 const vector<Index>& A,
                    const vector<Index>& Y,
                    const vector<Index>& O) const
{
    double p = 1.0;
    for(Index o=0; o < O.size(); o++)
    {
        vector<Index> X_restr(GetXSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(X, GetXSoI_O(o), X_restr );
        vector<Index> A_restr(GetASoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_O(o), A_restr );
        vector<Index> Y_restr(GetYSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(Y, GetYSoI_O(o), Y_restr );
        vector<Index> O_restr(GetOSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToScope(O, GetOSoI_O(o), O_restr );
        Index iiI = IndividualToJointOiiIndices(o, X_restr, A_restr, Y_restr, O_restr);
        Index oVal = O[o];
        double p_o = _m_O_CPDs[o]->Get(oVal, iiI);
        p *= p_o;
    }
    return(p);
}
double TwoStageDynamicBayesianNetwork::
GetYOProbability(       const Scope& X, const vector<Index>& Xs,
                        const Scope& A, const vector<Index>& As,
                        const Scope& Y, const vector<Index>& Ys,
                        const Scope& O, const vector<Index>& Os) const
{
    double p = 1.0;
    for(Index yI=0; yI < Y.size(); yI++)
    {
        //get the index of the variable Y we are looking at...
        Index y = Y[yI];
        Index yVal = Ys[yI]; //and its value acc. to Ys
        vector<Index> Xs_restr(GetXSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(Xs, X, GetXSoI_Y(y), Xs_restr );
        vector<Index> As_restr(GetASoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(As, A, GetASoI_Y(y), As_restr );
        vector<Index> Ys_restr(GetYSoI_Y(y).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(Ys, Y, GetYSoI_Y(y), Ys_restr );
        Index iiI = IndividualToJointYiiIndices(y,Xs_restr, As_restr, Ys_restr);

        double p_y = _m_Y_CPDs.at(y)->Get(yVal, iiI);
        p *= p_y;
    }
    for(Index oI=0; oI < O.size(); oI++)
    {
        //get the index of the variable Y we are looking at...
        Index o = O[oI];
        Index oVal = Os[oI]; // and its value according to Os
        vector<Index> Xs_restr(GetXSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(Xs, X, GetXSoI_O(o), Xs_restr );
        vector<Index> As_restr(GetASoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(As, A, GetASoI_O(o), As_restr );
        vector<Index> Ys_restr(GetYSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(Ys, Y, GetYSoI_O(o), Ys_restr );
        vector<Index> Os_restr(GetOSoI_O(o).size());
        IndexTools::RestrictIndividualIndicesToNarrowerScope(Os, O, GetOSoI_O(o), Os_restr );
        Index iiI = IndividualToJointOiiIndices(o,As_restr, Ys_restr, Os_restr);

        double p_o = _m_O_CPDs.at(o)->Get(oVal, iiI);
        p *= p_o;
    }
    return(p);
}
///Sample a NS state
vector<Index> TwoStageDynamicBayesianNetwork::
SampleY(  const vector<Index>& X,
                        const vector<Index>& A) const
{
    for(Index y=0; y < _m_nrY; y++)
    {
        IndexTools::RestrictIndividualIndicesToScope(X, GetXSoI_Y(y), 
                *_m_X_restr_perY[y]);
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_Y(y), 
                *_m_A_restr_perY[y]);
        //because Y->Y dependencies can only depend on lower index, we have already
        //sampled the relevant Y's in _m_SampleY:
        IndexTools::RestrictIndividualIndicesToScope(*_m_SampleY, GetYSoI_Y(y), 
                *_m_Y_restr_perY[y]);
        Index iiI = IndividualToJointYiiIndices(y,
                                                *_m_X_restr_perY[y],
                                                *_m_A_restr_perY[y],
                                                *_m_Y_restr_perY[y]);
        Index sampledYVal = _m_Y_CPDs[y]->Sample(iiI);
        (*_m_SampleY)[y] = sampledYVal;    
    }
    return(*_m_SampleY);
    /*
    double randNr=rand() / (RAND_MAX + 1.0);

    double sum=0;


    for(Index y=0; y < _m_nrY; y++)
    {
        IndexTools::RestrictIndividualIndicesToScope(X, GetXSoI_Y(y), 
                *_m_X_restr_perY[y]);
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_Y(y), 
                *_m_A_restr_perY[y]);
        (*_m_SampleY)[y]=0;    
    }

    do {
        
        double p = 1.0;
        for(Index y=0; y < _m_SampleY->size(); y++)
        {
            IndexTools::RestrictIndividualIndicesToScope(*_m_SampleY, GetYSoI_Y(y),
                                                         *_m_Y_restr_perY[y] );
            Index iiI = IndividualToJointYiiIndices(y,
                                                    *_m_X_restr_perY[y],
                                                    *_m_A_restr_perY[y],
                                                    *_m_Y_restr_perY[y]);
            Index yVal = (*_m_SampleY)[y];
            double p_y = _m_Y_CPDs[y]->Get(yVal, iiI);
            p *= p_y;
            // if the probability is 0, there is no need to check the
            // rest of the Ys
            //if(EqualProbability(p,0.0))
            //    break;
        }
        sum+=p;
#if 0
        cout << "Y " << SoftPrintVector(Y) << " : " << p << " sum " 
             << sum << " rand " << randNr << endl;
#endif
        if(randNr<=sum)
            break;

    } while(! IndexTools::Increment( *_m_SampleY,_m_madp->GetNrValuesPerFactor()));// _m_Yii_size_Y) );

    return(*_m_SampleY);
    */
}

///Sample an observation.
vector<Index> TwoStageDynamicBayesianNetwork::
SampleO(  const vector<Index>& X,
          const vector<Index>& A,
          const vector<Index>& Y) const
{
    for(Index o=0; o < _m_nrO; o++)
    {
        IndexTools::RestrictIndividualIndicesToScope(X, GetXSoI_O(o), 
                *_m_X_restr_perO[o]);
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_O(o), 
                *_m_A_restr_perO[o]);
        IndexTools::RestrictIndividualIndicesToScope(Y, GetYSoI_O(o), 
                *_m_Y_restr_perO[o] );
        //because O->O dependencies can only depend on lower index, we have already
        //sampled the relevant O's in _m_SampleO:
        IndexTools::RestrictIndividualIndicesToScope(
                *_m_SampleO, GetOSoI_O(o), *_m_O_restr_perO[o] );
        Index iiI = IndividualToJointOiiIndices(o,
                                                *_m_X_restr_perO[o],
                                                *_m_A_restr_perO[o],
                                                *_m_Y_restr_perO[o],
                                                *_m_O_restr_perO[o]);
        Index sampledOVal = _m_O_CPDs[o]->Sample(iiI);
        (*_m_SampleO)[o] = sampledOVal;    
    }
    return(*_m_SampleO);
/*    
    double randNr=rand() / (RAND_MAX + 1.0);

    double sum=0;
    
    for(Index o=0; o < _m_nrO; o++)
    {
        IndexTools::RestrictIndividualIndicesToScope(A, GetASoI_O(o), 
                *_m_A_restr_perO[o] );
        IndexTools::RestrictIndividualIndicesToScope(Y, GetYSoI_O(o), 
                *_m_Y_restr_perO[o] );
        (*_m_SampleO)[o]=0;    
    }

    do {
        
        double p = 1.0;
        for(Index o=0; o < _m_SampleO->size(); o++)
        {
            IndexTools::RestrictIndividualIndicesToScope(
                    *_m_SampleO, GetOSoI_O(o), *_m_O_restr_perO[o] );
            Index iiI = IndividualToJointOiiIndices(o,
                                                    *_m_A_restr_perO[o],
                                                    *_m_Y_restr_perO[o],
                                                    *_m_O_restr_perO[o]);
            Index oVal = (*_m_SampleO)[o];
            double p_o = _m_O_CPDs[o]->Get(oVal, iiI);
            p *= p_o;
            // if the probability is 0, there is no need to check the
            // rest of the Os
            if(EqualProbability(p,0.0))
                break;
        }

        sum+=p;
#if 0
        cout << "O " << SoftPrintVector(O) << " : " << p << " sum " 
             << sum << " rand " << randNr << endl;
#endif
        if(randNr<=sum)
            break;

    } while(! IndexTools::Increment( *_m_SampleO, *_m_SampleNrO) );

    return(*_m_SampleO);
    */
}

#define DEBUG_INIT_SOIs 0
void TwoStageDynamicBayesianNetwork::InitializeStorage()
{
    _m_nrY =  _m_madp->GetNrStateFactors();
    _m_nrO =  _m_madp->GetNrAgents();

#if DEBUG_INIT_SOIs
    cout << ">>>>>>\nInitializeStorage called, checking SoftPrint..." << endl;
    cout << this->SoftPrint() << endl;
#endif
    _m_XSoI_Y.clear();
    _m_ASoI_Y.clear();
    _m_YSoI_Y.clear();
    _m_XSoI_O.clear();
    _m_ASoI_O.clear();
    _m_YSoI_O.clear();
    _m_OSoI_O.clear();

    _m_XSoI_Y.resize(_m_nrY);
    _m_ASoI_Y.resize(_m_nrY);
    _m_YSoI_Y.resize(_m_nrY);
    _m_XSoI_O.resize(_m_nrO);
    _m_ASoI_O.resize(_m_nrO);
    _m_YSoI_O.resize(_m_nrO);
    _m_OSoI_O.resize(_m_nrO);

    for(Index i=0;i!=_m_Y_CPDs.size();++i)
        delete _m_Y_CPDs.at(i);
    for(Index i=0;i!=_m_O_CPDs.size();++i)
        delete _m_O_CPDs.at(i);
    _m_Y_CPDs.resize(_m_nrY);
    _m_O_CPDs.resize(_m_nrO);

#if DEBUG_INIT_SOIs
    cout << "InitializeStorage finished, checking SoftPrint..." << endl;
    cout << this->SoftPrint() << "<<<<<<<<"<<endl;
#endif

    _m_SampleY=new vector<Index>(_m_nrY);
    _m_SampleO=new vector<Index>(_m_nrO);
    _m_SampleNrO=new vector<size_t>(_m_nrO);
    for(Index o=0; o < _m_SampleNrO->size(); o++)
        (*_m_SampleNrO)[o]=_m_madp->GetNrObservations(o);

    _m_SoIStorageInitialized = true;
}

void TwoStageDynamicBayesianNetwork::SetSoI_Y( 
        Index y, 
        const Scope& XSoI, 
        const Scope& ASoI, 
        const Scope& YSoI)
{
    if(!_m_SoIStorageInitialized)
        throw E("Scopes of influence not yet initialized");
    
    _m_XSoI_Y.at(y) = XSoI;
    _m_ASoI_Y.at(y) = ASoI;
    _m_YSoI_Y.at(y) = YSoI;

    _m_XSoI_Y.at(y).Sort();
    _m_ASoI_Y.at(y).Sort();
    _m_YSoI_Y.at(y).Sort();
}

void TwoStageDynamicBayesianNetwork::SetSoI_O( 
        Index o, 
        const Scope& ASoI, 
        const Scope& YSoI, 
        const Scope& OSoI)
{
    if(!_m_SoIStorageInitialized)
        throw E("Scopes of influence not yet initialized");

    _m_ASoI_O.at(o) = ASoI;
    _m_YSoI_O.at(o) = YSoI;
    _m_OSoI_O.at(o) = OSoI;

    _m_ASoI_O.at(o).Sort();
    _m_YSoI_O.at(o).Sort();
    _m_OSoI_O.at(o).Sort();
}

void TwoStageDynamicBayesianNetwork::SetSoI_O( 
        Index o, 
        const Scope& XSoI, 
        const Scope& ASoI, 
        const Scope& YSoI, 
        const Scope& OSoI)
{
    if(!_m_SoIStorageInitialized)
        throw E("Scopes of influence not yet initialized");

    _m_XSoI_O.at(o) = XSoI;
    _m_ASoI_O.at(o) = ASoI;
    _m_YSoI_O.at(o) = YSoI;
    _m_OSoI_O.at(o) = OSoI;

    _m_XSoI_O.at(o).Sort();
    _m_ASoI_O.at(o).Sort();
    _m_YSoI_O.at(o).Sort();
    _m_OSoI_O.at(o).Sort();
}


void TwoStageDynamicBayesianNetwork::InitializeIIs()
{
    vector<size_t> nrValsPerSF = _m_madp->GetNrValuesPerFactor();

    //initialize meta data for Ys
    _m_nrVals_XSoI_Y.clear(); //for possible re-initialization (e.g. in marginalization)
    _m_nrVals_ASoI_Y.clear();
    _m_nrVals_YSoI_Y.clear();
    _m_nrVals_SoI_Y.clear();
    _m_nrVals_XSoI_Y.resize(_m_nrY);
    _m_nrVals_ASoI_Y.resize(_m_nrY);
    _m_nrVals_YSoI_Y.resize(_m_nrY);
    _m_nrVals_SoI_Y.resize(_m_nrY);

    _m_Xii_size_Y.clear();
    _m_Aii_size_Y.clear();
    _m_Yii_size_Y.clear();
    _m_ii_size_Y.clear();
    _m_Xii_size_Y.resize(_m_nrY);
    _m_Aii_size_Y.resize(_m_nrY);
    _m_Yii_size_Y.resize(_m_nrY);
    _m_ii_size_Y.resize(_m_nrY);

    for(Index yI=0; yI < _m_nrY; yI++)
    {
        _m_nrVals_XSoI_Y.at(yI).resize(GetXSoI_Y(yI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            nrValsPerSF, GetXSoI_Y(yI), _m_nrVals_XSoI_Y.at(yI));

        size_t Xii_size = 1;
        for(Index i=0; i < _m_nrVals_XSoI_Y.at(yI).size(); i++) 
            Xii_size *= _m_nrVals_XSoI_Y.at(yI).at(i);
        _m_Xii_size_Y.at(yI) = Xii_size;

        _m_nrVals_ASoI_Y.at(yI).resize(GetASoI_Y(yI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            _m_madp->GetNrActions(), GetASoI_Y(yI), _m_nrVals_ASoI_Y.at(yI));

        size_t Aii_size = 1;
        for(Index i=0; i < _m_nrVals_ASoI_Y.at(yI).size(); i++) 
            Aii_size *= _m_nrVals_ASoI_Y.at(yI).at(i);
        _m_Aii_size_Y.at(yI) = Aii_size;

        _m_nrVals_YSoI_Y.at(yI).resize(GetYSoI_Y(yI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            nrValsPerSF, GetYSoI_Y(yI), _m_nrVals_YSoI_Y.at(yI) );
        size_t Yii_size = 1;
        for(Index i=0; i < _m_nrVals_YSoI_Y.at(yI).size(); i++) 
            Yii_size *= _m_nrVals_YSoI_Y.at(yI).at(i);
        _m_Yii_size_Y.at(yI) = Yii_size;

        _m_ii_size_Y.at(yI) = Xii_size * Aii_size * Yii_size;

        vector< size_t >::iterator pos, it1, it2;
        pos = _m_nrVals_SoI_Y.at(yI).end();
        it1 = _m_nrVals_XSoI_Y.at(yI).begin();
        it2 = _m_nrVals_XSoI_Y.at(yI).end();
        _m_nrVals_SoI_Y.at(yI).insert(  pos, it1, it2 );
        pos = _m_nrVals_SoI_Y.at(yI).end();
        it1 = _m_nrVals_ASoI_Y.at(yI).begin();
        it2 = _m_nrVals_ASoI_Y.at(yI).end();
        _m_nrVals_SoI_Y.at(yI).insert(  pos, it1, it2 );
        pos = _m_nrVals_SoI_Y.at(yI).end();
        it1 = _m_nrVals_YSoI_Y.at(yI).begin();
        it2 = _m_nrVals_YSoI_Y.at(yI).end();
        _m_nrVals_SoI_Y.at(yI).insert(  pos, it1, it2 );



    }

    //initialize meta data for Os
    for(size_t i = 0; i < _m_nrVals_XSoI_O.size(); i++)
      _m_nrVals_XSoI_O.at(i).clear();
    _m_nrVals_XSoI_O.clear();
    for(size_t i = 0; i < _m_nrVals_ASoI_O.size(); i++)
      _m_nrVals_ASoI_O.at(i).clear();
    _m_nrVals_ASoI_O.clear();
    for(size_t i = 0; i < _m_nrVals_YSoI_O.size(); i++)
      _m_nrVals_YSoI_O.at(i).clear();
    _m_nrVals_YSoI_O.clear();
    for(size_t i = 0; i < _m_nrVals_OSoI_O.size(); i++)
      _m_nrVals_OSoI_O.at(i).clear();
    _m_nrVals_OSoI_O.clear();
    for(size_t i = 0; i < _m_nrVals_SoI_O.size(); i++)
      _m_nrVals_SoI_O.at(i).clear();
    _m_nrVals_SoI_O.clear();
    _m_nrVals_XSoI_O.resize(_m_nrO);
    _m_nrVals_ASoI_O.resize(_m_nrO);
    _m_nrVals_YSoI_O.resize(_m_nrO);
    _m_nrVals_OSoI_O.resize(_m_nrO);
    _m_nrVals_SoI_O.resize(_m_nrO);

    _m_Xii_size_O.clear();
    _m_Aii_size_O.clear();
    _m_Yii_size_O.clear();
    _m_Oii_size_O.clear();
    _m_ii_size_O.clear();
    _m_Xii_size_O.resize(_m_nrO);
    _m_Aii_size_O.resize(_m_nrO);
    _m_Yii_size_O.resize(_m_nrO);
    _m_Oii_size_O.resize(_m_nrO);
    _m_ii_size_O.resize(_m_nrO);

    for(Index oI=0; oI < _m_nrO; oI++)
    {
        _m_nrVals_XSoI_O.at(oI).resize(GetXSoI_O(oI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            nrValsPerSF, GetXSoI_O(oI), _m_nrVals_XSoI_O.at(oI));

        size_t Xii_size = 1;
        for(Index i=0; i < _m_nrVals_XSoI_O.at(oI).size(); i++) 
            Xii_size *= _m_nrVals_XSoI_O.at(oI).at(i);
        _m_Xii_size_O.at(oI) = Xii_size;

        _m_nrVals_ASoI_O.at(oI).resize(GetASoI_O(oI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            _m_madp->GetNrActions(), GetASoI_O(oI), _m_nrVals_ASoI_O.at(oI));

        size_t Aii_size = 1;
        for(Index i=0; i < _m_nrVals_ASoI_O.at(oI).size(); i++) 
            Aii_size *= _m_nrVals_ASoI_O.at(oI).at(i);
        _m_Aii_size_O.at(oI) = Aii_size;

        _m_nrVals_YSoI_O.at(oI).resize(GetYSoI_O(oI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            nrValsPerSF, GetYSoI_O(oI), _m_nrVals_YSoI_O.at(oI));

        size_t Yii_size = 1;
        for(Index i=0; i < _m_nrVals_YSoI_O.at(oI).size(); i++) 
            Yii_size *= _m_nrVals_YSoI_O.at(oI).at(i);
        _m_Yii_size_O.at(oI) = Yii_size;

        _m_nrVals_OSoI_O.at(oI).resize(GetOSoI_O(oI).size());
        IndexTools::RestrictIndividualIndicesToScope(
            _m_madp->GetNrObservations(), GetOSoI_O(oI), _m_nrVals_OSoI_O.at(oI));

        size_t Oii_size = 1;
        for(Index i=0; i < _m_nrVals_OSoI_O.at(oI).size(); i++) 
            Oii_size *= _m_nrVals_OSoI_O.at(oI).at(i);
        _m_Oii_size_O.at(oI) = Oii_size;

        _m_ii_size_O.at(oI) = Oii_size * Xii_size * Aii_size * Yii_size;

        vector< size_t >::iterator pos, it1, it2;
        pos = _m_nrVals_SoI_O.at(oI).end();
        it1 = _m_nrVals_XSoI_O.at(oI).begin();
        it2 = _m_nrVals_XSoI_O.at(oI).end();
        _m_nrVals_SoI_O.at(oI).insert(  pos, it1, it2 );
        pos = _m_nrVals_SoI_O.at(oI).end();
        it1 = _m_nrVals_ASoI_O.at(oI).begin();
        it2 = _m_nrVals_ASoI_O.at(oI).end();
        _m_nrVals_SoI_O.at(oI).insert(  pos, it1, it2 );
        pos = _m_nrVals_SoI_O.at(oI).end();
        it1 = _m_nrVals_YSoI_O.at(oI).begin();
        it2 = _m_nrVals_YSoI_O.at(oI).end();
        _m_nrVals_SoI_O.at(oI).insert(  pos, it1, it2 );
        pos = _m_nrVals_SoI_O.at(oI).end();
        it1 = _m_nrVals_OSoI_O.at(oI).begin();
        it2 = _m_nrVals_OSoI_O.at(oI).end();
        _m_nrVals_SoI_O.at(oI).insert(  pos, it1, it2 );
    }

    // initialize some memory and variables used to speed up index
    // conversion functions
    _m_IndividualToJointYiiIndices_catVector=new vector<Index>(_m_nrVals_SoI_Y.size());
    _m_IndividualToJointOiiIndices_catVector=new vector<Index>(_m_nrVals_SoI_O.size());

    _m_nrVals_SoI_Y_stepsize.resize(_m_nrY);
    for(Index yI=0; yI < _m_nrY; yI++)
        _m_nrVals_SoI_Y_stepsize[yI]=
            IndexTools::CalculateStepSize(_m_nrVals_SoI_Y[yI]);

    for(size_t i = 0; i < _m_X_restr_perY.size(); i++)
      delete(_m_X_restr_perY[i]);
    _m_X_restr_perY.clear();
    for(size_t i = 0; i < _m_A_restr_perY.size(); i++)
      delete(_m_A_restr_perY[i]);
    _m_A_restr_perY.clear();
    for(size_t i = 0; i < _m_Y_restr_perY.size(); i++)
      delete(_m_Y_restr_perY[i]);
    _m_Y_restr_perY.clear();
    for(Index y=0; y < _m_nrY; y++)
    {
        _m_X_restr_perY.push_back(new vector<Index>(GetXSoI_Y(y).size()));
        _m_A_restr_perY.push_back(new vector<Index>(GetASoI_Y(y).size()));
        _m_Y_restr_perY.push_back(new vector<Index>(GetYSoI_Y(y).size()));
    }

    _m_nrVals_SoI_O_stepsize.resize(_m_nrO);
    
    for(Index oI=0; oI < _m_nrO; oI++)
        _m_nrVals_SoI_O_stepsize[oI]=
            IndexTools::CalculateStepSize(_m_nrVals_SoI_O[oI]);

    for(size_t i = 0; i < _m_X_restr_perO.size(); i++)
      delete(_m_X_restr_perO[i]);
    _m_X_restr_perO.clear();
    for(size_t i = 0; i < _m_A_restr_perO.size(); i++)
      delete(_m_A_restr_perO[i]);
    _m_A_restr_perO.clear();
    for(size_t i = 0; i < _m_Y_restr_perO.size(); i++)
      delete(_m_Y_restr_perO[i]);
    _m_Y_restr_perO.clear();    
    for(size_t i = 0; i < _m_O_restr_perO.size(); i++)
      delete(_m_O_restr_perO[i]);
    _m_O_restr_perO.clear();
    for(Index o=0; o < _m_nrO; o++)
    {
        _m_X_restr_perO.push_back(new vector<Index>(GetXSoI_O(o).size()));
        _m_A_restr_perO.push_back(new vector<Index>(GetASoI_O(o).size()));
        _m_Y_restr_perO.push_back(new vector<Index>(GetYSoI_O(o).size()));
        _m_O_restr_perO.push_back(new vector<Index>(GetOSoI_O(o).size()));
    }

    _m_ii_initialized = true;
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_XSoI_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_XSoI_Y[yI];
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_ASoI_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_ASoI_Y[yI];
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_YSoI_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_YSoI_Y[yI];
}

size_t TwoStageDynamicBayesianNetwork::GetXiiSize_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Xii_size_Y[yI];
}

size_t TwoStageDynamicBayesianNetwork::GetAiiSize_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Aii_size_Y[yI];
}

size_t TwoStageDynamicBayesianNetwork::GetYiiSize_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Yii_size_Y[yI];
}

size_t TwoStageDynamicBayesianNetwork::GetiiSize_Y(Index yI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_ii_size_Y[yI];
}

//functions for _O

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_OSoI_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_OSoI_O[oI];
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_XSoI_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_XSoI_O[oI];
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_ASoI_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_ASoI_O[oI];
}

const vector<size_t>& TwoStageDynamicBayesianNetwork::
GetNrVals_YSoI_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_nrVals_YSoI_O[oI];
}

size_t TwoStageDynamicBayesianNetwork::GetOiiSize_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Oii_size_O[oI];
}

size_t TwoStageDynamicBayesianNetwork::GetXiiSize_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Xii_size_O[oI];
}

size_t TwoStageDynamicBayesianNetwork::GetAiiSize_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Aii_size_O[oI];
}

size_t TwoStageDynamicBayesianNetwork::GetYiiSize_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_Yii_size_O[oI];
}

size_t TwoStageDynamicBayesianNetwork::GetiiSize_O(Index oI) const
{
    if(!_m_ii_initialized)
        throw E("ii (influence instantiation) meta-info is not initialized");
    return _m_ii_size_O[oI];
}





void TwoStageDynamicBayesianNetwork::
JointToIndividualYiiIndices(Index y, Index iiI, 
        vector<Index>& Xs, 
        vector<Index>& As, 
        vector<Index>& Ys) const
{
    vector<Index> catVector = IndexTools::JointToIndividualIndices(iiI,
            _m_nrVals_SoI_Y[y]);

    size_t Xsize = _m_nrVals_XSoI_Y[y].size();
    size_t Asize = _m_nrVals_ASoI_Y[y].size();
    size_t Ysize = _m_nrVals_YSoI_Y[y].size();
    vector<Index>::iterator it1 = catVector.begin();
    vector<Index>::iterator it2 = it1 + Xsize;
    Xs.assign(it1, it2);
    it1 = it2;
    it2 += Asize;
    As.assign(it1, it2);
    it1 = it2;
    it2 += Ysize;
    if(it2 != catVector.end())
        cerr << "JointToIndividualYiiIndices (it2 != catVector.end() - check this code!"<<endl;
    Ys.assign(it1, it2);
    
}

Index TwoStageDynamicBayesianNetwork::
IndividualToJointYiiIndices(Index y, 
        const vector<Index>& Xs, 
        const vector<Index>& As, 
        const vector<Index>& Ys) const
{
    _m_IndividualToJointYiiIndices_catVector->clear();
    _m_IndividualToJointYiiIndices_catVector->insert(
        _m_IndividualToJointYiiIndices_catVector->end(), Xs.begin(), Xs.end() );
    _m_IndividualToJointYiiIndices_catVector->insert(
        _m_IndividualToJointYiiIndices_catVector->end(), As.begin(), As.end() );
    _m_IndividualToJointYiiIndices_catVector->insert(
        _m_IndividualToJointYiiIndices_catVector->end(), Ys.begin(), Ys.end() );
        
    Index iiI = IndexTools::IndividualToJointIndicesStepSize(
        *_m_IndividualToJointYiiIndices_catVector,
        _m_nrVals_SoI_Y_stepsize[y] );

#if 0    
    size_t iiS = GetiiSize_Y(y);
    if( iiI >= iiS)
    {
        cerr << "error in index computation, let's see what happened"<<endl;
        iiI = IndexTools::IndividualToJointIndices(catVector,
            _m_nrVals_SoI_Y[y] );
    }
#endif
    return iiI;

}


void TwoStageDynamicBayesianNetwork::
JointToIndividualOiiIndices(Index o, Index iiI, 
        vector<Index>& As, 
        vector<Index>& Ys, 
        vector<Index>& Os) const
{
    if(GetXSoI_O(o).size() > 0)
    {
      stringstream errormsg;
      errormsg << "Observation Factor " << o << " has a non-null PS SF scope, which was ignored.";
      throw E(errormsg.str());      
    }
    vector<Index> catVector = IndexTools::JointToIndividualIndices(iiI,
            _m_nrVals_SoI_O[o]);

    size_t Asize = _m_nrVals_ASoI_O[o].size();
    size_t Ysize = _m_nrVals_YSoI_O[o].size();
    size_t Osize = _m_nrVals_OSoI_O[o].size();
    vector<Index>::iterator it1 = catVector.begin();
    vector<Index>::iterator it2 = it1 + Asize;
    As.assign(it1, it2);
    it1 = it2;
    it2 += Ysize;
    Ys.assign(it1, it2);
    it1 = it2;
    it2 += Osize;
    if(it2 != catVector.end())
        cerr << "JointToIndividualYiiIndices (it2 != catVector.end() - check this code!"<<endl;
    Os.assign(it1, it2);    
}

void TwoStageDynamicBayesianNetwork::
JointToIndividualOiiIndices(Index o, Index iiI, 
        vector<Index>& Xs, 
        vector<Index>& As, 
        vector<Index>& Ys, 
        vector<Index>& Os) const
{
    vector<Index> catVector = IndexTools::JointToIndividualIndices(iiI,
            _m_nrVals_SoI_O[o]);

    size_t Xsize = _m_nrVals_XSoI_O[o].size();
    size_t Asize = _m_nrVals_ASoI_O[o].size();
    size_t Ysize = _m_nrVals_YSoI_O[o].size();
    size_t Osize = _m_nrVals_OSoI_O[o].size();
    vector<Index>::iterator it1 = catVector.begin();
    vector<Index>::iterator it2 = it1 + Xsize;
    Xs.assign(it1, it2);
    it1 = it2;
    it2 += Asize;
    As.assign(it1, it2);
    it1 = it2;
    it2 += Ysize;
    Ys.assign(it1, it2);
    it1 = it2;
    it2 += Osize;
    if(it2 != catVector.end())
        cerr << "JointToIndividualYiiIndices (it2 != catVector.end() - check this code!"<<endl;
    Os.assign(it1, it2);    
}

Index TwoStageDynamicBayesianNetwork::
IndividualToJointOiiIndices(Index o, 
        const vector<Index>& As, 
        const vector<Index>& Ys, 
        const vector<Index>& Os) const
{
    _m_IndividualToJointOiiIndices_catVector->clear();
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), As.begin(), As.end() );
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), Ys.begin(), Ys.end() );
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), Os.begin(), Os.end() );

    Index iiI = IndexTools::IndividualToJointIndicesStepSize(
        *_m_IndividualToJointOiiIndices_catVector,
        _m_nrVals_SoI_O_stepsize[o] );
    return iiI;

}

Index TwoStageDynamicBayesianNetwork::
IndividualToJointOiiIndices(Index o, 
        const vector<Index>& Xs, 
        const vector<Index>& As, 
        const vector<Index>& Ys, 
        const vector<Index>& Os) const
{
    _m_IndividualToJointOiiIndices_catVector->clear();
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), Xs.begin(), Xs.end() );
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), As.begin(), As.end() );
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), Ys.begin(), Ys.end() );
    _m_IndividualToJointOiiIndices_catVector->insert(
        _m_IndividualToJointOiiIndices_catVector->end(), Os.begin(), Os.end() );

    Index iiI = IndexTools::IndividualToJointIndicesStepSize(
        *_m_IndividualToJointOiiIndices_catVector,
        _m_nrVals_SoI_O_stepsize[o] );
    return iiI;

}

void TwoStageDynamicBayesianNetwork::
AddCPDForY(Index y)
{
    //get SoI of y
    //compute #local states
    //allocate a CPD
    throw E("AddCPDForY NYI");
}

void TwoStageDynamicBayesianNetwork::
AddCPDForO(Index o)
{
    //get SoI of o
    //compute #local states
    //allocate a CPD
    throw E("AddCPDForO NYI");
}


string TwoStageDynamicBayesianNetwork::SoftPrint() const
{
    string indent("\t");
    stringstream ss;
    ss << indent << "TwoStageDynamicBayesianNetwork::SoftPrint()" <<endl;
    ss << indent << "connections as specified by Scopes of Influence (SoI):" <<endl;
    if(!_m_SoIStorageInitialized)
        ss << indent << "storage not yet initialized!"<< endl;
    else
    {
        for(Index y=0; y < _m_madp->GetNrStateFactors(); y++)
            ss << indent << SoftPrintSoI_Y(y) << endl;
        for(Index agI=0; agI < _m_madp->GetNrAgents(); agI++)
            ss << indent << SoftPrintSoI_O(agI) << endl;
        ss << indent << "Probabilities: (not yet implemented)" << endl;
    }
    return (ss.str());
}

string TwoStageDynamicBayesianNetwork::SoftPrintSoI_Y(Index y) const
{
    stringstream ss;
    ss << "sfI=" << y <<
        ", XSoI=" << _m_XSoI_Y.at(y) <<
        ", ASoI=" << _m_ASoI_Y.at(y) <<
        ", YSoI=" << _m_YSoI_Y.at(y);
    return(ss.str());
}
string TwoStageDynamicBayesianNetwork::SoftPrintSoI_O(Index agI) const
{
    stringstream ss;
    ss << "agI=" << agI <<
        ", XSoI_O=" << _m_XSoI_O.at(agI) << 
        ", ASoI_O=" << _m_ASoI_O.at(agI) << 
        ", YSoI_O=" << _m_YSoI_O.at(agI) << 
        ", OSoI_O=" << _m_OSoI_O.at(agI);
    return(ss.str());
}


