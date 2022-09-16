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
#ifndef _POMDPDISCRETE_H_
#define _POMDPDISCRETE_H_ 1

/* the include directives */
#include "Globals.h"
#include "DecPOMDPDiscrete.h"

/** \brief POMDPDiscrete models discrete POMDPs. It is basically a
 * wrapper for a Dec-POMDP with a single agent.
 *
 **/
class POMDPDiscrete : public DecPOMDPDiscrete
{
    private:    
        
    
    protected:
        static const int SINGLE_AGENT_INDEX = 0;
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        /** Constructor that sets the name, description, and problem file,
         * and subsequently loads this problem file. */
        POMDPDiscrete(const std::string &name="received unspec. by POMDPDiscrete", 
                      const std::string &descr="received unspec. by POMDPDiscrete", 
                      const std::string &pf="received unspec. by POMDPDiscrete");

        /// Copy constructor.
        ///        POMDPDiscrete(const POMDPDiscrete& a);
        /// Destructor.
        virtual ~POMDPDiscrete(){};
        // Copy assignment operator
//         POMDPDiscrete& operator= (const POMDPDiscrete& o);

        size_t GetNrSingleAgentActions() const      { return(GetNrActions(SINGLE_AGENT_INDEX)); } 
        size_t GetNrSingleAgentObservations() const { return(GetNrObservations(SINGLE_AGENT_INDEX)); }

        ///set the number of actions for the single agent
        void SetNrSingleAgentActions( size_t nrA )      
            { this->SetNrActions(SINGLE_AGENT_INDEX, nrA); } 
        ///add an action for the single agent
        void AddSingleAgentAction(const std::string &name, const std::string &description="") 
            { this->AddAction(SINGLE_AGENT_INDEX, name, description); }
        ///set the number of obversations for the POMDP (single agent)
        void SetNrSingleAgentObservations( size_t nrO ) 
            { this->SetNrObservations(SINGLE_AGENT_INDEX, nrO); }
        ///add an observation for the single agent
        void AddSingleAgentObservation(const std::string &name, const std::string &description="") 
            { this->AddObservation(SINGLE_AGENT_INDEX, name, description); }
};


#endif /* !_POMDPDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
