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
#ifndef _REWARDMODEL_H_
#define _REWARDMODEL_H_ 1

/* the include directives */
#include "Globals.h"
#include "RewardModelDiscreteInterface.h"

/// RewardModel represents the reward model in a decision process.
class RewardModel :
    public RewardModelDiscreteInterface
{
private:
    /// The number of states.
    size_t _m_nrStates;
    /// The number of joint actions.
    size_t _m_nrJointActions;    
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    RewardModel(size_t nrS=1, size_t nrJA=1) :
        _m_nrStates(nrS),
        _m_nrJointActions(nrJA)
        {};

    size_t GetNrStates() const { return(_m_nrStates); }
    size_t GetNrJointActions() const { return(_m_nrJointActions); }

    /// Destructor.
    virtual ~RewardModel(){};

};

#endif /* !_REWARDMODEL_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***

