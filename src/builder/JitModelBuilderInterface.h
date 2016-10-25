#pragma once

#include <memory>

#include "src/models/sparse/Model.h"

namespace storm {
    namespace builder {
        
        template <typename ValueType>
        class JitModelBuilderInterface {
        public:
            virtual ~JitModelBuilderInterface() {
                // Intentionally left empty.
            }
            
            virtual storm::models::sparse::Model<ValueType>* build() {
                return nullptr;
            }
        };
        
    }
}
