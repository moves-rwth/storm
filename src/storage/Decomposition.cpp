#include "src/storage/Decomposition.h"
#include "src/storage/MaximalEndComponent.h"

namespace storm {
    namespace storage {
        
        template <typename BlockType>
        Decomposition<BlockType>::Decomposition() : blocks() {
            // Intentionally left empty.
        }
        
        template <typename BlockType>
        Decomposition<BlockType>::Decomposition(Decomposition const& other) : blocks(other.blocks) {
            // Intentionally left empty.
        }
        
        template <typename BlockType>
        Decomposition<BlockType>& Decomposition<BlockType>::operator=(Decomposition const& other) {
            this->blocks = other.blocks;
            return *this;
        }
        
        template <typename BlockType>
        Decomposition<BlockType>::Decomposition(Decomposition&& other) : blocks(std::move(other.blocks)) {
            // Intentionally left empty.
        }
        
        template <typename BlockType>
        Decomposition<BlockType>& Decomposition<BlockType>::operator=(Decomposition&& other) {
            this->blocks = std::move(other.blocks);
            return *this;
        }
        
        template <typename BlockType>
        size_t Decomposition<BlockType>::size() const {
            return blocks.size();
        }
        
        template <typename BlockType>
        typename Decomposition<BlockType>::iterator Decomposition<BlockType>::begin() {
            return blocks.begin();
        }
        
        template <typename BlockType>
        typename Decomposition<BlockType>::iterator Decomposition<BlockType>::end() {
            return blocks.end();
        }
        
        template <typename BlockType>
        typename Decomposition<BlockType>::const_iterator Decomposition<BlockType>::begin() const {
            return blocks.begin();
        }
        
        template <typename BlockType>
        typename Decomposition<BlockType>::const_iterator Decomposition<BlockType>::end() const {
            return blocks.end();
        }
        
        template <typename BlockType>
        BlockType const& Decomposition<BlockType>::getBlock(uint_fast64_t index) const {
            return blocks.at(index);
        }
        
        template <typename BlockType>
        BlockType& Decomposition<BlockType>::getBlock(uint_fast64_t index) {
            return blocks.at(index);
        }
        
        template <typename BlockType>
        BlockType const& Decomposition<BlockType>::operator[](uint_fast64_t index) const {
            return blocks[index];
        }
        
        template <typename BlockType>
        BlockType& Decomposition<BlockType>::operator[](uint_fast64_t index) {
            return blocks[index];
        }
        
        template <typename BlockType>
        std::ostream& operator<<(std::ostream& out, Decomposition<BlockType> const& decomposition) {
            out << "[";
            if (decomposition.size() > 0) {
                for (uint_fast64_t blockIndex = 0; blockIndex < decomposition.size() - 1; ++blockIndex) {
                    out << decomposition.blocks[blockIndex] << ", ";
                }
                out << decomposition.blocks.back();
            }
            out << "]";
            return out;
        }
        
        template class Decomposition<StateBlock>;
        template std::ostream& operator<<(std::ostream& out, Decomposition<StateBlock> const& decomposition);
        
        template class Decomposition<MaximalEndComponent>;
        template std::ostream& operator<<(std::ostream& out, Decomposition<MaximalEndComponent> const& decomposition);
    } // namespace storage
} // namespace storm