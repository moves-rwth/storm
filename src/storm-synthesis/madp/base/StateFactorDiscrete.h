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
#ifndef _STATEFACTORDISCRETE_H_
#define _STATEFACTORDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "NamedDescribedEntity.h"

/**\brief StateFactorDiscrete is a class that represents a state variable, or
 * factor. 
 *
 * It has a name and description (handled by NamedDescribedEntity), as well as
 * a domain: the list of discrete values it can take. These domain values
 * are indexed and associated with a string.
 *
 * E.g. a state variable 'color' could have domain
 * 0-'red', 1-'blue', 2-'yellow'.
 *
 * And Xpos could have domain:
 * 0 -'-2', 1-'-1', 2-'0', 3-'+1', 4-'+2'
 *
 * In the future we might include special state factor types for numerical 
 * domains(?).
 * */
class StateFactorDiscrete : public NamedDescribedEntity
{
    private:    
        //not necessary/practical.
        //the index of this state-factor
        //Index _m_index;
        
        ///The size of the domain
        size_t _m_domainSize;
        ///The vector containing the domain values.
        /**E.g. for factor 'color' this contains 'blue', 'red', etc.
         */ 
        std::vector<std::string> _m_domainValues;

        //name and description are stored in NamedDescribedEntity
    
    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        StateFactorDiscrete(const std::string &n="undef. name",
                            const std::string &d="undef. descr.");
        ///Constructor specifying the number of values.
        /**This constructs \a n unnamed values.*/
        StateFactorDiscrete(size_t nrVs, 
                            const std::string &n="undef. name",
                            const std::string &d="undef. descr.");
        /// Copy constructor.
        StateFactorDiscrete(const StateFactorDiscrete& a);
        /// Destructor.
        ~StateFactorDiscrete();
        /// Copy assignment operator
        StateFactorDiscrete& operator= (const StateFactorDiscrete& o);

        //operators:

        //data manipulation (set) functions:
        ///Adds a value to this state factor and returns the new index
        /**I.e., the domain index or 'state factor value' index.
         */
        Index AddStateFactorValue(const std::string &v="rec.undef.by StateFactorDiscrete");
        
        //get (data) functions:
        std::string GetStateFactorValue(Index domainI) const
        {return _m_domainValues.at(domainI); }
       
        ///Soft prints this state factor
        std::string SoftPrint() const;
};


#endif /* !_STATEFACTORDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
