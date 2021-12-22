#pragma once

namespace storm {
namespace builder {
namespace jit {

template<typename StateType, typename ValueType>
class DistributionEntry {
   public:
    DistributionEntry();
    DistributionEntry(StateType const& state, ValueType const& value);

    StateType const& getState() const;
    ValueType const& getValue() const;

    void addToValue(ValueType const& value);
    void divide(ValueType const& value);

   private:
    StateType state;
    ValueType value;
};

}  // namespace jit
}  // namespace builder
}  // namespace storm
