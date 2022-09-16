/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Philipp Robbel 
 *
 * For contact information please see the included AUTHORS file.
 */

#include "FactoredMMDPDiscrete.h"
#include "CPDKroneckerDelta.h"

using namespace std;

string FactoredMMDPDiscrete::SoftPrint() const {
    stringstream ss;
    ss << "Fully-observable MMDP" << endl;
    ss << FactoredDecPOMDPDiscrete::SoftPrint();
    return ss.str();
}

// Don't support flattening of observations by default
void FactoredMMDPDiscrete::CacheFlatModels(bool sparse) {
#if MADP_DFS_WARNINGS
    cout << "FactoredMMDPDiscrete::CacheFlatModels() does not flatten observation model" << endl;
#endif                 
    ConstructJointActions();
  
    CacheFlatTransitionModel(sparse);
    CacheFlatRewardModel(sparse);
}

/// Initialize a fully-observable transition and observation DBN.
void FactoredMMDPDiscrete::Initialize2DBN()
{
    // construct observations
    ConstructObservations();
    SetObservationsInitialized(true); // Note: joint indices are likely to break

    BoundScopeFunctor<FactoredMMDPDiscrete> sf(this,&FactoredMMDPDiscrete::SetScopes);
    BoundTransitionProbFunctor<FactoredMMDPDiscrete> tf(this,&FactoredMMDPDiscrete::ComputeTransitionProb);
    EmptyObservationProbFunctor of;
    
    MultiAgentDecisionProcessDiscreteFactoredStates::Initialize2DBN(sf, tf, of); 

    // above calls SetOScopes and initializes CPD vector for observation variables
    Initialize2DBNObservations(); // set actual CPDs
}

void FactoredMMDPDiscrete::ConstructObservations() {
    size_t nrAgents = GetNrAgents();
    size_t nrStateFactors = GetNrStateFactors();
    if(nrAgents == 0 || nrStateFactors == 0)
        throw(E("FactoredMMDPDiscrete::ConstructObservations() no agents specified or state space empty"));

    size_t nrStates = GetNrStates();
    for(Index i=0; i<nrAgents; i++) {
        // create nameless observations for this agent
        SetNrObservations(i, nrStates);
    }
}

void FactoredMMDPDiscrete::SetOScopes() {
    size_t nrAgents = GetNrAgents();
    size_t nrStateFactors = GetNrStateFactors();
    if(nrAgents == 0 || nrStateFactors == 0)
        throw(E("FactoredMMDPDiscrete::SetOScopes() no agents specified or state space empty"));

    const Scope& asfS = GetAllStateFactorScope();
    for(Index oI=0; oI<nrAgents; oI++) {
        SetSoI_O( oI, Scope("<>"), asfS, Scope("<>") );
    }
}

void FactoredMMDPDiscrete::Initialize2DBNObservations() {
    size_t nrAgents = GetNrAgents();
    if(nrAgents == 0)
        throw(E("FactoredMMDPDiscrete::Initialize2DBNObservations() no agents specified"));
    
    for(Index i=0; i<nrAgents; i++) {
        // attach identity function to 2BDN (fully-observable scenario)
        Get2DBN()->SetCPD_O(i, new CPDKroneckerDelta());
    }
}
