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

/* Only include this header file once. */
#ifndef _MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATESINTERFACE_H_
#define _MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATESINTERFACE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "MultiAgentDecisionProcessDiscreteInterface.h"
#include "IndexTools.h"
#include "Scope.h"
#include "FactoredStateDistribution.h"

class StateFactorDiscrete;
class TwoStageDynamicBayesianNetwork;

/**\brief MultiAgentDecisionProcessDiscreteFactoredStatesInterface is the 
 * interface for factored state problems.
 *
 * Currently it has one implementation: 
 * MultiAgentDecisionProcessDiscreteFactoredStates
 *
 *
 * This class defines the functions that implement/maintain the factored 
 * transition and observation models.
 * */
class MultiAgentDecisionProcessDiscreteFactoredStatesInterface
  : 
    virtual public MultiAgentDecisionProcessDiscreteInterface
{
    private:    

    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        //MultiAgentDecisionProcessDiscreteFactoredStatesInterface();
        /// Copy constructor.
        //MultiAgentDecisionProcessDiscreteFactoredStatesInterface(const MultiAgentDecisionProcessDiscreteFactoredStatesInterface& a);
        /// Destructor.
        virtual ~MultiAgentDecisionProcessDiscreteFactoredStatesInterface(){};
        /// Copy assignment operator
        //MultiAgentDecisionProcessDiscreteFactoredStatesInterface& operator= (const MultiAgentDecisionProcessDiscreteFactoredStatesInterface& o);

        //operators:

        //data manipulation (set) functions:
        
        //get (data) functions:
        virtual const TwoStageDynamicBayesianNetwork* Get2DBN() const
        { throw E("Get2DBN should be overriden by the implementation of MultiAgentDecisionProcessDiscreteFactoredStatesInterface. (actually, we may want to define a TwoStageDynamicBayesianNetworkInterface or something similar, such the derived class does not need to use TwoStageDynamicBayesianNetwork per se. This, however, is work for the future.)"); }

        virtual const StateFactorDiscrete* GetStateFactorDiscrete(Index sfacI) const = 0;

//stuff that is implemented by a factored state component:
//(i.e., the only current implementation is MADPComponentFactoredStates)
        virtual const FactoredStateDistribution* GetFactoredISD() const = 0;

        /// Get the number of state components. -no is inherited from 
        // MultiAgentDecisionProcessDiscreteInterface!
        //virtual size_t GetNrStates() const 
        //{ return (IndexTools::VectorProduct( GetNrValuesPerFactor() ) ); }

        /// Get the number of state components.
        virtual size_t GetNrStateFactors() const = 0;
        /// Convenience function to quickly get the full state scope
        virtual const Scope& GetAllStateFactorScope() const=0;
        /// Get the number of possible assignments or values to each factor.
        virtual const std::vector<size_t>& GetNrValuesPerFactor() const = 0;
        /// Get the number of possible values for a particular factor.
        virtual const size_t  GetNrValuesForFactor(Index sf) const = 0;
        /**\brief Get the vector of FactorValue indices corresponding to stateI
         *  used to be called
         *      virtual vector<Index> GetStateFactorValues(Index stateI) const
         */
        virtual std::vector<Index> StateIndexToFactorValueIndices(Index stateI) 
            const = 0;
        /// Get the value of a particular state factor given a joint flat state
        virtual Index StateIndexToFactorValueIndex(Index factor, Index s) 
            const = 0;
        ///convert std::vector of (indices of) factor values to (flat) state index.
        virtual Index FactorValueIndicesToStateIndex(const std::vector<Index> &fv) 
            const = 0;
    //functions with explicitly specified scope
        /**\brief convert an local state vector \a s_e_vec of scope \a sfScope 
         * to a joint index.
         */
        virtual Index FactorValueIndicesToStateIndex(const std::vector<Index>& 
                s_e_vec, const Scope& sfSC) const=0;
        /**\brief convert an local state index \a s_e to a vector of 
         * state factors (with scope \a sfScope). 
         */
        virtual std::vector<Index> StateIndexToFactorValueIndices(Index s_e, 
                const Scope& sfSC) const=0;


//not sure how to classify these, but they are implemented by 
//MultiAgentDecisionProcessDiscreteFactoredStatesInterface

        ///Get the number of joint instantiations for the factors in sfScope
        virtual size_t GetNrStateFactorInstantiations(const Scope& sfScope) const=0;        

// stuf that has to be implemented by something that represents the transition-
// and observation model.
        virtual Scope StateScopeBackup( const Scope & stateScope, 
                                        const Scope & agentScope) const = 0;
        virtual Scope AgentScopeBackup( const Scope & stateScope, 
                                        const Scope & agentScope) const = 0;

        virtual void SampleInitialState(std::vector<Index> &sIs) const = 0;
        virtual void SampleSuccessorState(const std::vector<Index> &sIs,
                                          const std::vector<Index> &aIs,
                                          std::vector<Index> &sucIs) const = 0;
        virtual void SampleJointObservation(const std::vector<Index> &aIs,
                                            const std::vector<Index> &sucIs,
                                            std::vector<Index> &oIs) const = 0;

        
        /// Returns a pointer to a copy of this class.
        virtual MultiAgentDecisionProcessDiscreteFactoredStatesInterface* Clone() const = 0;

        ///SoftPrints information on the MultiAgentDecisionProcessDiscrete.
        //string SoftPrint() const;
        std::string SoftPrintState(Index sI) const
        {throw E("MultiAgentDecisionProcessDiscreteInterface::SoftPrintState should be overriden");}
        
        virtual void CacheFlatModels(bool sparse)
        {throw E("MultiAgentDecisionProcessDiscreteInterface::CacheFlatModels should be overriden");}
};


#endif /* !_MULTIAGENTDECISIONPROCESSDISCRETEFACTOREDSTATESINTERFACE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
