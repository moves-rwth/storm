/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * João Messias 
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */

/* Only include this header file once. */
#ifndef _EVENTOBSERVATIONMODELMAPPING_H_
#define _EVENTOBSERVATIONMODELMAPPING_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix.hpp"
#include "Globals.h"
#include "ObservationModelDiscrete.h"
class OGet;
class OGet_EventObservationModelMapping;

/// EventObservationModelMapping implements an ObservationModelDiscrete which
/// depends not only on the resulting state but also on the current state of
/// the system, i.e. P(o(k+1) | s(k), ja(k), s(k+1))
class EventObservationModelMapping : 
    public ObservationModelDiscrete
{
public:

    typedef boost::numeric::ublas::matrix<double> Matrix;

private:

  std::vector< std::vector<Matrix* > > _m_O;
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    EventObservationModelMapping(int nrS = 1, int nrJA = 1, int nrJO = 1);

    /// Copy constructor.
    EventObservationModelMapping(const EventObservationModelMapping& OM);
    /// Destructor.
    ~EventObservationModelMapping();
    
    double Get(Index ja_i, Index suc_s_i, Index jo_i) const
        { throw E("Cannot refer to an Event Observation Model with (o,s',a). Use Get(s,a,s',o) instead."); return(0); }
    double Get(Index s_i, Index ja_i, Index suc_s_i, Index jo_i) const
        { return (*_m_O[ja_i][jo_i])(s_i,suc_s_i); }
    //data manipulation funtions:
    /// Sets P(o|s,ja,s')
    /** jo_i, Index s_i, Index ja_i, Index suc_s_i, are indices of the joint
     * observation, taken joint action and resulting successor
     * state. prob is the probability. The order of events is s, ja, s',
     * o, so is the arg. list
     */
    void Set(Index ja_i, Index suc_s_i, Index jo_i, double prob)
        { throw E("Cannot refer to an Event Observation Model with (o,s',a,p). Use Set(s,a,s',o,p) instead."); }
    void Set(Index s_i, Index ja_i, Index suc_s_i, Index jo_i, double prob)
        { (*_m_O[ja_i][jo_i])(s_i,suc_s_i)=prob; }
    
    const Matrix* GetMatrixPtr(Index a, Index jo_i) const
        { return(_m_O.at(a).at(jo_i)); }

    /// Returns a pointer to a copy of this class.
    virtual EventObservationModelMapping* Clone() const
        { return new EventObservationModelMapping(*this); }

    friend class OGet_EventObservationModelMapping;
};

#endif /* !_EVENTOBSERVATIONMODELMAPPING_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

