#include "storm-pars/settings/modules/MonotonicitySettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {

            const std::string MonotonicitySettings::moduleName = "monotonicity";
            const std::string MonotonicitySettings::monotonicityAnalysis = "monotonicity-analysis";
            const std::string MonotonicitySettings::usePLABounds = "mon-bounds";
            const std::string MonotonicitySettings::sccElimination = "mon-elim-scc";
            const std::string MonotonicitySettings::samplesMonotonicityAnalysis = "mon-samples";
            const std::string MonotonicitySettings::precision = "mon-precision";
            const std::string MonotonicitySettings::dotOutput = "dotOutput";
            const std::string MonotonicitySettings::dotOutputName = "exportDotOutput";
            const std::string MonotonicitySettings::exportMonotonicityName = "exportmonotonicity";
            const std::string MonotonicitySettings::monotonicityThreshold ="mon-threshold";
            const std::string MonotonicitySettings::monotoneParameters ="mon-parameters";
            const std::string MonotonicitySettings::monSolution ="mon-solution";


            MonotonicitySettings::MonotonicitySettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, monotonicityAnalysis, false, "Sets whether monotonicity analysis is done").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, usePLABounds, false, "Sets whether pla bounds should be used for monotonicity analysis").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, sccElimination, false, "Sets whether SCCs should be eliminated in the monotonicity analysis").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, samplesMonotonicityAnalysis, false, "Sets whether monotonicity should be checked on samples").setIsAdvanced()
                                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("mon-samples", "The number of samples taken in monotonicity-analysis can be given, default is 0, no samples").setDefaultValueUnsignedInteger(0).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, monSolution, false, "Sets whether monotonicity should be checked on solutionfiunction").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precision, false, "Sets precision of monotonicity checking on samples").setIsAdvanced()
                                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("mon-precision", "The precision of checking monotonicity on samples, default is 1e-6").setDefaultValueDouble(0.000001).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, dotOutput, false, "Sets whether a dot output of the ROs is needed").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportMonotonicityName, false, "Exports the result of monotonicity checking to the given file.").setIsAdvanced().addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The output file.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, dotOutputName, false, "Exports the dot output to the given file.").setIsAdvanced().addArgument(storm::settings::ArgumentBuilder::createStringArgument("dotFilename", "The output file.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, monotoneParameters, false, "Sets monotone parameters from file.").setIsAdvanced().addArgument(storm::settings::ArgumentBuilder::createStringArgument("monotoneParametersFilename", "The file where the monotone parameters are set").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, monotonicityThreshold, false, "Sets whether monotonotonicity should only be used beyond a certain depth threshold").setIsAdvanced()
                                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("mon-threshold", "The depth threshold from which on monotonicity is used for Parameter Lifting").setDefaultValueUnsignedInteger(0).build()).build());

            }

            bool MonotonicitySettings::isMonotonicityAnalysisSet() const {
                return this->getOption(monotonicityAnalysis).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isUsePLABoundsSet() const {
                return this->getOption(usePLABounds).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isSccEliminationSet() const {
                return this->getOption(sccElimination).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isDotOutputSet() const {
                return this->getOption(dotOutput).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isMonotoneParametersSet() const {
                return this->getOption(monotoneParameters).getHasOptionBeenSet();
            }

            std::string MonotonicitySettings::getDotOutputFilename() const {
                if(this->getOption(dotOutputName).getArgumentByName("dotFilename").getHasBeenSet()){
                    return this->getOption(dotOutputName).getArgumentByName("dotFilename").getValueAsString();
                }
                return "dotOutput";
            }

            std::string MonotonicitySettings::getMonotoneParameterFilename() const {
                return this->getOption(monotoneParameters).getArgumentByName("monotoneParametersFilename").getValueAsString();
            }

            uint_fast64_t MonotonicitySettings::getNumberOfSamples() const {
                return this->getOption(samplesMonotonicityAnalysis).getArgumentByName("mon-samples").getValueAsUnsignedInteger();
            }

            double MonotonicitySettings::getMonotonicityAnalysisPrecision() const {
                return this->getOption(precision).getArgumentByName("mon-precision").getValueAsDouble();
            }

            bool MonotonicitySettings::isExportMonotonicitySet() const {
                return this->getOption(exportMonotonicityName).getHasOptionBeenSet();
            }

            std::string MonotonicitySettings::getExportMonotonicityFilename() const {
                return this->getOption(exportMonotonicityName).getArgumentByName("filename").getValueAsString();
            }

            uint_fast64_t MonotonicitySettings::getMonotonicityThreshold() const {
                if (this->isSet("mon-threshold")) {
                    return this->getOption(monotonicityThreshold).getArgumentByName(
                            "mon-threshold").getValueAsUnsignedInteger();
                } else {
                    return 0;
                }
            }

            bool MonotonicitySettings::isMonSolutionSet() const {
                return this->getOption(monSolution).getHasOptionBeenSet();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
