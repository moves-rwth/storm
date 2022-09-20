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

#include "StringTools.h"

using namespace std;

namespace StringTools{
    
string Append(const std::string& ioString, int inValue)
{
    std::ostringstream o;
    o << ioString << inValue;
    return o.str();
}

string Trim(const std::string& ioString)
{
    string trimmed = ioString;
    size_t pos = trimmed.find_last_not_of(" \t");
    if(pos < trimmed.length()-1 && pos > 0)
      trimmed.erase(pos+1);
    return trimmed;
}

}

