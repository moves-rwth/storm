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
#ifndef _QTABLEINTERFACE_H_
#define _QTABLEINTERFACE_H_ 1

/* the include directives */
#include "Globals.h"

/** \brief QTableInterface is the abstract base class for Q(., a) functions.
 * It represents functions mapping from some domain (e.g. states, local states,
 * histories, etc.) and some action domain (individual, joint or group actions)
 * to a real number representing some form of payoff (long term reward, or 
 * immediate reward).
 *
 * Note the argument of the functions defined here assume Q(s,a), but is
 * should be clear that for s_i any general domain index may be used.
 *
 * */
class QTableInterface 
{
    private:    
    
    protected:
    
    public:
        virtual double Get(Index s_i, Index ja_i) const = 0;
        virtual void Set(Index s_i, Index ja_i, double rew) = 0;

        virtual ~QTableInterface(){};

        /// Returns a pointer to a copy of this class.
        virtual QTableInterface* Clone() const = 0;

};


#endif /* !_QTABLEINTERFACE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
