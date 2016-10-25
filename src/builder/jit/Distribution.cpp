#include "src/builder/jit/Distribution.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::add(DistributionEntry<IndexType, ValueType> const& entry) {
                storage.push_back(entry);
            }
            
            template <typename IndexType, typename ValueType>
            void Distribution<IndexType, ValueType>::compress() {
                std::sort(storage.begin(), storage.end(),
                          [] (DistributionEntry<IndexType, ValueType> const& a, DistributionEntry<IndexType, ValueType> const& b) {
                              return a.getIndex() < b.getIndex();
                          }
                );
                
                // Code taken from std::unique and modified to fit needs.
                auto first = storage.begin();
                auto last = storage.end();
                
                if (first != last) {
                    auto result = first;
                    while (++first != last) {
                        if (!(result->getColumn() == first->getColumn())) {
                            if (++result != first) {
                                *result = std::move(*first);
                            }
                        } else {
                            result->addToValue(first->getValue());
                        }
                    }
                    ++result;
                    
                    storage.resize(std::distance(storage.begin(), result));
                }
            }
            
        }
    }
}
