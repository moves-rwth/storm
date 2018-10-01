#include "storm/settings/modules/BuildSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/parser/CSVParser.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentValueException.h"

namespace storm {
    namespace settings {
        namespace modules {

            const std::string BuildSettings::moduleName = "build";

            const std::string jitOptionName = "jit";
            const std::string explorationOrderOptionName = "explorder";
            const std::string explorationOrderOptionShortName = "eo";
            const std::string explorationChecksOptionName = "explchecks";
            const std::string explorationChecksOptionShortName = "ec";
            const std::string prismCompatibilityOptionName = "prismcompat";
            const std::string prismCompatibilityOptionShortName = "pc";
            const std::string noBuildOptionName = "nobuild";
            const std::string fullModelBuildOptionName = "buildfull";
            const std::string buildChoiceLabelOptionName = "buildchoicelab";
            const std::string buildStateValuationsOptionName = "buildstateval";
            const std::string buildOutOfBoundsStateOptionName = "buildoutofboundsstate";
            const std::string bitsForUnboundedVariablesOptionName = "int-bits";
            BuildSettings::BuildSettings() : ModuleSettings(moduleName) {

                std::vector<std::string> explorationOrders = {"dfs", "bfs"};
                this->addOption(storm::settings::OptionBuilder(moduleName, prismCompatibilityOptionName, false, "Enables PRISM compatibility. This may be necessary to process some PRISM models.").setShortName(prismCompatibilityOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, jitOptionName, false, "If set, the model is built using the JIT model builder.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, fullModelBuildOptionName, false, "If set, include all rewards and labels.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, buildChoiceLabelOptionName, false, "If set, also build the choice labels").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, buildStateValuationsOptionName, false, "If set, also build the state valuations").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, noBuildOptionName, false, "If set, do not build the model.").build());

                this->addOption(storm::settings::OptionBuilder(moduleName, explorationOrderOptionName, false, "Sets which exploration order to use.").setShortName(explorationOrderOptionShortName)
                                        .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the exploration order to choose.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(explorationOrders)).setDefaultValueString("bfs").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, explorationChecksOptionName, false, "If set, additional checks (if available) are performed during model exploration to debug the model.").setShortName(explorationChecksOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, buildOutOfBoundsStateOptionName, false, "If set, a state for out-of-bounds valuations is added").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, bitsForUnboundedVariablesOptionName, false, "Sets the number of bits that is used for unbounded integer variables.")
                                        .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("number", "The number of bits.").addValidatorUnsignedInteger(ArgumentValidatorFactory::createUnsignedRangeValidatorExcluding(0,63)).setDefaultValueUnsignedInteger(32).build()).build());
            }

            bool BuildSettings::isJitSet() const {
                return this->getOption(jitOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isExplorationOrderSet() const {
                return this->getOption(explorationOrderOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isPrismCompatibilityEnabled() const {
                return this->getOption(prismCompatibilityOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isBuildFullModelSet() const {
                return this->getOption(fullModelBuildOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isNoBuildModelSet() const {
                return this->getOption(noBuildOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isBuildChoiceLabelsSet() const {
                return this->getOption(buildChoiceLabelOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isBuildStateValuationsSet() const {
                return this->getOption(buildStateValuationsOptionName).getHasOptionBeenSet();
            }

            bool BuildSettings::isBuildOutOfBoundsStateSet() const {
                return this->getOption(buildOutOfBoundsStateOptionName).getHasOptionBeenSet();
            }

            storm::builder::ExplorationOrder BuildSettings::getExplorationOrder() const {
                std::string explorationOrderAsString = this->getOption(explorationOrderOptionName).getArgumentByName("name").getValueAsString();
                if (explorationOrderAsString == "dfs") {
                    return storm::builder::ExplorationOrder::Dfs;
                } else if (explorationOrderAsString == "bfs") {
                    return storm::builder::ExplorationOrder::Bfs;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown exploration order '" << explorationOrderAsString << "'.");
            }
            
            bool BuildSettings::isExplorationChecksSet() const {
                return this->getOption(explorationChecksOptionName).getHasOptionBeenSet();
            }

            uint64_t BuildSettings::getBitsForUnboundedVariables() const {
                return this->getOption(bitsForUnboundedVariablesOptionName).getArgumentByName("number").getValueAsUnsignedInteger();
            }

        }


    }
}
