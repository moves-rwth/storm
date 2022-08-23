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
#ifndef _MADPCOMPONENTDISCRETESTATES_H_
#define _MADPCOMPONENTDISCRETESTATES_H_ 1

/* the include directives */
#include <iostream>
#include <vector>
#include "Globals.h"

#include "StateDiscrete.h"
#include "StateDistributionVector.h"

/** 
 * \brief MADPComponentDiscreteStates is a class that represents a discrete
 * state space.
 *
 *  It implements a part of the
 *  MultiAgentDecisionProcessDiscreteInterface. */
class MADPComponentDiscreteStates
{
    private:
        bool _m_initialized;
        size_t _m_nrStates;
    
        ///Returns a string with the states
        std::string SoftPrintStates() const;
        ///Returns a string with the initial state distribution.
        std::string SoftPrintInitialStateDistribution() const;

        /// A vector that contains all the states.
        std::vector<StateDiscrete> _m_stateVec;
        /// A StateDistributionVector that represents the initial state distribution. 
        StateDistributionVector* _m_initialStateDistribution;

    protected:
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        MADPComponentDiscreteStates();
        /// Constructor that sets the number of states.
        MADPComponentDiscreteStates(size_t nrS);

        /// Copy constructor.
        MADPComponentDiscreteStates(const MADPComponentDiscreteStates& a);

        /// Destructor.
        virtual ~MADPComponentDiscreteStates();

        //operators:

        //data manipulation (set) functions:
        /// 
        /** \brief Sets _m_initialized to b.
         *
         * If no initial state distribution had been set, this function
         * will call SetUniformISD(). (To accomodate .pomdp files where
         * specification of ISD is optional.)
         */
        bool SetInitialized(bool b);

        /// Adds a new state.
        void AddState(const std::string &StateName);

        /// Sets the number of states to nrS.
        void SetNrStates(size_t nrS);

        /// Sets the initial state distribution to a uniform one.
        void SetUniformISD();

        /// Sets the initial state distribution to v.
        void SetISD(StateDistribution* p);
        void SetISD(StateDistributionVector* p)
        { _m_initialStateDistribution = p;}
        void SetISD(std::vector<double> v);
        
        //get (data) functions:        
        /// Return the number of states.
        size_t GetNrStates() const {return _m_nrStates;};

        /// Returns the state index of state s.
        Index GetStateIndex(StateDiscrete s) const;

        /// Returns the state index of state s.
        Index GetStateIndexByName(const std::string &s) const;
        
        /// Returns a pointer to state i. */
        const State* GetState(Index i) const 
            {return(&_m_stateVec.at(i)); }

        virtual std::string SoftPrintState(Index sI) const
        { return GetStateName(sI);}
        ///  Returns the name of a particular state i. 
        std::string GetStateName(Index i) const {
            return(_m_stateVec.at(i).SoftPrintBrief()); }

        /// Return the initial state distribution.
        double GetInitialStateProbability(Index sI) const;

        /// Returns the complete initial state distribution.
        //std::vector<double> GetISD() const { return(_m_initialStateDistribution); }
        virtual StateDistribution* GetISD() 
        { return(_m_initialStateDistribution); }
        virtual StateDistribution* GetISD() const 
        { return(_m_initialStateDistribution); }
        
        /// Sample a state according to the initial state PDF.
        Index SampleInitialState() const;
        
        std::string SoftPrint() const;
        void Print() const
        {std::cout << SoftPrint();}
};


#endif /* !_MADPCOMPONENTDISCRETESTATES_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
