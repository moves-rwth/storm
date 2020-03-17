#pragma once

#include <string>
#include <map>
#include <vector>
#include <boost/optional.hpp>

namespace storm {
    namespace prism {
        class Program;
    }
    namespace jani {
        class Model;
        class ModelFeatures;
        class Property;
    }
    
    namespace api {
        
        storm::prism::Program parseProgram(std::string const& filename, bool prismCompatibility = false, bool simplify = true);
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> parseJaniModel(std::string const& filename, boost::optional<std::vector<std::string>> const& propertyFilter = boost::none);
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> parseJaniModel(std::string const& filename, storm::jani::ModelFeatures const& allowedFeatures, boost::optional<std::vector<std::string>> const& propertyFilter = boost::none);
        void simplifyJaniModel(storm::jani::Model& model, std::vector<storm::jani::Property>& properties , storm::jani::ModelFeatures const& supportedFeatures);

    }
}
