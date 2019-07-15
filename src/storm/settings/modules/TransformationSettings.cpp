#include "TransformationSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {

            const std::string TransformationSettings::moduleName = "transformation";

            const std::string TransformationSettings::chainEliminationOptionName = "eliminate-chains";
            const std::string TransformationSettings::ignoreLabelingOptionName = "ec-ignore-labeling";


            TransformationSettings::TransformationSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, chainEliminationOptionName, false,
                                                               "If set, chains of non-Markovian states are eliminated if the resulting model is a Markov Automaton.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, ignoreLabelingOptionName, false,
                                                               "If set, the elimination of chains ignores the labels for all non-Markovian states. This may cause wrong results.").build());
            }

            bool TransformationSettings::isChainEliminationSet() const {
                return this->getOption(chainEliminationOptionName).getHasOptionBeenSet();
            }

            bool TransformationSettings::isIgnoreLabelingSet() const {
                return this->getOption(ignoreLabelingOptionName).getHasOptionBeenSet();
            }


            bool TransformationSettings::check() const {
                // Ensure that labeling preservation is only set if chain elimination is set
                STORM_LOG_THROW(isChainEliminationSet() || !isIgnoreLabelingSet(),
                                storm::exceptions::InvalidSettingsException,
                                "Label preservation can only be chosen if chain elimination is applied.");

                return true;
            }

            void TransformationSettings::finalize() {
                //Intentionally left empty
            }


        } // namespace modules
    } // namespace settings
} // namespace storm
