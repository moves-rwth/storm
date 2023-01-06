/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Philipp Robbel 
 *
 * For contact information please see the included AUTHORS file.
 */

/* Only include this header file once. */
#ifndef _CPDKRONECKERDELTA_H_
#define _CPDKRONECKERDELTA_H_ 1

/* the include directives */
#include "Globals.h"
#include "CPDDiscreteInterface.h"

/// CPDKroneckerDelta implements a Kronecker delta-style CPD.
/**Centers probability mass of 1 on x == y.*/
class CPDKroneckerDelta : public CPDDiscreteInterface
{
public:
    /// Constructor.
    CPDKroneckerDelta() {}

    /// Returns \f$ P(x|y) \f$
    double Get(Index x, Index y) const
    { return x == y ? 1.0 : 0.0; }
    
    /// Returns an (index of a) x drawn according to \f$ P(x|y) \f$
    Index Sample (Index y) const
    { return y; }

    ///Sets P(x|y)
    /**Doesn't apply to Kronecker delta function.*/
    void Set(Index x, Index y, double prob)
    { std::cerr << "CPDKroneckerDelta::Set is unnecessary." << std::endl; }

    void SanityCheck() const
    { std::cerr << "CPDKroneckerDelta::SanityCheck is unnecessary." << std::endl; }

    /// Returns a pointer to a copy of this class.
    CPDKroneckerDelta* Clone() const
    { return new CPDKroneckerDelta(*this); }

    std::string SoftPrint() const
    { return "Kronecker delta-style CPD. Returns 1 iff x==y"; }

};

#endif /* !_CPDKRONECKERDELTA_H_ */
