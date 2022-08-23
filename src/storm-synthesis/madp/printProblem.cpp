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

// Very simple programs that only uses the Parser and Base libraries.

#include <iostream>

using namespace std;

#include "base/POMDPDiscrete.h"
#include "parser/MADPParser.h"

const char *argp_program_version = "printProblem";

// #include "argumentHandlersPostChild.h"

int main(int argc, char **argv)
{
    string dpomdpFile = "problems/dectiger.dpomdp";
    POMDPDiscrete* pomdp = new POMDPDiscrete("","",dpomdpFile);
    MADPParser parser(pomdp);
    cout << pomdp->SoftPrint() << endl;

    return(0);
}
