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
#ifndef _TRANSITIONMODELDISCRETE_H_
#define _TRANSITIONMODELDISCRETE_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix.hpp"
#include "Globals.h"
#include "TransitionModelDiscreteInterface.h"

/// TransitionModelDiscrete represents a discrete transition model.
class TransitionModelDiscrete : public TransitionModelDiscreteInterface
{
private:

    /// The number of states.
    int _m_nrStates;
    /// The number of joint actions.
    int _m_nrJointActions;
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// Constructor with the dimensions of the transition model.
    TransitionModelDiscrete(int nrS = 1, int nrJA = 1);

    virtual ~TransitionModelDiscrete();    

    /// Sample a successor state.
    Index SampleSuccessorState(Index sI, Index jaI);
       
    /// Returns a pointer to a copy of this class.
    virtual TransitionModelDiscrete* Clone() const = 0;

    /// SoftPrints tabular transition model.
    std::string SoftPrint() const;
};

#endif /* !_TRANSITIONMODELDISCRETE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
