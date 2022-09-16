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
#ifndef _EDEADLINE_H_
#define _EDEADLINE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "E.h"

/// EDeadline represents a deadline exceeded expection.
class EDeadline : public E 
{
    private:    
    
    protected:
    
    public:

    double _m_expectedTimeForCompletion;


    // Constructor, destructor and copy assignment.    
    /// Constructor with a C-style string
    EDeadline(const char* arg, double expectedTimeForCompletion=0):
        E(arg),
        _m_expectedTimeForCompletion(expectedTimeForCompletion)
        {}
    /// Constructor with an STL string
    EDeadline(std::string arg, double expectedTimeForCompletion=0):
        E(arg),
        _m_expectedTimeForCompletion(expectedTimeForCompletion)
        {}
    /// Constructor with an STL stringstream
    EDeadline(const std::stringstream& arg, double expectedTimeForCompletion=0) :
        E(arg),
        _m_expectedTimeForCompletion(expectedTimeForCompletion)
        {}

};


#endif /* !_EDEADLINE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
