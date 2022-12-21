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
#ifndef _ACTION_H_
#define _ACTION_H_ 1

/* the include directives */
#include <iostream>
#include <string>

#include "NamedDescribedEntity.h"

/// Action is a class that represent actions.
class Action : public NamedDescribedEntity
{
    private:
    
    protected:
    
    public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    Action(const std::string &name=std::string("undefined"),
           const std::string &description=std::string("undefined")) :
        NamedDescribedEntity(name, description){};

};

#endif /* !_ACTION_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
