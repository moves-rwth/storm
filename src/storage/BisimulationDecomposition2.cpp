#include "src/storage/BisimulationDecomposition2.h"

#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <iomanip>

namespace storm {
    namespace storage {
        
        static std::size_t globalId = 0;
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Block::Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next) : next(next), prev(prev), begin(begin), end(end), markedAsSplitter(false), markedPosition(begin), id(globalId++) {
            if (next != nullptr) {
                next->prev = this;
            }
            if (prev != nullptr) {
                prev->next = this;
            }
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::print(Partition const& partition) const {
            std::cout << "block " << this->getId() << " with marked position " << this->getMarkedPosition() << std::endl;
            std::cout << "begin: " << this->begin << " and end: " << this->end << " (number of states: " << this->getNumberOfStates() << ")" << std::endl;
            std::cout << "states:" << std::endl;
            for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                std::cout << partition.states[index] << " ";
            }
            std::cout << std::endl << "original: " << std::endl;
            for (storm::storage::sparse::state_type index = this->getOriginalBegin(); index < this->end; ++index) {
                std::cout << partition.states[index] << " ";
            }
            std::cout << std::endl << "values:" << std::endl;
            for (storm::storage::sparse::state_type index = this->begin; index < this->end; ++index) {
                std::cout << std::setprecision(3) << partition.values[index] << " ";
            }
            std::cout << std::endl;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::setBegin(storm::storage::sparse::state_type begin) {
            this->begin = begin;
            this->markedPosition = begin;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::setEnd(storm::storage::sparse::state_type end) {
            this->end = end;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::incrementBegin() {
            ++this->begin;
            std::cout << "incremented begin to " << this->begin << std::endl;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::decrementEnd() {
            ++this->begin;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type BisimulationDecomposition2<ValueType>::Block::getBegin() const {
            return this->begin;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type BisimulationDecomposition2<ValueType>::Block::getOriginalBegin() const {
            if (this->hasPreviousBlock()) {
                return this->getPreviousBlock().getEnd();
            } else {
                return 0;
            }
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type BisimulationDecomposition2<ValueType>::Block::getEnd() const {
            return this->end;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::setIterator(const_iterator it) {
            this->selfIt = it;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block::const_iterator BisimulationDecomposition2<ValueType>::Block::getIterator() const {
            return this->selfIt;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block::const_iterator BisimulationDecomposition2<ValueType>::Block::getNextIterator() const {
            return this->getNextBlock().getIterator();
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block::const_iterator BisimulationDecomposition2<ValueType>::Block::getPreviousIterator() const {
            return this->getPreviousBlock().getIterator();
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block& BisimulationDecomposition2<ValueType>::Block::getNextBlock() {
            return *this->next;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block const& BisimulationDecomposition2<ValueType>::Block::getNextBlock() const {
            return *this->next;
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Block::hasNextBlock() const {
            return this->next != nullptr;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block& BisimulationDecomposition2<ValueType>::Block::getPreviousBlock() {
            return *this->prev;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block* BisimulationDecomposition2<ValueType>::Block::getPreviousBlockPointer() {
            return this->prev;
        }

        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block* BisimulationDecomposition2<ValueType>::Block::getNextBlockPointer() {
            return this->next;
        }

        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block const& BisimulationDecomposition2<ValueType>::Block::getPreviousBlock() const {
            return *this->prev;
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Block::hasPreviousBlock() const {
            return this->prev != nullptr;
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Block::check() const {
            if (this->begin >= this->end) {
                std::cout << "beg: " << this->begin << " and end " << this->end << std::endl;
                assert(false);
            }
            if (this->prev != nullptr) {
                assert (this->prev->next == this);
            }
            if (this->next != nullptr) {
                assert (this->next->prev == this);
            }
            return true;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::Block::getNumberOfStates() const {
            // We need to take the original begin here, because the begin is temporarily moved.
            return (this->end - this->getOriginalBegin());
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Block::isMarkedAsSplitter() const {
            return this->markedAsSplitter;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::markAsSplitter() {
            this->markedAsSplitter = true;
        }

        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::unmarkAsSplitter() {
            this->markedAsSplitter = false;
        }

        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::Block::getId() const {
            return this->id;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type BisimulationDecomposition2<ValueType>::Block::getMarkedPosition() const {
            return this->markedPosition;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::setMarkedPosition(storm::storage::sparse::state_type position) {
            this->markedPosition = position;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::resetMarkedPosition() {
            this->markedPosition = this->begin;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::incrementMarkedPosition() {
            ++this->markedPosition;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::markAsPredecessorBlock() {
            this->markedAsPredecessorBlock = true;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Block::unmarkAsPredecessorBlock() {
            this->markedAsPredecessorBlock = false;
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Block::isMarkedAsPredecessor() const {
            return markedAsPredecessorBlock;
        }
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::Partition::Partition(std::size_t numberOfStates) : stateToBlockMapping(numberOfStates), states(numberOfStates), positions(numberOfStates), values(numberOfStates) {
            // Create the block and give it an iterator to itself.
            typename std::list<Block>::iterator it = blocks.emplace(this->blocks.end(), 0, numberOfStates, nullptr, nullptr);
            it->setIterator(it);
            
            // Set up the different parts of the internal structure.
            for (storm::storage::sparse::state_type state = 0; state < numberOfStates; ++state) {
                states[state] = state;
                positions[state] = state;
                stateToBlockMapping[state] = &blocks.back();
            }
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::swapStates(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2) {
            std::cout << "swapping states " << state1 << " and " << state2 << std::endl;
            std::swap(this->states[this->positions[state1]], this->states[this->positions[state2]]);
            std::swap(this->values[this->positions[state1]], this->values[this->positions[state2]]);
            std::swap(this->positions[state1], this->positions[state2]);
            assert(this->check());
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::swapStatesAtPositions(storm::storage::sparse::state_type position1, storm::storage::sparse::state_type position2) {
            storm::storage::sparse::state_type state1 = this->states[position1];
            storm::storage::sparse::state_type state2 = this->states[position2];
            std::cout << "swapping states " << state1 << " and " << state2 << " at positions " << position1 << " and " << position2 << std::endl;
            
            std::swap(this->states[position1], this->states[position2]);
            std::swap(this->values[position1], this->values[position2]);
            
            this->positions[state1] = position2;
            this->positions[state2] = position1;
            std::cout << "pos of " << state1 << " is now " << position2 << " and pos of " << state2 << " is now " << position1 << std::endl;
            std::cout << this->states[position1] << " vs " << state2 << " and " << this->states[position2] << " vs " << state1 << std::endl;
            assert(this->check());
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type const& BisimulationDecomposition2<ValueType>::Partition::getPosition(storm::storage::sparse::state_type state) const {
            return this->positions[state];
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::setPosition(storm::storage::sparse::state_type state, storm::storage::sparse::state_type position) {
            this->positions[state] = position;
        }
        
        template<typename ValueType>
        storm::storage::sparse::state_type const& BisimulationDecomposition2<ValueType>::Partition::getState(storm::storage::sparse::state_type position) const {
            return this->states[position];
        }
        
        template<typename ValueType>
        ValueType const& BisimulationDecomposition2<ValueType>::Partition::getValue(storm::storage::sparse::state_type state) const {
            return this->values[this->positions[state]];
        }
        
        template<typename ValueType>
        ValueType const& BisimulationDecomposition2<ValueType>::Partition::getValueAtPosition(storm::storage::sparse::state_type position) const {
            return this->values[position];
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::setValue(storm::storage::sparse::state_type state, ValueType value) {
            this->values[this->positions[state]] = value;
        }
        
        template<typename ValueType>
        std::vector<ValueType>& BisimulationDecomposition2<ValueType>::Partition::getValues() {
            return this->values;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::increaseValue(storm::storage::sparse::state_type state, ValueType value) {
            this->values[this->positions[state]] += value;
        }

        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::updateBlockMapping(Block& block, std::vector<storm::storage::sparse::state_type>::iterator first, std::vector<storm::storage::sparse::state_type>::iterator last) {
            for (; first != last; ++first) {
                this->stateToBlockMapping[*first] = &block;
            }
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block& BisimulationDecomposition2<ValueType>::Partition::getBlock(storm::storage::sparse::state_type state) {
            return *this->stateToBlockMapping[state];
        }
        
        template<typename ValueType>
        std::vector<storm::storage::sparse::state_type>::iterator BisimulationDecomposition2<ValueType>::Partition::getBeginOfStates(Block const& block) {
            return this->states.begin() + block.getBegin();
        }
        
        template<typename ValueType>
        std::vector<storm::storage::sparse::state_type>::iterator BisimulationDecomposition2<ValueType>::Partition::getEndOfStates(Block const& block) {
            return this->states.begin() + block.getEnd();
        }

        template<typename ValueType>
        std::vector<storm::storage::sparse::state_type>::const_iterator BisimulationDecomposition2<ValueType>::Partition::getBeginOfStates(Block const& block) const {
            return this->states.begin() + block.getBegin();
        }
        
        template<typename ValueType>
        std::vector<storm::storage::sparse::state_type>::const_iterator BisimulationDecomposition2<ValueType>::Partition::getEndOfStates(Block const& block) const {
            return this->states.begin() + block.getEnd();
        }
        
        template<typename ValueType>
        typename std::vector<ValueType>::iterator BisimulationDecomposition2<ValueType>::Partition::getBeginOfValues(Block const& block) {
            return this->values.begin() + block.getBegin();
        }
        
        template<typename ValueType>
        typename std::vector<ValueType>::iterator BisimulationDecomposition2<ValueType>::Partition::getEndOfValues(Block const& block) {
            return this->values.begin() + block.getEnd();
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block& BisimulationDecomposition2<ValueType>::Partition::splitBlock(Block& block, storm::storage::sparse::state_type position) {
            // FIXME: this could be improved by splitting off the smaller of the two resulting blocks.
            
            std::cout << "splitting block (" << block.getBegin() << "," << block.getEnd() << ") at position " << position << std::endl;
            block.print(*this);
            
            // In case one of the resulting blocks would be empty, we simply return the current block and do not create
            // a new one.
            if (position == block.getBegin() || position == block.getEnd()) {
                return block;
            }
            
            // Actually create the new block and insert it at the correct position.
            typename std::list<Block>::iterator selfIt = this->blocks.emplace(block.getIterator(), block.getBegin(), position, block.getPreviousBlockPointer(), &block);
            std::cout << "created new block from " << block.getBegin() << " to " << position << std::endl;
            selfIt->setIterator(selfIt);
            Block& newBlock = *selfIt;
            
            // Make the current block end at the given position.
            block.setBegin(position);
            
            std::cout << "old block: " << std::endl;
            block.print(*this);
            
            // Update the mapping of the states in the newly created block.
            for (auto it = this->getBeginOfStates(newBlock), ite = this->getEndOfStates(newBlock); it != ite; ++it) {
                stateToBlockMapping[*it] = &newBlock;
            }
            
            return newBlock;
        }
        
        template<typename ValueType>
        typename BisimulationDecomposition2<ValueType>::Block& BisimulationDecomposition2<ValueType>::Partition::insertBlock(Block& block) {
            // Find the beginning of the new block.
            storm::storage::sparse::state_type begin;
            if (block.hasPreviousBlock()) {
                begin = block.getPreviousBlock().getEnd();
                std::cout << "previous block ended at " << begin << std::endl;
                block.getPreviousBlock().print(*this);
            } else {
                begin = 0;
            }
            
            // Actually insert the new block.
            typename std::list<Block>::iterator it = this->blocks.emplace(block.getIterator(), begin, block.getBegin(), block.getPreviousBlockPointer(), &block);
            Block& newBlock = *it;
            newBlock.setIterator(it);
            
            // Update the mapping of the states in the newly created block.
            for (auto it = this->getBeginOfStates(newBlock), ite = this->getEndOfStates(newBlock); it != ite; ++it) {
                stateToBlockMapping[*it] = &newBlock;
            }
            
            return *it;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::splitLabel(storm::storage::BitVector const& statesWithLabel) {
            for (auto blockIterator = this->blocks.begin(), ite = this->blocks.end(); blockIterator != ite; ) { // The update of the loop was intentionally moved to the bottom of the loop.
                Block& block = *blockIterator;
                
                // Sort the range of the block such that all states that have the label are moved to the front.
                std::sort(this->getBeginOfStates(block), this->getEndOfStates(block), [&statesWithLabel] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return statesWithLabel.get(a) && !statesWithLabel.get(b); } );
                
                // Update the positions vector.
                storm::storage::sparse::state_type position = block.getBegin();
                for (auto stateIt = this->getBeginOfStates(block), stateIte = this->getEndOfStates(block); stateIt != stateIte; ++stateIt, ++position) {
                    this->positions[*stateIt] = position;
                }
                
                // Now we can find the first position in the block that does not have the label and create new blocks.
                std::vector<storm::storage::sparse::state_type>::iterator it = std::find_if(this->getBeginOfStates(block), this->getEndOfStates(block), [&] (storm::storage::sparse::state_type const& a) { return !statesWithLabel.get(a); });
                
                // If not all the states agreed on the validity of the label, we need to split the block.
                if (it != this->getBeginOfStates(block) && it != this->getEndOfStates(block)) {
                    auto cutPoint = std::distance(this->states.begin(), it);
                    this->splitBlock(block, cutPoint);
                } else {
                    // Otherwise, we simply proceed to the next block.
                    ++blockIterator;
                }
            }
        }
        
        template<typename ValueType>
        std::list<typename BisimulationDecomposition2<ValueType>::Block> const& BisimulationDecomposition2<ValueType>::Partition::getBlocks() const {
            return this->blocks;
        }
        
        template<typename ValueType>
        std::vector<storm::storage::sparse::state_type>& BisimulationDecomposition2<ValueType>::Partition::getStates() {
            return this->states;
        }
        
        template<typename ValueType>
        std::list<typename BisimulationDecomposition2<ValueType>::Block>& BisimulationDecomposition2<ValueType>::Partition::getBlocks() {
            return this->blocks;
        }
        
        template<typename ValueType>
        bool BisimulationDecomposition2<ValueType>::Partition::check() const {
            for (uint_fast64_t state = 0; state < this->positions.size(); ++state) {
                if (this->states[this->positions[state]] != state) {
                    assert(false);
                }
            }
            for (auto const& block : this->blocks) {
                assert(block.check());
                for (auto stateIt = this->getBeginOfStates(block), stateIte = this->getEndOfStates(block); stateIt != stateIte; ++stateIt) {
                    if (this->stateToBlockMapping[*stateIt] != &block) {
                        std::cout << "state " << *stateIt << " has wrong block mapping " << this->stateToBlockMapping[*stateIt] << " (should have " << &block << ")" << std::endl;
                        assert(false);
                    }
                }
            }
            return true;
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::Partition::print() const {
            for (auto const& block : this->blocks) {
                block.print(*this);
            }
            std::cout << "states in partition" << std::endl;
            for (auto const& state : states) {
                std::cout << state << " ";
            }
            std::cout << std::endl << "positions: " << std::endl;
            for (auto const& index : positions) {
                std::cout << index << " ";
            }
            std::cout << std::endl << "state to block mapping: " << std::endl;
            for (auto const& block : stateToBlockMapping) {
                std::cout << block << " ";
            }
            std::cout << std::endl;
            std::cout << "size: " << this->blocks.size() << std::endl;
            assert(this->check());
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::Partition::size() const {
            int counter = 0;
            for (auto const& element : blocks) {
                ++counter;
            }
            std::cout << "found " << counter << " elements" << std::endl;
            return this->blocks.size();
        }
        
        template<typename ValueType>
        BisimulationDecomposition2<ValueType>::BisimulationDecomposition2(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            computeBisimulationEquivalenceClasses(dtmc, weak);
        }
        
        template<typename ValueType>
        void BisimulationDecomposition2<ValueType>::computeBisimulationEquivalenceClasses(storm::models::Dtmc<ValueType> const& dtmc, bool weak) {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
            
            // We start by computing the initial partition.
            Partition partition(dtmc.getNumberOfStates());
            partition.print();
            for (auto const& label : dtmc.getStateLabeling().getAtomicPropositions()) {
                if (label == "init") {
                    continue;
                }
                partition.splitLabel(dtmc.getLabeledStates(label));
            }
            
            std::cout << "initial partition:" << std::endl;
            partition.print();
            assert(partition.check());
            
            // Initially, all blocks are potential splitter, so we insert them in the splitterQueue.
            std::deque<Block*> splitterQueue;
            std::for_each(partition.getBlocks().begin(), partition.getBlocks().end(), [&] (Block& a) { splitterQueue.push_back(&a); });
            
            storm::storage::SparseMatrix<ValueType> backwardTransitions = dtmc.getBackwardTransitions();
            
            // Then perform the actual splitting until there are no more splitters.
            while (!splitterQueue.empty()) {
                splitPartition(backwardTransitions, *splitterQueue.front(), partition, splitterQueue);
                splitterQueue.pop_front();
                
                std::cout << "####### updated partition ##############" << std::endl;
                partition.print();
                std::cout << "####### end of updated partition #######" << std::endl;
            }
            
            std::cout << "computed a quotient of size " << partition.size() << std::endl;
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            std::cout << "Bisimulation took " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms." << std::endl;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::splitBlockProbabilities(Block& block, Partition& partition, std::deque<Block*>& splitterQueue) {
            std::cout << "part before split prob" << std::endl;
            partition.print();
            
            // Sort the states in the block based on their probabilities.
            std::sort(partition.getBeginOfStates(block), partition.getEndOfStates(block), [&partition] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return partition.getValue(a) < partition.getValue(b); } );
            
            // FIXME: This can probably be done more efficiently.
            std::sort(partition.getBeginOfValues(block), partition.getEndOfValues(block));
            
            // Update the positions vector.
            storm::storage::sparse::state_type position = block.getBegin();
            for (auto stateIt = partition.getBeginOfStates(block), stateIte = partition.getEndOfStates(block); stateIt != stateIte; ++stateIt, ++position) {
                partition.setPosition(*stateIt, position);
            }
            
            // Finally, we need to scan the ranges of states that agree on the probability.
            storm::storage::sparse::state_type beginIndex = block.getBegin();
            storm::storage::sparse::state_type currentIndex = beginIndex;
            storm::storage::sparse::state_type endIndex = block.getEnd();
            
            // Now we can check whether the block needs to be split, which is the case iff the probabilities for the
            // first and the last state are different.
            std::size_t createdBlocks = 0;
            while (std::abs(partition.getValueAtPosition(beginIndex) - partition.getValueAtPosition(endIndex)) >= 1e-6) {
                // Now we scan for the first state in the block that disagrees on the probability value.
                // Note that we do not have to check currentIndex for staying within bounds, because we know the matching
                // state is within bounds.
                ValueType const& currentValue = partition.getValueAtPosition(beginIndex);
                
                typename std::vector<ValueType>::const_iterator valueIterator = partition.getValues().begin() + beginIndex + 1;
                ++currentIndex;
                while (currentIndex < endIndex && std::abs(*valueIterator - currentValue) <= 1e-6) {
                    ++valueIterator;
                    ++currentIndex;
                }
                
                // Now we split the block and mark it as a potential splitter.
                ++createdBlocks;
                Block& newBlock = partition.splitBlock(block, currentIndex);
                if (!newBlock.isMarkedAsPredecessor()) {
                    newBlock.markAsSplitter();
                    splitterQueue.push_back(&newBlock);
                }
                beginIndex = currentIndex;
            }
            
            assert(partition.check());
            
            return createdBlocks;
        }
        
        template<typename ValueType>
        std::size_t BisimulationDecomposition2<ValueType>::splitPartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, std::deque<Block*>& splitterQueue) {
            std::cout << "treating block " << splitter.getId() << " as splitter" << std::endl;
            splitter.print(partition);
            
            std::list<Block*> predecessorBlocks;
            
            // Iterate over all states of the splitter and check its predecessors.
            storm::storage::sparse::state_type currentPosition = splitter.getBegin();
            for (auto stateIterator = partition.getBeginOfStates(splitter), stateIte = partition.getEndOfStates(splitter); stateIterator != stateIte; ++stateIterator, ++currentPosition) {
                std::cout << "states -----" << std::endl;
                for (auto stateIterator = partition.getStates().begin() + splitter.getOriginalBegin(), stateIte = partition.getStates().begin() + splitter.getEnd(); stateIterator != stateIte; ++stateIterator) {
                    std::cout << *stateIterator << " ";
                }
                std::cout << std::endl;

                storm::storage::sparse::state_type currentState = *stateIterator;
                std::cout << "current state " << currentState << " at pos " << currentPosition << std::endl;
                
                uint_fast64_t elementsToSkip = 0;
                for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                    storm::storage::sparse::state_type predecessor = predecessorEntry.getColumn();
                    std::cout << "found predecessor " << predecessor << " of state " << currentState << std::endl;
                    
                    Block& predecessorBlock = partition.getBlock(predecessor);
                    std::cout << "predecessor block " << predecessorBlock.getId() << std::endl;
                    
                    // If the predecessor block has just one state, there is no point in splitting it.
                    if (predecessorBlock.getNumberOfStates() <= 1) {
                        continue;
                    }
                    
                    storm::storage::sparse::state_type predecessorPosition = partition.getPosition(predecessor);
                    
                    // If we have not seen this predecessor before, we move it to a part near the beginning of the block.
                    if (predecessorPosition >= predecessorBlock.getBegin()) {
                        if (&predecessorBlock == &splitter) {
                            // If the predecessor we just found was already processed (in terms of visiting its predecessors),
                            // we swap it with the state that is currently at the beginning of the block and move the start
                            // of the block one step further.
                            if (predecessorPosition <= currentPosition) {
                                partition.swapStates(predecessor, partition.getState(predecessorBlock.getBegin()));
                                predecessorBlock.incrementBegin();
                            } else {
                                std::cout << "current position is " << currentPosition << std::endl;
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
                    std::cout << "skipping " << elementsToSkip << " elements" << std::endl;
                    stateIterator += elementsToSkip;
                    currentPosition += elementsToSkip;
                }
            }
            
            // Now we can traverse the list of states of the splitter whose predecessors we have not yet explored.
            for (auto stateIterator = partition.getStates().begin() + splitter.getOriginalBegin(), stateIte = partition.getStates().begin() + splitter.getMarkedPosition(); stateIterator != stateIte; ++stateIterator) {
                storm::storage::sparse::state_type currentState = *stateIterator;
                
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
            
            splitter.setMarkedPosition(splitter.getBegin());
            
            std::list<Block*> blocksToSplit;
            
            std::cout << "having " << predecessorBlocks.size() << " pred blocks " << std::endl;
            
            // Now, we can iterate over the predecessor blocks and see whether we have to create a new block for
            // predecessors of the splitter.
            for (auto blockPtr : predecessorBlocks) {
                Block& block = *blockPtr;
                
                block.unmarkAsPredecessorBlock();
                block.resetMarkedPosition();
                
                // If we have moved the begin of the block to somewhere in the middle of the block, we need to split it.
                if (block.getBegin() != block.getEnd()) {
                    std::cout << "moved begin of block " << block.getId() << " to " << block.getBegin() << " and end to " << block.getEnd() << std::endl;
                    
                    Block& newBlock = partition.insertBlock(block);
                    std::cout << "created new block " << std::endl;
                    newBlock.print(partition);
                    splitterQueue.push_back(&newBlock);
                } else {
                    std::cout << "all states are predecessors" << std::endl;
                    
                    // In this case, we can keep the block by setting its begin to the old value.
                    block.setBegin(block.getOriginalBegin());
                    blocksToSplit.emplace_back(&block);
                }
            }
            
            assert(partition.check());
            
            // Finally, we walk through the blocks that have a transition to the splitter and split them using
            // probabilistic information.
            for (auto blockPtr : blocksToSplit) {
                std::cout << "block to split: " << blockPtr->getId() << std::endl;
                if (blockPtr->getNumberOfStates() <= 1) {
                    continue;
                }
                
                splitBlockProbabilities(*blockPtr, partition, splitterQueue);
            }
            
            return 0;
        }
        
        template class BisimulationDecomposition2<double>;
    }
}