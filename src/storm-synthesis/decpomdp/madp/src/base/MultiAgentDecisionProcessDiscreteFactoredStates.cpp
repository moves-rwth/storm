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

#include "MultiAgentDecisionProcessDiscreteFactoredStates.h"
#include "TransitionModelMappingSparse.h"
#include "TransitionModelMapping.h"
#include "ObservationModelMappingSparse.h"
#include "ObservationModelMapping.h"
#include "EventObservationModelMappingSparse.h"
#include "EventObservationModelMapping.h"
#include "TGet.h"
#include "OGet.h"
#include "VectorTools.h"
#include "CPT.h"
#include "StateFactorDiscrete.h"

using namespace std;

#define DEBUG_2DBN 0
#define DEBUG_DOSANITYCHECK 0
#define SKIP_IMPOSSIBLE_EVENTS 0

//Default constructor
MultiAgentDecisionProcessDiscreteFactoredStates::
MultiAgentDecisionProcessDiscreteFactoredStates(
    const string &name, const string &descr, const string &pf) :
    MultiAgentDecisionProcess(name, descr, pf)
    ,_m_p_tModel(0)
    ,_m_p_oModel(0)
    ,_m_cached_FlatTM(false)
    ,_m_sparse_FlatTM(false)
    ,_m_cached_FlatOM(false)
    ,_m_sparse_FlatOM(false)
    ,_m_eventObservability(false)
    ,_m_2dbn(*this)
{
}
//Copy constructor.    
MultiAgentDecisionProcessDiscreteFactoredStates::MultiAgentDecisionProcessDiscreteFactoredStates(const MultiAgentDecisionProcessDiscreteFactoredStates& o) 
    :
        _m_2dbn(o._m_2dbn)
{
}
//Destructor
MultiAgentDecisionProcessDiscreteFactoredStates::~MultiAgentDecisionProcessDiscreteFactoredStates()
{
    delete _m_p_tModel;
    delete _m_p_oModel;
}
//Copy assignment operator
MultiAgentDecisionProcessDiscreteFactoredStates& MultiAgentDecisionProcessDiscreteFactoredStates::operator= (const MultiAgentDecisionProcessDiscreteFactoredStates& o)
{
    if (this == &o) return *this;   // Gracefully handle self assignment
    // Put the normal assignment duties here...
    _m_2dbn = o._m_2dbn;
    throw(E("MultiAgentDecisionProcessDiscreteFactoredStates: ctor not yet implemented"));
    return *this;
}

string MultiAgentDecisionProcessDiscreteFactoredStates::SoftPrint() const
{
    stringstream ss;

    ss << MultiAgentDecisionProcess::SoftPrint();
    ss << _m_S.SoftPrint();
    ss << _m_A.SoftPrint();
    ss << _m_O.SoftPrint();   
    ss << _m_2dbn.SoftPrint();
    if(_m_initialized)
    {
        ss << "Transition model: " << 
            "(not yet implemented)"
            << endl;
        //ss << _m_p_tModel->SoftPrint();
        ss << "Observation model: " << 
            "(not yet implemented)"
            << endl;
        //ss << _m_p_oModel->SoftPrint();
    }
    return(ss.str());
}

bool MultiAgentDecisionProcessDiscreteFactoredStates::SetInitialized(bool b)
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
        ////check if transition- and observation model are present...
        //if(_m_p_tModel == 0)
        //{
            //throw E("MultiAgentDecisionProcessDiscrete::SetInitialized() -initializing a MultiAgentDecisionProcessDiscrete which has no transition model! - make sure that CreateNewObservationModel() has been called before SetInitialized()");
        //}
        //if(_m_p_oModel == 0)
        //{
            //throw E("MultiAgentDecisionProcessDiscrete::SetInitialized() -initializing a MultiAgentDecisionProcessDiscrete which has no observation model! - make sure that CreateNewObservationModel() has been called before SetInitialized()");

        //}

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

bool MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheckTransitions(void) const
{
    // mainly a copy from
    // MultiAgentDecisionProcessDiscrete::SanityCheck(), should
    // perhaps be more customized for the factored version

    bool sane=true;

#if DEBUG_DOSANITYCHECK
    size_t nrJA=GetNrJointActions(),
            nrS=GetNrStates();

    double sum,p;

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
                    ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck "<<
                        "failed: negative probability " << p << 
                        " for p(s'|s,a)==p("
                       << SoftPrintVector(StateIndexToFactorValueIndices(to))
                       << "|" << 
                        SoftPrintVector(StateIndexToFactorValueIndices(from)) <<"," << a << ")";
                    throw E(ss);
                }
                if(std::isnan(p))
                {
                    sane=false;
                    stringstream ss;
                    ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck "<<
                        "failed: NaN " << 
                        " for p(s'|s,a)==p(" 
                       << SoftPrintVector(StateIndexToFactorValueIndices(to))
                       << "|" 
                       << SoftPrintVector(StateIndexToFactorValueIndices(from))
                       <<","
                       << SoftPrintVector(JointToIndividualActionIndices(a))
                       << ")";
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
                ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck failed:"<<
                    " transition does not sum to 1 but to:\n" << float_str << 
                    "\n for (s,a)==(" << SoftPrintVector(StateIndexToFactorValueIndices(from)) << "," << SoftPrintVector(JointToIndividualActionIndices(a)) << ")";
                throw E(ss);
            }
        }
    }
#else
#if MADP_DFS_WARNINGS
    cout << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheckTransitions() not implemented, no check performed" << endl;
#endif                 
#endif
    return(sane);
}

bool MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheckObservations(void) const
{
    bool sane=true;

#if DEBUG_DOSANITYCHECK
    size_t nrJA=GetNrJointActions(),
          nrS=GetNrStates(),
          nrJO=GetNrJointObservations();

    double sum,p;

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
                    p=GetObservationProbability(from,a,to,o);
                    if(p<0)
                    {
                        stringstream ss;
                        ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck "
                           << "failed: negative probability " << p 
                           << " for p(o|s',a)==p(" 
                           << o
                           << "|" << SoftPrintVector(StateIndexToFactorValueIndices(to)) 
                           << "," 
                           << SoftPrintVector(JointToIndividualActionIndices(a)) 
                           << ")";
                        throw E(ss);
                    }
                    if(std::isnan(p))
                    {
                        stringstream ss;
                        ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck "
                           << "failed: NaN for p(o|s',a)==p(" 
                           << o
                           << "|" << SoftPrintVector(StateIndexToFactorValueIndices(to)) 
                           << "," 
                           << SoftPrintVector(JointToIndividualActionIndices(a)) 
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
                    ss << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheck "
                       << "failed: observation does not sum to 1 but to \n" 
                       << float_str << "\n for (s',a)==("
                       << SoftPrintVector(StateIndexToFactorValueIndices(to)) 
                       << ","
                       << SoftPrintVector(JointToIndividualActionIndices(a)) 
                       << ")";
                    throw E(ss);
                }
            }
        }
    }
#else
#if MADP_DFS_WARNINGS
    cout << "MultiAgentDecisionProcessDiscreteFactoredStates::SanityCheckObservations() not implemented, no check performed" << endl;
#endif                 
#endif
    return(sane);
}

double MultiAgentDecisionProcessDiscreteFactoredStates::
GetTransitionProbability (Index sI, Index jaI, Index sucSI) const
{
    if(_m_cached_FlatTM)
        return _m_p_tModel->Get(sI, jaI, sucSI);

    vector<Index> X = StateIndexToFactorValueIndices(sI);
    vector<Index> Y = StateIndexToFactorValueIndices(sucSI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    return(_m_2dbn.GetYProbability(X,A,Y));

}

TGet* MultiAgentDecisionProcessDiscreteFactoredStates::
GetTGet() const
{    
    if(!_m_cached_FlatTM)
        return 0;

    if(_m_sparse_FlatTM)
        return new TGet_TransitionModelMappingSparse(
                ((TransitionModelMappingSparse*)_m_p_tModel)  ); 
    else
        return new TGet_TransitionModelMapping(
                ((TransitionModelMapping*)_m_p_tModel)  );


}

double MultiAgentDecisionProcessDiscreteFactoredStates::
GetObservationProbability  (Index jaI, Index sucSI, Index joI) const
{
    if(_m_cached_FlatOM)
        return _m_p_oModel->Get(jaI, sucSI, joI);
    vector<Index> O = JointToIndividualObservationIndices(joI);
    vector<Index> Y = StateIndexToFactorValueIndices(sucSI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    return(_m_2dbn.GetOProbability(A,Y,O));    
}

double MultiAgentDecisionProcessDiscreteFactoredStates::
GetObservationProbability  (Index sI, Index jaI, Index sucSI, Index joI) const
{
    if(_m_cached_FlatOM)
        return _m_p_oModel->Get(sI, jaI, sucSI, joI);
    vector<Index> X = StateIndexToFactorValueIndices(sI);
    vector<Index> O = JointToIndividualObservationIndices(joI);
    vector<Index> Y = StateIndexToFactorValueIndices(sucSI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    return(_m_2dbn.GetOProbability(X,A,Y,O));
}

// hack: base class shouldn't need to know about
// derived observation models
OGet* MultiAgentDecisionProcessDiscreteFactoredStates::
GetOGet() const
{    
    if(!_m_cached_FlatOM)
        return 0;

    if(!_m_eventObservability) //default
    {
        if(_m_sparse_FlatOM)
            return new OGet_ObservationModelMappingSparse(
                    ((ObservationModelMappingSparse*)_m_p_oModel)  ); 
        else
            return new OGet_ObservationModelMapping(
                    ((ObservationModelMapping*)_m_p_oModel)  );
    }
    else
    {
        if(_m_sparse_FlatOM)
            return new OGet_EventObservationModelMappingSparse(
                    ((EventObservationModelMappingSparse*)_m_p_oModel)  ); 
        else
            return new OGet_EventObservationModelMapping(
                    ((EventObservationModelMapping*)_m_p_oModel)  );
    }
}

Index MultiAgentDecisionProcessDiscreteFactoredStates::
SampleSuccessorState (Index sI, Index jaI) const
{
    if(_m_cached_FlatTM && _m_eventObservability)
        return _m_p_tModel->SampleSuccessorState(sI, jaI);

    vector<Index> X = StateIndexToFactorValueIndices(sI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    vector<Index> Y;
    SampleSuccessorState(X,A,Y);
    return( FactorValueIndicesToStateIndex(Y) );
}

void  MultiAgentDecisionProcessDiscreteFactoredStates::
SampleSuccessorState(const std::vector<Index> &sIs,
                     const std::vector<Index> &aIs,
                     std::vector<Index> &sucIs) const
{
    sucIs=_m_2dbn.SampleY(sIs,aIs);
}

Index MultiAgentDecisionProcessDiscreteFactoredStates::
SampleJointObservation(Index jaI, Index sucSI) const
{
    if(_m_cached_FlatOM && _m_eventObservability)
      return _m_p_oModel->SampleJointObservation(jaI, sucSI);

    vector<Index> X;
    vector<Index> Y = StateIndexToFactorValueIndices(sucSI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    vector<Index> O;
    SampleJointObservation(X,A,Y,O);
    return( IndividualToJointObservationIndices(O) );
}

Index MultiAgentDecisionProcessDiscreteFactoredStates::
SampleJointObservation(Index sI, Index jaI, Index sucSI) const
{
    if(_m_cached_FlatOM && _m_eventObservability)
      return _m_p_oModel->SampleJointObservation(sI, jaI, sucSI);

    vector<Index> X = StateIndexToFactorValueIndices(sI);
    vector<Index> Y = StateIndexToFactorValueIndices(sucSI);
    vector<Index> A = JointToIndividualActionIndices(jaI);
    vector<Index> O;
    SampleJointObservation(X,A,Y,O);
    return( IndividualToJointObservationIndices(O) );
}

void MultiAgentDecisionProcessDiscreteFactoredStates::
SampleJointObservation(const std::vector<Index> &sIs,
                       const std::vector<Index> &aIs,
                       const std::vector<Index> &sucIs,
                       std::vector<Index> &oIs) const
{
    oIs=_m_2dbn.SampleO(sIs, aIs,sucIs);
}

void MultiAgentDecisionProcessDiscreteFactoredStates::
CreateNewTransitionModel()
{
    if(!_m_connectionsSpecified)
        throw E("CreateNewTransitionModel connections are not yet specified");

    for(Index y=0; y < GetNrStateFactors(); y++)
        _m_2dbn.AddCPDForY(y);
}

void MultiAgentDecisionProcessDiscreteFactoredStates::
CreateNewObservationModel()
{
    if(!_m_connectionsSpecified)
        throw E("CreateNewTransitionModel connections are not yet specified");
    
    for(Index o=0; o < GetNrAgents(); o++)
        _m_2dbn.AddCPDForO(o);

}

///Get the number of joint instantiations for the factors in sfScope
size_t MultiAgentDecisionProcessDiscreteFactoredStates::GetNrStateFactorInstantiations(const Scope& sfScope) const
{
    if(sfScope.size()>0)
    {
        const vector<size_t>& nrSFvals = GetNrValuesPerFactor();
        vector<size_t> restr_nrSFvals(sfScope.size());
        IndexTools::RestrictIndividualIndicesToScope( nrSFvals, sfScope, restr_nrSFvals );
        size_t restr_nrJSFvals = VectorTools::VectorProduct(restr_nrSFvals);
        return restr_nrJSFvals;
    }
    else
        return(0);
}


void MultiAgentDecisionProcessDiscreteFactoredStates::CacheFlatTransitionModel(bool sparse)
{
    if(!JointAIndicesValid())
        throw EOverflow("MultiAgentDecisionProcessDiscreteFactoredStates::CacheFlatTransitionModel() joint action indices are not available, overflow detected");

    if(_m_cached_FlatTM)
        delete(_m_p_tModel);

    _m_sparse_FlatTM = sparse;
    if(sparse)
        _m_p_tModel=new TransitionModelMappingSparse(GetNrStates(),
                                                     GetNrJointActions());
    else
        _m_p_tModel=new TransitionModelMapping(GetNrStates(),
                                               GetNrJointActions());


    for(Index sI=0; sI<GetNrStates(); sI++)
        for(Index jaI=0; jaI<GetNrJointActions(); jaI++)
            for(Index sucsI=0; sucsI<GetNrStates(); sucsI++)
            {
                double p = GetTransitionProbability(sI, jaI, sucsI);
                if(! Globals::EqualProbability(p, 0) )                
                    _m_p_tModel->Set(sI, jaI, sucsI, p);
            }

    _m_cached_FlatTM = true;
}

void MultiAgentDecisionProcessDiscreteFactoredStates::CacheFlatObservationModel(bool sparse)
{
    if(!JointIndicesValid())
        throw EOverflow("MultiAgentDecisionProcessDiscreteFactoredStates::CacheFlatObservationModel() joint action and/or observation indices are not available, overflow detected");

    if(_m_cached_FlatOM)
        delete(_m_p_oModel);

    _m_sparse_FlatOM = sparse;
    if(!_m_eventObservability)
    {
        if(sparse)
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
        if(sparse)
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
    
    for(Index sI=0; sI<GetNrStates(); sI++)
    {
        for(Index jaI=0; jaI<GetNrJointActions(); jaI++)
            for(Index sucsI=0; sucsI<GetNrStates(); sucsI++)
                for(Index joI=0; joI<GetNrJointObservations(); joI++)
                {
                    if(_m_eventObservability)
                    {
                        double p = GetObservationProbability(sI, jaI, sucsI, joI);
                        if(! Globals::EqualProbability(p, 0) )                
                            _m_p_oModel->Set(sI, jaI, sucsI, joI, p);
                    }
                    else
                    {
                        double p = GetObservationProbability(jaI, sucsI, joI);
                        if(! Globals::EqualProbability(p, 0) )                
                            _m_p_oModel->Set(jaI, sucsI, joI, p);
                    }
                }
        if(!_m_eventObservability)
            break;
    }
    _m_cached_FlatOM = true;
}

void MultiAgentDecisionProcessDiscreteFactoredStates::Initialize2DBN()
{
    BoundScopeFunctor<MultiAgentDecisionProcessDiscreteFactoredStates> sf(this, &MultiAgentDecisionProcessDiscreteFactoredStates::SetScopes);
    BoundTransitionProbFunctor<MultiAgentDecisionProcessDiscreteFactoredStates> tf(this, &MultiAgentDecisionProcessDiscreteFactoredStates::ComputeTransitionProb);
    BoundObservationProbFunctor<MultiAgentDecisionProcessDiscreteFactoredStates> of(this, &MultiAgentDecisionProcessDiscreteFactoredStates::ComputeObservationProb);
    Initialize2DBN(sf,tf,of);
}

void MultiAgentDecisionProcessDiscreteFactoredStates::Initialize2DBN(ScopeFunctor& SetScopes,
                                                                     TransitionProbFunctor& ComputeTransitionProb,
                                                                     ObservationProbFunctor& ComputeObservationProb)
{
//Initialize storage in the 2DBN
    _m_2dbn.InitializeStorage(); 
#if DEBUG_2DBN
    cout << "MultiAgentDecisionProcessDiscreteFactoredStates: 2DBN storage initialized"<<endl;
    cout << _m_2dbn.SoftPrint();
#endif
    SetScopes();
    SetConnectionsSpecified(true);
#if DEBUG_2DBN
    cout << "MultiAgentDecisionProcessDiscreteFactoredStates::scopes set and thus all connections specified"<<endl;
    cout << _m_2dbn.SoftPrint();
#endif

//initialize II meta-information
    _m_2dbn.InitializeIIs();

    const vector<size_t>& nryVals = GetNrValuesPerFactor();
//Create Transition model
#if DEBUG_2DBN
    if(DEBUG_2DBN) cout << ">>>Adding Transition model..."<<endl;
#endif
    //For each state factor, we create the CPT here
    for(Index y=0; y< GetNrStateFactors(); y++)
    {
#if DEBUG_2DBN
        cout << "Creating CPT for sfI=" << GetStateFactorDiscrete(y)->GetName() << endl;
#endif
        //1)determine the SoI(y)
        //2)allocate the CPT
        //3)fill it up
        //4)attach it to 2BDN
        
        //determine the SoI(y)
        size_t nrVals_y = nryVals.at(y);
        const Scope& XSoI_y = _m_2dbn.GetXSoI_Y(y);
        const Scope& ASoI_y = _m_2dbn.GetASoI_Y(y);
        const Scope& YSoI_y = _m_2dbn.GetYSoI_Y(y);
        size_t XSoI_y_size = XSoI_y.size();
        size_t ASoI_y_size = ASoI_y.size();
        size_t YSoI_y_size = YSoI_y.size();
        
        vector<size_t> r_nrX = _m_2dbn.GetNrVals_XSoI_Y(y);
        vector<size_t> r_nrA = _m_2dbn.GetNrVals_ASoI_Y(y);
        vector<size_t> r_nrY = _m_2dbn.GetNrVals_YSoI_Y(y);

        size_t ii_size = _m_2dbn.GetiiSize_Y(y);

        //2)allocate the CPT
        CPT* cpt = new CPT(nrVals_y, ii_size);

        //3)fill it up
        vector<Index> Xs(XSoI_y_size, 0 );
        vector<Index> As(ASoI_y_size, 0 );
        vector<Index> Ys(YSoI_y_size, 0 );
        size_t ii_size2=0;
#if DEBUG_2DBN
        cout << "starting loop through Xs of size " << XSoI_y_size << endl;
#endif
        do{
#if DEBUG_2DBN
          //            cout << SoftPrintVector(Xs) << " - PS state is " <<
          //      SoftPrintPartialState(XSoI_y, Xs) << endl;
#endif
            do{
#if DEBUG_2DBN
                cout << "\ta="<< SoftPrintVector(As) << " (scope="<<
                    ASoI_y <<")" << endl;
#endif
                do{
#if DEBUG_2DBN
                    cout << "\t\t";// << SoftPrintVector(Ys);
#endif
                    ii_size2++;
                    for(Index yVal=0; yVal < nrVals_y; yVal++)
                    {
                        //compute P(y = yVal | ii=<Xs, As, Ys>) :
                        double p = ComputeTransitionProb(y, yVal, Xs,As,Ys);
                        
                        if(p > Globals::PROB_PRECISION)
                        {
#if DEBUG_2DBN
                            cout << "P("<<GetStateFactorDiscrete(y)->GetName()<<":"<<GetStateFactorDiscrete(y)->GetStateFactorValue(yVal)<<"|...)=";
                            printf("%.3f, ", p);
#endif
                            Index iiI = _m_2dbn.IndividualToJointYiiIndices(
                                y, Xs, As, Ys);
                            cpt->Set(yVal, iiI, p);
                        }
                    }
#if DEBUG_2DBN
                    cout << endl;
#endif

                } while(! IndexTools::Increment( Ys, r_nrY ) ); 

            } while(! IndexTools::Increment( As, r_nrA ) ); 

        } while(! IndexTools::Increment( Xs, r_nrX ) ); 
        //cout << "ii_size:" << ii_size << ", ii_size2:" << ii_size2 << endl;
        if (ii_size != ii_size2 )
            throw E("ii_size != ii_size2 ");

        //4)attach it to 2BDN
        _m_2dbn.SetCPD_Y(y, cpt);
    }//end (for y)

    //Only continue if an initialization function for observation probabilities is given
    if(!ComputeObservationProb.isEmpty())
    {
        const vector<size_t>& nroVals = GetNrObservations();
//Create the observation model    
#if DEBUG_2DBN
        cout << ">>>Adding Observation model..."<<endl;
#endif
        //For each observation, we create the CPT here
        for(Index o=0; o < GetNrAgents(); o++)
        {
#if DEBUG_2DBN
            cout << "Creating CPT for oI (i.e., agentI)=" << o << endl;
#endif
            //1)determine the SoI(o)
            //2)allocate the CPT
            //3)fill it up
            //4)attach it to 2BDN
        
            //determine the SoI(o)
            size_t nrVals_o = nroVals.at(o);
            const Scope& OSoI_o = _m_2dbn.GetOSoI_O(o);
            const Scope& ASoI_o = _m_2dbn.GetASoI_O(o);
            const Scope& YSoI_o = _m_2dbn.GetYSoI_O(o);
            const Scope& XSoI_o = _m_2dbn.GetXSoI_O(o);
            size_t OSoI_o_size = OSoI_o.size();
            size_t ASoI_o_size = ASoI_o.size();
            size_t YSoI_o_size = YSoI_o.size();
            size_t XSoI_o_size = XSoI_o.size();
            
            SetEventObservability(GetEventObservability() || XSoI_o_size > 0); //if true, we use O(o,s,a,s') instead of O(o,a,s')
            
            vector<size_t> r_nrO = _m_2dbn.GetNrVals_OSoI_O(o);
            vector<size_t> r_nrA = _m_2dbn.GetNrVals_ASoI_O(o);
            vector<size_t> r_nrY = _m_2dbn.GetNrVals_YSoI_O(o);
            vector<size_t> r_nrX = _m_2dbn.GetNrVals_XSoI_O(o);
            
            size_t ii_size = _m_2dbn.GetiiSize_O(o);
            
            //2)allocate the CPT
            CPT* cpt = new CPT(nrVals_o, ii_size);
            
            //3)fill it up
            vector<Index> Os(OSoI_o_size, 0 );
            vector<Index> As(ASoI_o_size, 0 );
            vector<Index> Ys(YSoI_o_size, 0 );
            vector<Index> Xs(XSoI_o_size, 0 );
            size_t ii_size2=0;
#if DEBUG_2DBN
            cout << "starting loop through Os of size " << OSoI_o_size << endl;
#endif
            do{
                do{
#if DEBUG_2DBN
                    cout << SoftPrintVector(As)<<endl;
#endif
                    do{
                        bool skipY = false;
#if SKIP_IMPOSSIBLE_EVENTS
                        if(_m_eventObservability){
                          for(Index i = 0; i < YSoI_o_size; i++){
                              const Scope& XSoI_y = _m_2dbn.GetXSoI_Y(YSoI_o[i]);
                              const Scope& YSoI_y = _m_2dbn.GetYSoI_Y(YSoI_o[i]);
                              const Scope& ASoI_y = _m_2dbn.GetASoI_Y(YSoI_o[i]);
                              Scope Y;
                              if(XSoI_y.IsSubSetOf(XSoI_o) &&
                                 YSoI_y.IsSubSetOf(YSoI_o) &&
                                 ASoI_y.IsSubSetOf(ASoI_o)){ //I have enough information to know if these transitions are possible
                                Y.Insert(YSoI_o[i]);
                                vector<Index> yVal(1,Ys[YSoI_o[i]]);
                                if(_m_2dbn.GetYProbabilityGeneral(XSoI_o,Xs,
                                                                  ASoI_o,As,
                                                                  YSoI_o,Ys,
                                                                  Y,yVal) <= Globals::PROB_PRECISION){
                                  skipY = true;
                                  break;
                                }                              
                              }
                          }
                        }
#endif
#if DEBUG_2DBN
                  //                    cout << "\t" << SoftPrintVector(Ys) << " - NS state is " <<
                  //        SoftPrintPartialState(YSoI_o, Ys) << endl;
#endif
                        do{
#if DEBUG_2DBN
                          cout << "\t\t";// << SoftPrintVector(Os) //<-empty
#endif
                          ii_size2++;
                          for(Index oVal=0; oVal < nrVals_o; oVal++)
                            {
                              //compute P(o=oVal | ii=<Os, [Xs], As, Ys>) :
                              double p;
                              if(!skipY){
                                p = ComputeObservationProb(o, oVal, Xs, As, Ys, Os);
                              }else{
                                p = 1.0/(float) nrVals_o;
                              }
#if DEBUG_2DBN
                              cout << "P(o="<<oVal<<"|...)=";
                              printf("%.3f, ", p);
#endif
                              Index iiI = _m_2dbn.IndividualToJointOiiIndices(o, Xs, As, Ys, Os);
                              if(p > Globals::PROB_PRECISION)
                                cpt->Set(oVal, iiI, p);
                            }
#if DEBUG_2DBN
                          cout << endl;
#endif
                        } while(! IndexTools::Increment( Os, r_nrO ) ); 
                   
                    } while(! IndexTools::Increment( Ys, r_nrY ) ); 
                
                } while(_m_eventObservability && !IndexTools::Increment( Xs, r_nrX ) ); 
           
            } while(! IndexTools::Increment( As, r_nrA ) ); 
                
            //cout << "ii_size:" << ii_size << ", ii_size2:" << ii_size2 << endl;
            if (ii_size != ii_size2 )
                throw E("ii_size != ii_size2 ");
            
            //4)attach it to 2BDN
            _m_2dbn.SetCPD_O(o, cpt);
        }//end (for o)

    } else {
#if DEBUG_2DBN
        cout << ">>>Skipping addition of Observation model CPTs." << endl;
#endif
    }
    
    SetInitialized(true);
}

void MultiAgentDecisionProcessDiscreteFactoredStates::MarginalizeTransitionObservationModel(const Index sf, bool sparse)
{
    const Scope& YSoI_sf = _m_2dbn.GetYSoI_Y(sf);
  
    if(!YSoI_sf.empty()){
      throw E("Cannot marginalize a state factor with NS dependencies. NYI.");
    }

    cout << "Marginalizing State Factor " << sf << endl;

    size_t nrS = 1;
    vector<size_t> new_factor_sizes;
    for(size_t i = 0; i < GetNrStateFactors(); i++){
        if(i == sf)
            continue;
        size_t nrX = GetNrValuesForFactor(i);
        nrS *= nrX;
        new_factor_sizes.push_back(nrX);
    }
    vector<size_t> input_factor_sizes = GetNrValuesPerFactor();
    input_factor_sizes[sf] = 1;
    vector<size_t> output_factor_sizes = GetNrValuesPerFactor();
    const vector<size_t>& actions = GetNrActions();

    if(!_m_cached_FlatTM)
        ConstructJointActions();
    if(!_m_cached_FlatOM)
        ConstructJointObservations();
    
    TransitionModelDiscrete* marginalized_tm;
    ObservationModelDiscrete* marginalized_om;

    _m_sparse_FlatTM = sparse;
    _m_sparse_FlatOM = sparse;

    if(_m_eventObservability)
    {
        if(_m_sparse_FlatTM)
            marginalized_tm = new 
                TransitionModelMappingSparse(nrS,
                                             GetNrJointActions());
        else
            marginalized_tm = new 
                TransitionModelMapping(nrS,
                                       GetNrJointActions());
        if(_m_sparse_FlatOM)
            marginalized_om = new 
                EventObservationModelMappingSparse(nrS,
                                                   GetNrJointActions(), 
                                                   GetNrJointObservations());
        else
            marginalized_om = new 
                EventObservationModelMapping(nrS,
                                             GetNrJointActions(), 
                                             GetNrJointObservations());
    }
    else
    {
        if(_m_sparse_FlatTM)
            marginalized_tm = new 
                TransitionModelMappingSparse(nrS,
                                             GetNrJointActions());
        else
            marginalized_tm = new 
                TransitionModelMapping(nrS,
                                       GetNrJointActions());      
        if(_m_sparse_FlatOM)
            marginalized_om = new 
                ObservationModelMappingSparse(nrS,
                                              GetNrJointActions(), 
                                              GetNrJointObservations());
        else
            marginalized_om = new 
                ObservationModelMapping(nrS,
                                        GetNrJointActions(), 
                                        GetNrJointObservations());
    }

    Scope XScope = GetAllStateFactorScope();
    Scope YScope(XScope);
    Scope AScope;
    for(size_t i = 0; i < GetNrAgents(); i++)
        AScope.Insert(i);
    Scope sfScope;
    sfScope.Insert(sf);

    vector<Index> X(GetNrStateFactors(),0);
    vector<Index> A(GetNrAgents(),0);
    vector<Index> Y(GetNrStateFactors(),0);
    do{
        do{
            do{
                Index jaI = IndividualToJointActionIndices(A);
                vector<Index> post_X;
                vector<Index> post_Y;
                Scope post_Y_Sc;
                for(size_t i = 0; i < Y.size(); i++)
                    if(i != sf){
                        post_X.push_back(X[i]);
                        post_Y.push_back(Y[i]);
                        post_Y_Sc.Insert(i);
                    }

                Index post_sI = IndexTools::IndividualToJointIndices(post_X, new_factor_sizes);
                Index post_sucsI = IndexTools::IndividualToJointIndices(post_Y, new_factor_sizes);
        
                double p_t = _m_2dbn.GetYProbability(X,A,Y);
                if(! Globals::EqualProbability(p_t, 0) )
                {
                    double post_p_t = marginalized_tm->Get(post_sI, jaI, post_sucsI);
                    double p_t_sum = post_p_t+p_t;
                    if(p_t_sum > Globals::PROB_PRECISION)
                        marginalized_tm->Set(post_sI, jaI, post_sucsI, p_t_sum); //marginalization
                }
                double post_p_given_sf = _m_2dbn.GetYProbabilityGeneral(XScope,
                                                                        X,
                                                                        AScope,
                                                                        A,
                                                                        YScope,
                                                                        Y,
                                                                        post_Y_Sc,
                                                                        post_Y);

                for(Index joI = 0; joI < GetNrJointObservations(); joI++){
                    vector<Index> O = JointToIndividualObservationIndices(joI);
    
                    double p_o = _m_2dbn.GetOProbability(X,A,Y,O);
                    if(! Globals::EqualProbability(p_o, 0) )
                    {
                        double post_p_o = marginalized_om->Get(post_sI, jaI, post_sucsI, joI);
                        double p_o_sum = post_p_o + p_o*post_p_given_sf*p_t;
                        if(p_o_sum > Globals::PROB_PRECISION)
                            marginalized_om->Set(post_sI, jaI, post_sucsI, joI, p_o_sum); //marginalization
                    }
                }
            }while(!IndexTools::Increment( Y, output_factor_sizes ));
        }while(!IndexTools::Increment( A, actions ));
    }while(!IndexTools::Increment( X, input_factor_sizes ));

    A.assign(GetNrAgents(),0);
    //now we need to sanitize the observation model.
    for(Index post_sI = 0; post_sI < nrS; post_sI++){
        for(Index jaI = 0; jaI < GetNrJointActions(); jaI++){
            for(Index post_sucsI = 0; post_sucsI < nrS; post_sucsI++){
                double p_total = 0;
                for(Index joI = 0; joI < GetNrJointObservations(); joI++){
                    p_total += marginalized_om->Get(post_sI, jaI, post_sucsI, joI);
                }
                for(Index joI = 0; joI < GetNrJointObservations(); joI++){
                    if(p_total > 0){
                        double post_p_o = marginalized_om->Get(post_sI, jaI, post_sucsI, joI);
                        if(post_p_o > Globals::PROB_PRECISION)
                          marginalized_om->Set(post_sI, jaI, post_sucsI, joI, post_p_o/p_total); //normalization
                    }
                    else
                        marginalized_om->Set(post_sI, jaI, post_sucsI, joI, 1.0/GetNrJointObservations()); //impossible transition
                }
            }
        }
    }
    
    if(_m_cached_FlatTM)
        delete(_m_p_tModel);
    if(_m_cached_FlatOM)
        delete(_m_p_oModel);

    _m_p_tModel = marginalized_tm;
    _m_p_oModel = marginalized_om;

    _m_cached_FlatTM = true;
    _m_cached_FlatOM = true;  

    RemoveStateFactor(sf);

    SanityCheck();
}

void MultiAgentDecisionProcessDiscreteFactoredStates::RemoveStateFactor(Index sf)
{
    Scope XScSf = _m_2dbn.GetXSoI_Y(sf);
    vector<CPDDiscreteInterface*> Y_cpts;
    vector<CPDDiscreteInterface*> O_cpts(GetNrAgents());
    vector<Scope> XSoIY, ASoIY, YSoIY;
    vector<Scope> XSoIO, ASoIO, YSoIO, OSoIO;

    for(size_t i = 0; i < GetNrAgents(); i++)
    {
        O_cpts[i] = _m_2dbn.GetCPD_O(i)->Clone();
        Scope XSc(_m_2dbn.GetXSoI_O(i)), YSc(_m_2dbn.GetYSoI_O(i));
        for(Index x = sf+1; x < GetNrStateFactors(); x++){
            //This corrects the scope indices, which will be changed after removing sf
            Scope sc;
            sc.Insert(x);
            if(XSc.Contains(x)){
                XSc.Remove(sc);
                XSc.Insert(x-1);
            }
            if(YSc.Contains(x)){
                YSc.Remove(sc);
                YSc.Insert(x-1);
            }
        }
        XSoIO.push_back(XSc);
        ASoIO.push_back(_m_2dbn.GetASoI_O(i));
        YSoIO.push_back(YSc);
        OSoIO.push_back(_m_2dbn.GetOSoI_O(i));
    }

    for(size_t i = 0; i < GetNrStateFactors(); i++)
    {
        if(i == sf)
            continue;    
        Scope YScI = _m_2dbn.GetYSoI_Y(i);
        if(YScI.Contains(sf))
        {
            Scope sfSc;
            sfSc.Insert(sf);
            Scope iSc;
            iSc.Insert(i);
            Scope XSc_i_sf = _m_2dbn.GetXSoI_Y(i);
            XSc_i_sf.Insert(XScSf);
            Scope ASc_i_sf = _m_2dbn.GetASoI_Y(i);
            ASc_i_sf.Insert(_m_2dbn.GetASoI_Y(sf));
            Scope YScI_r(YScI);
            YScI_r.Remove(sfSc);

            size_t ii_size = 1;
            vector<size_t> size_XSc_i_sf(XSc_i_sf.size(),0);
            vector<size_t> size_ASc_i_sf(ASc_i_sf.size(),0);
            vector<size_t> size_YScI_r(YScI_r.size(),0);
            for(size_t j = 0; j < XSc_i_sf.size(); j++){
                size_XSc_i_sf[j] = GetNrValuesForFactor(XSc_i_sf[j]);
                ii_size *= size_XSc_i_sf[j];
            }
            for(size_t j = 0; j < ASc_i_sf.size(); j++){
                size_ASc_i_sf[j] = GetNrActions(ASc_i_sf[j]);
                ii_size *= size_ASc_i_sf[j];
            }
            for(size_t j = 0; j < YScI_r.size(); j++){
                size_YScI_r[j] = GetNrValuesForFactor(YScI_r[j]);
                ii_size *= size_YScI_r[j];
            }
            size_t nrVals_i = GetNrValuesForFactor(i);
            CPT* cpt = new CPT(nrVals_i, ii_size);

            //NOTE: This already fixes the scopes and CPT sizes in the DBN, but it does not yet marginalize
            //each CPT. This will be necessary for factored event-driven algorithms.

            Y_cpts.push_back(cpt);
            Scope XSc(XSc_i_sf), YSc(YScI_r);
            for(Index x = sf+1; x < GetNrStateFactors(); x++){
                Scope sc;
                sc.Insert(x);
                if(XSc.Contains(x)){
                    XSc.Remove(sc);
                    XSc.Insert(x-1);
                }
                if(YSc.Contains(x)){
                    YSc.Remove(sc);
                    YSc.Insert(x-1);
                }
            }
            XSoIY.push_back(XSc);
            ASoIY.push_back(ASc_i_sf);
            YSoIY.push_back(YSc);
        }
        else
        {
            Y_cpts.push_back(_m_2dbn.GetCPD_Y(i)->Clone());
            Scope XSc(_m_2dbn.GetXSoI_Y(i)), YSc(_m_2dbn.GetYSoI_Y(i));
            for(Index x = sf+1; x < GetNrStateFactors(); x++){
                Scope sc;
                sc.Insert(x);
                if(XSc.Contains(x)){
                    XSc.Remove(sc);
                    XSc.Insert(x-1);
                }
                if(YSc.Contains(x)){
                    YSc.Remove(sc);
                    YSc.Insert(x-1);
                }
            }
            XSoIY.push_back(XSc);
            ASoIY.push_back(_m_2dbn.GetASoI_Y(i));
            YSoIY.push_back(YSc);
        }
    }

    _m_S.RemoveStateFactor(sf);
    _m_S.SetInitialized(false);
    _m_S.SetInitialized(true);

    //now we need to fix the DBN
    _m_2dbn.InitializeStorage();
    for(size_t i = 0; i < GetNrAgents(); i++)
    {
        _m_2dbn.SetSoI_O(i, XSoIO[i], ASoIO[i], YSoIO[i], OSoIO[i]);
        _m_2dbn.SetCPD_O(i, O_cpts[i]);
    }
    for(size_t i = 0; i < GetNrStateFactors(); i++)
    {
        _m_2dbn.SetSoI_Y(i, XSoIY[i], ASoIY[i], YSoIY[i]);
        _m_2dbn.SetCPD_Y(i, Y_cpts[i]);
    }
    _m_2dbn.InitializeIIs();
}
