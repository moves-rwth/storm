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
#ifndef _JOINTACTIONINTEGER_H_
#define _JOINTACTIONINTEGER_H_ 1

/* the include directives */

#include <iostream>
#include "Globals.h"
#include "JointAction.h"
#include "DiscreteEntity.h"

class ActionDiscrete;

/// JointActionDiscrete represents discrete joint actions.
class JointActionDiscrete : public JointAction,
                            public DiscreteEntity
{
    private:
    
    /// Pointers to the individual actions that make up this joint action.
    std::vector<Index> _m_aIndexVector;    
    /// Indices of the individual actions that make up this joint action.
    std::vector<const ActionDiscrete*> _m_apVector;

    /// Constructs the vector of individual Action indices from _m_apVector
    std::vector<Index> ConstructIndividualActionDiscretesIndices() const;

    protected:


    
    public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    JointActionDiscrete(Index index = INDEX_MAX);
    /// Constructor with an index and a vector of individual actions.
    JointActionDiscrete(Index index, std::vector<const ActionDiscrete*> a);
    /// Copy constructor.
    JointActionDiscrete(const JointActionDiscrete& a);
    /// Destructor.
    virtual ~JointActionDiscrete();

    //operators:

    //data manipulation (set) functions
    /// Deletes the individual actions to which this joint action points.
    /** This function will typically *NOT* be used: normally multiple
     * joint actions share their individual actions...*/
    void DeleteIndividualActions();

    /// Adds an individual action for agentI to this joint action.
    /** This has to be called ordered: i.e., first for agent 0, then
     * for agent 1, etc.  up to nrAgents. This function is also
     * typically only used to construct the joint actions.*/
    void AddIndividualAction(const ActionDiscrete* a, Index agentI);
    
    //get (data) functions:
    /// Get the ActionDiscretes for this joint action.
    const std::vector<const ActionDiscrete*>& GetIndividualActionDiscretes() const
    { return(_m_apVector);}
    /// Get the Action indices for this joint action.
    const std::vector<Index>& GetIndividualActionDiscretesIndices() const
    { return(_m_aIndexVector);}

    /// Returns a pointer to a copy of this class.
    virtual JointActionDiscrete* Clone() const
        { return new JointActionDiscrete(*this); }

    //other
    std::string SoftPrint() const;
    std::string SoftPrintBrief() const;
};


#endif /* !_JOINTACTIONINTEGER_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
