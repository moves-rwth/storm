#include "TransformationSettings.h"

#include "storm/exceptions/IllegalArgumentValueException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"

namespace storm {
namespace settings {
namespace modules {

const std::string TransformationSettings::moduleName = "transformation";

const std::string TransformationSettings::chainEliminationOptionName = "eliminate-chains";
const std::string TransformationSettings::labelBehaviorOptionName = "ec-label-behavior";
const std::string TransformationSettings::toNondetOptionName = "to-nondet";
const std::string TransformationSettings::toDiscreteTimeOptionName = "to-discrete";

TransformationSettings::TransformationSettings() : ModuleSettings(moduleName) {
    this->addOption(storm::settings::OptionBuilder(moduleName, chainEliminationOptionName, false,
                                                   "If set, chains of non-Markovian states are eliminated if the resulting model is a Markov Automaton.")
                        .setIsAdvanced()
                        .build());
    std::vector<std::string> labelBehavior;
    labelBehavior.push_back("keep");
    labelBehavior.push_back("merge");
    labelBehavior.push_back("delete");
    this->addOption(
        storm::settings::OptionBuilder(moduleName, labelBehaviorOptionName, false,
                                       "Sets the behavior of labels for all non-Markovian states. Some options may cause wrong results.")
            .setIsAdvanced()
            .addArgument(storm::settings::ArgumentBuilder::createStringArgument(
                             "behavior",
                             "The behavior how the transformer handles labels of non-Markovian states. 'keep' does not eliminate states with different labels, "
                             "'merge' builds the union of labels of all eliminated states, 'delete' only keeps the labels of the last state.")
                             .setDefaultValueString("keep")
                             .addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(labelBehavior))
                             .build())
            .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, toNondetOptionName, false,
                                                   "If set, DTMCs/CTMCs are converted to MDPs/MAs (without actual nondeterminism) before model checking.")
                        .setIsAdvanced()
                        .build());
    this->addOption(storm::settings::OptionBuilder(moduleName, toDiscreteTimeOptionName, false,
                                                   "If set, CTMCs/MAs are converted to DTMCs/MDPs (which might or might not preserve the provided properties).")
                        .setIsAdvanced()
                        .build());
}

bool TransformationSettings::isChainEliminationSet() const {
    return this->getOption(chainEliminationOptionName).getHasOptionBeenSet();
}

storm::transformer::EliminationLabelBehavior TransformationSettings::getLabelBehavior() const {
    std::string labelBehaviorAsString = this->getOption(labelBehaviorOptionName).getArgumentByName("behavior").getValueAsString();
    if (labelBehaviorAsString == "keep") {
        return storm::transformer::EliminationLabelBehavior::KeepLabels;
    } else if (labelBehaviorAsString == "merge") {
        return storm::transformer::EliminationLabelBehavior::MergeLabels;
    } else if (labelBehaviorAsString == "delete") {
        return storm::transformer::EliminationLabelBehavior::DeleteLabels;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException,
                    "Illegal value '" << labelBehaviorAsString << "' set as label behavior for the elimination.");
}

bool TransformationSettings::isToNondeterministicModelSet() const {
    return this->getOption(toNondetOptionName).getHasOptionBeenSet();
}

bool TransformationSettings::isToDiscreteTimeModelSet() const {
    return this->getOption(toDiscreteTimeOptionName).getHasOptionBeenSet();
}

bool TransformationSettings::check() const {
    // Ensure that labeling preservation is only set if chain elimination is set
    STORM_LOG_THROW(isChainEliminationSet() || !this->getOption(labelBehaviorOptionName).getHasOptionBeenSet(), storm::exceptions::InvalidSettingsException,
                    "Label preservation can only be chosen if chain elimination is applied.");

    return true;
}

void TransformationSettings::finalize() {
    // Intentionally left empty
}

}  // namespace modules
}  // namespace settings
}  // namespace storm
