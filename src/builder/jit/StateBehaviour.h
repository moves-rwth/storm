#pragma once

#include "src/storage/jani/ModelType.h"
#include "src/builder/jit/Choice.h"

namespace storm {
    namespace builder {
        namespace jit {
         
            template <typename IndexType, typename ValueType>
            class StateBehaviour {
            public:
                typedef std::vector<Choice<IndexType, ValueType>> ContainerType;
                
                StateBehaviour();
                
                void addChoice(Choice<IndexType, ValueType>&& choice);
                Choice<IndexType, ValueType>& addChoice();
                
                ContainerType const& getChoices() const;
                
                void reduce(storm::jani::ModelType const& modelType);
                void compress();
                
                bool isExpanded() const;
                void setExpanded();
                
                bool empty() const;
                std::size_t size() const;
                void clear();
                
            private:
                ContainerType choices;
                bool compressed;
                bool expanded;
            };
            
        }
    }
}
