#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <boost/optional.hpp>

namespace storm {
    namespace parser {
        class FormulaParser;
    }
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
        boost::optional<std::set<std::string>> parsePropertyFilter(std::string const& propertyFilter);

        // Parsing properties.
        std::vector<storm::jani::Property> parseProperties(storm::parser::FormulaParser& formulaParser, std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
        std::vector<storm::jani::Property> parseProperties(std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
        std::vector<storm::jani::Property> parsePropertiesForPrismProgram(std::string const& inputString, storm::prism::Program const& program, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
        std::vector<storm::jani::Property> parsePropertiesForJaniModel(std::string const& inputString, storm::jani::Model const& model, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
        std::vector<storm::jani::Property> parsePropertiesForSymbolicModelDescription(std::string const& inputString, storm::storage::SymbolicModelDescription const& modelDescription, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);

        // Process properties.
        std::vector<storm::jani::Property> substituteConstantsInProperties(std::vector<storm::jani::Property> const& properties, std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
        std::vector<storm::jani::Property> filterProperties(std::vector<storm::jani::Property> const& properties, boost::optional<std::set<std::string>> const& propertyFilter);
        std::vector<std::shared_ptr<storm::logic::Formula const>> extractFormulasFromProperties(std::vector<storm::jani::Property> const& properties);

    }
}
