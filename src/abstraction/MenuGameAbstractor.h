#pragma once

#include "src/storage/dd/DdType.h"

#include "src/abstraction/MenuGame.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor {
        public:
            virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() = 0;
            virtual void refine(std::vector<storm::expressions::Expression> const& predicates) = 0;
        };
        
    }
}