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

#include "TOIDecPOMDPDiscrete.h"
#include "RewardModelMappingSparse.h"
#include "RewardModelMapping.h"

using namespace std;

TOIDecPOMDPDiscrete::
TOIDecPOMDPDiscrete(
    const string &name, const string &descr, const string &pf, 
    bool cacheFlatModels) :
    TransitionObservationIndependentMADPDiscrete(name, descr, pf, 
                                                 cacheFlatModels)
{
    _m_initialized = false;
    _m_p_rModel = 0;
}

TOIDecPOMDPDiscrete::TOIDecPOMDPDiscrete(const TOIDecPOMDPDiscrete& o) 
{
    throw(E("TOIDecPOMDPDiscrete: copy ctor not yet implemented"));
}
//Destructor
TOIDecPOMDPDiscrete::~TOIDecPOMDPDiscrete()
{
    delete(_m_p_rModel);
}
//Copy assignment operator
TOIDecPOMDPDiscrete& TOIDecPOMDPDiscrete::operator= (const TOIDecPOMDPDiscrete& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...

    throw(E("TOIDecPOMDPDiscrete: assignment not yet implemented"));

    return *this;
}

void TOIDecPOMDPDiscrete::SetIndividualDecPOMDPD(DecPOMDPDiscrete *model,
                                                 Index agentI)
{
    if(_m_individualDecPOMDPDs.size()<=agentI)
        _m_individualDecPOMDPDs.resize(agentI+1);

    _m_individualDecPOMDPDs[agentI]=model;
}

bool TOIDecPOMDPDiscrete::SetInitialized(bool b)
{
    if(b == false)
    {
        _m_initialized = false;
        return(true);
    }

    if( TransitionObservationIndependentMADPDiscrete::SetInitialized(true) 
        )
    {
        if( b == true )
        {
            if (_m_p_rModel == 0)
                throw E("TOIDecPOMDPDiscrete::SetInitialized(true) : no reward model specified yet! ( _m_p_rModel == 0 )");

        }
        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}

void TOIDecPOMDPDiscrete::CreateNewRewardModel()
{
    if(_m_initialized)
    delete(_m_p_rModel);
#if 0
    // cannot call GetNrJointStates() and GetNrJointActions() because
    // we're not initialized yet
    size_t nrJS=1, nrJA=1;
    for(Index i=0;i!=GetNrAgents();++i)
    {
        nrJS*=GetIndividualMADPD(i)->GetNrStates();
        nrJA*=GetIndividualMADPD(i)->GetNrJointActions();
    }

    if(GetSparse())
        _m_p_rModel = new RewardModelMappingSparse(nrJS, 
                                                   nrJA);
    else
        _m_p_rModel = new RewardModelMapping(nrJS, 
                                             nrJA);
#else
    _m_p_rModel = new RewardModelTOISparse();
#endif
}

string TOIDecPOMDPDiscrete::SoftPrint() const
{
    stringstream ss;
    ss << TransitionObservationIndependentMADPDiscrete::SoftPrint();
    ss << DecPOMDP::SoftPrint();

    if(_m_initialized)
    {
        ss << "Reward model: " << endl;
        ss << _m_p_rModel->SoftPrint();
    }
    else
        throw E("TOIDecPOMDPDiscrete components (reward model) not initialized");

    return(ss.str());
}
