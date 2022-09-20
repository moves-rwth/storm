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
#ifndef _REWARDMODELDISCRETEINTERFACE_H_
#define _REWARDMODELDISCRETEINTERFACE_H_ 1

/* the include directives */
#include "Globals.h"
#include "QTableInterface.h"

/// RewardModelDiscreteInterface is an interface for discrete reward models.
class RewardModelDiscreteInterface :
    public QTableInterface
{
private:

protected:

public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    RewardModelDiscreteInterface(){};

    /// Destructor.
    virtual ~RewardModelDiscreteInterface(){};

    /// Returns R(s,ja)
    virtual double Get(Index s_i, Index ja_i) const = 0;

    //data manipulation funtions:
    /// Sets R(s_i,ja_i)
    /** Index ja_i, Index s_i, are indices of the state and taken
     * joint action. r is the reward. The order of events is s, ja, so
     * is the arg. list. */
    virtual void Set(Index s_i, Index ja_i, double rew) = 0;

    /// Returns a pointer to a copy of this class.
    virtual RewardModelDiscreteInterface* Clone() const = 0;

    /// Prints a description of *this* to a string.
    virtual std::string SoftPrint() const = 0;

    ///Print *this* to cout.
    void Print() const
        { std::cout << SoftPrint();}

};

#endif /* !_REWARDMODELDISCRETEINTERFACE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***

