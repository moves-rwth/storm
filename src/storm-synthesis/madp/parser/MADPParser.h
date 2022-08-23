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
#ifndef _MADPPARSER_H_
#define _MADPPARSER_H_ 1

/* the include directives */
#include <iostream>
#include <string.h>
#include "Globals.h"

class DecPOMDPDiscrete;
class FactoredDecPOMDPDiscrete;
class TOIDecPOMDPDiscrete;
class TOIDecMDPDiscrete;
class TOIFactoredRewardDecPOMDPDiscrete;
class TOICompactRewardDecPOMDPDiscrete;
class POMDPDiscrete;

/// MADPParser is a general class for parsers in MADP.
/** It is templatized to allow for different parsers to be
 * implemented. When constructed with particular model, the proper
 * parser is instantiated, and its Parse() function is called (see
 * ParserInterface).
 */
class MADPParser 
{
private:    

    /// Parse a DecPOMDPDiscrete using ParserDPOMDPFormat_Spirit.
    void Parse(DecPOMDPDiscrete *model);
    void Parse(TOIDecPOMDPDiscrete *model);
    void Parse(TOIDecMDPDiscrete *model);
    void Parse(TOIFactoredRewardDecPOMDPDiscrete *model);
    void Parse(TOICompactRewardDecPOMDPDiscrete *model);
    void Parse(FactoredDecPOMDPDiscrete *model);
    void Parse(POMDPDiscrete *model);

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// Constructor, on return the model has been parsed.
    template <class A>
    MADPParser(A* model){ Parse(model); }

    /// Destructor.
    ~MADPParser(){};

};

#endif /* !_MADPPARSER_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
