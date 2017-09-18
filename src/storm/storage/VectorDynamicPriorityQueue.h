#ifndef STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_
#define STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_

#include <algorithm>
#include <vector>

#include "storm/utility/macros.h"

namespace storm {
    namespace storage {
        
        template<typename Compare = std::less<uint64_t>>
        class VectorDynamicPriorityQueue {
        public:
            typedef uint64_t T;
            typedef std::vector<T> Container;
            
        private:
            Container container;
            Compare compare;
            
            std::vector<uint64_t> positions;
            uint64_t upperBound;
            
            uint64_t numberOfSortedEntriesAtBack;
            
            uint64_t const NUMBER_OF_ENTRIES_TO_SORT = 100;
            
        public:
            explicit DynamicPriorityQueue(Compare const& compare, uint64_t upperBound) : container(), compare(compare), positions(upperBound) {
                // Intentionally left empty
            }
            
            explicit DynamicPriorityQueue(Container&& container, Compare const& compare) : container(std::move(container)), compare(compare), positions(this->container.size()) {
                sortAndUpdatePositions(container.begin(), container.end());
            }
            
            void fix() {
                sortAndUpdatePositions(container.begin(), container.end());
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

            template<typename TemplateType>
            void push(TemplateType&& item) {
                if (this->empty() || container.back() < item) {
                    container.emplace_back(std::forward<TemplateType>(item));
                } else {
                    
                }
            }
            
            void pop() {
                container.pop_back();
                --numberOfSortedEntriesAtBack;
                if (numberOfSortedEntriesAtBack == 0) {
                    if (container.size() > NUMBER_OF_ENTRIES_TO_SORT) {
                        sortAndUpdatePositions(container.end() - NUMBER_OF_ENTRIES_TO_SORT, container.end());
                        numberOfSortedEntriesAtBack = NUMBER_OF_ENTRIES_TO_SORT;
                    } else {
                        sortAndUpdatePositions(container.begin(), container.end());
                        numberOfSortedEntriesAtBack = container.size();
                    }
                }
            }
            
            T popTop() {
                T item = top();
                pop();
                return item;
            }
            
        private:
            void sortAndUpdatePositions(Container::const_iterator start, Container::const_iterator end) {
                std::sort(start, end);
                updatePositions(start, end);
            }
            
            void updatePositions(Container::const_iterator start, Container::const_iterator end) {
                for (; start != end; ++start) {
                    position = std::distance(container.begin(), start);
                    positions[container[position]] = position;
                }
            }
        };
    }
}

#endif // STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_
