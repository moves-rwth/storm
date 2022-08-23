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
#ifndef _OBSERVATIONMODELDISCRETE_H_
#define _OBSERVATIONMODELDISCRETE_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "ObservationModelDiscreteInterface.h"

/// ObservationModelDiscrete represents a discrete observation model.
class ObservationModelDiscrete : public ObservationModelDiscreteInterface
{
private:    
    
    /// The number of states.
    int _m_nrStates;
    /// The number of joint actions.
    int _m_nrJointActions;    
    /// The number of joint observations
    int _m_nrJointObservations;

protected:
    
public:
    /// Constructor with the dimensions of the observation model.
    ObservationModelDiscrete(int nrS = 1, int nrJA = 1, int nrJO = 1);

    /// Destructor.
    virtual ~ObservationModelDiscrete();
    
    /// Sample a joint observation.
    Index SampleJointObservation(Index jaI, Index sucI);

    /// Sample a joint observation.
    Index SampleJointObservation(Index sI, Index jaI, Index sucI);

    /// Returns a pointer to a copy of this class.
    virtual ObservationModelDiscrete* Clone() const = 0;

    /// SoftPrints tabular observation model.
    std::string SoftPrint() const;
};


#endif /* !_OBSERVATIONMODELDISCRETE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
