#include "storm/storage/jani/FunctionDefinition.h"

namespace storm {
    namespace jani {
        
        FunctionDefinition::FunctionDefinition(std::string const& name, storm::expressions::Type const& type, std::vector<storm::expressions::Variable> const& parameters, storm::expressions::Expression const& functionBody) : name(name), type(type), parameters(parameters), functionBody(functionBody) {
            // Intentionally left empty.
        }
        
        std::string const& FunctionDefinition::getName() const {
            return name;
        }
        
        storm::expressions::Type const& FunctionDefinition::getType() const {
            return type;
        }
        
        std::vector<storm::expressions::Variable> const& FunctionDefinition::getParameters() const {
            return parameters;
        }
        
        storm::expressions::Expression const& FunctionDefinition::getFunctionBody() const {
            return functionBody;
        }
        
        void FunctionDefinition::setFunctionBody(storm::expressions::Expression const& body) {
            functionBody = body;
        }
    }
}
