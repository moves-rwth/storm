#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string AbstractionSettings::methodOptionName = "method";
            const std::string AbstractionSettings::moduleName = "abstraction";
            const std::string AbstractionSettings::useDecompositionOptionName = "decomposition";
            const std::string AbstractionSettings::splitModeOptionName = "split";
            const std::string AbstractionSettings::addAllGuardsOptionName = "all-guards";
            const std::string AbstractionSettings::useInterpolationOptionName = "interpolation";
            const std::string AbstractionSettings::precisionOptionName = "precision";
            const std::string AbstractionSettings::pivotHeuristicOptionName = "pivot-heuristic";
            const std::string AbstractionSettings::reuseResultsOptionName = "reuse";
            const std::string AbstractionSettings::restrictToRelevantStatesOptionName = "relevant";
            
            AbstractionSettings::AbstractionSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> methods = {"games", "bisimulation", "bisim"};
                this->addOption(storm::settings::OptionBuilder(moduleName, methodOptionName, true, "Sets which abstraction-refinement method to use.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(methods))
                                             .setDefaultValueString("bisim").build())
                                .build());
                
                std::vector<std::string> onOff = {"on", "off"};
                
                this->addOption(storm::settings::OptionBuilder(moduleName, useDecompositionOptionName, true, "Sets whether to apply decomposition during the abstraction.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("value", "The value of the flag.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(onOff))
                                             .setDefaultValueString("on").build())
                                .build());
                
                std::vector<std::string> splitModes = {"all", "none", "non-guard"};
                this->addOption(storm::settings::OptionBuilder(moduleName, splitModeOptionName, true, "Sets which predicates are split into atoms for the refinement.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("mode", "The mode to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(splitModes))
                                             .setDefaultValueString("all").build())
                                .build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, addAllGuardsOptionName, true, "Sets whether all guards are added as initial predicates.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("value", "The value of the flag.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(onOff))
                                             .setDefaultValueString("on").build())
                                .build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, useInterpolationOptionName, true, "Sets whether interpolation is to be used to eliminate spurious pivot blocks.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("value", "The value of the flag.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(onOff))
                                             .setDefaultValueString("on").build())
                                .build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for detecting convergence.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-03).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                
                std::vector<std::string> pivotHeuristic = {"nearest-max-dev", "most-prob-path", "max-weighted-dev"};
                this->addOption(storm::settings::OptionBuilder(moduleName, pivotHeuristicOptionName, true, "Sets the pivot selection heuristic.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an available heuristic.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(pivotHeuristic))
                                             .setDefaultValueString("nearest-max-dev").build()).build());
                
                std::vector<std::string> reuseModes = {"all", "none", "qualitative", "quantitative"};
                this->addOption(storm::settings::OptionBuilder(moduleName, reuseResultsOptionName, true, "Sets whether to reuse all results.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("mode", "The mode to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(reuseModes))
                                             .setDefaultValueString("all").build())
                                .build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, restrictToRelevantStatesOptionName, true, "Sets whether to restrict to relevant states during the abstraction.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("value", "The value of the flag.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(onOff))
                                             .setDefaultValueString("off").build())
                                .build());
            }
            
            AbstractionSettings::Method AbstractionSettings::getAbstractionRefinementMethod() const {
                std::string methodAsString = this->getOption(methodOptionName).getArgumentByName("name").getValueAsString();
                if (methodAsString == "games") {
                    return Method::Games;
                } else if (methodAsString == "bisimulation" || methodAsString == "bisim") {
                    return Method::Bisimulation;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown abstraction-refinement method '" << methodAsString << "'.");
            }
            
            bool AbstractionSettings::isUseDecompositionSet() const {
                return this->getOption(useDecompositionOptionName).getHasOptionBeenSet();
            }
            
            AbstractionSettings::SplitMode AbstractionSettings::getSplitMode() const {
                std::string splitModeAsString = this->getOption(splitModeOptionName).getArgumentByName("mode").getValueAsString();
                if (splitModeAsString == "all") {
                    return SplitMode::All;
                } else if (splitModeAsString == "none") {
                    return SplitMode::None;
                } else if (splitModeAsString == "non-guard") {
                    return SplitMode::NonGuard;
                }
                return SplitMode::All;
            }
            
            bool AbstractionSettings::isAddAllGuardsSet() const {
                return this->getOption(addAllGuardsOptionName).getArgumentByName("value").getValueAsString() == "on";
            }
            
            void AbstractionSettings::setAddAllGuards(bool value) {
                this->getOption(addAllGuardsOptionName).getArgumentByName("value").setFromStringValue(value ? "on" : "off");
            }
            
            bool AbstractionSettings::isUseInterpolationSet() const {
                return this->getOption(useInterpolationOptionName).getArgumentByName("value").getValueAsString() == "on";
            }
            
            bool AbstractionSettings::isRestrictToRelevantStatesSet() const {
                return this->getOption(restrictToRelevantStatesOptionName).getArgumentByName("value").getValueAsString() == "on";
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
            
            AbstractionSettings::ReuseMode AbstractionSettings::getReuseMode() const {
                std::string reuseModeAsString = this->getOption(reuseResultsOptionName).getArgumentByName("mode").getValueAsString();
                if (reuseModeAsString == "all") {
                    return ReuseMode::All;
                } else if (reuseModeAsString == "none") {
                    return ReuseMode::None;
                } else if (reuseModeAsString == "qualitative") {
                    return ReuseMode::Qualitative;
                } else if (reuseModeAsString == "quantitative") {
                    return ReuseMode::Quantitative;
                }
                return ReuseMode::All;
            }
            
        }
    }
}
