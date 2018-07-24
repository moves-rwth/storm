#include "storm-conv/settings/modules/ConversionOutputSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ConversionOutputSettings::moduleName = "output";
            const std::string ConversionOutputSettings::stdoutOptionName = "stdout";
            const std::string ConversionOutputSettings::janiOutputOptionName = "tojani";

            ConversionOutputSettings::ConversionOutputSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, stdoutOptionName, false, "If set, the output will be printed to stdout.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, janiOutputOptionName, false, "converts the input model to Jani.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "the name of the output file (if not empty).").setDefaultValueString("")).build());
            }
            
            bool ConversionOutputSettings::isStdOutOutputEnabled() const {
                return this->getOption(stdoutOptionName).getHasOptionBeenSet();
            }
            
            bool ConversionOutputSettings::isJaniOutputSet() const {
                return this->getOption(janiOutputOptionName).getHasOptionBeenSet();
            }
            
            bool ConversionOutputSettings::isJaniOutputFilenameSet() const {
                return isJaniOutputSet()
                       && !this->getOption(janiOutputOptionName).getArgumentByName("filename").wasSetFromDefaultValue()
                       && this->getOption(janiOutputOptionName).getArgumentByName("filename").getHasBeenSet()
                       && this->getOption(janiOutputOptionName).getArgumentByName("filename").getValueAsString() != "";
            }

            std::string ConversionOutputSettings::getJaniOutputFilename() const {
                STORM_LOG_THROW(isJaniOutputFilenameSet(), storm::exceptions::InvalidOperationException, "Tried to get the jani output name although none was specified.");
                return this->getOption(janiOutputOptionName).getArgumentByName("filename").getValueAsString();
            }

			void ConversionOutputSettings::finalize() {
                // Intentionally left empty.
            }

            bool ConversionOutputSettings::check() const {
                // Ensure that exactly one Output is specified
                STORM_LOG_THROW(isJaniOutputSet(), storm::exceptions::InvalidSettingsException, "No Output specified.");
                STORM_LOG_THROW(!isJaniOutputFilenameSet() || ArgumentValidatorFactory::createWritableFileValidator()->isValid(getJaniOutputFilename()), storm::exceptions::InvalidSettingsException, "Unable to write at file " + getJaniOutputFilename());
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
