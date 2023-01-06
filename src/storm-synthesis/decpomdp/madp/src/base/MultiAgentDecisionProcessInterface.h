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
#ifndef _MULTIAGENTDECISIONPROCESSINTERFACE_H_
#define _MULTIAGENTDECISIONPROCESSINTERFACE_H_ 1

/* the include directives */
#include <string>
#include <vector>

#include "Globals.h"
class Scope; 

/**\brief MultiAgentDecisionProcessInterface is an abstract base class that
 * declares the primary properties of a multiagent decision process.
 *
 * These primary properties are:
 * \li the number of agents
 * \li possibly, a vector of (named) agents
 * \li the filename to be parsed, if applicable.
 * 
 * It is implemented by MultiAgentDecisionProcess
 * */
class MultiAgentDecisionProcessInterface 
{
    private:    

    protected:

    public:

        ///Destructor. 
        //(Can't make a virt.destr. pure abstract!)
        virtual ~MultiAgentDecisionProcessInterface() {}

        ///Return the number of agents
        virtual size_t GetNrAgents() const = 0;
        virtual const Scope& GetAllAgentScope() const=0;

        /// Returns the base part of the problem filename.
        virtual std::string GetUnixName() const = 0;

        /// Returns a copy of this class.
        virtual MultiAgentDecisionProcessInterface* Clone() const = 0;

};

#endif /* !_MULTIAGENTDECISIONPROCESSINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
