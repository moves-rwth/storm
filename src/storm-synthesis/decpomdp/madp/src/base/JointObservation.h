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
#ifndef _JOINTOBSERVATION_H_
#define _JOINTOBSERVATION_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"

/// JointObservation is represents joint observations.
class JointObservation
{
    private:    
    
    protected:
    
    public:

    /// Destructor.
    virtual ~JointObservation() {};

    /// Returns a pointer to a copy of this class.
    virtual JointObservation* Clone() const = 0;

    virtual std::string SoftPrint() const = 0;
    virtual std::string SoftPrintBrief() const = 0;
    virtual void Print() const { std::cout << SoftPrint();}
    virtual void PrintBrief() const { std::cout << SoftPrintBrief();}
};


#endif /* !_JOINTOBSERVATION_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
