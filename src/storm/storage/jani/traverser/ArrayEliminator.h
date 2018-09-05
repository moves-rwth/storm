#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/traverser/JaniTraverser.h"

namespace storm {
    namespace jani {
        
        struct ArrayEliminatorData {
            std::vector<std::shared_ptr<ArrayVariable>> eliminatedArrayVariables;
            std::unordered_map<storm::expressions::Variable, std::vector<storm::jani::Variable const*>> replacements;
        };
        
        class ArrayEliminator {
        public:
            ArrayEliminator() = default;
            
            ArrayEliminatorData eliminate(Model& model, bool keepNonTrivialArrayAccess = false);

        private:
        
        
        };
    }
}

