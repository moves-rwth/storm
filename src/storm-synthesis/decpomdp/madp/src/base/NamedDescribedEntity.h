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
#ifndef _NAMED_DESCRIBED_ENTITY_H_
#define _NAMED_DESCRIBED_ENTITY_H_ 1

/* the include directives */
#include <iostream>
#include <string>
#include "Globals.h"

/// NamedDescribedEntity represents named entities.
/** For example actions and observations in a decision process.  */
class NamedDescribedEntity
{
    private:

    protected:
    /// The name.
    std::string _m_name;
    /// The description.
    std::string _m_description;
    
    public:
    // Constructor, destructor and copy assignment.
    /// (default) Constructor
    NamedDescribedEntity(const std::string &name=std::string("undefined"),
                         const std::string &description=std::string("undefined"));
    /// Destructor.
    virtual ~NamedDescribedEntity(){};
    // 'get' functions:
    /// Get the name of the NamedDescribedEntity
    std::string GetName() const {return _m_name;};
    /// Get the description of the NamedDescribedEntity
    std::string GetDescription() const {return _m_description;};

    /// Set the name.
    void SetName(const std::string &name){
        _m_name=name;
    }

    /// Set the description.
    void SetDescription(const std::string &description){
        _m_description=description;
    }

    /// Returns the name and description (if not reimplemented).
    virtual std::string SoftPrint() const;
    /// Returns the name (if not reimplemented).
    virtual std::string SoftPrintBrief() const;
    /// Prints the name and description (by default).
    void Print() const {std::cout << SoftPrint() << std::endl; }
    /// Prints the name (by default).
    void PrintBrief() const {std::cout << SoftPrintBrief() << std::endl; }
};

#endif /* !_NAMED_DESCRIBED_ENTITY_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
