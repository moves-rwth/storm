#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {
class PGCLSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new PGCL setting
     */
    PGCLSettings();

    /**
     * Retrievew whether the pgcl file option was set
     */
    bool isPgclFileSet() const;

    /**
     * Retrieves the pgcl file name
     */
    std::string getPgclFilename() const;

    /**
     * Whether the pgcl should be transformed to Jani
     */
    bool isToJaniSet() const;

    /**
     * returns the file name where jani output should be stored.
     */
    std::string getWriteToJaniFilename() const;

    /**
     * Whether the program graph should be drawn (dot output)
     */
    bool isProgramGraphToDotSet() const;

    /**
     * Destination where to write dot output of the program graph.
     */
    std::string getProgramGraphDotOutputFilename() const;

    /**
     * Is program variable restriction string given
     */
    bool isProgramVariableRestrictionSet() const;

    /**
     * String for program variable restrictions
     */
    std::string getProgramVariableRestrictions() const;

    /*!
     * Retrieves whether the property option was set.
     *
     * @return True if the property option was set.
     */
    bool isPropertyInputSet() const;

    /*!
     * Retrieves the property specified with the property option.
     *
     * @return The property specified with the property option.
     */
    std::string getPropertyInput() const;

    /*!
     * Retrieves the property filter.
     *
     * @return The property filter.
     */
    std::string getPropertyInputFilter() const;

    bool check() const override;
    void finalize() override;

    static const std::string moduleName;

   private:
    static const std::string pgclFileOptionName;
    static const std::string pgclFileOptionShortName;
    static const std::string pgclToJaniOptionName;
    static const std::string pgclToJaniOptionShortName;
    static const std::string programGraphToDotOptionName;
    static const std::string programGraphToDotShortOptionName;
    static const std::string programVariableRestrictionsOptionName;
    static const std::string programVariableRestrictionShortOptionName;
    static const std::string propertyOptionName;
    static const std::string propertyOptionShortName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm
