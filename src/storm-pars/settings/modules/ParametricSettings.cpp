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
            const std::string ParametricSettings::simplifyOptionName = "simplify";
            const std::string ParametricSettings::derivativesOptionName = "derivatives";
            
            ParametricSettings::ParametricSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultOptionName, true, "A path to a file where the parametric result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, simplifyOptionName, true, "Sets whether to perform simplification steps before model analysis.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, derivativesOptionName, true, "Sets whether to generate the derivatives of the resulting rational function.").build());
            }
            
            bool ParametricSettings::exportResultToFile() const {
                return this->getOption(exportResultOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportResultPath() const {
                return this->getOption(exportResultOptionName).getArgumentByName("path").getValueAsString();
            }
            
            bool ParametricSettings::isSimplifySet() const {
                return this->getOption(simplifyOptionName).getHasOptionBeenSet();
            }
            
            bool ParametricSettings::isDerivativesSet() const {
                return this->getOption(derivativesOptionName).getHasOptionBeenSet();
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
