#include "src/storage/StateBlock.h"

namespace storm {
    namespace storage {
        template <typename ContainerType>
        typename StateBlock<ContainerType>::iterator StateBlock<ContainerType>::begin() {
            return states.begin();
        }
        
        template <typename ContainerType>
        typename StateBlock<ContainerType>::const_iterator StateBlock<ContainerType>::begin() const {
            return states.begin();
        }
        
        template <typename ContainerType>
        typename StateBlock<ContainerType>::iterator StateBlock<ContainerType>::end() {
            return states.end();
        }
        
        template <typename ContainerType>
        typename StateBlock<ContainerType>::const_iterator StateBlock<ContainerType>::end() const {
            return states.end();
        }
        
        template <typename ContainerType>
        std::size_t StateBlock<ContainerType>::size() const {
            return states.size();
        }
        
        template <typename ContainerType>
        void StateBlock<ContainerType>::insert(value_type const& state) {
            states.insert(state);
        }

        template <typename ContainerType>
        void StateBlock<ContainerType>::erase(value_type const& state) {
            states.erase(state);
        }

        template <typename ContainerType>
        bool StateBlock<ContainerType>::containsState(value_type const& state) const {
            return this->states.find(state) != this->states.end();
        }
        
        std::ostream& operator<<(std::ostream& out, FlatSetStateContainer const& block) {
            out << "{";
            for (auto const& element : block) {
                out << element << ", ";
            }
            out << "}";
            return out;
        }
        
        // Explicitly instantiate template.
        template Block<FlatSetStateContainer>;
    }
}