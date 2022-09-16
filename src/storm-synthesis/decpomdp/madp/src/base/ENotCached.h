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
#ifndef _ENOTCACHED_H_
#define _ENOTCACHED_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "E.h"

/// ENotCached represents an invalid index exception.
class ENotCached : public E 
{
    private:    
    
    protected:
    
    public:
    // Constructor, destructor and copy assignment.    
    /// Constructor with a C-style string
    ENotCached(const char* arg):E(arg) {}
    /// Constructor with an STL string
    ENotCached(std::string arg):E(arg) {}
    /// Constructor with an STL stringstream
    ENotCached(const std::stringstream& arg) : E(arg) {}

};


#endif /* !_ENOTCACHED_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
