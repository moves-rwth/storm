#include "storm/settings/modules/ParametricSettings.h"

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
            const std::string ParametricSettings::encodeSmt2StrategyOptionName = "smt2strategy";
            const std::string ParametricSettings::exportSmt2DestinationPathOptionName = "smt2path";
            const std::string ParametricSettings::exportResultDestinationPathOptionName = "resultfile";
            const std::string ParametricSettings::parameterSpaceOptionName = "parameterspace";
            const std::string ParametricSettings::refinementThresholdOptionName = "refinementthreshold";
            const std::string ParametricSettings::exactValidationOptionName = "exactvalidation";
            const std::string ParametricSettings::derivativesOptionName = "derivatives";
            
            ParametricSettings::ParametricSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeSmt2StrategyOptionName, true, "Set the smt2 encoding strategy.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("strategy", "the used strategy").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportSmt2DestinationPathOptionName, true, "A path to a file where the result should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportResultDestinationPathOptionName, true, "A path to a file where the smt2 encoding should be saved.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("path", "the location.").addValidatorString(ArgumentValidatorFactory::createWritableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, parameterSpaceOptionName, true, "Sets the considered parameter-space (i.e., the initial region) for parameter lifting.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("region", "The parameter-space (given in format a<=x<=b,c<=y<=d).").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, refinementThresholdOptionName, true, "Parameter space refinement converges if the fraction of unknown area falls below this threshold.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("threshold", "The threshold").setDefaultValueDouble(0.05).addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0,1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exactValidationOptionName, true, "Sets whether numerical results from Parameter lifting should be validated with exact techniques.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, derivativesOptionName, true, "Sets whether to generate the derivatives of the resulting rational function.").build());
            }
            
            bool ParametricSettings::exportResultToFile() const {
                return this->getOption(exportResultDestinationPathOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportResultPath() const {
                return this->getOption(exportResultDestinationPathOptionName).getArgumentByName("path").getValueAsString();
            }
            
            bool ParametricSettings::isParameterSpaceSet() const {
                return this->getOption(parameterSpaceOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::getParameterSpace() const {
                return this->getOption(parameterSpaceOptionName).getArgumentByName("region").getValueAsString();
            }
            
            double ParametricSettings::getRefinementThreshold() const {
                return this->getOption(refinementThresholdOptionName).getArgumentByName("threshold").getValueAsDouble();
            }
            
            bool ParametricSettings::isExactValidationSet() const {
                return this->getOption(exactValidationOptionName).getHasOptionBeenSet();
            }
            
            bool ParametricSettings::exportToSmt2File() const {
                return this->getOption(exportSmt2DestinationPathOptionName).getHasOptionBeenSet();
            }
            
            std::string ParametricSettings::exportSmt2Path() const {
                return this->getOption(exportSmt2DestinationPathOptionName).getArgumentByName("path").getValueAsString();
            }
            
            ParametricSettings::Smt2EncodingStrategy ParametricSettings::smt2EncodingStrategy() const {
                std::string strategy = this->getOption(encodeSmt2StrategyOptionName).getArgumentByName("strategy").getValueAsString();
                
                if(strategy == "fts") {
                    return Smt2EncodingStrategy::FULL_TRANSITION_SYSTEM;
                } else if(strategy == "rf") {
                    return Smt2EncodingStrategy::RATIONAL_FUNCTION;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown smt2encoding strategy '" << strategy << "'.");
                }
            }
            
            bool ParametricSettings::isDerivativesSet() const {
                return this->getOption(derivativesOptionName).getHasOptionBeenSet();
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
