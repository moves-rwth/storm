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
#ifndef _TRANSITIONMODELMAPPING_H_
#define _TRANSITIONMODELMAPPING_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix.hpp"
#include "Globals.h"
#include "TransitionModelDiscrete.h"
//#include "TGet.h"
class TGet;
class TGet_TransitionModelMapping;

/// TransitionModelMapping implements a TransitionModelDiscrete.
/** Uses full matrices. */
class TransitionModelMapping : public TransitionModelDiscrete
{
public:

    typedef boost::numeric::ublas::matrix<double> Matrix;

private:

    std::vector<Matrix* > _m_T;

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    TransitionModelMapping(int nrS = 1, int nrJA = 1);

    /// Copy constructor.
    TransitionModelMapping(const TransitionModelMapping& TM);

    /// Destructor.
    ~TransitionModelMapping();    
        
    /// Returns P(s'|s,ja)
    double Get(Index sI, Index jaI, Index sucSI) const
        { return((*_m_T[jaI])(sI,sucSI)); }

    //data manipulation funtions:
    ///Sets P(s'|s,ja)
    /**sI, jaI, sucSI, are indices of the state,
     * taken joint action and resulting successor state. prob is 
     * the probability. The order of events is s, ja, s', so is the arg. list
     */
    void Set(Index sI, Index jaI, Index sucSI, double prob)
        { (*_m_T[jaI])(sI,sucSI)=prob; }

    /// Get a pointer to a transition matrix for a particular action.
    const Matrix* GetMatrixPtr(Index a) const
        { return(_m_T.at(a)); }

    /// Returns a pointer to a copy of this class.
    virtual TransitionModelMapping* Clone() const
        { return new TransitionModelMapping(*this); }

    friend class TGet_TransitionModelMapping;
};

#endif /* !_TRANSITIONMODELMAPPING_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
