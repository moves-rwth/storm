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
#ifndef _STATEDISCRETE_H_
#define _STATEDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include <string>
#include "Globals.h"

#include "State.h"
#include "DiscreteEntity.h"


/// StateDiscrete represents discrete states.
/** 
 * StateDiscrete is a class that represent states in a discrete state
 * set, which are identified by their index.  */
class StateDiscrete : public State,
                      public DiscreteEntity
{
private:    
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    StateDiscrete(Index i=INDEX_MAX,
                  const std::string &name=std::string("undefined"),
                  const std::string &description=std::string("undefined")) :
        State(name, description),
        DiscreteEntity(i){};

};

#endif /* !_STATEDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
