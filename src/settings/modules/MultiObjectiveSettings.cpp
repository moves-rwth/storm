#include "src/settings/modules/MultiObjectiveSettings.h"

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/settings/ArgumentValidators.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MultiObjectiveSettings::moduleName = "multiobjective";
            const std::string MultiObjectiveSettings::exportPlotOptionName = "exportplot";
            const std::string MultiObjectiveSettings::precisionOptionName = "precision";
            const std::string MultiObjectiveSettings::maxStepsOptionName = "maxsteps";
            
            MultiObjectiveSettings::MultiObjectiveSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportPlotOptionName, true, "Saves data for plotting of pareto curves and achievable values. The latter will be intersected with some boundaries to obtain a bounded polytope.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("directory", "A path to a directory in which the results will be saved.").build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("underapproximation", "The name of the file in which vertices of the under approximation of achievable values will be stored. (default: underapproximation.csv)").setDefaultValueString("underapproximation.csv").build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("overapproximation", "The name of the file in which vertices of the over approximation of achievable values will be stored. (default: overapproximation.csv)").setDefaultValueString("overapproximation.csv").build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("paretopoints", "The name of the file in which the computed pareto optimal points will be stored. (default: paretopoints.csv)").setDefaultValueString("paretopoints.csv").build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("boundaries", "The name of the file in which the corner points of the bounding rectangle will be stored. (default: boundaries.csv)").setDefaultValueString("boundaries.csv").build())
                                .build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for the approximation of numerical- and pareto queries.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision. Default is 1e-04.").setDefaultValueDouble(1e-04).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxStepsOptionName, true, "Aborts the computation after the given number of refinement steps (= computed pareto optimal points).")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "the threshold for the number of refinement steps to be performed.").build()).build());
            }
            
            bool MultiObjectiveSettings::isExportPlotSet() const {
                return this->getOption(exportPlotOptionName).getHasOptionBeenSet();
            }
            
            std::string MultiObjectiveSettings::getExportPlotUnderApproximationFileName() const {
                return this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString() + this->getOption(exportPlotOptionName).getArgumentByName("underapproximation").getValueAsString();
            }
            
            std::string MultiObjectiveSettings::getExportPlotOverApproximationFileName() const {
                return this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString() + this->getOption(exportPlotOptionName).getArgumentByName("overapproximation").getValueAsString();
            }
            
            std::string MultiObjectiveSettings::getExportPlotParetoPointsFileName() const {
                return this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString() + this->getOption(exportPlotOptionName).getArgumentByName("paretopoints").getValueAsString();
            }
            
            std::string MultiObjectiveSettings::getExportPlotBoundariesFileName() const {
                return this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString() + this->getOption(exportPlotOptionName).getArgumentByName("boundaries").getValueAsString();
            }
            
            double MultiObjectiveSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool MultiObjectiveSettings::isMaxStepsSet() const {
                return this->getOption(maxStepsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t MultiObjectiveSettings::getMaxSteps() const {
                return this->getOption(maxStepsOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
            }
            
            bool MultiObjectiveSettings::check() const {
                return !isExportPlotSet()
                    || (ArgumentValidators::writableFileValidator()(getExportPlotUnderApproximationFileName())
                        && ArgumentValidators::writableFileValidator()(getExportPlotOverApproximationFileName())
                        && ArgumentValidators::writableFileValidator()(getExportPlotParetoPointsFileName())
                        && ArgumentValidators::writableFileValidator()(getExportPlotBoundariesFileName()));
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
