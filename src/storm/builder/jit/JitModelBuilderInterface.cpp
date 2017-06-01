#include "storm/builder/jit/JitModelBuilderInterface.h"

#include "storm/adapters/RationalFunctionAdapter.h"

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
            void JitModelBuilderInterface<IndexType, ValueType>::addStateBehaviour(IndexType const& stateId, StateBehaviour<IndexType, ValueType>& behaviour) {
                modelComponentsBuilder.addStateBehaviour(stateId, behaviour);
            }
            
            template class JitModelBuilderInterface<uint32_t, double>;
            template class JitModelBuilderInterface<uint32_t, storm::RationalNumber>;
            template class JitModelBuilderInterface<uint32_t, storm::RationalFunction>;
            
        }
    }
}
