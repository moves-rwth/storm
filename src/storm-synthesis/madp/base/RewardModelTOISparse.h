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
#ifndef _REWARDMODELTOISPARSE_H_
#define _REWARDMODELTOISPARSE_H_ 1

/* the include directives */
#include "Globals.h"
#include <map>

/// RewardModelTOISparse represents a discrete reward model based on
/// vectors of states and actions.
class RewardModelTOISparse
{
private:

    std::string _m_s_str;
    std::string _m_ja_str;
    
    std::map<std::pair<std::vector<Index>,
                       std::vector<Index> >,
             double> _m_R;

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
    RewardModelTOISparse(const std::string &s_str="s",
                         const std::string &ja_str="ja");
    /// Copy constructor.
    //RewardModelTOISparse(const RewardModelTOISparse&);
    /// Destructor.
    ~RewardModelTOISparse();
        
    /// Returns R(s,ja)
    double Get(const std::vector<Index> &sIs,
               const std::vector<Index> &aIs) const;
#if 0
    double Get(Index s_i, Index ja_i) const
        {
            return(GetReward(JointToIndividualStateIndices(s_i),
                             JointToIndividualActionIndices(ja_i)));
        }
#endif
    //data manipulation funtions:
    /// Sets R(s_i,ja_i)
    /** Index ja_i, Index s_i, are indices of the state and taken
     * joint action. r is the reward. The order of events is s, ja, so
     * is the arg. list. */
    void Set(const std::vector<Index> &sIs,
             const std::vector<Index> &aIs,
             double reward);

    /// Prints a description of *this* to a string.
    std::string SoftPrint() const; 
    ///Print *this* to cout.
    void Print() const
        { std::cout << SoftPrint();}
};

#endif /* !_REWARDMODELTOISPARSE_H_*/

// Local Variables: ***
// mode:c++ ***
// End: ***

