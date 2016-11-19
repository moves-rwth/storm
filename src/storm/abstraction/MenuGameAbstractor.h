#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/MenuGame.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor {
        public:
            virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() = 0;
            virtual void refine(std::vector<storm::expressions::Expression> const& predicates) = 0;
            virtual void refine(storm::dd::Bdd<DdType> const& pivotState, storm::dd::Bdd<DdType> const& player1Choice, storm::dd::Bdd<DdType> const& lowerChoice, storm::dd::Bdd<DdType> const& upperChoice) = 0;
        };
        
    }
}
