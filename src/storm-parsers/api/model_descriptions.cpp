#include "model_descriptions.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm-parsers/parser/JaniParser.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"


namespace storm {
    namespace api {
        
        storm::prism::Program parseProgram(std::string const& filename, bool prismCompatibility, bool simplify) {
            storm::prism::Program program = storm::parser::PrismParser::parse(filename, prismCompatibility);
            if (simplify) {
                program = program.simplify().simplify();
            }
            program.checkValidity();
            return program;
        }

        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& filename) {
            storm::jani::ModelFeatures features;
            // Add derived-operators and state-exit-rewards as these can be handled by all model builders
            features.add(storm::jani::ModelFeature::DerivedOperators);
            features.add(storm::jani::ModelFeature::StateExitRewards);
            auto parsedResult = parseJaniModel(filename, features);
            std::map<std::string, storm::jani::Property> propertyMap;
            for (auto const& property : parsedResult.second) {
                propertyMap.emplace(property.getName(), property);
            }
            return std::make_pair(std::move(parsedResult.first), std::move(propertyMap));
        }
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> parseJaniModel(std::string const& filename, storm::jani::ModelFeatures const& allowedFeatures, boost::optional<std::vector<std::string>> const& propertyFilter) {
            
            bool parseProperties = !propertyFilter.is_initialized() || !propertyFilter.get().empty();
            std::pair<storm::jani::Model, std::vector<storm::jani::Property>> modelAndFormulae = storm::parser::JaniParser::parse(filename, parseProperties);
            
            // eliminate unselected properties.
            if (propertyFilter.is_initialized()) {
                std::vector<storm::jani::Property> newProperties;
                // Make sure to preserve the provided order
                for (auto const& propName : propertyFilter.get()) {
                    bool found = false;
                    for (auto const& property : modelAndFormulae.second) {
                        if (property.getName() == propName) {
                            newProperties.push_back(std::move(property));
                            found = true;
                            break;
                        }
                    }
                    STORM_LOG_ERROR_COND(found, "No JANI property with name '" << propName << "' is known.");
                }
                modelAndFormulae.second = std::move(newProperties);
            }
            
            modelAndFormulae.first.checkValid();
            auto nonEliminatedFeatures = modelAndFormulae.first.restrictToFeatures(allowedFeatures, modelAndFormulae.second);
            STORM_LOG_THROW(nonEliminatedFeatures.empty(), storm::exceptions::NotSupportedException, "The used model feature(s) " << nonEliminatedFeatures.toString() << " is/are not in the list of allowed features.");
            return modelAndFormulae;
        }
        
        
    }
}
