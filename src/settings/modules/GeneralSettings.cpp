#include "src/settings/modules/GeneralSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/solver/SolverSelectionOptions.h"

#include "src/storage/dd/DdType.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GeneralSettings::moduleName = "general";
            const std::string GeneralSettings::helpOptionName = "help";
            const std::string GeneralSettings::helpOptionShortName = "h";
            const std::string GeneralSettings::versionOptionName = "version";
            const std::string GeneralSettings::verboseOptionName = "verbose";
            const std::string GeneralSettings::verboseOptionShortName = "v";
            const std::string GeneralSettings::precisionOptionName = "precision";
            const std::string GeneralSettings::precisionOptionShortName = "eps";
            const std::string GeneralSettings::configOptionName = "config";
            const std::string GeneralSettings::configOptionShortName = "c";
            const std::string GeneralSettings::propertyOptionName = "prop";
            const std::string GeneralSettings::propertyOptionShortName = "prop";
            const std::string GeneralSettings::timeoutOptionName = "timeout";
            const std::string GeneralSettings::timeoutOptionShortName = "t";
            const std::string GeneralSettings::bisimulationOptionName = "bisimulation";
            const std::string GeneralSettings::bisimulationOptionShortName = "bisim";
            
#ifdef STORM_HAVE_CARL
            const std::string GeneralSettings::parametricOptionName = "parametric";
#endif

            GeneralSettings::GeneralSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, helpOptionName, false, "Shows all available options, arguments and descriptions.").setShortName(helpOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("hint", "A regular expression to show help for all matching entities or 'all' for the complete help.").setDefaultValueString("all").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, versionOptionName, false, "Prints the version information.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, verboseOptionName, false, "Enables more verbose output.").setShortName(verboseOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The internally used precision.").setShortName(precisionOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to use.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, configOptionName, false, "If given, this file will be read and parsed for additional configuration settings.").setShortName(configOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the configuration.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, propertyOptionName, false, "Specifies the formulas to be checked on the model.").setShortName(propertyOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("formula or filename", "The formula or the file containing the formulas.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, timeoutOptionName, false, "If given, computation will abort after the timeout has been reached.").setShortName(timeoutOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("time", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, bisimulationOptionName, false, "Sets whether to perform bisimulation minimization.").setShortName(bisimulationOptionShortName).build());
                
#ifdef STORM_HAVE_CARL
                this->addOption(storm::settings::OptionBuilder(moduleName, parametricOptionName, false, "Sets whether to use the parametric engine.").build());
#endif
            }
            
            bool GeneralSettings::isHelpSet() const {
                return this->getOption(helpOptionName).getHasOptionBeenSet();
            }
            
            bool GeneralSettings::isVersionSet() const {
                return this->getOption(versionOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getHelpModuleName() const {
                return this->getOption(helpOptionName).getArgumentByName("hint").getValueAsString();
            }
            
            bool GeneralSettings::isVerboseSet() const {
                return this->getOption(verboseOptionName).getHasOptionBeenSet();
            }
            
            double GeneralSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool GeneralSettings::isConfigSet() const {
                return this->getOption(configOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getConfigFilename() const {
                return this->getOption(configOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isPropertySet() const {
                return this->getOption(propertyOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getProperty() const {
                return this->getOption(propertyOptionName).getArgumentByName("formula or filename").getValueAsString();
            }
            
            bool GeneralSettings::isTimeoutSet() const {
                return this->getOption(timeoutOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t GeneralSettings::getTimeoutInSeconds() const {
                return this->getOption(timeoutOptionName).getArgumentByName("time").getValueAsUnsignedInteger();
            }
            
            bool GeneralSettings::isBisimulationSet() const {
                return this->getOption(bisimulationOptionName).getHasOptionBeenSet();
            }
            
#ifdef STORM_HAVE_CARL
            bool GeneralSettings::isParametricSet() const {
                return this->getOption(parametricOptionName).getHasOptionBeenSet();
            }
#endif

            void GeneralSettings::finalize() {
            }

            bool GeneralSettings::check() const {
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
