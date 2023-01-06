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

#include "FactoredDecPOMDPDiscrete.h"
#include "DecPOMDPDiscrete.h"
#include "StateFactorDiscrete.h"
#include <fstream>
#include <algorithm>

#include <RGet.h>

#define DEBUG_SETR 0

using namespace std;
    
bool FactoredDecPOMDPDiscrete::
ConsistentVectorsOnSpecifiedScopes( 
                                        const std::vector<Index>& v1, 
                                        const Scope& scope1,
                                        const std::vector<Index>& v2,
                                        const Scope& scope2)
{
    Scope::const_iterator s2_it = scope2.begin();
    Scope::const_iterator s2_last = scope2.end();
    Index v2_I = 0;
    while(s2_it != s2_last)
    {
        //check that v1 specifies the same value for variable *s2_it
        Index varI = *s2_it;
        Index pos_in_v1 = scope1.GetPositionForIndex(varI);
        if(v1[pos_in_v1] != v2[v2_I] )
            return false;

        s2_it++;
        v2_I++;
    }
    return true;

}

FactoredDecPOMDPDiscrete::FactoredDecPOMDPDiscrete(string name, string descr, string pf) :
    FactoredDecPOMDPDiscreteInterface()
    ,MultiAgentDecisionProcessDiscreteFactoredStates(name, descr, pf)   
    ,_m_p_rModel()
    ,_m_cached_FlatRM(false)
    ,_m_sparse_FlatRM(false)
{
 }

//Destructor
FactoredDecPOMDPDiscrete::~FactoredDecPOMDPDiscrete()
{
    for(Index i=0;i!=_m_LRFs.size();++i)
        delete _m_LRFs.at(i);
    delete _m_p_rModel;
}

string FactoredDecPOMDPDiscrete::SoftPrint() const
{
    stringstream ss;
    ss << MultiAgentDecisionProcessDiscreteFactoredStates::SoftPrint();
    ss << DecPOMDP::SoftPrint();
    ss << "FactoredDecPOMDPDiscrete contains " << _m_nrLRFs
       << " local reward functions." << endl;
    for(Index i=0;i!=_m_nrLRFs;++i)
    {
        ss << "LRF " << i << " statefactor scope " << _m_sfScopes.at(i)
           << " agent scope " << _m_agScopes.at(i) << endl;
        ss << _m_LRFs.at(i)->SoftPrint() << endl;
    }
    
    //print components specific to FactoredDecPOMDPDiscrete:
    return(ss.str());
}


void FactoredDecPOMDPDiscrete::InitializeStorage()
{
    _m_sfScopes.clear();
    _m_agScopes.clear();
    _m_LRFs.clear();
    _m_sfScopes.resize(_m_nrLRFs);
    _m_agScopes.resize(_m_nrLRFs);
    _m_LRFs.resize(_m_nrLRFs, 0);
    
    //allocate storage for the instantiation information
    _m_nrXIs.resize(_m_nrLRFs);
    _m_nrAIs.resize(_m_nrLRFs);
    _m_nrSFVals.resize(_m_nrLRFs);
    _m_nrActionVals.resize(_m_nrLRFs);
}

void FactoredDecPOMDPDiscrete::SetScopeForLRF(Index LRF, 
            const Scope& X, //the X scope
            const Scope& A, //the A scope
            const Scope& Y,
            const Scope& O
        )
{
    Scope Xbackedup = StateScopeBackup( Y, O);
    Scope Abackedup = AgentScopeBackup( Y, O);
    // X'' = X' + X
    Xbackedup.Insert(X);
    Xbackedup.Sort();
    // A'' = A' + A
    Abackedup.Insert(A);
    Abackedup.Sort();
    SetScopeForLRF(LRF, Xbackedup, Abackedup);
}

const FactoredQFunctionScopeForStage 
FactoredDecPOMDPDiscrete::GetImmediateRewardScopes() const
{
    FactoredQFunctionScopeForStage immRewScope;
    //add scopes:
    for(Index i=0; i < _m_nrLRFs; i++)
    {
        immRewScope.AddLocalQ( _m_sfScopes.at(i), _m_agScopes.at(i) );
    }
    return immRewScope;
}

void FactoredDecPOMDPDiscrete::InitializeInstantiationInformation()
{
    const vector< size_t>& nrVals = GetNrValuesPerFactor();
    const vector< size_t>& nrActions = GetNrActions();
    for(Index e = 0; e < GetNrLRFs(); e++)
    {
        const Scope& X = GetStateFactorScopeForLRF(e);
        vector< size_t> restrXVals(X.size());
        IndexTools::RestrictIndividualIndicesToScope(nrVals, X, restrXVals);
        _m_nrSFVals.at(e) = restrXVals;
        size_t nrXIs = 1;
        for( vector< size_t >::const_iterator it = restrXVals.begin();
                it != restrXVals.end();
                it++)
            nrXIs *= *it;
        _m_nrXIs.at(e) = nrXIs; 

        const Scope& A = GetAgentScopeForLRF(e);
        vector< size_t> restrAVals(A.size());
        IndexTools::RestrictIndividualIndicesToScope(nrActions, A, restrAVals);
        _m_nrActionVals.at(e) = restrAVals;
        size_t nrAIs = 1;
        for( vector< size_t >::const_iterator it = restrAVals.begin();
                it != restrAVals.end();
                it++)
            nrAIs *= *it;
        _m_nrAIs.at(e) = nrAIs;

    }


}

size_t FactoredDecPOMDPDiscrete::GetNrXIs(Index lrf) const
{
    return(_m_nrXIs.at(lrf));
}

size_t FactoredDecPOMDPDiscrete::GetNrAIs(Index lrf) const
{
    return(_m_nrAIs.at(lrf));
}


void FactoredDecPOMDPDiscrete::
SetRewardForLRF(Index LRF,
        const vector<Index>& Xs,
        const vector<Index>& As,
        double reward
    )
{
#if DEBUG_SETR 
    string indent = "\t\t";
    cerr<<indent;
    cerr<< "SetRewardForLRF without scopes called:"<<endl;
    cerr<<indent;
    cerr<<"LRF="<<LRF
        << ", Xs="<<SoftPrintVector(Xs) 
        << ", As" <<SoftPrintVector(As) 
        << ", r="<<reward<<endl;
#endif
    if(_m_LRFs.at(LRF) == 0)
        throw E("no reward model set yet.");
    //check that scopes seem alright:
    if(     Xs.size() != _m_sfScopes.at(LRF).size() ||
            As.size() != _m_agScopes.at(LRF).size() )
        throw E("indices do not match registered scopes!");

    //this class to handle conversion from Xs and As to
    //joint indices and then use an already available reward model
    
    //convert Xs-> local joint index
    Index jointXIndex = RestrictedStateVectorToJointIndex(LRF, Xs);
    Index jointAIndex = RestrictedActionVectorToJointIndex(LRF, As);

    _m_LRFs.at(LRF)->Set(jointXIndex, jointAIndex, reward);
}

void FactoredDecPOMDPDiscrete::SetRewardForLRF(Index LRF,
            const vector<Index>& Xs,
            const vector<Index>& As,
            const Scope& Y,
            const vector<Index>& Ys,
            const Scope& O,
            const vector<Index>& Os,
            double reward
        )
{
#if DEBUG_SETR 
    string indent = "\t";
    cerr<<indent;
    cerr<< "SetRewardForLRF with Y and O scopes called: LRF="<<LRF<<
        ", r="<<reward<<endl;
#endif
    double r_xa = GetLRFReward(LRF, Xs, As);
    double P_yo_xa = GetYOProbability( 
            _m_sfScopes.at(LRF), Xs,
            _m_agScopes.at(LRF), As,
            Y, Ys,
            O, Os);
    double r_weighted = P_yo_xa * reward;
    r_xa += r_weighted;
    SetRewardForLRF(LRF, Xs, As, r_xa);
}

void FactoredDecPOMDPDiscrete::SetRewardForLRF(
        Index LRF,
        const Scope& X,
        const vector<Index>& Xs,
        const Scope& A,
        const vector<Index>& As,
        const Scope& Y,
        const vector<Index>& Ys,
        const Scope& O,
        const vector<Index>& Os,
        double reward
    )
{
#if DEBUG_SETR 
    string indent = "";
    cerr<<indent;
    cerr<< "SetRewardForLRF with all scopes called: LRF="<<LRF<<
        ", r="<<reward<<endl;
#endif
    const Scope& X_LRF = _m_sfScopes.at(LRF);
    const Scope& A_LRF = _m_agScopes.at(LRF);
    vector<Index> Xs2(X_LRF.size(), 0 );
    vector<Index> As2(A_LRF.size(), 0 );
    do{
        if( ! ConsistentVectorsOnSpecifiedScopes(Xs2, X_LRF, Xs, X) )
            continue;
        do{
            if( ! ConsistentVectorsOnSpecifiedScopes(As2, A_LRF, As, A) )
                continue;
            // Xs2, As2 is consistent with Xs, As:
            SetRewardForLRF(LRF, Xs2, As2, Y, Ys, O, Os, reward);

        } while(! IndexTools::Increment( As2, _m_nrActionVals.at(LRF) ) ); 

    } while(! IndexTools::Increment( Xs2, _m_nrSFVals.at(LRF) ) ); 

}


void FactoredDecPOMDPDiscrete::SetReward(Index sI, Index jaI, Index sucSI, double r)
{
    double rOld=GetReward(sI,jaI),
        rExp=GetTransitionProbability(sI,jaI,sucSI)*r;
    SetReward(sI,jaI,rOld+rExp);
}

void FactoredDecPOMDPDiscrete::SetReward(Index sI, Index jaI, Index sucSI,
                                 Index joI, double r)
{
    throw(E("DecPOMDPDiscrete::SetReward(sI,jaI,sucSI,joI,r) not implemented"));
}

double FactoredDecPOMDPDiscrete::GetReward(Index sI, Index jaI) const
{
    if(_m_cached_FlatRM)
        return _m_p_rModel->Get(sI,jaI);

    //sum over local reward functions
    double r = 0.0;
    for(Index e=0; e < GetNrLRFs(); e++)
    {
        double this_e_r = GetLRFRewardFlat(e, sI, jaI);
        r += this_e_r;
    }
    return(r);
}

double FactoredDecPOMDPDiscrete::GetReward(const vector<Index> &sIs,
                                           const vector<Index> &aIs) const
{
    //sum over local reward functions
    double r = 0.0;
    for(Index e=0; e < GetNrLRFs(); e++)
    {
        double this_e_r = GetLRFRewardFlat(e, sIs, aIs);
        r += this_e_r;
    }
    return(r);
}

RGet * FactoredDecPOMDPDiscrete::GetRGet() const
{
    if(!_m_cached_FlatRM)
        return 0;
        //throw E("FactoredDecPOMDPDiscrete: can't get RGet if flat reward model not chached!");

    if(_m_sparse_FlatRM)
        return new RGet_RewardModelMappingSparse(
                ((RewardModelMappingSparse*)_m_p_rModel)  ); 
    else
        return new RGet_RewardModelMapping(
                ((RewardModelMapping*)_m_p_rModel)  );
}



double FactoredDecPOMDPDiscrete::
GetLRFRewardFlat(Index lrf, Index flat_s, Index full_ja) const
{
    vector<Index> sfacs = StateIndexToFactorValueIndices(flat_s);
    const vector<Index>& ja = JointToIndividualActionIndices(full_ja);
    return(GetLRFRewardFlat(lrf,sfacs,ja));
}

double FactoredDecPOMDPDiscrete::
GetLRFRewardFlat(Index lrf,
                 const vector<Index>& sfacs,
                 const vector<Index>& as) const
{
    const Scope &Xsc = GetStateFactorScopeForLRF(lrf);
    const Scope &Asc = GetAgentScopeForLRF(lrf);

    vector<Index> restr_X(Xsc.size());
    IndexTools::RestrictIndividualIndicesToScope(
        sfacs, Xsc, restr_X);
    vector<Index> restr_A(Asc.size());
    IndexTools::RestrictIndividualIndicesToScope(
        as, Asc, restr_A);

    double r =  GetLRFReward(lrf, restr_X, restr_A);
    return r;
}

double FactoredDecPOMDPDiscrete::
GetLRFReward(Index lrf, 
        const vector<Index>& s_e_vec, 
        const vector<Index>& a_e_vec)const
{
    Index s_e = RestrictedStateVectorToJointIndex(lrf, s_e_vec);
    Index a_e = RestrictedActionVectorToJointIndex(lrf, a_e_vec);
    double r =   GetLRFReward(lrf, s_e, a_e);
    return r;
}
/* inline
double FactoredDecPOMDPDiscrete::
GetLRFReward(Index lrf, Index sI_e, Index jaI_e) const
{
    return( _m_LRFs[lrf]->Get(sI_e, jaI_e) );
}
*/



Index FactoredDecPOMDPDiscrete::RestrictedStateVectorToJointIndex(
            Index LRF, const vector<Index>& stateVec_e) const
{
    const vector<size_t>& nrSFVals = _m_nrSFVals.at(LRF);
    Index jointXI = IndexTools::IndividualToJointIndices(stateVec_e, nrSFVals);
    return jointXI;
}

Index FactoredDecPOMDPDiscrete::RestrictedActionVectorToJointIndex(
            Index LRF, const vector<Index>& actionVec_e) const
{
    const vector<size_t>& nrActionVals = _m_nrActionVals.at(LRF);
    Index jointAI = 
        IndexTools::IndividualToJointIndices(actionVec_e, nrActionVals);
    return jointAI;
}



void FactoredDecPOMDPDiscrete::CacheFlatRewardModel(bool sparse)
{
    if(_m_cached_FlatRM)
    {
        _m_cached_FlatRM=false; // set to false, otherwise GetReward()
                                // call below will wrongly assume the
                                // rewards have already been cached
                                // and return only zeroes
        delete(_m_p_rModel);
    }
    _m_sparse_FlatRM = sparse;
    if(sparse)
        _m_p_rModel=new RewardModelMappingSparse(GetNrStates(),
                                                     GetNrJointActions());
    else
        _m_p_rModel=new RewardModelMapping(GetNrStates(),
                                               GetNrJointActions());


    //cout << "caching rewards"<<endl;
    for(Index sI=0; sI<GetNrStates(); sI++)
        for(Index jaI=0; jaI<GetNrJointActions(); jaI++)
        {
            double r = GetReward(sI, jaI);
            //cout << "R(s=" << sI << ",jaI=" <<jaI <<") = " << r;
            if( abs(r) >= REWARD_PRECISION )                
                _m_p_rModel->Set(sI, jaI, r);

            //cout << " - from cache:" << _m_p_rModel->Get(sI, jaI) << endl;

        }

    _m_cached_FlatRM = true;
}

void FactoredDecPOMDPDiscrete::CacheFlatModels(bool sparse)
{
    // if we are generating the full models anyway, also create
    // the joint actions/observations
    ConstructJointActions();
    ConstructJointObservations();
    
    CacheFlatTransitionModel(sparse);
    CacheFlatObservationModel(sparse);
    CacheFlatRewardModel(sparse);
}

void FactoredDecPOMDPDiscrete::ExportSpuddFile(const string& filename) const
{
  ofstream fp(filename.c_str());
  if(!fp.is_open()) {
    cerr << "FactoredDecPOMDPDiscrete::ExportSpuddFile: failed to "
         << "open file " << filename << endl;
    return;
  }

  // write header
  fp << "// Automatically produced by FactoredDecPOMDPDiscrete::ExportSpuddFile"
     << endl << "// SPUDD / Symbolic Perseus Format for '" << GetName() << "'"
     << endl << endl;
  
  // write variables
  fp << "(variables" << endl;
  for(Index yI = 0; yI < GetNrStateFactors(); yI++) {
    const StateFactorDiscrete* sfac = GetStateFactorDiscrete(yI);
    fp << " (" << sfac->GetName();
    for(Index valI=0; valI < GetNrValuesForFactor(yI); valI++)
      fp << " " << sfac->GetStateFactorValue(valI);
    fp << ")" << endl;
  }
  fp << ")" << endl << endl;
  
  // write actions
  const Scope& jAsc = GetAllAgentScope();
  for(Index jaI = 0; jaI < GetNrJointActions(); jaI++) {
    vector<Index> A = JointToIndividualActionIndices(jaI);

    // construct and print joint action name
    stringstream ss;
    for(Index agentI = 0; agentI < GetNrAgents(); agentI++)
      ss << GetAgentNameByIndex(agentI) << "_"
         << GetAction(agentI, A[agentI])->GetName() << "__";
    string aname = ss.str();
    fp << "action " << aname.substr(0, aname.length()-2) << endl;

    // write out CPT for each state factor
    for(Index y = 0; y < GetNrStateFactors(); y++) {
      fp << GetStateFactorDiscrete(y)->GetName() << endl;

      // figure out action subset for ii
      const Scope& ASoI_y = Get2DBN()->GetASoI_Y(y);
      size_t ASoI_y_size = ASoI_y.size();
      vector<Index> As_restr(ASoI_y_size);
      IndexTools::RestrictIndividualIndicesToNarrowerScope(A, jAsc, ASoI_y, As_restr);

      // loop over X instantiations
      const Scope& XSoI_y  = Get2DBN()->GetXSoI_Y(y);
      size_t XSoI_y_size = XSoI_y.size(); // number of variables X in y's scope
      vector<size_t> r_nrX = Get2DBN()->GetNrVals_XSoI_Y(y); // number of values for X
      vector<Index> Xs(XSoI_y_size, 0 ); // instantiation for X variables in XSoI_y
      vector<Index> prevXs(XSoI_y_size, 1 ); // previous iteration
      const Scope emptySc; // for Y variables (no within-stage influences now)
      bool firstIter = true;

      do { // for each permutation
        //cout << SoftPrintVector(Xs) << endl;
        // close previously opened variable blocks
        size_t nrXchanges = 0;
        for(Index scI=0; scI < XSoI_y_size; scI++) { // for all state factors in ii
          if(prevXs[scI] != Xs[scI]) 
            nrXchanges++;
        }
        if(!firstIter) fp << string(2*nrXchanges,')') << endl; else firstIter = false;
          
        // check where indices changed from previous iteration
        for(Index scI=0; scI < XSoI_y_size; scI++) { // for all state factors in ii
          Index sfI = XSoI_y.at(scI);
          const StateFactorDiscrete* sfac = GetStateFactorDiscrete(sfI);

          if(prevXs[scI] != Xs[scI]) {
            if(Xs[scI] == 0) {
              // write out next variable name
              string name = sfac->GetName();
              fp << " (" << name;
            }
            // print variable value
            string value = sfac->GetStateFactorValue(Xs[scI]);
            fp << " (" << value;
          }
        }

        // write distribution as vector
        vector<double> dist = Get2DBN()->GetYProbabilitiesExactScopes(Xs, As_restr, emptySc, y);
        // if(p > Globals::PROB_PRECISION)
        fp << " (";
        for(vector<double>::const_iterator pI = dist.begin(); pI != dist.end(); ++pI)
          fp << *pI << " ";

        prevXs = Xs;

      } while(! IndexTools::Increment( Xs, r_nrX ) );
      // write out last closing braces
      fp << string(XSoI_y_size*2+1,')') << endl << endl;
    }

    // write generic cost term
    fp << "cost [+" << endl;
    for(Index rI=0; rI < GetNrLRFs(); rI++) {
      const Scope& agSC = GetAgentScopeForLRF(rI);
      size_t agSC_size = agSC.size();
      vector<Index> As_restr(agSC_size);
      IndexTools::RestrictIndividualIndicesToNarrowerScope(A, jAsc, agSC, As_restr);

      //XXX this lookup can be replaced with cached values
      // in _m_nrSFVals.at(LRF), cf. line 286
      const vector< size_t>& nrVals = GetNrValuesPerFactor(); //XXX move out here
      const Scope& sfSC = GetStateFactorScopeForLRF(rI);
      size_t sfSC_size = sfSC.size();
      vector< size_t> restrXVals(sfSC_size);
      IndexTools::RestrictIndividualIndicesToScope(nrVals, sfSC, restrXVals);
      
      vector<Index> Xs2(sfSC_size, 0 );
      vector<Index> prevXs2(sfSC_size, 1 ); // previous iteration
      const Scope emptySc;
      bool firstIter = true;

      do { // for each permutation
        // close previously opened variable blocks
        size_t nrXchanges = 0;
        for(Index scI=0; scI < sfSC_size; scI++) { // for all ii state factors
          if(prevXs2[scI] != Xs2[scI])
            nrXchanges++;
        }
        if(!firstIter) fp << string(2*nrXchanges,')') << endl; else firstIter = false;
          
        // check where indices changed from previous iteration
        for(Index scI=0; scI < sfSC_size; scI++) { // for all ii state factors
          Index sfI = sfSC.at(scI);
          const StateFactorDiscrete* sfac = GetStateFactorDiscrete(sfI);

          if(prevXs2[scI] != Xs2[scI]) {
            if(Xs2[scI] == 0) {
              // write out next variable name
              string name = sfac->GetName();
              fp << " (" << name;
            }
            // print variable value
            string value = sfac->GetStateFactorValue(Xs2[scI]);
            fp << " (" << value;
          }
        }

        // write reward as cost for this ii instantiation
        double reward = GetLRFReward(rI, Xs2, As_restr);
        fp << " (" << -reward;

        prevXs2 = Xs2;

      } while(! IndexTools::Increment( Xs2, restrXVals ) );
      // write out last closing braces
      fp << string(sfSC_size*2+1,')') << endl;
    }
    fp << "     ]" << endl
       << "endaction" << endl << endl;
  }

  // write reward function (note: subsumed as costs inside each individual action)
  fp << "reward (0.0)" << endl << endl;

  // write footer
  fp << "discount " << GetDiscount() << endl //XXX add warning
     << "//horizon 10" << endl
     << "tolerance 0.1" << endl;
}

void FactoredDecPOMDPDiscrete::ClipRewardModel(Index sf, bool sparse)
{
    if(_m_cached_FlatRM){
        delete(_m_p_rModel);
        _m_cached_FlatRM = false;
    }

    _m_sparse_FlatRM = sparse;

    size_t nrS = 1;
    for(size_t i = 0; i < GetNrStateFactors(); i++){
      if(i == sf)
        continue;
      size_t nrX = GetNrValuesForFactor(i);
      nrS *= nrX;
    }

    if(sparse)
        _m_p_rModel=new RewardModelMappingSparse(nrS,
                                                 GetNrJointActions());
    else
        _m_p_rModel=new RewardModelMapping(nrS,
                                           GetNrJointActions());

    for(Index sI=0; sI<nrS; sI++)
        for(Index jaI=0; jaI<GetNrJointActions(); jaI++)
        {
            double r = GetReward(sI, jaI);
        
            if( abs(r) >= REWARD_PRECISION )                
                _m_p_rModel->Set(sI, jaI, r);
        }

    _m_cached_FlatRM = true;
}

void FactoredDecPOMDPDiscrete::MarginalizeISD(Index sf, vector<size_t>& factor_sizes, const FactoredStateDistribution* fsd)
{
    vector<Index> X(factor_sizes.size(),0);
    vector<size_t> new_factor_sizes = factor_sizes;
    new_factor_sizes[sf] = 1;
  
    vector<vector<double> > f_isd;
    for(size_t i = 0; i < X.size(); i++){
        if(i == sf)
            continue;
        new_factor_sizes = factor_sizes;
        new_factor_sizes[i] = 1; //Going to iterate over every factor except i (there's no easier way).
        X.assign(X.size(),0);
        vector<double> dist;
        for(size_t j = 0; j < factor_sizes[i]; j++){
            double p = 0;
            do{
                X[i] = j;
                p += fsd->GetProbability(X);
            }while(!IndexTools::Increment( X, new_factor_sizes));
            if(p > 1 + PROB_PRECISION)
            {
                cout << "FactoredDecPOMDPDiscrete::MarginalizeISD - probability does not sum to 1 but to " << p << " instead. Correcting." << endl;
                p = 1;
            }
            dist.push_back(p);
        }
        f_isd.push_back(dist);
    }
    
    FSDist_COF* new_isd = new FSDist_COF(*this);
    
    for(size_t i = 0; i < f_isd.size(); i++)
        for(size_t j = 0; j < f_isd[i].size(); j++)
            new_isd->SetProbability(i,j,f_isd[i][j]);
    
    new_isd->SanityCheck();
    
    delete(GetISD());
    SetISD(new_isd);
}

void FactoredDecPOMDPDiscrete::MarginalizeStateFactor(Index sf, bool sparse)
{
    vector<size_t> old_factor_sizes = GetNrValuesPerFactor();
    const FactoredStateDistribution* old_isd = GetFactoredISD();

    ClipRewardModel(sf, sparse);
    MarginalizeTransitionObservationModel(sf, sparse);
    MarginalizeISD(sf, old_factor_sizes, old_isd);

    delete(old_isd);
}
