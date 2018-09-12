#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/traverser/JaniTraverser.h"
#include "storm/storage/jani/Property.h"

namespace storm {
    namespace jani {
        
        struct ArrayEliminatorData {
            std::vector<std::shared_ptr<ArrayVariable>> eliminatedArrayVariables;
            std::unordered_map<storm::expressions::Variable, std::vector<storm::jani::Variable const*>> replacements;
            
            // Transforms the given expression (which might contain array expressions) to an equivalent expression without array variables.
            storm::expressions::Expression transformExpression(storm::expressions::Expression const& arrayExpression) const;
            // Transforms the given property (which might contain array expressions) to an equivalent property without array variables.
            void transformProperty(storm::jani::Property& property) const;
        };
        
        class ArrayEliminator {
        public:
            ArrayEliminator() = default;
            
            /*!
             * Eliminates all array references in the given model by replacing them with basic variables.
             */
            
            ArrayEliminatorData eliminate(Model& model, bool keepNonTrivialArrayAccess = false);

        private:
        
        
        };
    }
}

