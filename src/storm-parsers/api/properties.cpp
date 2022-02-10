#include "storm-parsers/api/properties.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/api/properties.h"

#include "storm/logic/RewardAccumulationEliminationVisitor.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/prism/Program.h"

#include "storm/logic/Formula.h"

#include "storm/utility/cli.h"

namespace storm {
namespace api {

boost::optional<std::set<std::string>> parsePropertyFilter(std::string const& propertyFilter) {
    if (propertyFilter == "all") {
        return boost::none;
    }
    std::vector<std::string> propertyNames = storm::utility::cli::parseCommaSeparatedStrings(propertyFilter);
    std::set<std::string> propertyNameSet(propertyNames.begin(), propertyNames.end());
    return propertyNameSet;
}

std::vector<storm::jani::Property> parseProperties(storm::parser::FormulaParser& formulaParser, std::string const& inputString,
                                                   boost::optional<std::set<std::string>> const& propertyFilter) {
    // If the given property is a file, we parse it as a file, otherwise we assume it's a property.
    std::vector<storm::jani::Property> properties;
    if (std::ifstream(inputString).good()) {
        STORM_LOG_INFO("Loading properties from file: " << inputString << '\n');
        properties = formulaParser.parseFromFile(inputString);
    } else {
        properties = formulaParser.parseFromString(inputString);
    }

    return filterProperties(properties, propertyFilter);
}

std::vector<storm::jani::Property> parseProperties(std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter) {
    auto exprManager = std::make_shared<storm::expressions::ExpressionManager>();
    storm::parser::FormulaParser formulaParser(exprManager);
    return parseProperties(formulaParser, inputString, propertyFilter);
}

std::vector<storm::jani::Property> parsePropertiesForJaniModel(std::string const& inputString, storm::jani::Model const& model,
                                                               boost::optional<std::set<std::string>> const& propertyFilter) {
    storm::parser::FormulaParser formulaParser(model.getManager().getSharedPointer());
    auto formulas = parseProperties(formulaParser, inputString, propertyFilter);
    // Eliminate reward accumulations if possible
    storm::logic::RewardAccumulationEliminationVisitor v(model);
    v.eliminateRewardAccumulations(formulas);
    return substituteConstantsInProperties(formulas, model.getConstantsSubstitution());
}

std::vector<storm::jani::Property> parsePropertiesForPrismProgram(std::string const& inputString, storm::prism::Program const& program,
                                                                  boost::optional<std::set<std::string>> const& propertyFilter) {
    storm::parser::FormulaParser formulaParser(program);
    auto formulas = parseProperties(formulaParser, inputString, propertyFilter);
    return substituteConstantsInProperties(formulas, program.getConstantsFormulasSubstitution());
}

std::vector<storm::jani::Property> parsePropertiesForSymbolicModelDescription(std::string const& inputString,
                                                                              storm::storage::SymbolicModelDescription const& modelDescription,
                                                                              boost::optional<std::set<std::string>> const& propertyFilter) {
    std::vector<storm::jani::Property> result;
    if (modelDescription.isPrismProgram()) {
        result = storm::api::parsePropertiesForPrismProgram(inputString, modelDescription.asPrismProgram(), propertyFilter);
    } else {
        STORM_LOG_ASSERT(modelDescription.isJaniModel(), "Unknown model description type.");
        result = storm::api::parsePropertiesForJaniModel(inputString, modelDescription.asJaniModel(), propertyFilter);
    }
    return result;
}
}  // namespace api
}  // namespace storm
