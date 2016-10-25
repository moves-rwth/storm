#pragma once

#include "src/builder/jit/Choice.h"

namespace storm {
    namespace builder {
        namespace jit {
         
            template <typename IndexType, typename ValueType>
            class StateBehaviour {
            public:
                typedef std::vector<Choice<IndexType, ValueType>> ContainerType;
                
                void addChoice(Choice<IndexType, ValueType>&& choice);
                Choice<IndexType, ValueType>& addChoice();
                
                void clear();
                
            private:
                ContainerType choices;
            };
            
        }
    }
}
