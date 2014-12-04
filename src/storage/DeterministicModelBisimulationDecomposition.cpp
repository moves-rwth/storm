#include "src/storage/DeterministicModelBisimulationDecomposition.h"

#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <boost/iterator/transform_iterator.hpp>

#include "src/utility/graph.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::Block::Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next, std::size_t id, std::shared_ptr<std::string> const& label) : next(next), prev(prev), begin(begin), end(end), markedAsSplitter(false), markedAsPredecessorBlock(false), markedPosition(begin), absorbing(false), id(id), label(label) {
            if (next != nullptr) {
                next->prev = this;
            }
            if (prev != nullptr) {
                prev->next = this;
            }
            STORM_LOG_ASSERT(begin < end, "Unable to create block of illegal size.");
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::print(Partition const& partition) const {
            std::cout << "block " << this->getId() << " with marked position " << this->getMarkedPosition() << " and original begin " << this->getOriginalBegin() << std::endl;
            std::cout << "begin: " << this->begin << " and end: " << this->end << " (number of states: " << this->getNumberOfStates() << ")" << std::endl;
            std::cout << "states:" << std::endl;
            for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                std::cout << partition.statesAndValues[index].first << " ";
            }
            std::cout << std::endl << "original: " << std::endl;
            for (storm::storage::sparse::state_type index = this->getOriginalBegin(); index < this->end; ++index) {
                std::cout << partition.statesAndValues[index].first << " ";
            }
            std::cout << std::endl << "values:" << std::endl;
            for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                std::cout << std::setprecision(3) << partition.statesAndValues[index].second << " ";
            }
            if (partition.keepSilentProbabilities) {
                std::cout << std::endl << "silent:" << std::endl;
                for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                    std::cout << std::setprecision(3) << partition.silentProbabilities[partition.statesAndValues[index].first] << " ";
                }
            }
            std::cout << std::endl;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::setBegin(storm::storage::sparse::state_type begin) {
            this->begin = begin;
            this->markedPosition = begin;
            STORM_LOG_ASSERT(begin < end, "Unable to resize block to illegal size.");
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::setEnd(storm::storage::sparse::state_type end) {
            this->end = end;
            STORM_LOG_ASSERT(begin < end, "Unable to resize block to illegal size.");
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::incrementBegin() {
            ++this->begin;
            STORM_LOG_ASSERT(begin <= end, "Unable to resize block to illegal size.");
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type DeterministicModelBisimulationDecomposition<ValueType>::Block::getBegin() const {
            return this->begin;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type DeterministicModelBisimulationDecomposition<ValueType>::Block::getOriginalBegin() const {
            if (this->hasPreviousBlock()) {
                return this->getPreviousBlock().getEnd();
            } else {
                return 0;
            }
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type DeterministicModelBisimulationDecomposition<ValueType>::Block::getEnd() const {
            return this->end;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::setIterator(iterator it) {
            this->selfIt = it;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block::iterator DeterministicModelBisimulationDecomposition<ValueType>::Block::getIterator() const {
            return this->selfIt;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block::iterator DeterministicModelBisimulationDecomposition<ValueType>::Block::getNextIterator() const {
            return this->getNextBlock().getIterator();
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block::iterator DeterministicModelBisimulationDecomposition<ValueType>::Block::getPreviousIterator() const {
            return this->getPreviousBlock().getIterator();
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Block::getNextBlock() {
            return *this->next;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block const& DeterministicModelBisimulationDecomposition<ValueType>::Block::getNextBlock() const {
            return *this->next;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::hasNextBlock() const {
            return this->next != nullptr;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Block::getPreviousBlock() {
            return *this->prev;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block* DeterministicModelBisimulationDecomposition<ValueType>::Block::getPreviousBlockPointer() {
            return this->prev;
        }

        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block* DeterministicModelBisimulationDecomposition<ValueType>::Block::getNextBlockPointer() {
            return this->next;
        }

        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block const& DeterministicModelBisimulationDecomposition<ValueType>::Block::getPreviousBlock() const {
            return *this->prev;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::hasPreviousBlock() const {
            return this->prev != nullptr;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::check() const {
            assert(this->begin < this->end);
            assert(this->prev == nullptr || this->prev->next == this);
            assert(this->next == nullptr || this->next->prev == this);
            return true;
        }
        
        template<typename ValueType>
        std::size_t DeterministicModelBisimulationDecomposition<ValueType>::Block::getNumberOfStates() const {
            // We need to take the original begin here, because the begin is temporarily moved.
            return (this->end - this->getOriginalBegin());
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::isMarkedAsSplitter() const {
            return this->markedAsSplitter;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::markAsSplitter() {
            this->markedAsSplitter = true;
        }

        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::unmarkAsSplitter() {
            this->markedAsSplitter = false;
        }

        template<typename ValueType>
        std::size_t DeterministicModelBisimulationDecomposition<ValueType>::Block::getId() const {
            return this->id;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::setAbsorbing(bool absorbing) {
            this->absorbing = absorbing;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::isAbsorbing() const {
            return this->absorbing;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type DeterministicModelBisimulationDecomposition<ValueType>::Block::getMarkedPosition() const {
            return this->markedPosition;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::setMarkedPosition(storm::storage::sparse::state_type position) {
            this->markedPosition = position;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::resetMarkedPosition() {
            this->markedPosition = this->begin;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::incrementMarkedPosition() {
            ++this->markedPosition;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::markAsPredecessorBlock() {
            this->markedAsPredecessorBlock = true;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Block::unmarkAsPredecessorBlock() {
            this->markedAsPredecessorBlock = false;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::isMarkedAsPredecessor() const {
            return markedAsPredecessorBlock;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Block::hasLabel() const {
            return this->label != nullptr;
        }
        
        template<typename ValueType>
        std::string const& DeterministicModelBisimulationDecomposition<ValueType>::Block::getLabel() const {
            STORM_LOG_THROW(this->label != nullptr, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve label of block that has none.");
            return *this->label;
        }
        
        template<typename ValueType>
        std::shared_ptr<std::string> const& DeterministicModelBisimulationDecomposition<ValueType>::Block::getLabelPtr() const {
            return this->label;
        }
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::Partition::Partition(std::size_t numberOfStates, bool keepSilentProbabilities) : stateToBlockMapping(numberOfStates), statesAndValues(numberOfStates), positions(numberOfStates), keepSilentProbabilities(keepSilentProbabilities), silentProbabilities() {
            // Create the block and give it an iterator to itself.
            typename std::list<Block>::iterator it = blocks.emplace(this->blocks.end(), 0, numberOfStates, nullptr, nullptr, this->blocks.size());
            it->setIterator(it);
            
            // Set up the different parts of the internal structure.
            for (storm::storage::sparse::state_type state = 0; state < numberOfStates; ++state) {
                statesAndValues[state] = std::make_pair(state, storm::utility::zero<ValueType>());
                positions[state] = state;
                stateToBlockMapping[state] = &blocks.back();
            }
            
            // If we are requested to store silent probabilities, we need to prepare the vector.
            if (this->keepSilentProbabilities) {
                silentProbabilities = std::vector<ValueType>(numberOfStates);
            }
        }
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::Partition::Partition(std::size_t numberOfStates, storm::storage::BitVector const& prob0States, storm::storage::BitVector const& prob1States, std::string const& otherLabel, std::string const& prob1Label, bool keepSilentProbabilities) : stateToBlockMapping(numberOfStates), statesAndValues(numberOfStates), positions(numberOfStates), keepSilentProbabilities(keepSilentProbabilities), silentProbabilities() {
            typename std::list<Block>::iterator firstIt = blocks.emplace(this->blocks.end(), 0, prob0States.getNumberOfSetBits(), nullptr, nullptr, this->blocks.size());
            Block& firstBlock = *firstIt;
            firstBlock.setIterator(firstIt);

            storm::storage::sparse::state_type position = 0;
            for (auto state : prob0States) {
                statesAndValues[position] = std::make_pair(state, storm::utility::zero<ValueType>());
                positions[state] = position;
                stateToBlockMapping[state] = &firstBlock;
                ++position;
            }
            firstBlock.setAbsorbing(true);

            typename std::list<Block>::iterator secondIt = blocks.emplace(this->blocks.end(), position, position + prob1States.getNumberOfSetBits(), &firstBlock, nullptr, this->blocks.size(), std::shared_ptr<std::string>(new std::string(prob1Label)));
            Block& secondBlock = *secondIt;
            secondBlock.setIterator(secondIt);
            
            for (auto state : prob1States) {
                statesAndValues[position] = std::make_pair(state, storm::utility::zero<ValueType>());
                positions[state] = position;
                stateToBlockMapping[state] = &secondBlock;
                ++position;
            }
            secondBlock.setAbsorbing(true);
            
            typename std::list<Block>::iterator thirdIt = blocks.emplace(this->blocks.end(), position, numberOfStates, &secondBlock, nullptr, this->blocks.size(), otherLabel == "true" ? std::shared_ptr<std::string>(nullptr) : std::shared_ptr<std::string>(new std::string(otherLabel)));
            Block& thirdBlock = *thirdIt;
            thirdBlock.setIterator(thirdIt);
            
            storm::storage::BitVector otherStates = ~(prob0States | prob1States);
            for (auto state : otherStates) {
                statesAndValues[position] = std::make_pair(state, storm::utility::zero<ValueType>());
                positions[state] = position;
                stateToBlockMapping[state] = &thirdBlock;
                ++position;
            }
            
            // If we are requested to store silent probabilities, we need to prepare the vector.
            if (this->keepSilentProbabilities) {
                silentProbabilities = std::vector<ValueType>(numberOfStates);
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::swapStates(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
            std::swap(this->statesAndValues[this->positions[state1]], this->statesAndValues[this->positions[state2]]);
            std::swap(this->positions[state1], this->positions[state2]);
        }

        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::swapStatesAtPositions(storm::storage::sparse::state_type position1, storm::storage::sparse::state_type position2) {
            storm::storage::sparse::state_type state1 = this->statesAndValues[position1].first;
            storm::storage::sparse::state_type state2 = this->statesAndValues[position2].first;
            
            std::swap(this->statesAndValues[position1], this->statesAndValues[position2]);
            
            this->positions[state1] = position2;
            this->positions[state2] = position1;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getPosition(storm::storage::sparse::state_type state) const {
            return this->positions[state];
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::setPosition(storm::storage::sparse::state_type state, storm::storage::sparse::state_type position) {
            this->positions[state] = position;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getState(storm::storage::sparse::state_type position) const {
            return this->statesAndValues[position].first;
        }
        
        template<typename ValueType>
        ValueType const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getValue(storm::storage::sparse::state_type state) const {
            return this->statesAndValues[this->positions[state]].second;
        }
        
        template<typename ValueType>
        ValueType const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getValueAtPosition(storm::storage::sparse::state_type position) const {
            return this->statesAndValues[position].second;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::setValue(storm::storage::sparse::state_type state, ValueType value) {
            this->statesAndValues[this->positions[state]].second = value;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::increaseValue(storm::storage::sparse::state_type state, ValueType value) {
            this->statesAndValues[this->positions[state]].second += value;
        }

        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::updateBlockMapping(Block& block, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last) {
            for (; first != last; ++first) {
                this->stateToBlockMapping[first->first] = &block;
            }
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getFirstBlock() {
            return *this->blocks.begin();
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBlock(storm::storage::sparse::state_type state) {
            return *this->stateToBlockMapping[state];
        }

        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBlock(storm::storage::sparse::state_type state) const {
            return *this->stateToBlockMapping[state];
        }

        template<typename ValueType>
        typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBegin(Block const& block) {
            return this->statesAndValues.begin() + block.getBegin();
        }
        
        template<typename ValueType>
        typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator DeterministicModelBisimulationDecomposition<ValueType>::Partition::getEnd(Block const& block) {
            return this->statesAndValues.begin() + block.getEnd();
        }

        template<typename ValueType>
        typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBegin(Block const& block) const {
            return this->statesAndValues.begin() + block.getBegin();
        }
        
        template<typename ValueType>
        typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator DeterministicModelBisimulationDecomposition<ValueType>::Partition::getEnd(Block const& block) const {
            return this->statesAndValues.begin() + block.getEnd();
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Partition::splitBlock(Block& block, storm::storage::sparse::state_type position) {
            // In case one of the resulting blocks would be empty, we simply return the current block and do not create
            // a new one.
            if (position == block.getBegin() || position == block.getEnd()) {
                return block;
            }
            
            // Actually create the new block and insert it at the correct position.
            typename std::list<Block>::iterator selfIt = this->blocks.emplace(block.getIterator(), block.getBegin(), position, block.getPreviousBlockPointer(), &block, this->blocks.size(), block.getLabelPtr());
            selfIt->setIterator(selfIt);
            Block& newBlock = *selfIt;
            
            // Resize the current block appropriately.
            block.setBegin(position);
            
            // Update the mapping of the states in the newly created block.
            this->updateBlockMapping(newBlock, this->getBegin(newBlock), this->getEnd(newBlock));
            
            return newBlock;
        }
        
        template<typename ValueType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Block& DeterministicModelBisimulationDecomposition<ValueType>::Partition::insertBlock(Block& block) {
            // Find the beginning of the new block.
            storm::storage::sparse::state_type begin;
            if (block.hasPreviousBlock()) {
                begin = block.getPreviousBlock().getEnd();
            } else {
                begin = 0;
            }
            
            // Actually insert the new block.
            typename std::list<Block>::iterator it = this->blocks.emplace(block.getIterator(), begin, block.getBegin(), block.getPreviousBlockPointer(), &block, this->blocks.size());
            Block& newBlock = *it;
            newBlock.setIterator(it);
            
            // Update the mapping of the states in the newly created block.
            for (auto it = this->getBegin(newBlock), ite = this->getEnd(newBlock); it != ite; ++it) {
                stateToBlockMapping[it->first] = &newBlock;
            }
            
            return *it;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::splitLabel(storm::storage::BitVector const& statesWithLabel) {
            for (auto blockIterator = this->blocks.begin(), ite = this->blocks.end(); blockIterator != ite; ) { // The update of the loop was intentionally moved to the bottom of the loop.
                Block& block = *blockIterator;
                
                // Sort the range of the block such that all states that have the label are moved to the front.
                std::sort(this->getBegin(block), this->getEnd(block), [&statesWithLabel] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return statesWithLabel.get(a.first) && !statesWithLabel.get(b.first); } );
                
                // Update the positions vector.
                storm::storage::sparse::state_type position = block.getBegin();
                for (auto stateIt = this->getBegin(block), stateIte = this->getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                    this->positions[stateIt->first] = position;
                }
                
                // Now we can find the first position in the block that does not have the label and create new blocks.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator it = std::find_if(this->getBegin(block), this->getEnd(block), [&] (std::pair<storm::storage::sparse::state_type, ValueType> const& a) { return !statesWithLabel.get(a.first); });
                
                // If not all the states agreed on the validity of the label, we need to split the block.
                if (it != this->getBegin(block) && it != this->getEnd(block)) {
                    auto cutPoint = std::distance(this->statesAndValues.begin(), it);
                    this->splitBlock(block, cutPoint);
                } else {
                    // Otherwise, we simply proceed to the next block.
                    ++blockIterator;
                }
            }
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Partition::isSilent(storm::storage::sparse::state_type state, storm::utility::ConstantsComparator<ValueType> const& comparator) const {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to retrieve silentness of state, because silent probabilities are not tracked.");
            return comparator.isOne(this->silentProbabilities[state]);
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Partition::hasSilentProbability(storm::storage::sparse::state_type state, storm::utility::ConstantsComparator<ValueType> const& comparator) const {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to retrieve silentness of state, because silent probabilities are not tracked.");
            return !comparator.isZero(this->silentProbabilities[state]);
        }
        
        template<typename ValueType>
        ValueType const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getSilentProbability(storm::storage::sparse::state_type state) const {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to retrieve silent probability of state, because silent probabilities are not tracked.");
            return this->silentProbabilities[state];
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::setSilentProbabilities(typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last) {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to set silent probability of state, because silent probabilities are not tracked.");
            for (; first != last; ++first) {
                this->silentProbabilities[first->first] = first->second;
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::setSilentProbabilitiesToZero(typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator last) {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to set silent probability of state, because silent probabilities are not tracked.");
            for (; first != last; ++first) {
                this->silentProbabilities[first->first] = storm::utility::zero<ValueType>();
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::setSilentProbability(storm::storage::sparse::state_type state, ValueType const& value) {
            STORM_LOG_ASSERT(this->keepSilentProbabilities, "Unable to set silent probability of state, because silent probabilities are not tracked.");
            this->silentProbabilities[state] = value;
        }
        
        template<typename ValueType>
        std::list<typename DeterministicModelBisimulationDecomposition<ValueType>::Block> const& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBlocks() const {
            return this->blocks;
        }
        
        template<typename ValueType>
        std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getStatesAndValues() {
            return this->statesAndValues;
        }
        
        template<typename ValueType>
        std::list<typename DeterministicModelBisimulationDecomposition<ValueType>::Block>& DeterministicModelBisimulationDecomposition<ValueType>::Partition::getBlocks() {
            return this->blocks;
        }
        
        template<typename ValueType>
        bool DeterministicModelBisimulationDecomposition<ValueType>::Partition::check() const {
            for (uint_fast64_t state = 0; state < this->positions.size(); ++state) {
                if (this->statesAndValues[this->positions[state]].first != state) {
                    assert(false);
                }
            }
            for (auto const& block : this->blocks) {
                assert(block.check());
                for (auto stateIt = this->getBegin(block), stateIte = this->getEnd(block); stateIt != stateIte; ++stateIt) {
                    if (this->stateToBlockMapping[stateIt->first] != &block) {
                        assert(false);
                    }
                }
            }
            return true;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::Partition::print() const {
            for (auto const& block : this->blocks) {
                block.print(*this);
            }
            std::cout << "states in partition" << std::endl;
            for (auto const& stateValue : statesAndValues) {
                std::cout << stateValue.first << " ";
            }
            std::cout << std::endl << "positions: " << std::endl;
            for (auto const& index : positions) {
                std::cout << index << " ";
            }
            std::cout << std::endl << "state to block mapping: " << std::endl;
            for (auto const& block : stateToBlockMapping) {
                std::cout << block << "[" << block->getId() <<"] ";
            }
            std::cout << std::endl;
            if (this->keepSilentProbabilities) {
                std::cout << "silent probabilities" << std::endl;
                for (auto const& prob : silentProbabilities) {
                    std::cout << prob << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "size: " << this->blocks.size() << std::endl;
            assert(this->check());
        }
        
        template<typename ValueType>
        std::size_t DeterministicModelBisimulationDecomposition<ValueType>::Partition::size() const {
            return this->blocks.size();
        }
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::DeterministicModelBisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, boost::optional<std::set<std::string>> const& atomicPropositions, bool keepRewards, bool weak, bool buildQuotient) : comparator() {
            STORM_LOG_THROW(!model.hasTransitionRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently only supported for models without transition rewards.");
            storm::storage::SparseMatrix<ValueType> backwardTransitions = model.getBackwardTransitions();
            BisimulationType bisimulationType = weak ? BisimulationType::WeakDtmc : BisimulationType::Strong;
            Partition initialPartition = getLabelBasedInitialPartition(model, backwardTransitions, bisimulationType, atomicPropositions, keepRewards);
            partitionRefinement(model, atomicPropositions, backwardTransitions, initialPartition, bisimulationType, keepRewards, buildQuotient);
        }

        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::DeterministicModelBisimulationDecomposition(storm::models::Ctmc<ValueType> const& model, boost::optional<std::set<std::string>> const& atomicPropositions, bool keepRewards, bool weak, bool buildQuotient) {
            STORM_LOG_THROW(!keepRewards || !model.hasTransitionRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently only supported for models without transition rewards.");
            storm::storage::SparseMatrix<ValueType> backwardTransitions = model.getBackwardTransitions();
            BisimulationType bisimulationType = weak ? BisimulationType::WeakCtmc : BisimulationType::Strong;
            Partition initialPartition = getLabelBasedInitialPartition(model, backwardTransitions, bisimulationType, atomicPropositions, keepRewards);
            partitionRefinement(model, atomicPropositions, backwardTransitions, initialPartition, bisimulationType, keepRewards,buildQuotient);
        }
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::DeterministicModelBisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, std::string const& phiLabel, std::string const& psiLabel, bool keepRewards, bool weak, bool bounded, bool buildQuotient) {
            STORM_LOG_THROW(!keepRewards || !model.hasTransitionRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently only supported for models without transition rewards.");
            STORM_LOG_THROW(!weak || !bounded, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation does not preserve bounded properties.");
            storm::storage::SparseMatrix<ValueType> backwardTransitions = model.getBackwardTransitions();
            BisimulationType bisimulationType = weak ? BisimulationType::WeakDtmc : BisimulationType::Strong;
            Partition initialPartition = getMeasureDrivenInitialPartition(model, backwardTransitions, phiLabel, psiLabel, bisimulationType, keepRewards, bounded);
            partitionRefinement(model, std::set<std::string>({phiLabel, psiLabel}), model.getBackwardTransitions(), initialPartition, bisimulationType, keepRewards, buildQuotient);
        }
        
        template<typename ValueType>
        DeterministicModelBisimulationDecomposition<ValueType>::DeterministicModelBisimulationDecomposition(storm::models::Ctmc<ValueType> const& model, std::string const& phiLabel, std::string const& psiLabel, bool keepRewards, bool weak, bool bounded, bool buildQuotient) {
            STORM_LOG_THROW(!keepRewards || !model.hasTransitionRewards(), storm::exceptions::IllegalFunctionCallException, "Bisimulation is currently only supported for models without transition rewards.");
            STORM_LOG_THROW(!weak || !bounded, storm::exceptions::IllegalFunctionCallException, "Weak bisimulation does not preserve bounded properties.");
            storm::storage::SparseMatrix<ValueType> backwardTransitions = model.getBackwardTransitions();
            BisimulationType bisimulationType = weak ? BisimulationType::WeakCtmc : BisimulationType::Strong;
            Partition initialPartition = getMeasureDrivenInitialPartition(model, backwardTransitions, phiLabel, psiLabel, bisimulationType, keepRewards, bounded);
            partitionRefinement(model, std::set<std::string>({phiLabel, psiLabel}), model.getBackwardTransitions(), initialPartition, bisimulationType, keepRewards, buildQuotient);
        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>> DeterministicModelBisimulationDecomposition<ValueType>::getQuotient() const {
            STORM_LOG_THROW(this->quotient != nullptr, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve quotient model from bisimulation decomposition, because it was not built.");
            return this->quotient;
        }
        
        template<typename ValueType>
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ValueType>::buildQuotient(ModelType const& model, boost::optional<std::set<std::string>> const& selectedAtomicPropositions, Partition const& partition, BisimulationType bisimulationType, bool keepRewards) {
            // In order to create the quotient model, we need to construct
            // (a) the new transition matrix,
            // (b) the new labeling,
            // (c) the new reward structures.
            
            // Prepare a matrix builder for (a).
            storm::storage::SparseMatrixBuilder<ValueType> builder(this->size(), this->size());
            
            // Prepare the new state labeling for (b).
            storm::models::AtomicPropositionsLabeling newLabeling(this->size(), model.getStateLabeling().getNumberOfAtomicPropositions());
            std::set<std::string> atomicPropositionsSet = selectedAtomicPropositions ? selectedAtomicPropositions.get() : model.getStateLabeling().getAtomicPropositions();
            atomicPropositionsSet.insert("init");
            std::vector<std::string> atomicPropositions = std::vector<std::string>(atomicPropositionsSet.begin(), atomicPropositionsSet.end());
            for (auto const& ap : atomicPropositions) {
                newLabeling.addAtomicProposition(ap);
            }
            
            // If the model had state rewards, we need to build the state rewards for the quotient as well.
            boost::optional<std::vector<ValueType>> stateRewards;
            if (keepRewards && model.hasStateRewards()) {
                stateRewards = std::vector<ValueType>(this->blocks.size());
            }
            
            // Now build (a) and (b) by traversing all blocks.
            for (uint_fast64_t blockIndex = 0; blockIndex < this->blocks.size(); ++blockIndex) {
                auto const& block = this->blocks[blockIndex];
                
                // Pick one representative state. For strong bisimulation it doesn't matter which state it is, because
                // they all behave equally.
                storm::storage::sparse::state_type representativeState = *block.begin();
                
                // However, for weak bisimulation, we need to make sure the representative state is a non-silent one (if
                // there is any such state).
                if (bisimulationType == BisimulationType::WeakDtmc) {
                    for (auto const& state : block) {
                        if (!partition.isSilent(state, comparator)) {
                            representativeState = state;
                            break;
                        }
                    }
                }
                
                Block const& oldBlock = partition.getBlock(representativeState);
                
                // If the block is absorbing, we simply add a self-loop.
                if (oldBlock.isAbsorbing()) {
                    builder.addNextValue(blockIndex, blockIndex, storm::utility::constantOne<ValueType>());
                    
                    if (oldBlock.hasLabel()) {
                        newLabeling.addAtomicPropositionToState(oldBlock.getLabel(), blockIndex);
                    } else {
                        // Otherwise add all atomic propositions to the equivalence class that the representative state
                        // satisfies.
                        for (auto const& ap : atomicPropositions) {
                            if (model.getStateLabeling().getStateHasAtomicProposition(ap, representativeState)) {
                                newLabeling.addAtomicPropositionToState(ap, blockIndex);
                            }
                        }
                    }
                } else {
                    // Compute the outgoing transitions of the block.
                    std::map<storm::storage::sparse::state_type, ValueType> blockProbability;
                    for (auto const& entry : model.getTransitionMatrix().getRow(representativeState)) {
                        storm::storage::sparse::state_type targetBlock = partition.getBlock(entry.getColumn()).getId();
                        
                        // If we are computing a weak bisimulation quotient, there is no need to add self-loops.
                        if ((bisimulationType == BisimulationType::WeakDtmc || bisimulationType == BisimulationType::WeakCtmc) && targetBlock == blockIndex) {
                            continue;
                        }
                        
                        auto probIterator = blockProbability.find(targetBlock);
                        if (probIterator != blockProbability.end()) {
                            probIterator->second += entry.getValue();
                        } else {
                            blockProbability[targetBlock] = entry.getValue();
                        }
                    }
                    
                    // Now add them to the actual matrix.
                    for (auto const& probabilityEntry : blockProbability) {
                        if (bisimulationType == BisimulationType::WeakDtmc) {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second / (storm::utility::one<ValueType>() - partition.getSilentProbability(representativeState)));
                        } else {
                            builder.addNextValue(blockIndex, probabilityEntry.first, probabilityEntry.second);
                        }
                    }
                    
                    // If the block has a special label, we only add that to the block.
                    if (oldBlock.hasLabel()) {
                        newLabeling.addAtomicPropositionToState(oldBlock.getLabel(), blockIndex);
                    } else {
                        // Otherwise add all atomic propositions to the equivalence class that the representative state
                        // satisfies.
                        for (auto const& ap : atomicPropositions) {
                            if (model.getStateLabeling().getStateHasAtomicProposition(ap, representativeState)) {
                                newLabeling.addAtomicPropositionToState(ap, blockIndex);
                            }
                        }
                    }
                }
                
                // If the model has state rewards, we simply copy the state reward of the representative state, because
                // all states in a block are guaranteed to have the same state reward.
                if (keepRewards && model.hasStateRewards()) {
                    stateRewards.get()[blockIndex] = model.getStateRewardVector()[representativeState];
                }
            }
            
            // Now check which of the blocks of the partition contain at least one initial state.
            for (auto initialState : model.getInitialStates()) {
                Block const& initialBlock = partition.getBlock(initialState);
                newLabeling.addAtomicPropositionToState("init", initialBlock.getId());
            }
            
            // Finally construct the quotient model.
            this->quotient = std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>>(new ModelType(builder.build(), std::move(newLabeling), std::move(stateRewards)));
        }
        
        static int callsToRefinePartition = 0;
        static int statesIteratedOverInRefinePartition = 0;
        static std::vector<int> refinementOrder;
        
        template<typename ValueType>
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ValueType>::partitionRefinement(ModelType const& model, boost::optional<std::set<std::string>> const& atomicPropositions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Partition& partition, BisimulationType bisimulationType, bool keepRewards, bool buildQuotient) {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

            // Initially, all blocks are potential splitter, so we insert them in the splitterQueue.
            std::chrono::high_resolution_clock::time_point refinementStart = std::chrono::high_resolution_clock::now();
            std::deque<Block*> splitterQueue;
            std::for_each(partition.getBlocks().begin(), partition.getBlocks().end(), [&] (Block& a) { splitterQueue.push_back(&a); a.markAsSplitter(); });

            // Then perform the actual splitting until there are no more splitters.
            while (!splitterQueue.empty()) {
                // Optionally: sort the splitter queue according to some criterion (here: prefer small splitters).
                std::sort(splitterQueue.begin(), splitterQueue.end(), [] (Block const* b1, Block const* b2) { return b1->getNumberOfStates() < b2->getNumberOfStates() || (b1->getNumberOfStates() == b2->getNumberOfStates() && b1->getId() < b2->getId()); } );

                // Get and prepare the next splitter.
                Block* splitter = splitterQueue.front();
                splitterQueue.pop_front();
                splitter->unmarkAsSplitter();
                
                // Now refine the partition using the current splitter.
                refinePartition(model.getTransitionMatrix(), backwardTransitions, *splitter, partition, bisimulationType, splitterQueue);
            }
            std::chrono::high_resolution_clock::duration refinementTime = std::chrono::high_resolution_clock::now() - refinementStart;

            // Now move the states from the internal partition into their final place in the decomposition. We do so in
            // a way that maintains the block IDs as indices.
            std::chrono::high_resolution_clock::time_point extractionStart = std::chrono::high_resolution_clock::now();
            this->blocks.resize(partition.size());
            for (auto const& block : partition.getBlocks()) {
                // We need to sort the states to allow for rapid construction of the blocks.
                std::sort(partition.getBegin(block), partition.getEnd(block), [] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return a.first < b.first; });
                
                // Convert the state-value-pairs to states only.
				std::function<storm::storage::sparse::state_type (std::pair<storm::storage::sparse::state_type, ValueType> const&)> projection = [] (std::pair<storm::storage::sparse::state_type, ValueType> const& a) -> storm::storage::sparse::state_type { return a.first; };
				this->blocks[block.getId()] = block_type(boost::make_transform_iterator(partition.getBegin(block), projection), boost::make_transform_iterator(partition.getEnd(block), projection), true);
            }

            // If we are required to build the quotient model, do so now.
            if (buildQuotient) {
                this->buildQuotient(model, atomicPropositions, partition, bisimulationType, keepRewards);
            }

            std::chrono::high_resolution_clock::duration extractionTime = std::chrono::high_resolution_clock::now() - extractionStart;
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::chrono::milliseconds refinementTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(refinementTime);
                std::chrono::milliseconds extractionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(extractionTime);
                std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                std::cout << std::endl;
                std::cout << "Time breakdown:" << std::endl;
                std::cout << "    * time for partitioning: " << refinementTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "    * time for extraction: " << extractionTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "------------------------------------------" << std::endl;
                std::cout << "    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << std::endl;
                std::cout << "Other:" << std::endl;
                std::cout << "    * calls to refine partition: " << callsToRefinePartition << std::endl;
                std::cout << "    * states iterated over: " << statesIteratedOverInRefinePartition << std::endl;
                std::cout << "    * order: ";
                for (auto const& element : refinementOrder) {
                    std::cout << element << ", ";
                }
                std::cout << std::endl;
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refineBlockProbabilities(Block& block, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue) {
            // Sort the states in the block based on their probabilities.
            std::sort(partition.getBegin(block), partition.getEnd(block), [&partition] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return a.second < b.second; } );
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = block.getBegin();
            for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
            
            // Finally, we need to scan the ranges of states that agree on the probability.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getBegin(block);
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator current = begin;
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getEnd(block) - 1;
            storm::storage::sparse::state_type currentIndex = block.getBegin();
            
            // Now we can check whether the block needs to be split, which is the case iff the probabilities for the
            // first and the last state are different.
            bool blockSplit = !comparator.isEqual(begin->second, end->second);
            while (!comparator.isEqual(begin->second, end->second)) {
                // Now we scan for the first state in the block that disagrees on the probability value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                ValueType const& currentValue = begin->second;
                
                ++begin;
                ++currentIndex;
                while (begin != end && comparator.isEqual(begin->second, currentValue)) {
                    ++begin;
                    ++currentIndex;
                }
                
                // Now we split the block and mark it as a potential splitter.
                Block& newBlock = partition.splitBlock(block, currentIndex);
                if (!newBlock.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&newBlock);
                    newBlock.markAsSplitter();
                }
            }
            
            // If the block was split, we also need to insert itself into the splitter queue.
            if (blockSplit) {
                if (!block.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&block);
                    block.markAsSplitter();
                }
            }
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refineBlockWeak(Block& block, Partition& partition, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::deque<Block*>& splitterQueue) {
            std::vector<uint_fast64_t> splitPoints = getSplitPointsWeak(block, partition);
            
            // Restore the original begin of the block.
            block.setBegin(block.getOriginalBegin());
            
            // Now that we have the split points of the non-silent states, we perform a backward search from
            // each non-silent state and label the predecessors with the class of the non-silent state.
            std::vector<storm::storage::BitVector> stateLabels(block.getEnd() - block.getBegin(), storm::storage::BitVector(splitPoints.size() - 1));
            
            std::vector<storm::storage::sparse::state_type> stateStack;
            stateStack.reserve(block.getEnd() - block.getBegin());
            for (uint_fast64_t stateClassIndex = 0; stateClassIndex < splitPoints.size() - 1; ++stateClassIndex) {
                for (auto stateIt = partition.getStatesAndValues().begin() + splitPoints[stateClassIndex], stateIte = partition.getStatesAndValues().begin() + splitPoints[stateClassIndex + 1]; stateIt != stateIte; ++stateIt) {
                    
                    stateStack.push_back(stateIt->first);
                    stateLabels[partition.getPosition(stateIt->first) - block.getBegin()].set(stateClassIndex);
                    while (!stateStack.empty()) {
                        storm::storage::sparse::state_type currentState = stateStack.back();
                        stateStack.pop_back();
                        
                        for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                            if (comparator.isZero(predecessorEntry.getValue())) {
                                continue;
                            }
                            
                            storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                            
                            // Only if the state is in the same block, is a silent state and it has not yet been
                            // labeled with the current label.
                            if (&partition.getBlock(predecessor) == &block && partition.isSilent(predecessor, comparator) && !stateLabels[partition.getPosition(predecessor) - block.getBegin()].get(stateClassIndex)) {
                                stateStack.push_back(predecessor);
                                stateLabels[partition.getPosition(predecessor) - block.getBegin()].set(stateClassIndex);
                            }
                        }
                    }
                }
            }
            
            // Now that all states were appropriately labeled, we can sort the states according to their labels and then
            // scan for ranges that agree on the label.
            std::sort(partition.getBegin(block), partition.getEnd(block), [&] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return stateLabels[partition.getPosition(a.first) - block.getBegin()] < stateLabels[partition.getPosition(b.first) - block.getBegin()]; });
            
            // Note that we do not yet repair the positions vector, but for the sake of efficiency temporariliy keep the
            // data structure in an inconsistent state.
            
            // Now we have everything in place to actually split the block by just scanning for ranges of equal label.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getBegin(block);
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator current = begin;
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getEnd(block) - 1;
            storm::storage::sparse::state_type currentIndex = block.getBegin();
            
            // Now we can check whether the block needs to be split, which is the case iff the labels for the first and
            // the last state are different. Store the offset of the block seperately, because it will potentially
            // modified by splits.
            storm::storage::sparse::state_type blockOffset = block.getBegin();
            bool blockSplit = stateLabels[partition.getPosition(begin->first) - blockOffset] != stateLabels[partition.getPosition(end->first) - blockOffset];
            while (stateLabels[partition.getPosition(begin->first) - blockOffset] != stateLabels[partition.getPosition(end->first) - blockOffset]) {
                // Now we scan for the first state in the block that disagrees on the labeling value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                storm::storage::BitVector const& currentValue = stateLabels[partition.getPosition(begin->first) - blockOffset];
                
                ++begin;
                ++currentIndex;
                while (begin != end && stateLabels[partition.getPosition(begin->first) - blockOffset] == currentValue) {
                    ++begin;
                    ++currentIndex;
                }
                
                // Now we split the block and mark it as a potential splitter.
                Block& newBlock = partition.splitBlock(block, currentIndex);
                
                // Update the silent probabilities for all the states in the new block.
                for (auto stateIt = partition.getBegin(newBlock), stateIte = partition.getEnd(newBlock); stateIt != stateIte; ++stateIt) {
                    if (partition.hasSilentProbability(stateIt->first, comparator)) {
                        ValueType newSilentProbability = storm::utility::zero<ValueType>();
                        for (auto const& successorEntry : forwardTransitions.getRow(stateIt->first)) {
                            if (&partition.getBlock(successorEntry.getColumn()) == &newBlock) {
                                newSilentProbability += successorEntry.getValue();
                            }
                        }
                        partition.setSilentProbability(stateIt->first, newSilentProbability);
                    }
                }
                
                if (!newBlock.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&newBlock);
                    newBlock.markAsSplitter();
                }
            }
            
            // If the block was split, we also need to insert itself into the splitter queue.
            if (blockSplit) {
                if (!block.isMarkedAsSplitter()) {
                    splitterQueue.push_back(&block);
                    block.markAsSplitter();
                }
                
                // Update the silent probabilities for all the states in the old block.
                for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt) {
                    if (partition.hasSilentProbability(stateIt->first, comparator)) {
                        ValueType newSilentProbability = storm::utility::zero<ValueType>();
                        for (auto const& successorEntry : forwardTransitions.getRow(stateIt->first)) {
                            if (&partition.getBlock(successorEntry.getColumn()) == &block) {
                                newSilentProbability += successorEntry.getValue();
                            }
                        }
                        partition.setSilentProbability(stateIt->first, newSilentProbability);
                    }
                }
            }
            
            // Finally update the positions vector.
            storm::storage::sparse::state_type position = blockOffset;
            for (auto stateIt = partition.getStatesAndValues().begin() + blockOffset, stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> DeterministicModelBisimulationDecomposition<ValueType>::getSplitPointsWeak(Block& block, Partition& partition) {
            std::vector<uint_fast64_t> result;
            // We first scale all probabilities with (1-p[s]) where p[s] is the silent probability of state s.
            std::for_each(partition.getStatesAndValues().begin() + block.getOriginalBegin(), partition.getStatesAndValues().begin() + block.getBegin(), [&] (std::pair<storm::storage::sparse::state_type, ValueType>& stateValuePair) {
                ValueType const& silentProbability = partition.getSilentProbability(stateValuePair.first);
                if (!comparator.isOne(silentProbability) && !comparator.isZero(silentProbability)) {
                    stateValuePair.second /= storm::utility::one<ValueType>() - silentProbability;
                }
            });
            
            // Now sort the states based on their probabilities.
            std::sort(partition.getStatesAndValues().begin() + block.getOriginalBegin(), partition.getStatesAndValues().begin() + block.getBegin(), [&partition] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return a.second < b.second; } );
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = block.getOriginalBegin();
            for (auto stateIt = partition.getStatesAndValues().begin() + block.getOriginalBegin(), stateIte = partition.getStatesAndValues().begin() + block.getBegin(); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(stateIt->first, position);
            }
            
            // Then, we scan for the ranges of states that agree on the probability.
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getStatesAndValues().begin() + block.getOriginalBegin();
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator current = begin;
            typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getStatesAndValues().begin() + block.getBegin() - 1;
            storm::storage::sparse::state_type currentIndex = block.getOriginalBegin();
            result.push_back(currentIndex);

            // Now we can check whether the block needs to be split, which is the case iff the probabilities for the
            // first and the last state are different.
            while (!comparator.isEqual(begin->second, end->second)) {
                // Now we scan for the first state in the block that disagrees on the probability value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                ValueType const& currentValue = begin->second;
                
                ++begin;
                ++currentIndex;
                while (begin != end && comparator.isEqual(begin->second, currentValue)) {
                    ++begin;
                    ++currentIndex;
                }

                // Remember the index at which the probabilities were different.
                result.push_back(currentIndex);
            }
            
            // Push a sentinel element and return result.
            result.push_back(block.getBegin());
            return result;
        }
        
        template<typename ValueType>
        void DeterministicModelBisimulationDecomposition<ValueType>::refinePartition(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, BisimulationType bisimulationType, std::deque<Block*>& splitterQueue) {
            ++callsToRefinePartition;
            refinementOrder.push_back(splitter.getId());
            std::list<Block*> predecessorBlocks;
            
            // Iterate over all states of the splitter and check its predecessors.
            bool splitterIsPredecessor = false;
            storm::storage::sparse::state_type currentPosition = splitter.getBegin();
            for (auto stateIterator = partition.getBegin(splitter), stateIte = partition.getEnd(splitter); stateIterator != stateIte; ++stateIterator, ++currentPosition) {
                ++statesIteratedOverInRefinePartition;
                storm::storage::sparse::state_type currentState = stateIterator->first;
                
                uint_fast64_t elementsToSkip = 0;
                for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    
                    // Get predecessor block and remember if the splitter was a predecessor of itself.
                    Block& predecessorBlock = partition.getBlock(predecessor);
                    if (&predecessorBlock == &splitter) {
                        splitterIsPredecessor = true;
                    }
                    
                    // If the predecessor block has just one state or is marked as being absorbing, we must not split it.
                    if (predecessorBlock.getNumberOfStates() <= 1 || predecessorBlock.isAbsorbing()) {
                        continue;
                    }
                    
                    storm::storage::sparse::state_type predecessorPosition = partition.getPosition(predecessor);
                    
                    // If we have not seen this predecessor before, we move it to a part near the beginning of the block.
                    if (predecessorPosition >= predecessorBlock.getBegin()) {
                        if (&predecessorBlock == &splitter) {
                            // If the predecessor we just found was already processed (in terms of visiting its predecessors),
                            // we swap it with the state that is currently at the beginning of the block and move the start
                            // of the block one step further.
                            if (predecessorPosition <= currentPosition + elementsToSkip) {
                                partition.swapStates(predecessor, partition.getState(predecessorBlock.getBegin()));
                                predecessorBlock.incrementBegin();
                            } else {
                                // Otherwise, we need to move the predecessor, but we need to make sure that we explore its
                                // predecessors later.
                                if (predecessorBlock.getMarkedPosition() == predecessorBlock.getBegin()) {
                                    partition.swapStatesAtPositions(predecessorBlock.getMarkedPosition(), predecessorPosition);
                                    partition.swapStatesAtPositions(predecessorPosition, currentPosition + elementsToSkip + 1);
                                } else {
                                    partition.swapStatesAtPositions(predecessorBlock.getMarkedPosition(), predecessorPosition);
                                    partition.swapStatesAtPositions(predecessorPosition, predecessorBlock.getBegin());
                                    partition.swapStatesAtPositions(predecessorPosition, currentPosition + elementsToSkip + 1);
                                }
                                
                                ++elementsToSkip;
                                predecessorBlock.incrementMarkedPosition();
                                predecessorBlock.incrementBegin();
                            }
                        } else {
                            partition.swapStates(predecessor, partition.getState(predecessorBlock.getBegin()));
                            predecessorBlock.incrementBegin();
                        }
                        partition.setValue(predecessor, predecessorEntry.getValue());
                    } else {
                        // Otherwise, we just need to update the probability for this predecessor.
                        partition.increaseValue(predecessor, predecessorEntry.getValue());
                    }
                    
                    if (!predecessorBlock.isMarkedAsPredecessor()) {
                        predecessorBlocks.emplace_back(&predecessorBlock);
                        predecessorBlock.markAsPredecessorBlock();
                    }
                }
                
                // If we had to move some elements beyond the current element, we may have to skip them.
                if (elementsToSkip > 0) {
                    stateIterator += elementsToSkip;
                    currentPosition += elementsToSkip;
                }
            }
            
            // Now we can traverse the list of states of the splitter whose predecessors we have not yet explored.
            for (auto stateIterator = partition.getStatesAndValues().begin() + splitter.getOriginalBegin(), stateIte = partition.getStatesAndValues().begin() + splitter.getMarkedPosition(); stateIterator != stateIte; ++stateIterator) {
                storm::storage::sparse::state_type currentState = stateIterator->first;
                
                for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    Block& predecessorBlock = partition.getBlock(predecessor);
                    storm::storage::sparse::state_type predecessorPosition = partition.getPosition(predecessor);

                    if (predecessorPosition >= predecessorBlock.getBegin()) {
                        partition.swapStatesAtPositions(predecessorPosition, predecessorBlock.getBegin());
                        predecessorBlock.incrementBegin();
                        partition.setValue(predecessor, predecessorEntry.getValue());
                    } else {
                        partition.increaseValue(predecessor, predecessorEntry.getValue());
                    }
                    
                    if (!predecessorBlock.isMarkedAsPredecessor()) {
                        predecessorBlocks.emplace_back(&predecessorBlock);
                        predecessorBlock.markAsPredecessorBlock();
                    }
                }
            }

            if (bisimulationType == BisimulationType::Strong || bisimulationType == BisimulationType::WeakCtmc) {
                std::list<Block*> blocksToSplit;
                
                // Now, we can iterate over the predecessor blocks and see whether we have to create a new block for
                // predecessors of the splitter.
                for (auto blockPtr : predecessorBlocks) {
                    Block& block = *blockPtr;
                    
                    block.unmarkAsPredecessorBlock();
                    block.resetMarkedPosition();
                    
                    // If we have moved the begin of the block to somewhere in the middle of the block, we need to split it.
                    if (block.getBegin() != block.getEnd()) {
                        Block& newBlock = partition.insertBlock(block);
                        if (!newBlock.isMarkedAsSplitter()) {
                            splitterQueue.push_back(&newBlock);
                            newBlock.markAsSplitter();
                        }
                        
                        // Schedule the block of predecessors for refinement based on probabilities.
                        blocksToSplit.emplace_back(&newBlock);
                    } else {
                        // In this case, we can keep the block by setting its begin to the old value.
                        block.setBegin(block.getOriginalBegin());
                        blocksToSplit.emplace_back(&block);
                    }
                }
                
                // Finally, we walk through the blocks that have a transition to the splitter and split them using
                // probabilistic information.
                for (auto blockPtr : blocksToSplit) {
                    if (blockPtr->getNumberOfStates() <= 1) {
                        continue;
                    }
                    
                    // In the case of weak bisimulation for CTMCs, we don't need to make sure the rate of staying inside
                    // the own block is the same.
                    if (bisimulationType == BisimulationType::WeakCtmc && blockPtr == &splitter) {
                        continue;
                    }
                    
                    refineBlockProbabilities(*blockPtr, partition, bisimulationType, splitterQueue);
                }
            } else { // In this case, we are computing a weak bisimulation on a DTMC.
                // If the splitter was a predecessor of itself and we are computing a weak bisimulation, we need to update
                // the silent probabilities.
                if (splitterIsPredecessor) {
                    partition.setSilentProbabilities(partition.getStatesAndValues().begin() + splitter.getOriginalBegin(), partition.getStatesAndValues().begin() + splitter.getBegin());
                    partition.setSilentProbabilitiesToZero(partition.getStatesAndValues().begin() + splitter.getBegin(), partition.getStatesAndValues().begin() + splitter.getEnd());
                }

                // Now refine all predecessor blocks in a weak manner. That is, we split according to the criterion of
                // weak bisimulation.
                for (auto blockPtr : predecessorBlocks) {
                    Block& block = *blockPtr;
                    
                    // If the splitter is also the predecessor block, we must not refine it at this point.
                    if (&block != &splitter) {
                        refineBlockWeak(block, partition, forwardTransitions, backwardTransitions, splitterQueue);
                    } else {
                        // Restore the begin of the block.
                        block.setBegin(block.getOriginalBegin());
                    }

                    block.unmarkAsPredecessorBlock();
                    block.resetMarkedPosition();
                }
            }
            
            STORM_LOG_ASSERT(partition.check(), "Partition became inconsistent.");
        }
        
        template<typename ValueType>
        template<typename ModelType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Partition DeterministicModelBisimulationDecomposition<ValueType>::getMeasureDrivenInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::string const& phiLabel, std::string const& psiLabel, BisimulationType bisimulationType, bool keepRewards, bool bounded) {
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(backwardTransitions, phiLabel == "true" ? storm::storage::BitVector(model.getNumberOfStates(), true) : model.getLabeledStates(phiLabel), model.getLabeledStates(psiLabel));
            Partition partition(model.getNumberOfStates(), statesWithProbability01.first, bounded ? model.getLabeledStates(psiLabel) : statesWithProbability01.second, phiLabel, psiLabel, bisimulationType == BisimulationType::WeakDtmc);
            
            // If the model has state rewards, we need to consider them, because otherwise reward properties are not
            // preserved.
            if (keepRewards && model.hasStateRewards()) {
                this->splitRewards(model, partition);
            }
            
            // If we are creating the initial partition for weak bisimulation, we need to (a) split off all divergent
            // states of each initial block and (b) initialize the vector of silent probabilities held by the partition.
            if (bisimulationType == BisimulationType::WeakDtmc) {
                this->splitOffDivergentStates(model, backwardTransitions, partition);
                this->initializeSilentProbabilities(model, partition);
            }
            
            return partition;
        }
        
        template<typename ValueType>
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ValueType>::splitOffDivergentStates(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Partition& partition) {
            std::vector<storm::storage::sparse::state_type> stateStack;
            stateStack.reserve(model.getNumberOfStates());
            storm::storage::BitVector nondivergentStates(model.getNumberOfStates());
            for (auto& block : partition.getBlocks()) {
                nondivergentStates.clear();
                
                for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt) {
                    if (nondivergentStates.get(stateIt->first)) {
                        continue;
                    }
                    
                    // Now traverse the forward transitions of the current state and check whether there is a
                    // transition to some other block.
                    bool isDirectlyNonDivergent = false;
                    for (auto const& successor : model.getRows(stateIt->first)) {
                        // If there is such a transition, then we can mark all states in the current block that can
                        // reach the state as non-divergent.
                        if (&partition.getBlock(successor.getColumn()) != &block) {
                            isDirectlyNonDivergent = true;
                            break;
                        }
                    }
                    
                    if (isDirectlyNonDivergent) {
                        stateStack.push_back(stateIt->first);
                        
                        while (!stateStack.empty()) {
                            storm::storage::sparse::state_type currentState = stateStack.back();
                            stateStack.pop_back();
                            nondivergentStates.set(currentState);
                            
                            for (auto const& predecessor : backwardTransitions.getRow(currentState)) {
                                if (&partition.getBlock(predecessor.getColumn()) == &block && !nondivergentStates.get(predecessor.getColumn())) {
                                    stateStack.push_back(predecessor.getColumn());
                                }
                            }
                        }
                    }
                }
                
                
                if (nondivergentStates.getNumberOfSetBits() > 0 && nondivergentStates.getNumberOfSetBits() != block.getNumberOfStates()) {
                    // Now that we have determined all (non)divergent states in the current block, we need to split them
                    // off.
                    std::sort(partition.getBegin(block), partition.getEnd(block), [&nondivergentStates] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return nondivergentStates.get(a.first) && !nondivergentStates.get(b.first); } );
                    // Update the positions vector.
                    storm::storage::sparse::state_type position = block.getBegin();
                    for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                        partition.setPosition(stateIt->first, position);
                    }
                    
                    // Finally, split the block.
                    Block& nondivergentBlock = partition.splitBlock(block, block.getBegin() + nondivergentStates.getNumberOfSetBits());
                    
                    // Since the remaining states in the block are divergent, we can mark the block as absorbing.
                    // This also guarantees that the self-loop will be added to the state of the quotient
                    // representing this block of states.
                    block.setAbsorbing(true);
                } else if (nondivergentStates.getNumberOfSetBits() == 0) {
                    // If there are only diverging states in the block, we need to make it absorbing.
                    block.setAbsorbing(true);
                }
            }
        }
        
        template<typename ValueType>
        template<typename ModelType>
        typename DeterministicModelBisimulationDecomposition<ValueType>::Partition DeterministicModelBisimulationDecomposition<ValueType>::getLabelBasedInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, BisimulationType bisimulationType, boost::optional<std::set<std::string>> const& atomicPropositions, bool keepRewards) {
            Partition partition(model.getNumberOfStates(), bisimulationType == BisimulationType::WeakDtmc);
            
            if (atomicPropositions) {
                for (auto const& label : atomicPropositions.get()) {
                    if (label == "init") {
                        continue;
                    }
                    partition.splitLabel(model.getLabeledStates(label));
                }
            } else {
                for (auto const& label : model.getStateLabeling().getAtomicPropositions()) {
                    if (label == "init") {
                        continue;
                    }
                    partition.splitLabel(model.getLabeledStates(label));
                }
            }
            
            // If the model has state rewards, we need to consider them, because otherwise reward properties are not
            // preserved.
            if (keepRewards && model.hasStateRewards()) {
                this->splitRewards(model, partition);
            }
            
            // If we are creating the initial partition for weak bisimulation, we need to (a) split off all divergent
            // states of each initial block and (b) initialize the vector of silent probabilities held by the partition.
            if (bisimulationType == BisimulationType::WeakDtmc) {
                this->splitOffDivergentStates(model, backwardTransitions, partition);
                this->initializeSilentProbabilities(model, partition);
            }
            return partition;
        }
        
        template<typename ValueType>
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ValueType>::initializeSilentProbabilities(ModelType const& model, Partition& partition) {
            for (auto const& block : partition.getBlocks()) {
                for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt) {
                    ValueType silentProbability = storm::utility::zero<ValueType>();
                    
                    for (auto const& successorEntry : model.getRows(stateIt->first)) {
                        if (&partition.getBlock(successorEntry.getColumn()) == &block) {
                            silentProbability += successorEntry.getValue();
                        }
                    }
                    
                    partition.setSilentProbability(stateIt->first, silentProbability);
                }
            }
        }
        
        template<typename ValueType>
        template<typename ModelType>
        void DeterministicModelBisimulationDecomposition<ValueType>::splitRewards(ModelType const& model, Partition& partition) {
            if (!model.hasStateRewards()) {
                return;
            }
            
            for (auto& block : partition.getBlocks()) {
                std::sort(partition.getBegin(block), partition.getEnd(block), [&model] (std::pair<storm::storage::sparse::state_type, ValueType> const& a, std::pair<storm::storage::sparse::state_type, ValueType> const& b) { return model.getStateRewardVector()[a.first] < model.getStateRewardVector()[b.first]; } );
                
                // Update the positions vector and put the (state) reward values next to the states so we can easily compare them later.
                storm::storage::sparse::state_type position = block.getBegin();
                for (auto stateIt = partition.getBegin(block), stateIte = partition.getEnd(block); stateIt != stateIte; ++stateIt, ++position) {
                    partition.setPosition(stateIt->first, position);
                    stateIt->second = model.getStateRewardVector()[stateIt->first];
                }
                
                // Finally, we need to scan the ranges of states that agree on the probability.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator begin = partition.getBegin(block);
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator current = begin;
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator end = partition.getEnd(block) - 1;
                storm::storage::sparse::state_type currentIndex = block.getBegin();
                
                // Now we can check whether the block needs to be split, which is the case iff the rewards for the first
                // and the last state are different.
                while (!comparator.isEqual(begin->second, end->second)) {
                    // Now we scan for the first state in the block that disagrees on the reward value. Note that we do
                    // not have to check currentIndex for staying within bounds, because we know the matching state is
                    // within bounds.
                    ValueType const& currentValue = begin->second;
                    
                    ++begin;
                    ++currentIndex;
                    while (begin != end && comparator.isEqual(begin->second, currentValue)) {
                        ++begin;
                        ++currentIndex;
                    }
                    
                    // Now we split the block.
                    partition.splitBlock(block, currentIndex);
                }
            }
        }
        
        template class DeterministicModelBisimulationDecomposition<double>;
        
#ifdef PARAMETRIC_SYSTEMS
        template class DeterministicModelBisimulationDecomposition<storm::RationalFunction>;
#endif
    }
}