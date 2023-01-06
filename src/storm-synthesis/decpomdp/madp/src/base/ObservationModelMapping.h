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
#ifndef _OBSERVATIONMODELMAPPING_H_
#define _OBSERVATIONMODELMAPPING_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix.hpp"
#include "Globals.h"
#include "ObservationModelDiscrete.h"
class OGet;
class OGet_ObservationModelMapping;

/// ObservationModelMapping implements an ObservationModelDiscrete.
/** Uses full matrices. */
class ObservationModelMapping : 
    public ObservationModelDiscrete
{
public:

    typedef boost::numeric::ublas::matrix<double> Matrix;

private:

    std::vector<Matrix* > _m_O;
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    ObservationModelMapping(int nrS = 1, int nrJA = 1, int nrJO = 1);

    /// Copy constructor.
    ObservationModelMapping(const ObservationModelMapping& OM);
    /// Destructor.
    ~ObservationModelMapping();
    
    /// Returns P(jo|ja,s')
    double Get(Index ja_i, Index suc_s_i, Index jo_i) const
        { return((*_m_O[ja_i])(suc_s_i,jo_i)); }

    //data manipulation funtions:
    /// Sets P(o|ja,s')
    /** jo_i, Index ja_i, Index suc_s_i, are indices of the joint
     * observation, taken joint action and resulting successor
     * state. prob is the probability. The order of events is ja, s',
     * o, so is the arg. list
     */
    void Set(Index ja_i, Index suc_s_i, Index jo_i, double prob)
        {  (*_m_O[ja_i])(suc_s_i,jo_i)=prob; }
        
    /// Get a pointer to a transition matrix for a particular action.
    const Matrix* GetMatrixPtr(Index a) const
        { return(_m_O.at(a)); }

    /// Returns a pointer to a copy of this class.
    virtual ObservationModelMapping* Clone() const
        { return new ObservationModelMapping(*this); }

    friend class OGet_ObservationModelMapping;
};

#endif /* !_OBSERVATIONMODELMAPPING_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

