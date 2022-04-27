#include <storm-pars/modelchecker/region/RegionResultHypothesis.h>
#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string RegionSettings::moduleName = "region";
            const std::string RegionSettings::regionOptionName = "region";
            const std::string RegionSettings::regionShortOptionName = "reg";
            const std::string RegionSettings::regionBoundOptionName = "regionbound";
            const std::string RegionSettings::hypothesisOptionName = "hypothesis";
            const std::string RegionSettings::hypothesisShortOptionName = "hyp";
            const std::string RegionSettings::refineOptionName = "refine";
            const std::string RegionSettings::extremumOptionName = "extremum";
            const std::string RegionSettings::extremumSuggestionOptionName = "extremum-init";
            const std::string RegionSettings::splittingThresholdName = "splitting-threshold";
            const std::string RegionSettings::checkEngineOptionName = "engine";
            const std::string RegionSettings::printNoIllustrationOptionName = "noillustration";
            const std::string RegionSettings::printFullResultOptionName = "printfullresult";
            
            RegionSettings::RegionSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, regionOptionName, false, "Sets the region(s) considered for analysis.").setShortName(regionShortOptionName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("regioninput", "The region(s) given in format a<=x<=b,c<=y<=d seperated by ';'. Can also be a file.").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, regionBoundOptionName, false, "Sets the region bound considered for analysis.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("regionbound", "The bound for the region result for all variables: 0+bound <= var <=1-bound").build()).build());

                std::vector<std::string> hypotheses = {"unknown", "allsat", "allviolated"};
                this->addOption(storm::settings::OptionBuilder(moduleName, hypothesisOptionName, false, "Sets a hypothesis for region analysis. If given, the region(s) are only analyzed w.r.t. that hypothesis.").setShortName(hypothesisShortOptionName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("hypothesis", "The hypothesis.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(hypotheses)).setDefaultValueString("unknown").build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, refineOptionName, false, "Enables region refinement.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("coverage-threshold", "Refinement converges if the fraction of unknown area falls below this threshold.").setDefaultValueDouble(0.05).addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0.0,1.0)).build())
                                .addArgument(storm::settings::ArgumentBuilder::createIntegerArgument("depth-limit", "If given, limits the number of times a region is refined.").setDefaultValueInteger(-1).makeOptional().build()).build());
                
                std::vector<std::string> directions = {"min", "max"};
                std::vector<std::string> precisiontype = {"rel", "abs"};
                this->addOption(storm::settings::OptionBuilder(moduleName, extremumOptionName, false, "Computes the extremum within the region.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("direction", "The optimization direction").addValidatorString(storm::settings::ArgumentValidatorFactory::createMultipleChoiceValidator(directions)).build())
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("precision", "The desired precision").setDefaultValueDouble(0.05).makeOptional().addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0.0,1.0)).build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("precisiontype", "The desired precision type.").setDefaultValueString("rel").makeOptional().addValidatorString(storm::settings::ArgumentValidatorFactory::createMultipleChoiceValidator(precisiontype)).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, extremumSuggestionOptionName, false, "Checks whether the provided value is indeed the extremum")
                                        .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("extremum-suggestion", "The provided value for the extremum").addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0.0,1.0)).build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, splittingThresholdName, false, "Sets the threshold for number of parameters in which to split regions.")
                                        .addArgument(storm::settings::ArgumentBuilder::createIntegerArgument("splitting-threshold", "The threshold for splitting, should be an integer > 0").build()).build());

                std::vector<std::string> engines = {"pl", "exactpl", "validatingpl"};
                this->addOption(storm::settings::OptionBuilder(moduleName, checkEngineOptionName, true, "Sets which engine is used for analyzing regions.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(engines)).setDefaultValueString("pl").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, printNoIllustrationOptionName, false, "If set, no illustration of the result is printed.").build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, printFullResultOptionName, false, "If set, the full result for every region is printed.").build());
            }
            
            bool RegionSettings::isRegionSet() const {
                return this->getOption(regionOptionName).getHasOptionBeenSet();
            }
            
            std::string RegionSettings::getRegionString() const {
                return this->getOption(regionOptionName).getArgumentByName("regioninput").getValueAsString();
            }

            bool RegionSettings::isRegionBoundSet() const {
                return this->getOption(regionBoundOptionName).getHasOptionBeenSet();
            }

            std::string RegionSettings::getRegionBoundString() const {
                return this->getOption(regionBoundOptionName).getArgumentByName("regionbound").getValueAsString();
            }
            
            bool RegionSettings::isHypothesisSet() const {
                return this->getOption(hypothesisOptionName).getHasOptionBeenSet();
            }
            
            storm::modelchecker::RegionResultHypothesis RegionSettings::getHypothesis() const {
                std::string hypString = this->getOption(hypothesisOptionName).getArgumentByName("hypothesis").getValueAsString();
                
                storm::modelchecker::RegionResultHypothesis result;
                
                if (hypString == "unknown") {
                    result = storm::modelchecker::RegionResultHypothesis::Unknown;
                } else if (hypString == "allsat") {
                    result = storm::modelchecker::RegionResultHypothesis::AllSat;
                } else if (hypString == "allviolated") {
                    result = storm::modelchecker::RegionResultHypothesis::AllViolated;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Hypothesis " << hypString << " not known.");
                }
                
                return result;
            }
            
            bool RegionSettings::isRefineSet() const {
                return this->getOption(refineOptionName).getHasOptionBeenSet();
            }
            
            double RegionSettings::getCoverageThreshold() const {
                return this->getOption(refineOptionName).getArgumentByName("coverage-threshold").getValueAsDouble();
            }
            
            bool RegionSettings::isDepthLimitSet() const {
                return this->getOption(refineOptionName).getArgumentByName("depth-limit").getHasBeenSet() && this->getOption(refineOptionName).getArgumentByName("depth-limit").getValueAsInteger() >= 0;
            }
            
            uint64_t RegionSettings::getDepthLimit() const {
                int64_t depth = this->getOption(refineOptionName).getArgumentByName("depth-limit").getValueAsInteger();
                STORM_LOG_THROW(depth >= 0, storm::exceptions::InvalidOperationException, "Tried to retrieve the depth limit but it was not set.");
                return (uint64_t) depth;
            }
            
            bool RegionSettings::isExtremumSet() const {
                return this->getOption(extremumOptionName).getHasOptionBeenSet();
            }
				
            storm::solver::OptimizationDirection RegionSettings::getExtremumDirection() const {
                auto str = this->getOption(extremumOptionName).getArgumentByName("direction").getValueAsString();
                if (str == "min") {
                    return storm::solver::OptimizationDirection::Minimize;
                } else {
                    assert(str == "max");
                    return storm::solver::OptimizationDirection::Maximize;
                }
            }
				
            double RegionSettings::getExtremumValuePrecision() const {
                auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
                if (!generalSettings.isPrecisionSet() && generalSettings.isSoundSet()) {
                    double prec = this->getOption(extremumOptionName).getArgumentByName("precision").getValueAsDouble() / 10;
                    generalSettings.setPrecision(std::to_string(prec));
                    STORM_LOG_WARN("Reset precision for solver to " << prec << " this is sufficient for extremum value precision of " << (prec)*10 << '\n');
                }
                return this->getOption(extremumOptionName).getArgumentByName("precision").getValueAsDouble();
            }

            bool RegionSettings::isAbsolutePrecisionSet() const {
                auto str = this->getOption(extremumOptionName).getArgumentByName("precisiontype").getValueAsString();
                if (str == "abs") {
                    return true;
                } else {
                    assert(str == "rel");
                    return false;
                }
            }

            bool RegionSettings::isExtremumSuggestionSet() const {
                return this->getOption(extremumSuggestionOptionName).getHasOptionBeenSet();
            }

            double RegionSettings::getExtremumSuggestion() const {
                return this->getOption(extremumSuggestionOptionName).getArgumentByName("extremum-suggestion").getValueAsDouble();
            }

            storm::modelchecker::RegionCheckEngine RegionSettings::getRegionCheckEngine() const {
                std::string engineString = this->getOption(checkEngineOptionName).getArgumentByName("name").getValueAsString();
                
                storm::modelchecker::RegionCheckEngine result;
                if (engineString == "pl") {
                    result = storm::modelchecker::RegionCheckEngine::ParameterLifting;
                } else if (engineString == "exactpl") {
                    result = storm::modelchecker::RegionCheckEngine::ExactParameterLifting;
                } else if (engineString == "validatingpl") {
                    result = storm::modelchecker::RegionCheckEngine::ValidatingParameterLifting;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown region check engine '" << engineString << "'.");
                }
                
                return result;
            }
            
            bool RegionSettings::check() const {
                if (isRefineSet() && isExtremumSet()) {
                    STORM_LOG_ERROR("Can not compute extremum values AND perform region refinement.");
                    return false;
                }
                if (getExtremumValuePrecision() < storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()) {
                    STORM_LOG_ERROR("Computing extremum value for precision " << getExtremumValuePrecision() << " makes no sense when solver precision is set to " << storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
                    return false;
                }
                return true;
            }

            bool RegionSettings::isPrintNoIllustrationSet() const {
                return this->getOption(printNoIllustrationOptionName).getHasOptionBeenSet();
            }
            
            bool RegionSettings::isPrintFullResultSet() const {
                return this->getOption(printFullResultOptionName).getHasOptionBeenSet();
            }

            int RegionSettings::getSplittingThreshold() const {
                return this->getOption(splittingThresholdName).getArgumentByName("splitting-threshold").getValueAsInteger();
            }

            bool RegionSettings::isSplittingThresholdSet() const {
                return this->getOption(splittingThresholdName).getHasOptionBeenSet();
            }


        } // namespace modules
    } // namespace settings
} // namespace storm
