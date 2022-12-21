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
#ifndef _PARSERPOMDPDISCRETE_H_
#define _PARSERPOMDPDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"

#include "POMDPDiscrete.h"
#include "ParserInterface.h"


/**\brief \deprecated ParserPOMDPDiscrete is a parser for
 * POMDPDiscrete that makes use of Tony Cassandra's POMDPsolve to do the parsing.*/
class ParserPOMDPDiscrete :
    public ParserInterface
{
private:    

    POMDPDiscrete *_m_problem;

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    ParserPOMDPDiscrete(POMDPDiscrete *problem);

    void Parse();

};


#endif /* !_PARSERPOMDPDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
