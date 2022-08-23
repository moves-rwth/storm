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
#ifndef _REWARDMODELMAPPINGSPARSEMAPPED_H_
#define _REWARDMODELMAPPINGSPARSEMAPPED_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix_sparse.hpp"
#include "Globals.h"
#include "RewardModel.h"

/// RewardModelMappingSparseMapped represents a discrete reward model.
/** This version uses a mapped matrix as sparse representation.
 */
class RewardModelMappingSparseMapped : public RewardModel
{
private:

    std::string _m_s_str;
    std::string _m_ja_str;
    
#if BOOST_1_32_OR_LOWER // they renamed sparse_vector to mapped_vector
    typedef boost::numeric::ublas::sparse_matrix<double> SparseMatrix;
#else    
    //typedef boost::numeric::ublas::compressed_matrix<double> SparseMatrix;
    // if the matrix is really large, we might need to resort to a
    // mapped matrix:
    typedef boost::numeric::ublas::mapped_matrix<double> SparseMatrix;
#endif    

    SparseMatrix _m_R;

protected:
    
public:
    // Constructor, destructor and copy assignment.
    /** default Constructor
     *  nrS - number of states
     *  nrJA - number of joint actions
     *  s_str - how to call a state (For example you can use this class to 
     *          create a mapping from observation histories and ja's to 
     *          reals. Then this argument could be "joh")
     *  ja_str - idem for the joint actions
     */
    RewardModelMappingSparseMapped(size_t nrS = 1, size_t nrJA = 1,
                             const std::string &s_str="s",
                             const std::string &ja_str="ja");
    /// Copy constructor.
    //RewardModelMappingSparseMapped(const RewardModelMappingSparseMapped&);
    /// Destructor.
    ~RewardModelMappingSparseMapped();
        
    /// Returns R(s,ja)
    double Get(Index s_i, Index ja_i) const
        { return(_m_R(s_i,ja_i)); }

    //data manipulation funtions:
    /// Sets R(s_i,ja_i)
    /** Index ja_i, Index s_i, are indices of the state and taken
     * joint action. r is the reward. The order of events is s, ja, so
     * is the arg. list. */
    void Set(Index s_i, Index ja_i, double rew)
        {
            // make sure reward is not 0
            if(fabs(rew) > REWARD_PRECISION)
                _m_R(s_i,ja_i)=rew;
            // check if we already defined this element, if so remove it
            else if(fabs(_m_R(s_i,ja_i))>REWARD_PRECISION)
                _m_R.erase_element(s_i,ja_i);

        }

    /// Returns a pointer to a copy of this class.
    virtual RewardModelMappingSparseMapped* Clone() const
        { return new RewardModelMappingSparseMapped(*this); }

    /// Prints a description of *this* to a string.
    std::string SoftPrint() const; 
    
    friend class RGet_RewardModelMappingSparseMapped;
};

#endif /* !_REWARDMODELMAPPINGSPARSEMAPPED_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

