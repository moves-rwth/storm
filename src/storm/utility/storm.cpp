#include "storm.h"

// Headers related to parsing.
#include "storm/parser/JaniParser.h"

#include "storm/parser/PrismParser.h"
#include "storm/parser/FormulaParser.h"
#include "storm/utility/macros.h"
#include "storm/storage/jani/Property.h"

namespace storm{
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> extractFormulasFromProperties(std::vector<storm::jani::Property> const& properties) {
        std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
        for (auto const& prop : properties) {
            formulas.push_back(prop.getRawFormula());
        }
        return formulas;
    }
   
     storm::prism::Program parseProgram(std::string const& path) {
        storm::prism::Program program = storm::parser::PrismParser::parse(path).simplify().simplify();
        program.checkValidity();
        return program;
    }

    std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& path) {
        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> modelAndFormulae = storm::parser::JaniParser::parse(path);
        modelAndFormulae.first.checkValid();
        return modelAndFormulae;
    }

    std::vector<storm::jani::Property> parseProperties(storm::parser::FormulaParser& formulaParser, std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter) {
        // If the given property looks like a file (containing a dot and there exists a file with that name),
        // we try to parse it as a file, otherwise we assume it's a property.
        std::vector<storm::jani::Property> properties;
        if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
            properties = formulaParser.parseFromFile(inputString);
        } else {
            properties = formulaParser.parseFromString(inputString);
        }
        
        return filterProperties(properties, propertyFilter);
    }

    std::vector<storm::jani::Property> parsePropertiesForExplicit(std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter) {
        auto exprManager = std::make_shared<storm::expressions::ExpressionManager>();
        storm::parser::FormulaParser formulaParser(exprManager);
        return parseProperties(formulaParser, inputString, propertyFilter);
    }

    std::vector<storm::jani::Property> substituteConstantsInProperties(std::vector<storm::jani::Property> const& properties, std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
        std::vector<storm::jani::Property> preprocessedProperties;
        for (auto const& property : properties) {
            preprocessedProperties.emplace_back(property.substitute(substitution));
        }
        return preprocessedProperties;
    }
    
    std::vector<storm::jani::Property> parsePropertiesForJaniModel(std::string const& inputString, storm::jani::Model const& model, boost::optional<std::set<std::string>> const& propertyFilter) {
        storm::parser::FormulaParser formulaParser(model.getManager().getSharedPointer());
        auto formulas = parseProperties(formulaParser, inputString, propertyFilter);
        return substituteConstantsInProperties(formulas, model.getConstantsSubstitution());
    }
    
    std::vector<storm::jani::Property> parsePropertiesForPrismProgram(std::string const& inputString, storm::prism::Program const& program, boost::optional<std::set<std::string>> const& propertyFilter) {
        storm::parser::FormulaParser formulaParser(program);
        auto formulas = parseProperties(formulaParser, inputString, propertyFilter);
        return substituteConstantsInProperties(formulas, program.getConstantsSubstitution());
    }
    
    boost::optional<std::set<std::string>> parsePropertyFilter(boost::optional<std::string> const& propertyFilter) {
        std::vector<std::string> propertyNames = storm::utility::cli::parseCommaSeparatedStrings(propertyFilter.get());
        std::set<std::string> propertyNameSet(propertyNames.begin(), propertyNames.end());
        return propertyNameSet;
    }
    
    std::vector<storm::jani::Property> filterProperties(std::vector<storm::jani::Property> const& properties, boost::optional<std::set<std::string>> const& propertyFilter) {
        if (propertyFilter) {
            std::set<std::string> const& propertyNameSet = propertyFilter.get();
            std::vector<storm::jani::Property> result;
            std::set<std::string> reducedPropertyNames;
            for (auto const& property : properties) {
                if (propertyNameSet.find(property.getName()) != propertyNameSet.end()) {
                    result.push_back(property);
                    reducedPropertyNames.insert(property.getName());
                }
            }
            
            if (reducedPropertyNames.size() < propertyNameSet.size()) {
                std::set<std::string> missingProperties;
                std::set_difference(propertyNameSet.begin(), propertyNameSet.end(), reducedPropertyNames.begin(), reducedPropertyNames.end(), std::inserter(missingProperties, missingProperties.begin()));
                STORM_LOG_WARN("Filtering unknown properties " << boost::join(missingProperties, ", ") << ".");
            }
            
            return result;
        } else {
            return properties;
        }
    }
}
