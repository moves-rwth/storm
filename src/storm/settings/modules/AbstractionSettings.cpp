#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

namespace storm {
    namespace settings {
        namespace modules {
         
            const std::string AbstractionSettings::moduleName = "abstraction";
            const std::string AbstractionSettings::addAllGuardsOptionName = "allguards";
            const std::string AbstractionSettings::splitPredicatesOptionName = "split-preds";
            const std::string AbstractionSettings::splitInitialGuardsOptionName = "split-init-guards";
            const std::string AbstractionSettings::splitGuardsOptionName = "split-guards";
            const std::string AbstractionSettings::useInterpolationOptionName = "interpolation";
            const std::string AbstractionSettings::splitInterpolantsOptionName = "split-interpolants";
            const std::string AbstractionSettings::splitAllOptionName = "split-all";
            const std::string AbstractionSettings::precisionOptionName = "precision";
            const std::string AbstractionSettings::pivotHeuristicOptionName = "pivot-heuristic";
            const std::string AbstractionSettings::invalidBlockStrategyOptionName = "invalid-blocks";
            const std::string AbstractionSettings::reuseQualitativeResultsOptionName = "reuse-qualitative";
            const std::string AbstractionSettings::reuseQuantitativeResultsOptionName = "reuse-quantitative";
            const std::string AbstractionSettings::reuseAllResultsOptionName = "reuse-all";

            AbstractionSettings::AbstractionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, addAllGuardsOptionName, true, "Sets whether all guards are added as initial predicates.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitPredicatesOptionName, true, "Sets whether the predicates are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitInitialGuardsOptionName, true, "Sets whether the initial guards are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitGuardsOptionName, true, "Sets whether the guards are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, splitAllOptionName, true, "Sets whether all predicates are split into atoms before they are added.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, useInterpolationOptionName, true, "Sets whether interpolation is to be used to eliminate spurious pivot blocks.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-03).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());

                std::vector<std::string> pivotHeuristic = {"nearest-max-dev", "most-prob-path", "max-weighted-dev"};
                this->addOption(storm::settings::OptionBuilder(moduleName, pivotHeuristicOptionName, true, "Sets the pivot selection heuristic.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an available heuristic. Available are: 'nearest-max-dev', 'most-prob-path' and 'max-weighted-dev'.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(pivotHeuristic)).setDefaultValueString("nearest-max-dev").build()).build());

                std::vector<std::string> invalidBlockStrategies = {"none", "command", "global"};
                this->addOption(storm::settings::OptionBuilder(moduleName, invalidBlockStrategyOptionName, true, "Sets the strategy to detect invalid blocks.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an available strategy. Available are: 'none', 'command' and 'global'.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(invalidBlockStrategies)).setDefaultValueString("global").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, reuseQualitativeResultsOptionName, true, "Sets whether to reuse qualitative results.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, reuseQuantitativeResultsOptionName, true, "Sets whether to reuse quantitative results.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, reuseAllResultsOptionName, true, "Sets whether to reuse all results.").build());
            }
            
            bool AbstractionSettings::isAddAllGuardsSet() const {
                return this->getOption(addAllGuardsOptionName).getHasOptionBeenSet();
            }

            bool AbstractionSettings::isSplitPredicatesSet() const {
                return this->getOption(splitPredicatesOptionName).getHasOptionBeenSet();
            }
         
            bool AbstractionSettings::isSplitInitialGuardsSet() const {
                return this->getOption(splitInitialGuardsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isSplitGuardsSet() const {
                return this->getOption(splitGuardsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isSplitAllSet() const {
                return this->getOption(splitAllOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isUseInterpolationSet() const {
                return this->getOption(useInterpolationOptionName).getHasOptionBeenSet();
            }
            
            double AbstractionSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            AbstractionSettings::PivotSelectionHeuristic AbstractionSettings::getPivotSelectionHeuristic() const {
                std::string heuristicName = this->getOption(pivotHeuristicOptionName).getArgumentByName("name").getValueAsString();
                if (heuristicName == "nearest-max-dev") {
                    return AbstractionSettings::PivotSelectionHeuristic::NearestMaximalDeviation;
                } else if (heuristicName == "most-prob-path") {
                    return AbstractionSettings::PivotSelectionHeuristic::MostProbablePath;
                } else if (heuristicName == "max-weighted-dev") {
                    return AbstractionSettings::PivotSelectionHeuristic::MaxWeightedDeviation;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown pivot selection heuristic '" << heuristicName << "'.");
            }
            
            AbstractionSettings::InvalidBlockDetectionStrategy AbstractionSettings::getInvalidBlockDetectionStrategy() const {
                std::string strategyName = this->getOption(invalidBlockStrategyOptionName).getArgumentByName("name").getValueAsString();
                if (strategyName == "none") {
                    return AbstractionSettings::InvalidBlockDetectionStrategy::None;
                } else if (strategyName == "command") {
                    return AbstractionSettings::InvalidBlockDetectionStrategy::Command;
                } else if (strategyName == "global") {
                    return AbstractionSettings::InvalidBlockDetectionStrategy::Global;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown invalid block detection strategy '" << strategyName << "'.");
            }
            
            bool AbstractionSettings::isReuseQualitativeResultsSet() const {
                return this->getOption(reuseQualitativeResultsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isReuseQuantitativeResultsSet() const {
                return this->getOption(reuseQuantitativeResultsOptionName).getHasOptionBeenSet();
            }
            
            bool AbstractionSettings::isReuseAllResultsSet() const {
                return this->getOption(reuseAllResultsOptionName).getHasOptionBeenSet();
            }

        }
    }
}
