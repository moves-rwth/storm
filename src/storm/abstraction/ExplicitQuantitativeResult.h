#pragma once

#include <cstdint>
#include <vector>

namespace storm {
namespace storage {
class BitVector;
}

namespace abstraction {

template<typename ValueType>
class ExplicitQuantitativeResult {
   public:
    ExplicitQuantitativeResult() = default;
    ExplicitQuantitativeResult(uint64_t numberOfStates);
    ExplicitQuantitativeResult(std::vector<ValueType>&& values);

    std::vector<ValueType> const& getValues() const;
    std::vector<ValueType>& getValues();
    void setValue(uint64_t state, ValueType const& value);

    std::pair<ValueType, ValueType> getRange(storm::storage::BitVector const& states) const;

   private:
    std::vector<ValueType> values;
};

}  // namespace abstraction
}  // namespace storm
