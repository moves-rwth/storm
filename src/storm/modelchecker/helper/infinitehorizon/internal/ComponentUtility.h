#pragma once

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace internal {
                
                /// Auxiliary functions that deal with the different kinds of components (MECs on potentially nondeterministic models and BSCCs on deterministic models)
                // BSCCS:
                uint64_t inline getComponentElementState(typename storm::storage::StronglyConnectedComponent::value_type const& element) { return element; }
                uint64_t inline getComponentElementChoiceCount(typename storm::storage::StronglyConnectedComponent::value_type const& element) { return 1; } // Assumes deterministic model!
                uint64_t inline const* getComponentChoicesBegin(typename storm::storage::StronglyConnectedComponent::value_type const& element) { return &element; }
                uint64_t inline const* getComponentChoicesEnd(typename storm::storage::StronglyConnectedComponent::value_type const& element) { return &element + 1; }
                // MECS:
                uint64_t inline getComponentElementState(typename storm::storage::MaximalEndComponent::map_type::value_type const& element) { return element.first; }
                uint64_t inline getComponentElementChoiceCount(typename storm::storage::MaximalEndComponent::map_type::value_type const& element) { return element.second.size(); }
                typename storm::storage::MaximalEndComponent::set_type::const_iterator inline getComponentChoicesBegin(typename storm::storage::MaximalEndComponent::map_type::value_type const& element) { return element.second.begin(); }
                typename storm::storage::MaximalEndComponent::set_type::const_iterator inline getComponentChoicesEnd(typename storm::storage::MaximalEndComponent::map_type::value_type const& element) { return element.second.end(); }
            }
        }
    }
}