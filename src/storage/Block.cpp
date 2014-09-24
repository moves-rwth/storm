#include "src/storage/Block.h"

namespace storm {
    namespace storage {
        Block::iterator Block::begin() {
            return states.begin();
        }
        
        Block::const_iterator Block::begin() const {
            return states.begin();
        }
        
        StronglyConnectedComponent::iterator Block::end() {
            return states.end();
        }
        
        StronglyConnectedComponent::const_iterator Block::end() const {
            return states.end();
        }
        
        std::size_t Block::size() const {
            return states.size();
        }
        
        void Block::insert(value_type const& state) {
            states.insert(state);
        }
        
        void Block::erase(value_type const& state) {
            states.erase(state);
        }
        
        bool Block::containsState(value_type const& state) const {
            return this->states.find(state) != this->states.end();
        }
        
        std::ostream& operator<<(std::ostream& out, StateBlock const& block) {
            out << "{";
            for (auto const& element : block) {
                out << element << ", ";
            }
            out << "}";
            return out;
        }
        
        // Explicitly instantiate template.
        template Block<>;
    }
}