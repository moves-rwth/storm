#pragma once

#include <set>

#include "src/storm/storage/dd/DdType.h"

namespace storm {
    namespace expressions {
        class Variable;
    }
    namespace dd {
        template<storm::dd::DdType Type>
        class Bdd;
    }
    
    namespace utility {
        namespace dd {
            
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> computeReachableStates(storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& transitions, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables);
            
        }
    }
}
