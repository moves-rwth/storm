#pragma once

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class DistributionEntry {
            public:
                DistributionEntry();
                DistributionEntry(IndexType const& index, ValueType const& value);
                
                IndexType const& getIndex() const;
                ValueType const& getValue() const;
                
                void addToValue(ValueType const& value);
                void divide(ValueType const& value);

            private:
                IndexType index;
                ValueType value;
            };
            
        }
    }
}
