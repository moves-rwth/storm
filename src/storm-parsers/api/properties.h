#pragma once

#include <boost/optional.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace storm {
namespace parser {
class FormulaParser;
}
namespace jani {
class Property;
class Model;
}  // namespace jani
namespace expressions {
class Variable;
class Expression;
}  // namespace expressions
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
std::vector<storm::jani::Property> parseProperties(storm::parser::FormulaParser& formulaParser, std::string const& inputString,
                                                   boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
std::vector<storm::jani::Property> parseProperties(std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
std::vector<storm::jani::Property> parsePropertiesForPrismProgram(std::string const& inputString, storm::prism::Program const& program,
                                                                  boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
std::vector<storm::jani::Property> parsePropertiesForJaniModel(std::string const& inputString, storm::jani::Model const& model,
                                                               boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
std::vector<storm::jani::Property> parsePropertiesForSymbolicModelDescription(std::string const& inputString,
                                                                              storm::storage::SymbolicModelDescription const& modelDescription,
                                                                              boost::optional<std::set<std::string>> const& propertyFilter = boost::none);

}  // namespace api
}  // namespace storm
