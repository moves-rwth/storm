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
#ifndef _STRINGTOOLS_H_
#define _STRINGTOOLS_H_ 1

/* the include directives */
#include "Globals.h"
#include <string>

/** \brief StringTools is a namespace that contains utility functions for
 * strings.
 * */
namespace StringTools 
{
    std::string Append(const std::string& ioString, int inValue);
    std::string Trim(const std::string& ioString);
};


#endif /* !_STRINGTOOLS_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
