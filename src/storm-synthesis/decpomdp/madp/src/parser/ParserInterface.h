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
#ifndef _PARSERINTERFACE_H_
#define _PARSERINTERFACE_H_ 1

/* the include directives */
#include <iostream>

/// ParserInterface is an interface for parsers.
class ParserInterface 
{
private:
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    ParserInterface(){};

    virtual ~ParserInterface(){};
    
    virtual void Parse() = 0;

};


#endif /* !_PARSERINTERFACE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
