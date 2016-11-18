#pragma once

#include <map>

namespace storm {
    namespace expressions {
        class Variable;
        class Expression;
    }
    
    namespace jani {
        class Model;
    }
    
    namespace utility {
        namespace jani {
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::jani::Model const& model, std::string const& constantDefinitionString);
            
        }
    }
}

