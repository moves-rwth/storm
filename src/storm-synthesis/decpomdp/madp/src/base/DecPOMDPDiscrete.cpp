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

#include "DecPOMDPDiscrete.h"    
#include "RGet.h"
#include "RewardModelMapping.h"
#include "RewardModelMappingSparse.h"

using namespace std;

#define DEBUG_DPOMDPD 0
//Debug Fill DecPOMDP Discrete - functions to initialize the DecPOMDPDiscrete
//(typically from a file)
#define DEBUG_FILLDPD 0

DecPOMDPDiscrete::DecPOMDPDiscrete(const string &name,
                                   const string &descr,
                                   const string &pf) :
    MultiAgentDecisionProcessDiscrete(name, descr, pf)
{
    _m_initialized = false;
    _m_p_rModel = 0;
}

DecPOMDPDiscrete::~DecPOMDPDiscrete()
{
    if(DEBUG_DPOMDPD)
        cout << "deleting DecPOMDPDiscrete (deleting rewards)"<<endl;
    delete(_m_p_rModel);
}

bool DecPOMDPDiscrete::SetInitialized(bool b)
{
    if( MultiAgentDecisionProcessDiscrete::SetInitialized(true) )
    {
        _m_initialized = b;
        return(true);
    }
    else
        return(false);
}

void DecPOMDPDiscrete::CreateNewRewardModel()
{
    if(_m_initialized)
    delete(_m_p_rModel);

    if(GetSparse())
        _m_p_rModel = new RewardModelMappingSparse( GetNrStates(), GetNrJointActions());
    else
        _m_p_rModel = new RewardModelMapping( GetNrStates(), GetNrJointActions());
}

RGet* DecPOMDPDiscrete::GetRGet() const
{ 
    return new RGet_RewardModelMapping(
        ((RewardModelMapping*)_m_p_rModel)  );
}

string DecPOMDPDiscrete::SoftPrint() const
{
    stringstream ss;
    ss << MultiAgentDecisionProcessDiscrete::SoftPrint();
    ss << DecPOMDP::SoftPrint();

    if(_m_initialized)
    {
        ss << "Reward model: " << endl;
        ss << _m_p_rModel->SoftPrint();
    }
    else
        throw E("DecPOMDPDiscrete components (reward model) not initialized");

    return(ss.str());
}

void DecPOMDPDiscrete::SetReward(Index sI, Index jaI, Index sucSI, double r)
{
    //this adds the fraction for this s' to R(s,a)= \sum_s P(s'|s,a) R(s,a,s')
    double rOld=GetReward(sI,jaI),
        rExp=GetTransitionProbability(sI,jaI,sucSI)*r;
    SetReward(sI,jaI,rOld+rExp);
}

void DecPOMDPDiscrete::SetReward(Index sI, Index jaI, Index sucSI,
                                 Index joI, double r)
{
    //this adds the fraction for this s', o to R(s,a)= \sum_s' \sum_o P(s',o|s,a) R(s,a,s',o)
    double rOld=GetReward(sI,jaI),
        rExp=GetObservationProbability(sI, jaI, sucSI, joI) * GetTransitionProbability(sI,jaI,sucSI) * r;
    SetReward(sI,jaI,rOld+rExp);
}

void
DecPOMDPDiscrete::ExtractMADPDiscrete(MultiAgentDecisionProcessDiscrete *madp)
{
    madp->SetNrAgents(GetNrAgents());
    madp->SetName(GetName());
    madp->SetDescription(GetDescription());

    // transition model
    madp->SetTransitionModelPtr(
        const_cast<TransitionModelDiscrete*>(GetTransitionModelDiscretePtr()));

    // observation model
    madp->SetObservationModelPtr(
        const_cast<ObservationModelDiscrete*>(GetObservationModelDiscretePtr()));

    // MADPComponentDiscreteStates
    for(Index s=0;s!=GetNrStates();++s)
        madp->AddState(GetState(s)->GetName());

    madp->SetISD(GetISD());

    // MADPComponentDiscreteObservations
    for(Index id=0;id!=GetNrAgents();++id)
        for(Index o=0;o!=GetNrObservations(id);++o)
            madp->AddObservation(id,GetObservation(id,o)->GetName());
    madp->ConstructJointObservations();

    // MADPComponentDiscreteActions
    for(Index id=0;id!=GetNrAgents();++id)
        for(Index o=0;o!=GetNrActions(id);++o)
            madp->AddAction(id,GetAction(id,o)->GetName());
    madp->ConstructJointActions();

    madp->Initialize();
}


void DecPOMDPDiscrete::CompareModels(
        const DecPOMDPDiscreteInterface& d1, 
        const DecPOMDPDiscreteInterface& d2)
{

    //MultiAgentDecisionProcessInterface level
    //size_t 	GetNrAgents ()
    size_t nrAg = d1.GetNrAgents();
    if(nrAg != d2.GetNrAgents())
    {
        cout << "number of agents not the same, stopping comparison"<<endl;
        return;
    }
    
    //MultiAgentDecisionProcessDiscreteInterface level
    //size_t 	GetNrActions (Index AgentI)
    //size_t 	GetNrJointActions ()
    //size_t 	GetNrJointObservations ()
    //size_t 	GetNrObservations (Index AgentI)
    //size_t 	GetNrStates ()
    //double 	GetInitialStateProbability (Index sI)
    //double 	GetTransitionProbability (Index sI, Index jaI, Index sucSI)
    //double 	GetObservationProbability (Index jaI, Index sucSI, Index joI)
    size_t nrJA = d1.GetNrJointActions();
    if(nrJA != d2.GetNrJointActions())
    {
        cout << "number of joint actions not the same, stopping comparison"
            <<endl;
        return;
    }
    for( Index agI=0; agI < nrAg; agI++)
        if( d1.GetNrActions(agI) != d2.GetNrActions(agI) )
        {
            cout << "number of actions not the same, stopping comparison"
                <<endl;
            return;
        }
    size_t nrJO = d1.GetNrJointObservations();
    if(nrJO != d2.GetNrJointObservations())
    {
        cout << "number of joint observations not the same, stopping comparison"
            <<endl;
        return;
    }
    for( Index agI=0; agI < nrAg; agI++)
        if( d1.GetNrObservations(agI) != d2.GetNrObservations(agI) )
        {
            cout << "number of observations not the same, stopping comparison"
                <<endl;
            return;
        }
    size_t nrS = d1.GetNrStates();
    if(d2.GetNrStates() != nrS)
    {
        cout << "number of states not the same, stopping comparison"
            <<endl;
        return;
    }
    cout.precision(8);
    for( Index sI=0; sI < nrS; sI++)
    {
        double p1 = d1.GetInitialStateProbability(sI);
        double p2 = d2.GetInitialStateProbability(sI);
        if(! Globals::EqualProbability(p1, p2) )
        {
            cout << "Initial probability of sI="<<sI<<" not equal: " <<
                "p1="<<p1<<", p2="<<p2<<endl;
        }
    }

    for( Index s1=0; s1 < nrS; s1++)
        for( Index ja=0; ja < nrJA; ja++)
            for( Index s2=0; s2 < nrS; s2++)
            {
                double p1 = d1.GetTransitionProbability(s1,ja,s2);
                double p2 = d2.GetTransitionProbability(s1,ja,s2);
                if(! Globals::EqualProbability(p1, p2) )
                {

                    cout << "T("<<s1<<","<<ja<<","<<s2<<") not equal: " <<
                        "p1="<<p1<<", p2="<<p2<<endl;
                }
            }
    for( Index s1=0; s1 < nrS; s1++)
    {
        for( Index jo=0; jo < nrJO; jo++)
            for( Index ja=0; ja < nrJA; ja++)
                for( Index s2=0; s2 < nrS; s2++)
                {
                    double p1 = d1.GetObservationProbability(s1,ja,s2,jo);
                    double p2 = d2.GetObservationProbability(s1,ja,s2,jo);
                    if(! Globals::EqualProbability(p1, p2) )
                    {
                        cout << "O("<<ja<<","<<s2<<","<<jo<<") not equal: " <<
                        "p1="<<p1<<", p2="<<p2<<endl;
                    }
                }
    }
    //DecPOMDPDiscreteInterface level
    //double 	GetReward (Index sI, Index jaI)
    //
    for( Index s1=0; s1 < nrS; s1++)
        for( Index ja=0; ja < nrJA; ja++)
        {
            double p1 = d1.GetReward(s1,ja);
            double p2 = d2.GetReward(s1,ja);
            if(! Globals::EqualProbability(p1, p2) )
            {
                cout << "R("<<s1<<","<<ja<<") not equal: " <<
                    "r1="<<p1<<", r2="<<p2<<endl;
            }
        }
    //DecPOMDPInterface level
    //double 	GetDiscount () const
    //reward_t 	GetRewardType () 
    //
    //POSGDiscreteInterface
    //nothing (reward functions, but that's already taken care of at
    //the DecPOMDPDiscreteInterface level)
    //
    //POSGInterface 
    //nothing
    //
    

}
