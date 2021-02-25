#pragma once

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/logic/AtomicLabelFormula.h"
#include "storm/logic/Formula.h"
#include "storm/settings/SettingsManager.h"

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"

namespace storm {
    namespace utility {

        class RelevantEvents {
        public:

            /*!
             * Create relevant events from given event names.
             * If name 'all' occurs, all elements are stored as relevant.
             *
             * @param relevantEvents List of relevant event names.
             */
            RelevantEvents(std::vector<std::string> const& relevantEvents = {}) {
                // check if the name "all" occurs
                if (std::any_of(relevantEvents.begin(), relevantEvents.end(),
                                [](auto const& name) { return name == "all"; })) {
                    this->allRelevant = true;
                } else {
                    this->names.insert(relevantEvents.begin(), relevantEvents.end());
                }
            }

            bool operator==(RelevantEvents const& rhs) {
                return this->allRelevant == rhs.allRelevant || this->names == rhs.names;
            }

            bool operator!=(RelevantEvents const& rhs) {
                return !(*this == rhs);
            }

            /*!
             * Add relevant event names required by the labels in properties.
             *
             * @param properties List of properties. All events occurring in a property are relevant.
             */
            void addNamesFromProperty(std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties) {
                if (this->allRelevant) {
                    return;
                }

                // Get necessary labels from properties
                std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabels;
                for (auto property : properties) {
                    property->gatherAtomicLabelFormulas(atomicLabels);
                }

                // Add relevant event names from properties
                for (auto atomic : atomicLabels) {
                    std::string label = atomic->getLabel();
                    if (label == "failed" or label == "skipped") {
                        // Ignore as these label will always be added if necessary
                    } else {
                        // Get name of event
                        if (boost::ends_with(label, "_failed")) {
                            // length of "_failed" = 7
                            this->addEvent(label.substr(0, label.size() - 7));
                        } else if (boost::ends_with(label, "_dc")) {
                            // length of "_dc" = 3
                            this->addEvent(label.substr(0, label.size() - 3));
                        } else if (label.find("_claimed_") != std::string::npos) {
                            STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::FaultTreeSettings>().isAddLabelsClaiming(), storm::exceptions::InvalidArgumentException, "Claiming labels will not be exported but are required for label '" << label << "'. Try setting --labels-claiming.");
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Label '" << label << "' not known.");
                        }
                    }
                }
            }

            /*!
             * Check that the relevant names correspond to existing elements in the DFT.
             *
             * @param dft DFT.
             * @return True iff the relevant names are consistent with the given DFT.
             */
            template <typename ValueType>
            bool checkRelevantNames(storm::storage::DFT<ValueType> const& dft) const {
                for (std::string const& relevantName : this->names) {
                    if (!dft.existsName(relevantName)) {
                        return false;
                    }
                }
                return true;
            }

            /*!
             * @return True iff the given name is the name of a relevant Event
             */
            bool isRelevant(std::string const& name) const {
                if (this->allRelevant) {
                    return true;
                } else {
                    return this->names.find(name) != this->names.end();
                }
            }

        private:

            /*!
             * Add relevant event.
             *
             * @param name Name of relevant event.
             */
            void addEvent(std::string const& name) {
                names.insert(name);
            }

            // Names of relevant events.
            std::set<std::string> names{};

            // Whether all elements are relevant.
            bool allRelevant{false};
        };

    } // namespace utility
} // namespace storm
