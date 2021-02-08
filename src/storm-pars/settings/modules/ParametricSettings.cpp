#include "storm-pars/settings/modules/ParametricSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string ParametricSettings::moduleName = "parametric";
            const std::string ParametricSettings::exportResultOptionName = "resultfile";
            const std::string ParametricSettings::derivativesOptionName = "derivatives";
            const std::string ParametricSettings::transformContinuousOptionName = "transformcontinuous";
            const std::string ParametricSettings::transformContinuousShortOptionName = "tc";
            const std::string ParametricSettings::onlyWellformednessConstraintsOptionName = "onlyconstraints";
            const std::string ParametricSettings::samplesOptionName = "samples";
            const std::string ParametricSettings::samplesGraphPreservingOptionName = "samples-graph-preserving";
            const std::string ParametricSettings::sampleExactOptionName = "sample-exact";
            const std::string ParametricSettings::useMonotonicityName = "use-monotonicity";
//            const std::string ParametricSettings::onlyGlobalName = "onlyGlobal";

            ParametricSettings::ParametricSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultOptionName, false, "A path to a file where the parametric result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, derivativesOptionName, false, "Sets whether to generate the derivatives of the resulting rational function.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, transformContinuousOptionName, false, "Sets whether to transform a continuous time input model to a discrete time model.").setShortName(transformContinuousShortOptionName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, onlyWellformednessConstraintsOptionName, false, "Sets whether you only want to obtain the wellformedness constraints").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, samplesOptionName, false, "The points at which to sample the model.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("samples", "The samples are semicolon-separated entries of the form 'Var1=Val1:Val2:...:Valk,Var2=... that span the sample spaces.").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, samplesGraphPreservingOptionName, false, "Sets whether it can be assumed that the samples are graph-preserving.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, sampleExactOptionName, false, "Sets whether to sample using exact arithmetic.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, useMonotonicityName, false, "If set, monotonicity will be used.").build());
//                this->addOption(storm::settings::OptionBuilder(moduleName, onlyGlobalName, false, "If set, only global monotonicity will be used.").build());
            }
            
            bool ParametricSettings::exportResultToFile() const {
                return this->getOption(exportResultOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportResultPath() const {
                return this->getOption(exportResultOptionName).getArgumentByName("path").getValueAsString();
            }
            
            bool ParametricSettings::isDerivativesSet() const {
                return this->getOption(derivativesOptionName).getHasOptionBeenSet();
            }
            
            bool ParametricSettings::transformContinuousModel() const {
                return this->getOption(transformContinuousOptionName).getHasOptionBeenSet();
            }

            bool ParametricSettings::onlyObtainConstraints() const {
                return this->getOption(onlyWellformednessConstraintsOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::getSamples() const {
                return this->getOption(samplesOptionName).getArgumentByName("samples").getValueAsString();
            }
            
            bool ParametricSettings::isSamplesAreGraphPreservingSet() const {
                return this->getOption(samplesGraphPreservingOptionName).getHasOptionBeenSet();
            }
            
            bool ParametricSettings::isSampleExactSet() const {
                return this->getOption(sampleExactOptionName).getHasOptionBeenSet();
            }

            bool ParametricSettings::isUseMonotonicitySet() const {
                return this->getOption(useMonotonicityName).getHasOptionBeenSet();
            }
//            bool ParametricSettings::isOnlyGlobalSet() const {
//                return this->getOption(onlyGlobalName).getHasOptionBeenSet();
//            }

        } // namespace modules
    } // namespace settings
} // namespace storm
