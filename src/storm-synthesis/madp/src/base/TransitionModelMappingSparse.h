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
#ifndef _TRANSITIONMODELMAPPINGSPARSE_H_
#define _TRANSITIONMODELMAPPINGSPARSE_H_ 1

/* the include directives */
#include "Globals.h"
#include "TransitionModelDiscrete.h"
#include "boost/numeric/ublas/matrix_sparse.hpp"

//#include "TGet.h"
class TGet;
class TGet_TransitionModelMappingSparse;

/// TransitionModelMappingSparse implements a TransitionModelDiscrete.
/** Uses sparse matrices. */
class TransitionModelMappingSparse : public TransitionModelDiscrete
{
public:

#if BOOST_1_32_OR_LOWER // they renamed sparse_vector to mapped_vector
    typedef boost::numeric::ublas::sparse_matrix<double> SparseMatrix;
#else    
    typedef boost::numeric::ublas::compressed_matrix<double> SparseMatrix;
#endif    

private:
    
    std::vector<SparseMatrix* > _m_T;

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    TransitionModelMappingSparse(int nrS = 1, int nrJA = 1);

    /// Copy constructor.
    TransitionModelMappingSparse(const TransitionModelMappingSparse& TM);

    /// Destructor.
    ~TransitionModelMappingSparse();    
        
    /// Returns P(s'|s,ja).
    double Get(Index sI, Index jaI, Index sucSI) const
        { return((*_m_T[jaI])(sI,sucSI)); }

    ////data manipulation funtions:
    /// Sets P(s'|s,ja) 
    /** sI, jaI, sucSI, are indices of the state, taken joint action
     * and resulting successor state. prob is the probability. The
     * order of events is s, ja, s', so is the arg. list
     */
    void Set(Index sI, Index jaI, Index sucSI, double prob)
        {
            // make sure probability is not 0
            if(prob > PROB_PRECISION)
                (*_m_T[jaI])(sI,sucSI)=prob;
            // check if we already defined this element, if so remove it
            else if((*_m_T[jaI])(sI,sucSI)>PROB_PRECISION)
                (*_m_T[jaI]).erase_element(sI,sucSI);
        }

    /// Get a pointer to a transition matrix for a particular action.
    const SparseMatrix* GetMatrixPtr(Index a) const
        { return(_m_T.at(a)); }

    /// Returns a pointer to a copy of this class.
    virtual TransitionModelMappingSparse* Clone() const
        { return new TransitionModelMappingSparse(*this); }

    friend class TGet_TransitionModelMappingSparse;

};

#endif /* !_TRANSITIONMODELMAPPINGSPARSE_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
