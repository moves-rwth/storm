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
#ifndef _MULTIAGENTDECISIONPROCESS_H_
#define _MULTIAGENTDECISIONPROCESS_H_ 1

/* the include directives */
#include <string>
#include <vector>

#include "NamedDescribedEntity.h"
#include "MultiAgentDecisionProcessInterface.h"
#include "Agent.h"
#include "Globals.h"
#include "Scope.h"

/**\brief MultiAgentDecisionProcess is an class that defines the primary
 * properties of a decision process. 
 *
 * These primary properties are:
 * -the number of agents
 * -possibly, a vector of (named) agents
 * -the filename to be parsed, if applicable.
 * 
 * It is the only current implementation of MultiAgentDecisionProcessInterface
 * */
class MultiAgentDecisionProcess : 
    virtual public MultiAgentDecisionProcessInterface 
    //,public NamedDescribedEntity 
{
    private:
        NamedDescribedEntity _m_name;
    
        void InitializeUnixName();

    protected:
        /**The number of agents participating in the MADP.*/
        size_t _m_nrAgents;
        /**Vector containing Agent objects, which are indexed named entities.*/
        std::vector<Agent> _m_agents;
        /**Scope containing all agents - usefull sometimes*/
        Scope _m_allAgentsScope;
        
        /**String holding the filename of the problem file to be parsed - if 
         * applicable.*/
        std::string _m_problemFile;

        /**String for the unix name of the MADP.*/
        std::string _m_unixName;

    public:
        // Constructor, destructor and copy assignment.
        /// Constructor.
        MultiAgentDecisionProcess(
            size_t nrAgents, 
            const std::string &name="received unspec. by MultiAgentDecisionProcess", 
            const std::string &description="received unspec. by MultiAgentDecisionProcess", 
            const std::string &pf="received unspec. by MultiAgentDecisionProcess");
        
        /// Default Constructor without specifying the number of agents.
        MultiAgentDecisionProcess(
            const std::string &name="received unspec. by MultiAgentDecisionProcess", 
            const std::string &description="received unspec. by MultiAgentDecisionProcess", 
            const std::string &pf="received unspec. by MultiAgentDecisionProcess");

        ///Destructor.    
        virtual ~MultiAgentDecisionProcess();

        /**Sets the number of agents. this creates nrAgents unnamed agents.*/
        void SetNrAgents(size_t nrAgents);
        /**Add a new agent with name "name" to the MADP. 
         * NOTE: This increments the number of agents as well!*/
        void AddAgent(std::string name);

        /**Returns the index of an agent given its name, if it exists. */
        Index GetAgentIndexByName(const std::string &s) const;

        /**Returns the name of the agent at the given index. */
        std::string GetAgentNameByIndex(Index i) const;

        /**Returns the number of agents in this MultiAgentDecisionProcess. */
        size_t GetNrAgents() const;

        const Scope& GetAllAgentScope() const
        {return _m_allAgentsScope;}

        /**Returns the name of the problem file. */
        std::string GetProblemFile() const;

        /// \brief Returns a name which can be in unix path, by
        /// default the base part of the problem filename.
        std::string GetUnixName() const { return(_m_unixName); }

        /// Sets the name which can be used inin unix paths.
        void SetUnixName(std::string unixName) { _m_unixName=unixName; }

        /** Prints some information on the MultiAgentDecisionProcess.*/    
        std::string SoftPrint() const;
        void Print() const
        {std::cout << SoftPrint();}

    // forwards to _m_name - remove these as much as possible!
        std::string GetName() const {return _m_name.GetName();};
        std::string GetDescription() const {return _m_name.GetDescription();};
        void SetName(const std::string &name) {_m_name.SetName(name);}
        void SetDescription(const std::string &description){_m_name.SetDescription(description);}
        //virtual std::string SoftPrint() const;
        //virtual std::string SoftPrintBrief() const;
        //void Print() const {std::cout << SoftPrint() << std::endl; }
        //void PrintBrief() const {std::cout << SoftPrintBrief() << std::endl; }
};

#endif /* !_MULTIAGENTDECISIONPROCESS_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
