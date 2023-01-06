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
#ifndef _EVENTOBSERVATIONMODELMAPPINGSPARSE_H_
#define _EVENTOBSERVATIONMODELMAPPINGSPARSE_H_ 1

/* the include directives */
#include "Globals.h"
#include "ObservationModelDiscrete.h"
#include "boost/numeric/ublas/matrix_sparse.hpp"
class OGet;
class OGet_ObservationModelMapping;

/// EventObservationModelMappingSparse implements an ObservationModelDiscrete.
/** Uses sparse matrices. */
class EventObservationModelMappingSparse : 
    public ObservationModelDiscrete
{
public:
#if BOOST_1_32_OR_LOWER // they renamed sparse_vector to mapped_vector
    typedef boost::numeric::ublas::sparse_matrix<double> SparseMatrix;
#else    
    typedef boost::numeric::ublas::compressed_matrix<double> SparseMatrix;
#endif    


private:

    std::vector< std::vector<SparseMatrix* > > _m_O;
    
protected:
    
public:
    // Constructor, destructor and copy assignment.
    /// default Constructor
    EventObservationModelMappingSparse(int nrS = 1, int nrJA = 1, int nrJO = 1);

    /// Copy constructor.
    EventObservationModelMappingSparse(const EventObservationModelMappingSparse& OM);
    /// Destructor.
    ~EventObservationModelMappingSparse();
    
    /// Returns P(jo|ja,s')
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
        {
            // make sure probability is not 0
            if(prob > PROB_PRECISION)
                (*_m_O[ja_i][jo_i])(s_i,suc_s_i)=prob;
            // check if we already defined this element, if so remove it
            else if((*_m_O[ja_i][jo_i])(s_i,suc_s_i)>PROB_PRECISION)
                (*_m_O[ja_i][jo_i]).erase_element(s_i,suc_s_i);
        }
        
    /// Get a pointer to a transition matrix for a particular action / observation pair.
    const SparseMatrix* GetMatrixPtr(Index a, Index jo_i) const
        { return(_m_O.at(a).at(jo_i)); }
        
    /// Returns a pointer to a copy of this class.
    virtual EventObservationModelMappingSparse* Clone() const
        { return new EventObservationModelMappingSparse(*this); }

    friend class OGet_EventObservationModelMappingSparse;
};

#endif /* !_EVENTOBSERVATIONMODELMAPPINGSPARSE_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

