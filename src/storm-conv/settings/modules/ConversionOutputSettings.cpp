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
            const std::string ConversionOutputSettings::prismOutputOptionName = "toprism";
            const std::string ConversionOutputSettings::aigerOutputOptionName = "toaiger";

            ConversionOutputSettings::ConversionOutputSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, stdoutOptionName, false, "If set, the output will be printed to stdout.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, janiOutputOptionName, false, "exports the model as Jani file.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "the name of the output file (if not empty).").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, prismOutputOptionName, false, "exports the model as Prism file.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "the name of the output file (if not empty).").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, aigerOutputOptionName, false, "exports the model as Aiger file.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "the name of the output file (if not empty).").setDefaultValueString("").build()).build());
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
            
            bool ConversionOutputSettings::isPrismOutputSet() const {
                return this->getOption(prismOutputOptionName).getHasOptionBeenSet();
            }
            
            bool ConversionOutputSettings::isPrismOutputFilenameSet() const {
                return isPrismOutputSet()
                       && !this->getOption(prismOutputOptionName).getArgumentByName("filename").wasSetFromDefaultValue()
                       && this->getOption(prismOutputOptionName).getArgumentByName("filename").getHasBeenSet()
                       && this->getOption(prismOutputOptionName).getArgumentByName("filename").getValueAsString() != "";
            }

            bool ConversionOutputSettings::isAigerOutputSet() const {
                return this->getOption(aigerOutputOptionName).getHasOptionBeenSet();
            }

            bool ConversionOutputSettings::isAigerOutputFilenameSet() const {
                return isAigerOutputSet()
                       && !this->getOption(aigerOutputOptionName).getArgumentByName("filename").wasSetFromDefaultValue()
                       && this->getOption(aigerOutputOptionName).getArgumentByName("filename").getHasBeenSet()
                       && this->getOption(aigerOutputOptionName).getArgumentByName("filename").getValueAsString() != "";
            }

            std::string ConversionOutputSettings::getPrismOutputFilename() const {
                STORM_LOG_THROW(isPrismOutputFilenameSet(), storm::exceptions::InvalidOperationException, "Tried to get the prism output name although none was specified.");
                return this->getOption(prismOutputOptionName).getArgumentByName("filename").getValueAsString();
            }

            std::string ConversionOutputSettings::getAigerOutputFilename() const {
                STORM_LOG_THROW(isAigerOutputFilenameSet(), storm::exceptions::InvalidOperationException, "Tried to get the aiger output name although none was specified.");
                return this->getOption(aigerOutputOptionName).getArgumentByName("filename").getValueAsString();
            }

			void ConversionOutputSettings::finalize() {
                // Intentionally left empty.
            }

            bool ConversionOutputSettings::check() const {
                STORM_LOG_THROW(!isJaniOutputFilenameSet() || ArgumentValidatorFactory::createWritableFileValidator()->isValid(getJaniOutputFilename()), storm::exceptions::InvalidSettingsException, "Unable to write at file " + getJaniOutputFilename());
                STORM_LOG_THROW(!isPrismOutputFilenameSet() || ArgumentValidatorFactory::createWritableFileValidator()->isValid(getPrismOutputFilename()), storm::exceptions::InvalidSettingsException, "Unable to write at file " + getPrismOutputFilename());
                STORM_LOG_THROW(!(isJaniOutputSet() && isPrismOutputSet()), storm::exceptions::InvalidSettingsException, "Can not export to both, prism and jani");
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
