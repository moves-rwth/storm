#pragma once
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace settings {
namespace modules {

class ConversionInputSettings : public ModuleSettings {
   public:
    ConversionInputSettings();

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

    /*!
     * Retrieves whether constant definition option was set.
     *
     * @return True if the constant definition option was set.
     */
    bool isConstantsSet() const;

    /*!
     * Retrieves the string that defines the constants of a symbolic model (given via the symbolic option).
     *
     * @return The string that defines the constants of a symbolic model.
     */
    std::string getConstantDefinitionString() const;

    /*!
     * Retrieves whether the PRISM language option was set.
     */
    bool isPrismInputSet() const;

    /*!
     * Retrieves the name of the file that contains the PRISM model specification if the model was given
     * using the PRISM input option.
     */
    std::string getPrismInputFilename() const;

    /*!
     * Retrieves whether the PRISM compatibility mode was enabled.
     *
     * @return True iff the PRISM compatibility mode was enabled.
     */
    bool isPrismCompatibilityEnabled() const;

    /*!
     * Retrieves whether the Jani option was set.
     */
    bool isJaniInputSet() const;

    /*!
     * Retrieves the name of the file that contains the jani model specification if the model was given.
     */
    std::string getJaniInputFilename() const;

    /*!
     * Retrieves whether the jani-property option was set
     * @return
     */
    bool isJaniPropertiesSet() const;

    /*!
     * Retrieves whether one or more jani-properties have been selected
     * @return
     */
    bool areJaniPropertiesSelected() const;

    /*!
     * @return  The names of the jani properties to check
     */
    std::vector<std::string> getSelectedJaniProperties() const;

    bool check() const override;
    void finalize() override;

    // The name of the module.
    static const std::string moduleName;

   private:
    // Define the string names of the options as constants.
    static const std::string propertyOptionName;
    static const std::string propertyOptionShortName;
    static const std::string constantsOptionName;
    static const std::string constantsOptionShortName;
    static const std::string prismInputOptionName;
    static const std::string prismCompatibilityOptionName;
    static const std::string prismCompatibilityOptionShortName;
    static const std::string janiInputOptionName;
    static const std::string janiPropertyOptionName;
    static const std::string janiPropertyOptionShortName;
};

}  // namespace modules
}  // namespace settings
}  // namespace storm