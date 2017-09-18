#pragma once

#include <algorithm>
#include <vector>

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        
        template<typename Compare = std::less<uint64_t>>
        class ConsecutiveUint64DynamicPriorityQueue {
        public:
            typedef uint64_t T;
            typedef std::vector<T> Container;
            
        private:
            Container container;
            Compare compare;
            
            std::vector<uint64_t> positions;
            
        public:
            explicit ConsecutiveUint64DynamicPriorityQueue(uint64_t numberOfIntegers, Compare const& compare) : container(numberOfIntegers), compare(compare), positions(numberOfIntegers) {
                std::iota(container.begin(), container.end(), 0);
            }
            
            void fix() {
                std::make_heap(container.begin(), container.end(), compare);
            }
            
            void increase(uint64_t element) {
                uint64_t position = positions[element];
                if (position >= container.size()) {
                    return;
                }
                
                uint64_t parentPosition = (position - 1) / 2;
                while (position > 0 && compare(container[parentPosition], container[position])) {
                    std::swap(container[parentPosition], container[position]);
                    std::swap(positions[container[parentPosition]], positions[container[position]]);
                    
                    position = parentPosition;
                    parentPosition = (position - 1) / 2;
                }
            }
            
            bool contains(uint64_t element) const {
                return positions[element] < container.size();
            }
            
            bool empty() const {
                return container.empty();
            }
            
            std::size_t size() const {
                return container.size();
            }
            
            const T& top() const {
                return container.front();
            }

            void push(uint64_t const& item) {
                container.emplace_back(item);
                std::push_heap(container.begin(), container.end(), compare);
            }
            
            void pop() {
                std::pop_heap(container.begin(), container.end(), compare);
                container.pop_back();
            }
            
            T popTop() {
                T item = top();
                pop();
                return item;
            }
        };
    }
}
