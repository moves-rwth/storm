#pragma once

#include <queue>

namespace storm {
namespace builder {
namespace jit {

template<typename StateType>
class StateSet {
   public:
    StateType const& peek() const {
        return storage.front();
    }

    StateType get() {
        StateType result = std::move(storage.front());
        storage.pop();
        return result;
    }

    void add(StateType const& state) {
        storage.push(state);
    }

    bool empty() const {
        return storage.empty();
    }

   private:
    std::queue<StateType> storage;
};

}  // namespace jit
}  // namespace builder
}  // namespace storm
