#pragma once

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"

namespace storm {
    namespace utility {

        /*!
         * Get relevant event names from labels in properties.
         *
         * @param dft DFT.
         * @param properties List of properties. All events occurring in a property are relevant.
         * @return List of relevant event names.
         */
        template <typename ValueType>
        std::vector<std::string> getRelevantEventNames(storm::storage::DFT<ValueType> const& dft, std::vector<std::shared_ptr<storm::logic::Formula const>> const& properties) {
            // Get necessary labels from properties
            std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabels;
            for (auto property : properties) {
                property->gatherAtomicLabelFormulas(atomicLabels);
            }
            // Add relevant event names from properties
            std::vector<std::string> relevantEventNames;
            for (auto atomic : atomicLabels) {
                std::string label = atomic->getLabel();
                if (label == "failed" or label == "skipped") {
                    // Ignore as these label will always be added if necessary
                } else {
                    // Get name of event
                    if (boost::ends_with(label, "_failed")) {
                        relevantEventNames.push_back(label.substr(0, label.size() - 7));
                    } else if (boost::ends_with(label, "_dc")) {
                        relevantEventNames.push_back(label.substr(0, label.size() - 3));
                    } else if (label.find("_claimed_") != std::string::npos) {
                        STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::FaultTreeSettings>().isAddLabelsClaiming(), storm::exceptions::InvalidArgumentException, "Claiming labels will not be exported but are required for label '" << label << "'. Try setting --labels-claiming.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Label '" << label << "' not known.");
                    }
                }
            }
            return relevantEventNames;
        }

        /*!
         * Get relevant event id from relevant event name.
         *
         * @param dft DFT.
         * @param relevantEventNames Names of relevant events.
         * @return Set of relevant event ids.
         */
        template <typename ValueType>
        std::set<size_t> getRelevantEvents(storm::storage::DFT<ValueType> const& dft, std::vector<std::string> const& relevantEventNames) {
            // Set relevant elements
            std::set<size_t> relevantEvents; // Per default no event (except the toplevel event) is relevant
            for (std::string const& relevantName : relevantEventNames) {
                if (relevantName == "all") {
                    // All events are relevant
                    return dft.getAllIds();
                } else {
                    // Find and add corresponding event id
                    relevantEvents.insert(dft.getIndex(relevantName));
                }
            }
            return relevantEvents;
        }

    } // namespace utility
} // namespace storm