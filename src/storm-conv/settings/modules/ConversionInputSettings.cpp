#include "storm-conv/settings/modules/ConversionInputSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ConversionInputSettings::moduleName = "input";
            const std::string ConversionInputSettings::propertyOptionName = "prop";
            const std::string ConversionInputSettings::propertyOptionShortName = "prop";
            const std::string ConversionInputSettings::constantsOptionName = "constants";
            const std::string ConversionInputSettings::constantsOptionShortName = "const";
            const std::string ConversionInputSettings::prismInputOptionName = "prism";
            const std::string ConversionInputSettings::prismCompatibilityOptionName = "prismcompat";
            const std::string ConversionInputSettings::prismCompatibilityOptionShortName = "pc";
            

            ConversionInputSettings::ConversionInputSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, propertyOptionName, false, "Specifies the properties to be checked on the model.").setShortName(propertyOptionShortName)
                                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("property or filename", "The formula or the file containing the formulas.").build())
                                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filter", "The names of the properties to check.").setDefaultValueString("all").build())
                                        .build());
                this->addOption(storm::settings::OptionBuilder(moduleName, constantsOptionName, false, "Specifies the constant replacements to use in symbolic models. Note that this requires the model to be given as an symbolic model (i.e., via --" + prismInputOptionName + ").").setShortName(constantsOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("values", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3.").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, prismInputOptionName, false, "Parses the model given in the PRISM format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the PRISM input.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, prismCompatibilityOptionName, false, "Enables PRISM compatibility. This may be necessary to process some PRISM models.").setShortName(prismCompatibilityOptionShortName).build());
            }
            
            bool ConversionInputSettings::isPrismInputSet() const {
                return this->getOption(prismInputOptionName).getHasOptionBeenSet();
            }

            std::string ConversionInputSettings::getPrismInputFilename() const {
                return this->getOption(prismInputOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool ConversionInputSettings::isPrismCompatibilityEnabled() const {
                return this->getOption(prismCompatibilityOptionName).getHasOptionBeenSet();
            }

            bool ConversionInputSettings::isConstantsSet() const {
                return this->getOption(constantsOptionName).getHasOptionBeenSet();
            }

            std::string ConversionInputSettings::getConstantDefinitionString() const {
                return this->getOption(constantsOptionName).getArgumentByName("values").getValueAsString();
            }

            bool ConversionInputSettings::isPropertyInputSet() const {
                return this->getOption(propertyOptionName).getHasOptionBeenSet();
            }

            std::string ConversionInputSettings::getPropertyInput() const {
                return this->getOption(propertyOptionName).getArgumentByName("property or filename").getValueAsString();
            }

            std::string ConversionInputSettings::getPropertyInputFilter() const {
                return this->getOption(propertyOptionName).getArgumentByName("filter").getValueAsString();
            }

			void ConversionInputSettings::finalize() {
                // Intentionally left empty.
            }

            bool ConversionInputSettings::check() const {
                // Ensure that exactly one input is specified
                STORM_LOG_THROW(isPrismInputSet(), storm::exceptions::InvalidSettingsException, "No Input specified.");
                return true;
            }
            
            
        } // namespace modules
    } // namespace settings
} // namespace storm
