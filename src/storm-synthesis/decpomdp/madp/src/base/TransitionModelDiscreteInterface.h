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
#ifndef _TRANSITIONMODELDISCRETEINTERFACE_H_
#define _TRANSITIONMODELDISCRETEINTERFACE_H_ 1

/* the include directives */
#include "Globals.h"
#include "TransitionModel.h"

/// TransitionModelDiscreteInterface represents a discrete transition model.
class TransitionModelDiscreteInterface : public TransitionModel
{
private:

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// Constructor
    TransitionModelDiscreteInterface(){};

    virtual ~TransitionModelDiscreteInterface(){};    
        
    /// Returns P(s'|s,ja).
    virtual double Get(Index sI, Index jaI, Index sucSI) const = 0;

    //data manipulation funtions:
    /// Sets P(s'|s,ja)
    /** sI, jaI, sucSI, are indices of the state, * taken joint action
     * and resulting successor state. prob is * the probability. The
     * order of events is s, ja, s', so is the arg. list
     */
    virtual void Set(Index sI, Index jaI, Index sucSI, double prob) = 0;

    /// Returns a pointer to a copy of this class.
    virtual TransitionModelDiscreteInterface* Clone() const = 0;

};

#endif /* !_TRANSITIONMODELDISCRETEINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
