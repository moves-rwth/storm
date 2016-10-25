#pragma once

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            class DistributionEntry {
            public:
                DistributionEntry(IndexType const& index, ValueType const& value);
                
                IndexType const& getIndex() const;
                ValueType const& getValue() const;
                
            private:
                IndexType index;
                ValueType value;
            };
            
        }
    }
}
