#include "src/builder/jit/StateBehaviour.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::addChoice(Choice<IndexType, ValueType>&& choice) {
                choices.emplace_back(std::move(choice));
            }
            
            template <typename IndexType, typename ValueType>
            Choice<IndexType, ValueType>& StateBehaviour<IndexType, ValueType>::addChoice() {
                choices.emplace_back();
                return choices.back();
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::clear() {
                choices.clear();
            }
            
        }
    }
}
