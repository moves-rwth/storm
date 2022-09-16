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
#ifndef _FACTOREDQFUNCTIONSCOPEFORSTAGE_H_
#define _FACTOREDQFUNCTIONSCOPEFORSTAGE_H_ 1

/* the include directives */
#include "Globals.h"
#include "Scope.h"

/** \brief FactoredQFunctionScopeForStage represents a Scope for one
 * stage of a factored QFunction. */
class FactoredQFunctionScopeForStage 
{
    private:   

        std::vector< Scope > _m_agentScopes;
        std::vector< Scope > _m_sfacScopes;
    
    protected:
    
    public:
        // Constructor, destructor and copy assignment.
        /// (default) Constructor
        FactoredQFunctionScopeForStage();

        //operators:

        //data manipulation (set) functions:
        ///Add a local Q function scope component
        void AddLocalQ( const Scope& sfacS, const Scope& agS);
        ///Remove the j-th local Q function scope component
        void RemoveLocalQ( Index j);
        
        //get (data) functions:
        const Scope& GetStateFactorScope(Index lqf) const
        {return _m_sfacScopes.at(lqf);}
        const Scope& GetAgentScope(Index lqf) const
        {return _m_agentScopes.at(lqf);}
        size_t GetNrLQFs() const
        {return _m_sfacScopes.size();}
        std::string SoftPrint() const;
        
};


#endif /* !_FACTOREDQFUNCTIONSCOPEFORSTAGE_H_ */

// Local Variables: ***
// mode:c++ ***
// End: ***
