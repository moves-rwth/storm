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

#include "MultiAgentDecisionProcessDiscrete.h"    
#include "ObservationModelMapping.h"
#include "ObservationModelMappingSparse.h"
#include "EventObservationModelMapping.h"
#include "EventObservationModelMappingSparse.h"
#include "TransitionModelMapping.h"    
#include "TransitionModelMappingSparse.h"    
#include <stdio.h>

#include "TGet.h"
#include "OGet.h"


using namespace std;

#define DEBUG_MADPD 0

MultiAgentDecisionProcessDiscrete::MultiAgentDecisionProcessDiscrete( 
    const string &name, const string &descr, const string &pf) :
    MultiAgentDecisionProcessDiscreteInterface(),
    MultiAgentDecisionProcess(name, descr, pf)
{
    _m_initialized = false;
    _m_sparse = false;
    _m_eventObservability = false;
    _m_p_tModel = 0;
    _m_p_oModel = 0;
    _m_falseNegativeObs = -1;
}

MultiAgentDecisionProcessDiscrete::MultiAgentDecisionProcessDiscrete(
    size_t nrAgents, size_t nrS,
    const string &name, const string &descr, const string &pf) :
    MultiAgentDecisionProcessDiscreteInterface(),
    MultiAgentDecisionProcess(nrAgents,name, descr, pf),
    _m_S(nrS)
{
    _m_initialized = false;
    _m_sparse = false;
    _m_eventObservability = false;
    _m_p_tModel = 0;
    _m_p_oModel = 0;
    _m_falseNegativeObs = -1;
}

MultiAgentDecisionProcessDiscrete::MultiAgentDecisionProcessDiscrete(
    const MultiAgentDecisionProcessDiscrete& a) :
    MultiAgentDecisionProcessDiscreteInterface(a),
    MultiAgentDecisionProcess(a)
{
    _m_S=a._m_S;
    _m_A=a._m_A;
    _m_O=a._m_O;
    _m_initialized=a._m_initialized;
    _m_sparse=a._m_sparse;
    _m_eventObservability=a._m_eventObservability;
    _m_p_tModel=a._m_p_tModel->Clone();
    _m_p_oModel=a._m_p_oModel->Clone();
    _m_falseNegativeObs = a._m_falseNegativeObs;
}

MultiAgentDecisionProcessDiscrete::~MultiAgentDecisionProcessDiscrete()
{
    if(DEBUG_MADPD)
        cout << "deleting MultiAgentDecisionProcessDiscrete "
             << "(deleting T and O model )"<<endl;
    delete (_m_p_tModel);
    delete (_m_p_oModel);
}

void MultiAgentDecisionProcessDiscrete::CreateNewTransitionModel()
{
    if(_m_initialized)
        delete(_m_p_tModel);

    if(_m_sparse)
        _m_p_tModel=new TransitionModelMappingSparse(GetNrStates(),
                                                     GetNrJointActions());
    else
        _m_p_tModel=new TransitionModelMapping(GetNrStates(),
                                               GetNrJointActions());

}

TGet* MultiAgentDecisionProcessDiscrete::GetTGet() const
{ 
    if(_m_sparse)
        return new TGet_TransitionModelMappingSparse(
                ((TransitionModelMappingSparse*)_m_p_tModel)  ); 
    else
        return new TGet_TransitionModelMapping(
                ((TransitionModelMapping*)_m_p_tModel)  );
}

OGet* MultiAgentDecisionProcessDiscrete::GetOGet() const
{ 
    if(!_m_eventObservability) //default
    {
        if(_m_sparse)
            return new OGet_ObservationModelMappingSparse(
                    ((ObservationModelMappingSparse*)_m_p_oModel)  ); 
        else
            return new OGet_ObservationModelMapping(
                    ((ObservationModelMapping*)_m_p_oModel)  );
    }
    else
    {
        if(_m_sparse)
            return new OGet_EventObservationModelMappingSparse(
                    ((EventObservationModelMappingSparse*)_m_p_oModel)  ); 
        else
            return new OGet_EventObservationModelMapping(
                    ((EventObservationModelMapping*)_m_p_oModel)  );
    }    
}

void MultiAgentDecisionProcessDiscrete::CreateNewObservationModel()
{
    if(_m_initialized)
        delete(_m_p_oModel);
    if(!_m_eventObservability)
    {
        if(_m_sparse)
            _m_p_oModel = new 
                ObservationModelMappingSparse(GetNrStates(), 
                                              GetNrJointActions(),
                                              GetNrJointObservations());
        else 
            _m_p_oModel = new 
                ObservationModelMapping(GetNrStates(),
                                        GetNrJointActions(), 
                                        GetNrJointObservations());
    }
    else
    {
        if(_m_sparse)
            _m_p_oModel = new 
                EventObservationModelMappingSparse(GetNrStates(), 
                                                   GetNrJointActions(),
                                                   GetNrJointObservations());
        else
            _m_p_oModel = new 
                EventObservationModelMapping(GetNrStates(), 
                                             GetNrJointActions(),
                                             GetNrJointObservations());
    }
}


bool MultiAgentDecisionProcessDiscrete::SetInitialized(bool b)
{

    if(b == true)
    {
        if(     !_m_A.SetInitialized(b)
            ||  !_m_O.SetInitialized(b)
            ||  !_m_S.SetInitialized(b) )
        {
            //error in initialization of sub-components.
            _m_initialized = false;
            return(false);
        }
        //check if transition- and observation model are present...
        if(_m_p_tModel == 0)
        {
            throw E("MultiAgentDecisionProcessDiscrete::SetInitialized() -initializing a MultiAgentDecisionProcessDiscrete which has no transition model! - make sure that CreateNewTransitionModel() has been called before SetInitialized()");
        }
        if(_m_p_oModel == 0)
        {
            throw E("MultiAgentDecisionProcessDiscrete::SetInitialized() -initializing a MultiAgentDecisionProcessDiscrete which has no observation model! - make sure that CreateNewObservationModel() has been called before SetInitialized()");

        }

        if( SanityCheck() )
        {
            _m_initialized = true;
            return(true);
        }
        else
        {
            _m_initialized = false;
            return(false);
        }
    }
    else
    {
        _m_A.SetInitialized(b);
        _m_O.SetInitialized(b);
        _m_S.SetInitialized(b); 
        _m_initialized = false;
        return(true);
    }

}


string MultiAgentDecisionProcessDiscrete::SoftPrint() const
{
    stringstream ss;

    ss << MultiAgentDecisionProcess::SoftPrint();
    ss << _m_S.SoftPrint();
    ss << _m_A.SoftPrint();
    ss << _m_O.SoftPrint();   
   
    if(_m_initialized)
    {
        ss << "Transition model: " << endl;
        ss << _m_p_tModel->SoftPrint();
        ss << "Observation model: " << endl;
        ss << _m_p_oModel->SoftPrint();
    }
    return(ss.str());
}

bool MultiAgentDecisionProcessDiscrete::SanityCheck()
{
    size_t nrJA=GetNrJointActions(),
          nrS=GetNrStates(),
          nrJO=GetNrJointObservations();

    double sum,p;
    bool sane=true;

    // check transition model
    for(Index a=0;a<nrJA;a++)
    {
        for(Index from=0;from<nrS;from++)
        {
            sum=0.0;
            for(Index to=0;to<nrS;to++)
            {
                p=GetTransitionProbability(from,a,to);
                if(p<0)
                {
                    sane=false;
                    stringstream ss;
                    ss << "MultiAgentDecisionProcessDiscrete::SanityCheck "<<
                        "failed: negative probability " << p << 
                        " for p(s'|s,a)==p(" << _m_S.GetStateName(to) << "|" << 
                        _m_S.GetStateName(from)<<"," << _m_A.GetJointActionName(a) << ")";
                    throw E(ss);
                }
                if(std::isnan(p))
                {
                    sane=false;
                    stringstream ss;
                    ss << "MultiAgentDecisionProcessDiscrete::SanityCheck "<<
                        "failed: NaN " << 
                        " for p(s'|s,a)==p(" << _m_S.GetStateName(to) << "|" << 
                        _m_S.GetStateName(from)<<"," << _m_A.GetJointActionName(a) << ")";
                    throw E(ss);
                }
                sum+=p;
            }
            if((sum>(1.0 + PROB_PRECISION/2)) ||
               (sum < (1.0 - PROB_PRECISION/2)))
            {
                sane=false;
                stringstream ss;
                //string float_str;
                char float_str[30];
                sprintf(float_str, "%10.10f", sum);
                ss << "MultiAgentDecisionProcessDiscrete::SanityCheck failed:"<<
                   " transition does not sum to 1 but to:\n" << float_str << 
                   "\n for (s,a)==(" << _m_S.GetStateName(from) << "[" << from << 
                   "]," << _m_A.GetJointActionName(a) << "[" << a << "])";
                throw E(ss);
            }
        }
    }

    // check observation model

    for(Index from=0;from<nrS;from++)
    {
      for(Index a=0;a<nrJA;a++)
      {
        for(Index to=0;to<nrS;to++)
        {
            sum=0;
            for(Index o=0;o<nrJO;o++)
            {
	        if(_m_eventObservability)
	            p=GetObservationProbability(from,a,to,o);
		else
		    p=GetObservationProbability(a,to,o);
                if(p<0)
                {
                    stringstream ss;
                    ss << "MultiAgentDecisionProcessDiscrete::SanityCheck "
                       << "failed: negative probability " << p 
                       << " for p(o|s',a)==p(" 
                       << _m_O.GetJointObservationName(o)
                       << "|" << _m_S.GetStateName(to) 
                       << "," 
                       << _m_A.GetJointActionName(a) 
                       << ")";
                    throw E(ss);
                }
                if(std::isnan(p))
                {
                    stringstream ss;
                    ss << "MultiAgentDecisionProcessDiscrete::SanityCheck "
                       << "failed: NaN for p(o|s',a)==p(" 
                       << _m_O.GetJointObservationName(o)
                       << "|" << _m_S.GetStateName(to) 
                       << "," 
                       << _m_A.GetJointActionName(a) 
                       << ")";
                    throw E(ss);
                }
                sum+=p;
            }
            if((sum>(1.0 + PROB_PRECISION/2)) ||
               (sum < (1.0 - PROB_PRECISION/2)))
            {
                char float_str[30];
                sprintf(float_str, "%10.10f", sum);
                sane=false;
                stringstream ss;
                ss << "MultiAgentDecisionProcessDiscrete::SanityCheck "
                   << "failed: observation does not sum to 1 but to \n" 
                   << float_str << "\n for (s',a)==(" << _m_S.GetStateName(to) 
                   << "[" << to << "],"
                   << _m_A.GetJointActionName(a) 
                   << "[" << a << "])";
                throw E(ss);
            }
        }
      }
      if(!_m_eventObservability) break;
    }

    return(sane);
}


void MultiAgentDecisionProcessDiscrete::SetSparse(bool sparse)
{
    _m_sparse=sparse;
}

void MultiAgentDecisionProcessDiscrete::SetEventObservability(bool eventO)
{
    _m_eventObservability=eventO;
}
