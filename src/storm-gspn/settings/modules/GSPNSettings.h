#pragma once

#include "storm-config.h"
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {
class GSPNSettings : public ModuleSettings {
   public:
    /*!
     * Creates a new PGCL setting
     */
    GSPNSettings();

    /**
     * Retrievew whether the pgcl file option was set
     */
    bool isGspnFileSet() const;

    /**
     * Retrieves the gspn file name
     */
    std::string getGspnFilename() const;

    /**
     * Retrievew whether the pgcl file option was set
     */
    bool isCapacitiesFileSet() const;

    /**
     * Retrieves the gspn file name
     */
    std::string getCapacitiesFilename() const;

    /**
     * Retrievew whether a global capacity was set
     */
    bool isCapacitySet() const;

    /**
     * Retrieves the global capacity
     */
    uint64_t getCapacity() const;

    /*!
     * Retrieves whether the constants ption was set.
     */
    bool isConstantsSet() const;

    /*!
     * Retrieves the string that defines the constants of a gspn
     */
    std::string getConstantDefinitionString() const;

    bool check() const override;
    void finalize() override;

    static const std::string moduleName;

   private:
    static const std::string gspnFileOptionName;
    static const std::string gspnFileOptionShortName;
    static const std::string capacitiesFileOptionName;
    static const std::string capacitiesFileOptionShortName;
    static const std::string capacityOptionName;
    static const std::string constantsOptionName;
    static const std::string constantsOptionShortName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm
