#include "DFTSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string DFTSettings::moduleName = "dft";
            const std::string DFTSettings::dftFileOptionName = "dftfile";
            const std::string DFTSettings::dftFileOptionShortName = "dft";
            const std::string DFTSettings::dftJsonFileOptionName = "dftfile-json";
            const std::string DFTSettings::dftJsonFileOptionShortName = "dftjson";
            const std::string DFTSettings::symmetryReductionOptionName = "symmetryreduction";
            const std::string DFTSettings::symmetryReductionOptionShortName = "symred";
            const std::string DFTSettings::modularisationOptionName = "modularisation";
            const std::string DFTSettings::disableDCOptionName = "disabledc";
            const std::string DFTSettings::approximationErrorOptionName = "approximation";
            const std::string DFTSettings::approximationErrorOptionShortName = "approx";
            const std::string DFTSettings::approximationHeuristicOptionName = "approximationheuristic";
            const std::string DFTSettings::propExpectedTimeOptionName = "expectedtime";
            const std::string DFTSettings::propExpectedTimeOptionShortName = "mttf";
            const std::string DFTSettings::propProbabilityOptionName = "probability";
            const std::string DFTSettings::propTimeboundOptionName = "timebound";
            const std::string DFTSettings::propTimepointsOptionName = "timepoints";
            const std::string DFTSettings::minValueOptionName = "min";
            const std::string DFTSettings::maxValueOptionName = "max";
            const std::string DFTSettings::firstDependencyOptionName = "firstdep";
            const std::string DFTSettings::transformToGspnOptionName = "gspn";
            const std::string DFTSettings::exportToJsonOptionName = "export-json";
#ifdef STORM_HAVE_Z3
            const std::string DFTSettings::solveWithSmtOptionName = "smt";
#endif

            DFTSettings::DFTSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, dftFileOptionName, false, "Parses the model given in the Galileo format.").setShortName(dftFileOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the DFT model.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, dftJsonFileOptionName, false, "Parses the model given in the Cytoscape JSON format.").setShortName(dftJsonFileOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the JSON file from which to read the DFT model.").addValidatorString(ArgumentValidatorFactory::createExistingFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, symmetryReductionOptionName, false, "Exploit symmetric structure of model.").setShortName(symmetryReductionOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, modularisationOptionName, false, "Use modularisation (not applicable for expected time).").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, disableDCOptionName, false, "Disable Dont Care propagation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, firstDependencyOptionName, false, "Avoid non-determinism by always taking the first possible dependency.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, approximationErrorOptionName, false, "Approximation error allowed.").setShortName(approximationErrorOptionShortName).addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("error", "The relative approximation error to use.").addValidatorDouble(ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, approximationHeuristicOptionName, false, "Set the heuristic used for approximation.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("heuristic", "Sets which heuristic is used for approximation. Must be in {depth, probability}. Default is").setDefaultValueString("depth").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator({"depth", "rateratio"})).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propExpectedTimeOptionName, false, "Compute expected time of system failure.").setShortName(propExpectedTimeOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propProbabilityOptionName, false, "Compute probability of system failure.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propTimeboundOptionName, false, "Compute probability of system failure up to given timebound.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("time", "The timebound to use.").addValidatorDouble(ArgumentValidatorFactory::createDoubleGreaterValidator(0.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, propTimepointsOptionName, false, "Compute probability of system failure up to given timebound for a set of given timepoints [starttime, starttime+inc, starttime+2inc, ... ,endtime]").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("starttime", "The timebound to start from.").addValidatorDouble(ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0.0)).build()).addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("endtime", "The timebound to end with.").addValidatorDouble(ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0.0)).build()).addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("inc", "The value to increment with to get the next timepoint.").addValidatorDouble(ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, minValueOptionName, false, "Compute minimal value in case of non-determinism.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxValueOptionName, false, "Compute maximal value in case of non-determinism.").build());
#ifdef STORM_HAVE_Z3
                this->addOption(storm::settings::OptionBuilder(moduleName, solveWithSmtOptionName, true, "Solve the DFT with SMT.").build());
#endif
                this->addOption(storm::settings::OptionBuilder(moduleName, transformToGspnOptionName, false, "Transform DFT to GSPN.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportToJsonOptionName, false,  "Export the model to the Cytoscape JSON format.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the JSON file to export to.").build()).build());
            }

            bool DFTSettings::isDftFileSet() const {
                return this->getOption(dftFileOptionName).getHasOptionBeenSet();
            }
            
            std::string DFTSettings::getDftFilename() const {
                return this->getOption(dftFileOptionName).getArgumentByName("filename").getValueAsString();
            }

            bool DFTSettings::isDftJsonFileSet() const {
                return this->getOption(dftJsonFileOptionName).getHasOptionBeenSet();
            }

            std::string DFTSettings::getDftJsonFilename() const {
                return this->getOption(dftJsonFileOptionName).getArgumentByName("filename").getValueAsString();
            }

            bool DFTSettings::useSymmetryReduction() const {
                return this->getOption(symmetryReductionOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::useModularisation() const {
                return this->getOption(modularisationOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::isDisableDC() const {
                return this->getOption(disableDCOptionName).getHasOptionBeenSet();
            }

            bool DFTSettings::isApproximationErrorSet() const {
                return this->getOption(approximationErrorOptionName).getHasOptionBeenSet();
            }

            double DFTSettings::getApproximationError() const {
                return this->getOption(approximationErrorOptionName).getArgumentByName("error").getValueAsDouble();
            }

            storm::builder::ApproximationHeuristic DFTSettings::getApproximationHeuristic() const {
                if (!isApproximationErrorSet() || getApproximationError() == 0.0) {
                    // No approximation is done
                    return storm::builder::ApproximationHeuristic::NONE;
                }
                std::string heuristicAsString = this->getOption(approximationHeuristicOptionName).getArgumentByName("heuristic").getValueAsString();
                if (heuristicAsString == "depth") {
                    return storm::builder::ApproximationHeuristic::DEPTH;
                } else if (heuristicAsString == "probability") {
                    return storm::builder::ApproximationHeuristic::PROBABILITY;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Illegal value '" << heuristicAsString << "' set as heuristic for approximation.");
            }

            bool DFTSettings::usePropExpectedTime() const {
                return this->getOption(propExpectedTimeOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::usePropProbability() const {
                return this->getOption(propProbabilityOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::usePropTimebound() const {
                return this->getOption(propTimeboundOptionName).getHasOptionBeenSet();
            }

            double DFTSettings::getPropTimebound() const {
                return this->getOption(propTimeboundOptionName).getArgumentByName("time").getValueAsDouble();
            }

            bool DFTSettings::usePropTimepoints() const {
                return this->getOption(propTimepointsOptionName).getHasOptionBeenSet();
            }

            std::vector<double> DFTSettings::getPropTimepoints() const {
                double starttime = this->getOption(propTimepointsOptionName).getArgumentByName("starttime").getValueAsDouble();
                double endtime = this->getOption(propTimepointsOptionName).getArgumentByName("endtime").getValueAsDouble();
                double inc = this->getOption(propTimepointsOptionName).getArgumentByName("inc").getValueAsDouble();
                std::vector<double> timepoints;
                for (double time = starttime; time <= endtime; time += inc) {
                    timepoints.push_back(time);
                }
                return timepoints;
            }

            bool DFTSettings::isComputeMinimalValue() const {
                return this->getOption(minValueOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::isComputeMaximalValue() const {
                return this->getOption(maxValueOptionName).getHasOptionBeenSet();
            }
            
            bool DFTSettings::isTakeFirstDependency() const {
                return this->getOption(firstDependencyOptionName).getHasOptionBeenSet();
            }

#ifdef STORM_HAVE_Z3
            bool DFTSettings::solveWithSMT() const {
                return this->getOption(solveWithSmtOptionName).getHasOptionBeenSet();
            }
#endif
            
            bool DFTSettings::isTransformToGspn() const {
                return this->getOption(transformToGspnOptionName).getHasOptionBeenSet();
            }

            bool DFTSettings::isExportToJson() const {
                return this->getOption(exportToJsonOptionName).getHasOptionBeenSet();
            }

            std::string DFTSettings::getExportJsonFilename() const {
                return this->getOption(exportToJsonOptionName).getArgumentByName("filename").getValueAsString();
            }

            void DFTSettings::finalize() {
            }

            bool DFTSettings::check() const {
                // Ensure that at most one of min or max is set
                STORM_LOG_THROW(!isComputeMinimalValue() || !isComputeMaximalValue(), storm::exceptions::InvalidSettingsException, "Min and max can not both be set.");
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
