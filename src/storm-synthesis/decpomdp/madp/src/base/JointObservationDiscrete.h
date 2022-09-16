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
#ifndef _JOINTOBSERVATIONINTEGER_H_
#define _JOINTOBSERVATIONINTEGER_H_ 1

/* the include directives */

#include <iostream>
#include <vector>
#include "Globals.h"
#include "JointObservation.h"
#include "DiscreteEntity.h"

class ObservationDiscrete;

/// JointObservationDiscrete represents discrete joint observations.
class JointObservationDiscrete : public JointObservation,
                                 public DiscreteEntity
{
    private:
    
    
    protected:

    ///Indices of individual observations that make up this joint observation.
    std::vector<Index> _m_oIndexVector;    
    ///Pointers to individual observations that make up this joint observation.
    std::vector<const ObservationDiscrete*> _m_opVector;

    /// Constructs the vector of individual Observation indices from _m_apVector
    std::vector<Index> ConstructIndividualObservationDiscretesIndices() const;
    
public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    JointObservationDiscrete(Index index = INDEX_MAX);
    /// Constructor with an index and a vector of individual observations.
    JointObservationDiscrete(Index index, std::vector<const ObservationDiscrete*> a);
    /// Copy constructor.
    JointObservationDiscrete(const JointObservationDiscrete& a);
    /// Destructor.
    virtual ~JointObservationDiscrete();

    //operators:
    
    /// Adds an individual observation for agentI to this joint observation.
    /** This has to be called ordered: i.e., first for agent 0,
     * then for agent 1, etc.  up to nrAgents. This function is
     * also typically only used to construct the joint
     * observations.*/
    void AddIndividualObservation(const ObservationDiscrete* a, Index agentI);
    
    //get (data) functions:

    /// Get the ObservationDiscretes for this joint action.
    const std::vector<const ObservationDiscrete*>& 
    GetIndividualObservationDiscretes() const
        { return _m_opVector;}
    /// Get the Observation indices for this joint action.
    const std::vector<Index>& GetIndividualObservationDiscretesIndices() const
        { return _m_oIndexVector; }

    /// Returns a pointer to a copy of this class.
    virtual JointObservationDiscrete* Clone() const
        { return new JointObservationDiscrete(*this); }

    //other
    std::string SoftPrint() const;
    std::string SoftPrintBrief() const;
};


#endif /* !_JOINTOBSERVATIONINTEGER_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
