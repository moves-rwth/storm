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
#ifndef _DISCRETEENTITY_H_
#define _DISCRETEENTITY_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"

/// DiscreteEntity is a general class for tracking discrete entities.
/**DiscreteEntity represents entities in discrete spaces, that hence
 * can be represented by an index. For example, actions in a finite
 * action space. */
class DiscreteEntity 
{
private:
    

    /// The index of this discrete entity.
    Index _m_index;

protected:

public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    DiscreteEntity(Index i=INDEX_MAX) : _m_index(i){}

    /// Destructor.
    virtual ~DiscreteEntity(){}

    /// Return this DiscreteEntity's index.
    Index GetIndex() const { return(_m_index); }

    /// Set this DiscreteEntity's index.
    void SetIndex(Index i) { _m_index=i; }

    /// The less (<) operator. This is needed to put DiscreteEntities in a set.
    bool operator< (const DiscreteEntity& a) const {
        return( _m_index < a._m_index );}

};


#endif /* !_DISCRETEENTITY_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
