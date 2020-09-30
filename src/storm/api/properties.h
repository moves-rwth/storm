#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

namespace storm {

    namespace jani {
        class Property;
        class Model;
    }
    namespace expressions {
        class Variable;
        class Expression;
    }
    namespace prism {
        class Program;
    }
    namespace storage {
        class SymbolicModelDescription;
    }
    namespace logic {
        class Formula;
    }
    
    namespace api {

        // Process properties.
        std::vector<storm::jani::Property> substituteConstantsInProperties(std::vector<storm::jani::Property> const& properties, std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
        std::vector<storm::jani::Property> filterProperties(std::vector<storm::jani::Property> const& properties, boost::optional<std::set<std::string>> const& propertyFilter);
        std::vector<std::shared_ptr<storm::logic::Formula const>> extractFormulasFromProperties(std::vector<storm::jani::Property> const& properties);
        storm::jani::Property createMultiObjectiveProperty(std::vector<storm::jani::Property> const& properties);

    }
}
