#include "storm-pomdp-cli/settings/modules/GridApproximationSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string GridApproximationSettings::moduleName = "grid";
            
            const std::string refineOption = "refine";
            const std::string resolutionOption = "resolution";
            const std::string limitBeliefExplorationOption = "limit-exploration";
            const std::string numericPrecisionOption = "numeric-precision";
            const std::string cacheSimplicesOption = "cache-simplices";
            const std::string unfoldBeliefMdpOption = "unfold-belief-mdp";

            GridApproximationSettings::GridApproximationSettings() : ModuleSettings(moduleName) {
                
                this->addOption(storm::settings::OptionBuilder(moduleName, refineOption, false,"Enables automatic refinement of the grid until the goal precision is reached").addArgument(
                        storm::settings::ArgumentBuilder::createDoubleArgument("precision","Allowed difference between upper and lower bound of the result.").setDefaultValueDouble(1e-6).makeOptional().addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, resolutionOption, false,"Sets the (initial-) resolution of the grid (higher means more precise results)").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value","the resolution").setDefaultValueUnsignedInteger(10).addValidatorUnsignedInteger(storm::settings::ArgumentValidatorFactory::createUnsignedGreaterValidator(0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, limitBeliefExplorationOption, false,"Sets whether the belief space exploration is stopped if upper and lower bound are close").addArgument(
                        storm::settings::ArgumentBuilder::createDoubleArgument("threshold","the difference between upper and lower bound when to stop").setDefaultValueDouble(0.0).addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleGreaterEqualValidator(0)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, numericPrecisionOption, false,"Sets the precision used to determine whether two belief-states are equal.").addArgument(
                        storm::settings::ArgumentBuilder::createDoubleArgument("value","the precision").setDefaultValueDouble(1e-9).makeOptional().addValidatorDouble(storm::settings::ArgumentValidatorFactory::createDoubleRangeValidatorIncluding(0, 1)).build()).build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, cacheSimplicesOption, false,"Enables caching of simplices which requires more memory but can be faster.").build());
                
                this->addOption(storm::settings::OptionBuilder(moduleName, unfoldBeliefMdpOption, false,"Sets the (initial-) size threshold of the unfolded belief MDP (higher means more precise results, 0 means automatic choice)").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value","the maximal number of states").setDefaultValueUnsignedInteger(0).build()).build());
            }

            bool GridApproximationSettings::isRefineSet() const {
                return this->getOption(refineOption).getHasOptionBeenSet();
            }
            
            double GridApproximationSettings::getRefinementPrecision() const {
                return this->getOption(refineOption).getArgumentByName("precision").getValueAsDouble();
            }
            
            uint64_t GridApproximationSettings::getGridResolution() const {
                return this->getOption(resolutionOption).getArgumentByName("value").getValueAsUnsignedInteger();
            }
            
            double GridApproximationSettings::getExplorationThreshold() const {
                return this->getOption(limitBeliefExplorationOption).getArgumentByName("threshold").getValueAsDouble();
            }
            
            bool GridApproximationSettings::isNumericPrecisionSetFromDefault() const {
                return !this->getOption(numericPrecisionOption).getHasOptionBeenSet() || this->getOption(numericPrecisionOption).getArgumentByName("value").wasSetFromDefaultValue();
            }
            
            double GridApproximationSettings::getNumericPrecision() const {
                return this->getOption(numericPrecisionOption).getArgumentByName("value").getValueAsDouble();
            }
            
            bool GridApproximationSettings::isCacheSimplicesSet() const {
                return this->getOption(cacheSimplicesOption).getHasOptionBeenSet();
            }
            
            bool GridApproximationSettings::isUnfoldBeliefMdpSizeThresholdSet() const {
                return this->getOption(unfoldBeliefMdpOption).getHasOptionBeenSet();
            }
            
            uint64_t GridApproximationSettings::getUnfoldBeliefMdpSizeThreshold() const {
                return this->getOption(unfoldBeliefMdpOption).getArgumentByName("value").getValueAsUnsignedInteger();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
