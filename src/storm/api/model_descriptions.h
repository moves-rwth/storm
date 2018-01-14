#pragma once

#include <string>
#include <map>

namespace storm {
    namespace prism {
        class Program;
    }
    namespace jani {
        class Model;
        class Property;
    }
    
    namespace api {
        
        storm::prism::Program parseProgram(std::string const& filename, bool prismCompatibility = false);
        
        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& filename);
        
    }
}
