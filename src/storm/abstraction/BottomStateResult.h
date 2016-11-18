#pragma once

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/Bdd.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType DdType>
        struct BottomStateResult {
        public:
            BottomStateResult(storm::dd::Bdd<DdType> const& states, storm::dd::Bdd<DdType> const& transitions);
            
            storm::dd::Bdd<DdType> states;
            storm::dd::Bdd<DdType> transitions;
        };
        
    }
}