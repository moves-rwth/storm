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
#ifndef _REWARDMODELMAPPING_H_
#define _REWARDMODELMAPPING_H_ 1

/* the include directives */
#include "boost/numeric/ublas/matrix.hpp"
#include "Globals.h"
#include "RewardModel.h"

/// RewardModelMapping represents a discrete reward model.
class RewardModelMapping : public RewardModel
{
private:

    std::string _m_s_str;
    std::string _m_ja_str;
    
    typedef boost::numeric::ublas::matrix<double> Matrix;

    Matrix _m_R;

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
    RewardModelMapping(size_t nrS = 1, size_t nrJA = 1,
                       const std::string &s_str="s",
                       const std::string &ja_str="ja");
    /// Copy constructor.
    //RewardModelMapping(const RewardModelMapping&);
    /// Destructor.
    ~RewardModelMapping();
        
    /// Returns R(s,ja)
    double Get(Index s_i, Index ja_i) const
        { return(_m_R(s_i,ja_i)); }

    //data manipulation funtions:
    /// Sets R(s_i,ja_i)
    /** Index ja_i, Index s_i, are indices of the state and taken
     * joint action. r is the reward. The order of events is s, ja, so
     * is the arg. list. */
    void Set(Index s_i, Index ja_i, double rew)
        { _m_R(s_i,ja_i)=rew; }

    /// Returns a pointer to a copy of this class.
    virtual RewardModelMapping* Clone() const
        { return new RewardModelMapping(*this); }

    /// Prints a description of *this* to a string.
    std::string SoftPrint() const; 

    friend class RGet_RewardModelMapping;
};

#endif /* !_REWARDMODELMAPPING_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

