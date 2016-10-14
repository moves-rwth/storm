#pragma once

namespace storm {
    namespace builder {
        
        template <typename ValueType>
        class JitModelBuilderInterface {
        public:
            virtual ~JitModelBuilderInterface() {
                // Intentionally left empty.
            }
            
            virtual void build() {
                // Intentionally left empty.
            }
        };
        
    }
}
