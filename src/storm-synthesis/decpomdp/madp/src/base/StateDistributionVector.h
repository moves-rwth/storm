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
#ifndef _STATEDISTRIBUTIONVECTOR_H_
#define _STATEDISTRIBUTIONVECTOR_H_ 1

/* the include directives */
#include "Globals.h"
#include "StateDistribution.h"


namespace {
    typedef std::vector<double> SDV;
}

/** \brief StateDistributionVector represents a probability
 * distribution over states as a vector of doubles. */
class StateDistributionVector : 
    public StateDistribution,
    public SDV
{
    private:    
    
    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        StateDistributionVector()
            :
                SDV()
        {};         

        /// Copy constructor.
        StateDistributionVector(const StateDistributionVector& a)
            :
                SDV(a)
        {};
        StateDistributionVector(const SDV& a)
            :
                SDV(a)
        {};
        /// Destructor.
        ~StateDistributionVector(){};
        /// Copy assignment operator
        StateDistributionVector& operator= (const StateDistributionVector& o)
        {
            if (this == &o) return *this;   // Gracefully handle self assignment
            this->SDV::operator=(o);
            return *this;
        }
        StateDistributionVector& operator= (const SDV& o)
        {
            if (this == &o) return *this;   // Gracefully handle self assignment
            this->SDV::operator=(o);
            return *this;
        }

        //operators:

        //data manipulation (set) functions:
        
        //get (data) functions:
        virtual double GetProbability( Index sI) const
        { return this->at(sI); }

        virtual std::vector<double> ToVectorOfDoubles() const
        { return *this; }
        
        virtual size_t GetNrStates() const { return this->size(); }

        /// Returns a pointer to a copy of this class.
        virtual StateDistributionVector* Clone() const
        { return new StateDistributionVector(*this); }

        virtual std::string SoftPrint() const
        { return SoftPrintVector( *((SDV*)this) ); }
};


#endif /* !_STATEDISTRIBUTIONVECTOR_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
