#pragma once
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

class ConversionGeneralSettings : public ModuleSettings {
   public:
    ConversionGeneralSettings();

    /*!
     * Retrieves whether the help option was set.
     *
     * @return True if the help option was set.
     */
    bool isHelpSet() const;

    /*!
     * Retrieves whether the version option was set.
     *
     * @return True if the version option was set.
     */
    bool isVersionSet() const;

    /*!
     * Retrieves the name of the module for which to show the help or "all" to indicate that the full help
     * needs to be shown.
     *
     * @return The name of the module for which to show the help or "all".
     */
    std::string getHelpFilterExpression() const;

    /*!
     * Retrieves whether the verbose option was set.
     *
     * @return True if the verbose option was set.
     */
    bool isVerboseSet() const;

    /*!
     * Retrieves whether the debug output option was set.
     *
     */
    bool isDebugOutputSet() const;

    /*!
     * Retrieves whether the trace output option was set.
     *
     */
    bool isTraceOutputSet() const;

    /*!
     * Retrieves whether the config option was set.
     *
     * @return True if the config option was set.
     */
    bool isConfigSet() const;

    /*!
     * Retrieves the name of the file that is to be scanned for settings.
     *
     * @return The name of the file that is to be scanned for settings.
     */
    std::string getConfigFilename() const;

    bool check() const override;
    void finalize() override;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.
    static const std::string helpOptionName;
    static const std::string helpOptionShortName;
    static const std::string versionOptionName;
    static const std::string verboseOptionName;
    static const std::string verboseOptionShortName;
    static const std::string debugOptionName;
    static const std::string traceOptionName;
    static const std::string configOptionName;
    static const std::string configOptionShortName;
};
}  // namespace modules
}  // namespace settings
}  // namespace storm