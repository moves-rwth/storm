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

#include "ParserPOMDPDiscrete.h"
#include <fstream>

#if USE_POMDPSOLVE_LIBRARY
extern "C" {
#include "mdp/mdp.h"
}
#endif

using namespace std;

ParserPOMDPDiscrete::
ParserPOMDPDiscrete(POMDPDiscrete *problem) :
    _m_problem(problem)
{
}

void ParserPOMDPDiscrete::Parse()
{
#if USE_POMDPSOLVE_LIBRARY
    string filename=_m_problem->GetProblemFile();

    readMDP(const_cast<char*>(filename.c_str()));

    _m_problem->SetNrAgents(1);
    _m_problem->SetNrStates(gNumStates);
    _m_problem->SetDiscount(gDiscount);
    if(gValueType==REWARD_value_type)
        _m_problem->SetRewardType(REWARD);
    else if(gValueType==COST_value_type)
        _m_problem->SetRewardType(COST);

    _m_problem->SetNrActions(0,gNumActions);
    _m_problem->ConstructJointActions();
    _m_problem->SetActionsInitialized(true);

    _m_problem->SetNrObservations(0,gNumObservations);
    _m_problem->ConstructJointObservations();
    _m_problem->SetObservationsInitialized(true);

    vector<double> isdVector(_m_problem->GetNrStates());
    for(Index s=0;s!=_m_problem->GetNrStates();++s)
        isdVector.at(s)=gInitialBelief[s];

    StateDistribution* isd=
        new StateDistributionVector(isdVector);
    _m_problem->SetISD(isd);

    _m_problem->CreateNewTransitionModel();
    for(Index a=0;a!=_m_problem->GetNrActions();++a)
        for(Index s=0;s!=_m_problem->GetNrStates();++s)
            for(Index s1=0;s1!=_m_problem->GetNrStates();++s1)
            {
                // this isn't the fastest way, since it does a lookup
                // in the sparse matrix, but at least we don't have to
                // mess with the internals
                double p=getEntryMatrix(P[a],s,s1);
                _m_problem->SetTransitionProbability(s,a,s1,p);
            }

    _m_problem->CreateNewObservationModel();
    for(Index a=0;a!=_m_problem->GetNrActions();++a)
        for(Index s=0;s!=_m_problem->GetNrStates();++s)
            for(Index o=0;o!=_m_problem->GetNrObservations();++o)
            {
                // this isn't the fastest way, since it does a lookup
                // in the sparse matrix, but at least we don't have to
                // mess with the internals
                double p=getEntryMatrix(R[a],s,o);
                _m_problem->SetObservationProbability(a,s,o,p);
            }


    _m_problem->CreateNewRewardModel();
    for(Index a=0;a!=_m_problem->GetNrActions();++a)
        for( Index i = Q->row_start[a]; 
             i < Q->row_start[a] +  Q->row_length[a];
             i++ ) 
            _m_problem->SetReward(Q->col[i],a,Q->mat_val[i]);



    _m_problem->SetInitialized(true);
#else
    throw(E("ParserPOMDPDiscrete needs to be compiled with USE_POMDPSOLVE_LIBRARY"));
#endif
}
