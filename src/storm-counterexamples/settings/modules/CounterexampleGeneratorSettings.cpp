#include "storm-counterexamples/settings/modules/CounterexampleGeneratorSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/modules/IOSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string CounterexampleGeneratorSettings::moduleName = "counterexample";
            const std::string CounterexampleGeneratorSettings::minimalCommandSetOptionName = "mincmd";
            const std::string CounterexampleGeneratorSettings::encodeReachabilityOptionName = "encreach";
            const std::string CounterexampleGeneratorSettings::schedulerCutsOptionName = "schedcuts";
            const std::string CounterexampleGeneratorSettings::noDynamicConstraintsOptionName = "nodyn";

            CounterexampleGeneratorSettings::CounterexampleGeneratorSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> techniques = {"maxsat", "milp"};
                this->addOption(storm::settings::OptionBuilder(moduleName, minimalCommandSetOptionName, true, "Computes a counterexample for the given model in terms of a minimal command/edge set. Note that this requires the model to be given in a symbolic format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("method", "Sets which technique is used to derive the counterexample.").setDefaultValueString("maxsat").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(techniques)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeReachabilityOptionName, true, "Sets whether to encode reachability for MAXSAT-based counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerCutsOptionName, true, "Sets whether to add the scheduler cuts for MILP-based counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, noDynamicConstraintsOptionName, true, "Disables the generation of dynamic constraints in the MAXSAT-based counterexample generation.").build());
            }
            
            bool CounterexampleGeneratorSettings::isMinimalCommandSetGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getHasOptionBeenSet();
            }
            
            bool CounterexampleGeneratorSettings::isUseMilpBasedMinimalCommandSetGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getArgumentByName("method").getValueAsString() == "milp";
            }
            
            bool CounterexampleGeneratorSettings::isUseMaxSatBasedMinimalCommandSetGenerationSet() const {
                return this->getOption(minimalCommandSetOptionName).getArgumentByName("method").getValueAsString() == "maxsat";
            }
            
            bool CounterexampleGeneratorSettings::isEncodeReachabilitySet() const {
                return this->getOption(encodeReachabilityOptionName).getHasOptionBeenSet();
            }
            
            bool CounterexampleGeneratorSettings::isUseSchedulerCutsSet() const {
                return this->getOption(schedulerCutsOptionName).getHasOptionBeenSet();
            }

            bool CounterexampleGeneratorSettings::isUseDynamicConstraintsSet() const {
                return !this->getOption(noDynamicConstraintsOptionName).getHasOptionBeenSet();
            }

            bool CounterexampleGeneratorSettings::check() const {
                // Ensure that the model was given either symbolically or explicitly.
                STORM_LOG_THROW(!isMinimalCommandSetGenerationSet() || storm::settings::getModule<storm::settings::modules::IOSettings>().isPrismInputSet() || storm::settings::getModule<storm::settings::modules::IOSettings>().isJaniInputSet(), storm::exceptions::InvalidSettingsException, "For the generation of a minimal command set, the model has to be specified in the PRISM/JANI format.");
                
                if (isMinimalCommandSetGenerationSet()) {
                    STORM_LOG_WARN_COND(isUseMaxSatBasedMinimalCommandSetGenerationSet() || !isEncodeReachabilitySet(), "Encoding reachability is only available for the MaxSat-based minimal command set generation, so selecting it has no effect.");
                    STORM_LOG_WARN_COND(isUseMilpBasedMinimalCommandSetGenerationSet() || !isUseSchedulerCutsSet(), "Using scheduler cuts is only available for the MaxSat-based minimal command set generation, so selecting it has no effect.");
                }
                
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
