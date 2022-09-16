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

#include "TOIDecMDPDiscrete.h"

using namespace std;

//Default constructor
TOIDecMDPDiscrete::TOIDecMDPDiscrete(
    const string &name, const string &descr, const string &pf,
    bool cacheFlatModels) :
    TOIDecPOMDPDiscrete(name, descr, pf, cacheFlatModels)
{
    _m_initialized = false;
}

//Destructor
TOIDecMDPDiscrete::~TOIDecMDPDiscrete()
{
}

bool TOIDecMDPDiscrete::SetInitialized(bool b)
{
    if( TOIDecPOMDPDiscrete::SetInitialized(true) )
    {
        if( b == true )
        {
            //\todo NOTE: this does not check that the Dec-MDP is indeed jointly (and thus locally) observable
        }
        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}
    
void TOIDecMDPDiscrete::CreateStateObservations()
{
    for(Index agI=0; agI < GetNrAgents(); agI++)
    {
        size_t nrStatesAgent = GetNrStates(agI);
        size_t nrActionsAgent = GetNrActions(agI);
        SetNrObservations(agI, nrStatesAgent);
        MultiAgentDecisionProcessDiscrete* ind_madp = GetIndividualMADPD(agI);
        ind_madp->CreateNewObservationModel();

        for(Index sI=0; sI < nrStatesAgent; sI++)
            for(Index aI=0; aI < nrActionsAgent; aI++)
                ind_madp->SetObservationProbability(aI, sI, sI, 1.0);
    }
}
