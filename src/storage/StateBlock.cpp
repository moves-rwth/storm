#include "src/storage/StateBlock.h"

namespace storm {
    namespace storage {
        typename StateBlock::iterator StateBlock::begin() {
            return states.begin();
        }
        
        typename StateBlock::const_iterator StateBlock::begin() const {
            return states.begin();
        }
        
        typename StateBlock::iterator StateBlock::end() {
            return states.end();
        }
        
        typename StateBlock::const_iterator StateBlock::end() const {
            return states.end();
        }
        
        std::size_t StateBlock::size() const {
            return states.size();
        }
        
        bool StateBlock::empty() const {
            return states.empty();
        }
        
        void StateBlock::insert(value_type const& state) {
            states.insert(state);
        }

        void StateBlock::erase(value_type const& state) {
            states.erase(state);
        }

        bool StateBlock::containsState(value_type const& state) const {
            return this->states.find(state) != this->states.end();
        }
        
        StateBlock::container_type const& StateBlock::getStates() const {
            return this->states;
        }
        
        std::ostream& operator<<(std::ostream& out, FlatSetStateContainer const& block) {
            out << "{";
            for (auto const& element : block) {
                out << element << ", ";
            }
            out << "}";
            return out;
        }
        
        std::ostream& operator<<(std::ostream& out, StateBlock const& block) {
            out << block.getStates();
            return out;
        }
    }
}