#ifndef STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_
#define STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_

#include <algorithm>
#include <vector>

namespace storm {
namespace storage {

template<typename T, typename Container = std::vector<T>, typename Compare = std::less<T>>
class DynamicPriorityQueue {
   private:
    Container container;
    Compare compare;

   public:
    explicit DynamicPriorityQueue(Compare const& compare) : container(), compare(compare) {
        // Intentionally left empty
    }

    explicit DynamicPriorityQueue(Container&& container, Compare const& compare) : container(std::move(container)), compare(compare) {
        std::make_heap(container.begin(), container.end(), compare);
    }

    void fix() {
        std::make_heap(container.begin(), container.end(), compare);
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

    void push(const T& item) {
        container.push_back(item);
        std::push_heap(container.begin(), container.end(), compare);
    }

    void push(T&& item) {
        container.push_back(std::move(item));
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

    Container getContainer() const {
        return container;
    }
};
}  // namespace storage
}  // namespace storm

#endif  // STORM_STORAGE_DYNAMICPRIORITYQUEUE_H_
