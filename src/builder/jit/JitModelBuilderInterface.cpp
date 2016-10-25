#include "src/builder/jit/JitModelBuilderInterface.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            JitModelBuilderInterface<IndexType, ValueType>::JitModelBuilderInterface(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) : modelComponentsBuilder(modelComponentsBuilder) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            JitModelBuilderInterface<IndexType, ValueType>::~JitModelBuilderInterface() {
                // Intentionally left empty.
            }

            template <typename IndexType, typename ValueType>
            void JitModelBuilderInterface<IndexType, ValueType>::addStateBehaviour(StateBehaviour<IndexType, ValueType> const& behaviour) {
                modelComponentsBuilder.addStateBehaviour(behaviour);
            }
            
        }
    }
}
