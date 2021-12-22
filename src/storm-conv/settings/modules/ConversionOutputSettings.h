#pragma once
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

class ConversionOutputSettings : public ModuleSettings {
   public:
    ConversionOutputSettings();

    /*!
     * Retrieves whether the output should be printed to stdout
     */
    bool isStdOutOutputEnabled() const;

    /*!
     * Retrieves whether the output should be in the Jani format
     */
    bool isJaniOutputSet() const;

    /*!
     * Retrieves whether an output filename for the jani file was specified
     */
    bool isJaniOutputFilenameSet() const;

    /*!
     * Retrieves the name of the jani output (if specified)
     */
    std::string getJaniOutputFilename() const;

    /*!
     * Retrieves whether the output should be in the Prism format
     */
    bool isPrismOutputSet() const;

    /*!
     * Retrieves whether an output filename for the prism file was specified
     */
    bool isPrismOutputFilenameSet() const;

    /*!
     * Retrieves the name of the prism output (if specified)
     */
    std::string getPrismOutputFilename() const;

    bool check() const override;
    void finalize() override;

    // The name of the module.
    static const std::string moduleName;
    // name of the option that enables output to stdout. It needs to be public because we have to check this option very early
    static const std::string stdoutOptionName;

   private:
    // Define the string names of the options as constants.
    static const std::string janiOutputOptionName;
    // Define the string names of the options as constants.
    static const std::string prismOutputOptionName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm