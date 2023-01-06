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
#ifndef _AGENT_H_
#define _AGENT_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "NamedDescribedEntity.h"
#include "DiscreteEntity.h"


/// Agent represents an agent.
/**Agent is a class that represents an agent, which can be identified
 * by its index. */
class Agent : public NamedDescribedEntity,
              public DiscreteEntity
{
private:    
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    Agent(Index i=INDEX_MAX,
          const std::string &name=std::string("undefined"),
          const std::string &description=std::string("undefined")) :
        NamedDescribedEntity(name, description),
        DiscreteEntity(i){};

};

#endif /* !_AGENT_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
