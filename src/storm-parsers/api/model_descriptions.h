#pragma once

#include <string>
#include <map>
#include <storm/storage/jani/ModelFeatures.h>

namespace storm {
    namespace prism {
        class Program;
    }
    namespace jani {
        class Model;
        class Property;
    }
    
    namespace api {
        
        storm::prism::Program parseProgram(std::string const& filename, bool prismCompatibility = false, bool simplify = true);
        
        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& filename);
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> parseJaniModel(std::string const& filename, storm::jani::ModelFeatures const& allowedFeatures);
    }
}
