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
#ifndef _RGET_H_
#define _RGET_H_ 1

/* the include directives */
#include "Globals.h"

#include "RewardModelMapping.h"
#include "RewardModelMappingSparse.h"

/** \brief RGet can be used for direct access to a reward model.
 */
class RGet 
{
public:
    virtual ~RGet() = 0;
    //get (data) functions:
    virtual double Get(Index sI, Index jaI) const = 0;
};

//http://www.parashift.com/c++-faq-lite/pointers-to-members.html
//says "defined even though it's pure virtual; it's faster this way; trust me"
inline RGet::~RGet() {}

/** \brief RGet can be used for direct access to a RewardModelMapping.
 */
class RGet_RewardModelMapping : public RGet
{
 
private:
    const RewardModelMapping::Matrix& _m_R;
public:
    RGet_RewardModelMapping( RewardModelMapping* rm)
        :
            _m_R ( rm->_m_R )
    {};

    virtual double Get(Index sI, Index jaI) const
    {
        return( _m_R(sI,jaI)) ;  
    }
};

/** \brief RGet can be used for direct access to a RewardModelMappingSparse.
 */
class RGet_RewardModelMappingSparse : public RGet
{
 
private:
    
    const RewardModelMappingSparse::SparseMatrix&  _m_R;
public:
    RGet_RewardModelMappingSparse( RewardModelMappingSparse* rm)
        :
            _m_R ( rm->_m_R )
    {};

    virtual double Get(Index sI, Index jaI) const
    {
        return( _m_R(sI,jaI)) ;  
    }
        

};

#endif /* !_RGET_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
