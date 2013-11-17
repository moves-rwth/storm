#include "src/storage/Decomposition.h"

namespace storm {
    namespace storage {
        
        Decomposition::Decomposition() : blocks() {
            // Intentionally left empty.
        }
        
        Decomposition::Decomposition(Decomposition const& other) : blocks(other.blocks) {
            // Intentionally left empty.
        }
        
        Decomposition& Decomposition::operator=(Decomposition const& other) {
            this->blocks = other.blocks;
            return *this;
        }
        
        Decomposition::Decomposition(Decomposition&& other) : blocks(std::move(other.blocks)) {
            // Intentionally left empty.
        }
        
        Decomposition& Decomposition::operator=(Decomposition&& other) {
            this->blocks = std::move(other.blocks);
            return *this;
        }
        
        size_t Decomposition::size() const {
            return blocks.size();
        }
        
        Decomposition::iterator Decomposition::begin() {
            return blocks.begin();
        }
        
        Decomposition::iterator Decomposition::end() {
            return blocks.end();
        }
        
        Decomposition::const_iterator Decomposition::begin() const {
            return blocks.begin();
        }
        
        Decomposition::const_iterator Decomposition::end() const {
            return blocks.end();
        }
        
        Decomposition::Block const& Decomposition::getBlock(uint_fast64_t index) const {
            return blocks.at(index);
        }
        
        Decomposition::Block const& Decomposition::operator[](uint_fast64_t index) const {
            return blocks[index];
        }
        
    } // namespace storage
} // namespace storm