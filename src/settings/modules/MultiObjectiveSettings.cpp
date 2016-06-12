#include "src/settings/modules/MultiObjectiveSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MultiObjectiveSettings::moduleName = "multiobjective";
            const std::string MultiObjectiveSettings::exportResultFileOptionName = "resultfile";
            const std::string MultiObjectiveSettings::precisionOptionName = "precision";
            const std::string MultiObjectiveSettings::maxIterationsOptionName = "maxiterations";
            
            MultiObjectiveSettings::MultiObjectiveSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultFileOptionName, true, "Saves the result as LaTeX document.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "A path to a file where the result should be saved.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for the approximation of numerical- and pareto queries.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision. Default is 1e-04.").setDefaultValueDouble(1e-04).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxIterationsOptionName, true, "Aborts the computation after the given number of iterations (= computed pareto optimal points).")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "the threshold for the number of iterations to be performed.").build()).build());
            }
            
            bool MultiObjectiveSettings::exportResultToFile() const {
                return this->getOption(exportResultFileOptionName).getHasOptionBeenSet();
            }
            
            std::string MultiObjectiveSettings::exportResultPath() const {
                return this->getOption(exportResultFileOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            double MultiObjectiveSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool MultiObjectiveSettings::isMaxIterationsSet() const {
                return this->getOption(maxIterationsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t MultiObjectiveSettings::getMaxIterations() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
