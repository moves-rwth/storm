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
            const std::string MonotonicitySettings::sccElimination = "mon-elim-scc";
            const std::string MonotonicitySettings::validateAssumptions = "mon-validate-assumptions";
            const std::string MonotonicitySettings::samplesMonotonicityAnalysis = "mon-samples";
            const std::string MonotonicitySettings::precision = "mon-precision";

            MonotonicitySettings::MonotonicitySettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, monotonicityAnalysis, false, "Sets whether monotonicity analysis is done").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, sccElimination, false, "Sets whether SCCs should be eliminated in the monotonicity analysis").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, validateAssumptions, false, "Sets whether assumptions made in monotonicity analysis are validated").setIsAdvanced().build());
                this->addOption(storm::settings::OptionBuilder(moduleName, samplesMonotonicityAnalysis, false, "Sets whether monotonicity should be checked on samples").setIsAdvanced()
                                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("mon-samples", "The number of samples taken in monotonicity-analysis can be given, default is 0, no samples").setDefaultValueUnsignedInteger(0).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precision, false, "Sets precision of monotonicity checking on samples").setIsAdvanced()
                                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("mon-precision", "The precision of checking monotonicity on samples, default is 1e-6").setDefaultValueDouble(0.000001).build()).build());

            }

            bool MonotonicitySettings::isMonotonicityAnalysisSet() const {
                return this->getOption(monotonicityAnalysis).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isSccEliminationSet() const {
                return this->getOption(sccElimination).getHasOptionBeenSet();
            }

            bool MonotonicitySettings::isValidateAssumptionsSet() const {
                return this->getOption(validateAssumptions).getHasOptionBeenSet();
            }

            uint_fast64_t MonotonicitySettings::getNumberOfSamples() const {
                return this->getOption(samplesMonotonicityAnalysis).getArgumentByName("mon-samples").getValueAsUnsignedInteger();
            }

            double MonotonicitySettings::getMonotonicityAnalysisPrecision() const {
                return this->getOption(precision).getArgumentByName("mon-precision").getValueAsDouble();
            }
        } // namespace modules
    } // namespace settings
} // namespace storm
