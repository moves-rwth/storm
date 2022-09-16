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
#ifndef _MADPCOMPONENTFACTOREDSTATES_H_
#define _MADPCOMPONENTFACTOREDSTATES_H_ 1
/* the include directives */
#include <iostream>
#include "Globals.h"
#include "IndexTools.h"
#include <map>
#include "State.h"

//need to include (need to know that it is a StateDistribution):
#include "FactoredStateDistribution.h"

class StateFactorDiscrete;

/**\brief MADPComponentFactoredStates is a class that represents a factored 
 * states space.
 * 
 * It implements part of the MultiAgentDecisionProcessDiscreteInterface 
 * (at the moment it implements the same part as MADPComponentDiscreteStates,
 * although this class offers more functions. Eventually a 
 * MultiAgentDecisionProcessFactoredStatesInterface might be made.)
 *
 * A factored state is one that is defined as an assignment to a set of 
 * state variables, or factors. 
 * A factored state space is defined as the cross-product of the set of 
 * state factors. This is in contrast to a 'flat' state space that explicitly
 * enumarates all possible states.
 *
 * */
class MADPComponentFactoredStates 
//    : 
//    virtual public MultiAgentDecisionProcessDiscreteFactoredStatesInterface
    //which is partly implemented by this class
{
    private:   
        

        
        ///Is the state space initialized? (is it completely specified?)
        bool _m_initialized;
        ///Var that keeps track of the nr. of 'flat' states.
        size_t _m_nrStates;
        
        ///The number of state factors
        size_t _m_nrStateFactors;

        ///Vector with size of the domain of each state factor (the nr. values)
        /**This is used to compute state indices.*/
        std::vector<size_t> _m_sfacDomainSizes;
        ///Array caching the stepsize - used for computing indices.
        /**Computed during initialization.*/
        size_t* _m_stepSize;
        /// A std::vector with a pointers to the different state factors.
        /**The state factors themself specify their name, description,
         * (size of) their domain, etc.
         */
        std::vector<StateFactorDiscrete*> _m_stateFactors;

        //store the scope containing all state factors for convenience
        Scope _m_allStateFactorScope;

        FactoredStateDistribution* _m_initialStateDistribution;
        
        ///Assure that \em this is inititialed. (throw E otherwise)
        void AssureInitialized(std::string caller="unspecified caller") const;

        std::map<std::vector<Index>, State*> *_m_jointStatesMap;

    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        MADPComponentFactoredStates();
        /// Copy constructor.
        MADPComponentFactoredStates(const MADPComponentFactoredStates& a);
        /// Destructor.
        virtual ~MADPComponentFactoredStates();
        /// Copy assignment operator
        MADPComponentFactoredStates& operator= (const 
                MADPComponentFactoredStates& o);

        //operators:

        //data manipulation (set) functions:
        /**\brief Initializes the states space. Should be called when all 
         * factors are added.
         *
         * From _m_stateFactors, this function initializes all the other
         * member variables.
         */
        bool SetInitialized(bool b);
        //void Initialize();

        ///Adds a factor to the statespace.
        /**Can only be used when not \em this is initialized yet.
         * returns the index of the state factor.
         */
        Index AddStateFactor(const std::string &n="undef. name",
                             const std::string &d="undef. descr.");
      
        ///Adds a state factor value \a to the \a sf -th state factor.
        /**Can only be used when not \em this is initialized yet.
         * returns the index of the state factor value.         * 
         */
        Index AddStateFactorValue(Index sf, const std::string &v="undef");
        
        //get (data) functions:
       
        const StateFactorDiscrete* GetStateFactorDiscrete(Index sfacI) const
        { return _m_stateFactors.at(sfacI); } 

        /// Removes state factor sf from the problem description, and fixes
        /// indices accordingly.
        void RemoveStateFactor(Index sf);

        
        //implementing MultiAgentDecisionProcessDiscreteFactoredStatesInterface



        ///SoftPrints the factored state space.
        std::string SoftPrintStates() const;
        ///SoftPrints a particular state
        std::string SoftPrintState(Index sI) const;
        ///SoftPrints the ISD
        std::string SoftPrintInitialStateDistribution() const;
        ///SoftPrints the factored state space and ISD.
        std::string SoftPrint() const;
        std::string SoftPrintPartialState(const Scope& sfSc, 
                            const std::vector<Index>& sfValues) const;
    

//implement the MultiAgentDecisionProcessDiscreteFactoredStatesInterface!

        /// return the number of state factors
        size_t 	GetNrStateFactors () const
        { return  _m_nrStateFactors;}
        const Scope& GetAllStateFactorScope() const
        { return _m_allStateFactorScope; }
        /// return the std::vector with the number of values per factor
        const std::vector< size_t >& GetNrValuesPerFactor() const
        { return _m_sfacDomainSizes;}
        /// return the std::vector with the number of values per factor
        const size_t  GetNrValuesForFactor(Index sf) const
        { return _m_sfacDomainSizes.at(sf);}
        /// convert a state index to the std::vector of factor value indices.
        std::vector<Index> StateIndexToFactorValueIndices(Index stateI) const
        { return IndexTools::JointToIndividualIndicesStepSize(
                stateI, _m_stepSize, _m_nrStateFactors);}
        /// returns the index of the value of factor fI
        Index StateIndexToFactorValueIndex(Index fI, Index s) const
        { return StateIndexToFactorValueIndices(s).at(fI); }
        ///convert std::vector of (indices of) factor values to (flat) state index.
        Index FactorValueIndicesToStateIndex(const std::vector<Index> &fv) const
        { return IndexTools::IndividualToJointIndicesStepSize(fv, _m_stepSize);}

        //functions with explicitly specified scope
        Index FactorValueIndicesToStateIndex(const std::vector<Index>& s_e_vec, 
                const Scope& sfSC) const;
        std::vector<Index> StateIndexToFactorValueIndices(Index s_e, 
                const Scope& sfSC) const;


//implement the MultiAgentDecisionProcessDiscreteInterface
        /// Return the number of states.
        size_t GetNrStates() const;

        ///Returns a pointer to state i.
        /**This function is defined in 
         * MultiAgentDecisionProcessDiscreteInterface.
         * We cache any requests made on the fly.
         */
        const State* GetState(Index i) const; 

        Index SampleInitialState () const;
        void SampleInitialState(std::vector<Index> &sIs) const;
        double  GetInitialStateProbability (Index sI) const;
        virtual const FactoredStateDistribution* GetFactoredISD () const
        {return _m_initialStateDistribution;}
        virtual FactoredStateDistribution* GetFactoredISD ()
        {return _m_initialStateDistribution;}
        virtual StateDistribution* GetISD () const
        {return _m_initialStateDistribution;}

//convenience, but not require by interface        
        /// Sets the initial state distribution to a uniform one.
        void SetUniformISD();
        //void SetISD(const std::vector<double>& v);
        void SetISD( FactoredStateDistribution* p)
        { _m_initialStateDistribution = p;}
        
        
        
        void PrintDebugStuff() const;
};

inline
size_t MADPComponentFactoredStates::GetNrStates() const 
{return _m_nrStates;};

#endif /* !_MADPCOMPONENTFACTOREDSTATES_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
