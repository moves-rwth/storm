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
#ifndef _STATEDISTRIBUTION_H_
#define _STATEDISTRIBUTION_H_ 1

/* the include directives */
#include "Globals.h"

/** \brief StateDistribution is an interface for probability
 * distributions over states. */
class StateDistribution 
{
    private:    
    
    protected:
    
    public:
        virtual ~StateDistribution(){};

        //operators:

        //data manipulation (set) functions:
        
        //get (data) functions:
        virtual double GetProbability( Index sI) const = 0;
        virtual std::vector<double> ToVectorOfDoubles() const = 0;
        virtual size_t GetNrStates() const = 0;

        /// Returns a pointer to a copy of this class.
        virtual StateDistribution* Clone() const = 0;

        virtual std::string SoftPrint() const = 0;
};


#endif /* !_STATEDISTRIBUTION_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
