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
#ifndef _INDEXTOOLS_H_
#define _INDEXTOOLS_H_ 1

/* the include directives */
#include <iostream>
#include "Globals.h"
#include "Scope.h"

template <typename T2,typename T1>
std::vector<T2> convertVector(std::vector<T1> v)
{
    std::vector<T2> r;
    for (typename std::vector<T1>::iterator it=v.begin();it!=v.end();it++)
        r.push_back(*it);
    return r;
}

/// IndexTools contains functionality for manipulating indices.
/** A detailed description of how joint indices etc are constructed,
 * see doc/manually_maintained/MADPToolbox-Histories+indices.ps.gz .
 */
namespace IndexTools {
    /// Increments index which ranges over nrElems
    bool Increment(Index& index, size_t nrElems);

    /// Increments vector of indices that range over nrElems
    bool Increment(std::vector<Index>& indexVec, const std::vector<size_t>& nrElems );

    /// Convert individual to joint indices.
    Index IndividualToJointIndices(const std::vector<Index>& indices, 
            const std::vector<size_t>& nrElems);
    /// Convert individual to joint indices. Only uses first n entries of vecs.
    Index IndividualToJointIndices(const std::vector<Index>& indices, 
            const std::vector<size_t>& nrElems, size_t n);
    /// A variant that takes an array instead of a vector for extra speed.
    Index IndividualToJointIndicesArray(const Index* indices,
            const std::vector<size_t>& nrElems); 
    ///A variant that takes a cached step_size vector for extra speed.
    Index IndividualToJointIndicesStepSize(const std::vector<Index>& indices, 
            const std::vector<size_t>& step_size);
    ///A variant that takes a cached step_size array for extra speed.
    Index IndividualToJointIndicesStepSize(const std::vector<Index>& indices, 
            const size_t * step_size);
    /** \brief A variant that 1) takes an array instead of a vector 
     * and 2) takes a cached step_size array for extra speed.*/
    Index IndividualToJointIndicesArrayStepSize(const Index* indices,
            const size_t * step_size, size_t vec_size); 

    /// A variant with a step_size vector.
    Index IndividualToJointIndicesArrayStepSize(const Index* indices,
            const std::vector<size_t> &step_size, size_t vec_size); 

    ///Convert individual to joint indices. 
    std::vector<Index> JointToIndividualIndices(Index jointI, 
            const std::vector<size_t>& nrElems );
    ///Convert individual to joint indices. 
    void JointToIndividualIndices(Index jointI, 
            const std::vector<size_t>& nrElems, std::vector<Index>& result );
    /** \brief Convert individual to joint indices - taking the
     * stepSize array as an argument. */
    std::vector<Index> JointToIndividualIndicesStepSize(Index jointI, 
            const size_t * stepSize, size_t vec_size ) ;
    /** Convert individual to joint indices, taking the
     * stepSize array as an argument. avoids return by value. */
    void  JointToIndividualIndicesStepSize(Index jointI, 
            const size_t * stepSize, size_t vec_size, std::vector<Index>& result ) ;
    /** \brief Convert individual to joint indices - taking the
     * stepSize array as an argument. */
    std::vector<Index> JointToIndividualIndicesStepSize(
            Index jointI, 
            const std::vector<size_t> &stepSize, 
            size_t  vec_size  
            ) ;

    std::vector<Index> JointToIndividualIndicesStepSize(
            Index jointI, 
            const std::vector<size_t> &stepSize
            );
    /** \brief Convert individual to joint indices - taking the
     * stepSize array as an argument and returning a pointer to a
     * array.
     *
     * Note: the returned array is allocate with new, so it must be
     * freed.*/
    const Index * JointToIndividualIndicesArrayStepSize(Index jointI, 
            const size_t * stepSize, size_t vec_size ) ;

    /** \brief Calculates the step size array for nrElems. (so is of
     * the same size as nrElems).
     */
    size_t * CalculateStepSize(const std::vector<size_t>& nrElems);
    /** \brief Calculates the step size vector for nrElems. (so is of
     * the same size as nrElems).
     */
    std::vector<size_t> CalculateStepSizeVector(const std::vector<size_t>& nrElems);
    /**\brief Calculates the step size array from the first n entries 
     * of nrElems. 
     * (so the array's size is n).
     */
    size_t * CalculateStepSize(const std::vector<size_t>& nrElems, size_t n);

    // LIndex versions of all functions

    /// LIndex equivalent function.
    bool Increment(LIndex& index, LIndex nrElems );
    /// LIndex equivalent function.
    bool Increment(std::vector<LIndex>& indexVec, const std::vector<LIndex>& nrElems );

    /// LIndex equivalent function.
    LIndex IndividualToJointIndices(const std::vector<LIndex>& indices, 
            const std::vector<LIndex>& nrElems) ;
    /// LIndex equivalent function.
    LIndex IndividualToJointIndicesArray(LIndex* indices,
            const std::vector<LIndex>& nrElems); 
    /// LIndex equivalent function.
    LIndex IndividualToJointIndicesStepSize(const std::vector<LIndex>& indices, 
            const std::vector<LIndex>& step_size) ;
    /// LIndex equivalent function.
    LIndex IndividualToJointIndicesStepSize(const std::vector<LIndex>& indices, 
            const LIndex * step_size) ;
    /// LIndex equivalent function.
    LIndex IndividualToJointIndicesArrayStepSize(LIndex* indices,
            const LIndex * step_size, size_t vec_size); 
    /// LIndex equivalent function.
    LIndex IndividualToJointIndicesArrayStepSize(LIndex* indices,
            const std::vector<LIndex> &step_size, size_t vec_size);
    /// LIndex equivalent function.
    const LIndex * JointToIndividualIndicesArrayStepSize(
            LIndex jointI, 
            const LIndex * stepSize, size_t vec_size ) ;
    /// LIndex equivalent function.
    std::vector<LIndex> JointToIndividualIndicesStepSize(LIndex jointI, 
            const LIndex * stepSize, size_t vec_size ) ;
    /// LIndex equivalent function.
    std::vector<LIndex> JointToIndividualIndicesStepSize(LIndex jointI, 
            const std::vector<LIndex> &stepSize, size_t vec_size ) ;
    std::vector<LIndex> JointToIndividualIndicesStepSize(LIndex jointI, 
            const std::vector<LIndex> &stepSize);
    /// LIndex equivalent function.
    std::vector<LIndex> JointToIndividualIndices(LIndex jointI, 
            const std::vector<LIndex>& nrElems ) ;
    /// LIndex equivalent function.
    LIndex * CalculateStepSize(const std::vector<LIndex>& nrElems);
    /// LIndex equivalent function.
    std::vector<LIndex> CalculateStepSizeVector(const std::vector<LIndex>& nrElems);

    /** \brief Computation of a index for (joint) actionObservations 
     *
     * ActionObservation indices (aoI's) are used as the basis for indexing
     * (Joint)ActionObservationHistories.
     * This function computes them.
     *
     * \sa manually maintained documentation
     */
    Index ActionAndObservation_to_ActionObservationIndex(Index aI, 
            Index oI, size_t nrA, size_t nrO);

    /// Convert (joint)  ActionObservation indices to (joint) Action indices.
    Index ActionObservation_to_ActionIndex(Index aoI, size_t nrA, size_t nrO);

    /** \brief Convert (joint) ActionObservation indices to (joint)
     * Observation indices.
     */
    Index ActionObservation_to_ObservationIndex(Index aoI, size_t nrA,size_t nrO);

    ///Restrict a vector of indices to a scope.
    /**
     * sc is a Scope (also a vector of indices), each index i in sc identifies
     * an element of indivIndices: namely indivIndices[i]
     *  
     * this function changes the vector restrictedIndivIndices to contain
     * < indivIndices[i] >
     * for all i in scope
     *
     * restrictedIndivIndices should already have the correct size:
     * I.e., it should be at least at big as the number of elements in the 
     * scope. However, restrictedIndivIndices may be larger than the number
     * of elements in the scope. In this case, the superfluous elements
     * remain untouched. (this is actually used in some places to
     * increase performance).
     */
    template <typename T>
    void RestrictIndividualIndicesToScope(
        const std::vector<T>& indivIndices, 
        const Scope& sc,
        std::vector<T> &restrictedIndivIndices)
    {
        size_t scSize = sc.size();
        for(Index s_I = 0; s_I < scSize; s_I++)
        {
            Index indivIndicesI=sc[s_I];
            Index indivIndicesIVal = indivIndices[indivIndicesI];
            restrictedIndivIndices[s_I]=indivIndicesIVal;
        }
    }

    ///Restricts a vector of indices with a current scope to a narrower scope
    template <typename T>
    void RestrictIndividualIndicesToNarrowerScope(
            const std::vector<T>& indivIndices, 
            const Scope& old_sc,
            const Scope& new_sc,
            std::vector<T> &restrictedIndivIndices)
    {
        size_t scSize = new_sc.size();
//        std::vector<T> restrictedIndivIndices;
        for(Index s_I = 0; s_I < scSize; s_I++)
        {
            //find which element is next according to new scope
            Index indivIndicesI=new_sc[s_I];
            //find location of that element according to old scope
            Index old_scope_i = 0;
            try {
                old_scope_i = old_sc.GetPositionForIndex(indivIndicesI);
            } catch( E& ) {
                throw ENoSubScope("IndexTools::RestrictIndividualIndicesToNarrowerScope -the new scope is no sub-scope of the old scope!");
            }
            Index indivIndicesIVal = indivIndices[old_scope_i];
//            restrictedIndivIndices.push_back(indivIndicesIVal);
            restrictedIndivIndices[s_I]=indivIndicesIVal;
        }
//        return(restrictedIndivIndices); 
    }


    /** \brief Calculate the number of sequences of length up to
     * seqLength, for which at every time step o options are
     * available.
     *
     * Calculation includes 1 empty sequence (of length 0). * 
     *
     */
    size_t CalculateNumberOfSequences(size_t o, size_t seqLength);

}

#endif /* !_INDEXTOOLS_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
