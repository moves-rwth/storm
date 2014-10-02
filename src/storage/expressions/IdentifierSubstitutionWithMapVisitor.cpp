#include <map>
#include <unordered_map>
#include <string>

#include "src/storage/expressions/IdentifierSubstitutionWithMapVisitor.h"
#include "src/storage/expressions/Expressions.h"

namespace storm {
    namespace expressions  {
		template<typename MapType>
        IdentifierSubstitutionWithMapVisitor<MapType>::IdentifierSubstitutionWithMapVisitor(MapType const& identifierToIdentifierMap) : identifierToIdentifierMap(identifierToIdentifierMap) {
            // Intentionally left empty.
        }
        
        template<typename MapType>
        void IdentifierSubstitutionWithMapVisitor<MapType>::visit(VariableExpression const* expression) {
            // If the variable is in the key set of the substitution, we need to replace it.
            auto const& namePair = this->identifierToIdentifierMap.find(expression->getVariableName());
            if (namePair != this->identifierToIdentifierMap.end()) {
                this->expressionStack.push(std::shared_ptr<BaseExpression>(new VariableExpression(expression->getReturnType(), namePair->second)));
            } else {
                this->expressionStack.push(expression->getSharedPointer());
            }
        }
                
        // Explicitly instantiate the class with map and unordered_map.
		template class IdentifierSubstitutionWithMapVisitor<std::map<std::string, std::string>>;
		template class IdentifierSubstitutionWithMapVisitor<std::unordered_map<std::string, std::string>>;
    }
}
