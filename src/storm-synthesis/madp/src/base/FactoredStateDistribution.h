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
#ifndef _FACTOREDSTATEDISTRIBUTION_H_
#define _FACTOREDSTATEDISTRIBUTION_H_ 1

/* the include directives */
#include "Globals.h"
#include "StateDistribution.h"

/** \brief FactoredStateDistribution is a class that represents 
 * a base class for factored state distributions.
 * 
 * A FactoredStateDistribution is a distribution over factored states.
 * Such a distribution might be flat (i.e. any distribution can be represented),
 * but typically this is infeasible, and therefore we use some other 
 * approximate representation.
 *
 * The actual representation is determined by the derived class.
 * */
class FactoredStateDistribution 
    : public StateDistribution
{
    private:    

    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// Destructor.
        virtual ~FactoredStateDistribution(){};
/*
        /// (default) Constructor
        FactoredStateDistribution();
        /// Copy constructor.
        FactoredStateDistribution(const FactoredStateDistribution& a);
        /// Copy assignment operator
        FactoredStateDistribution& operator= (const FactoredStateDistribution& o);
*/
        //operators:

        //data manipulation (set) functions:
        virtual void SetUniform() = 0;
        
        //get (data) functions:
        virtual double GetProbability(const std::vector<Index>& sfacValues) const = 0;

        /// Returns a pointer to a copy of this class.
        virtual FactoredStateDistribution* Clone() const = 0;

        virtual std::vector<Index> SampleState() const = 0;
        virtual std::string SoftPrint() const = 0;
};


#endif /* !_FACTOREDSTATEDISTRIBUTION_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
