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
            return parseJaniModel(filename, features);
        }
        
        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& filename, storm::jani::ModelFeatures const& allowedFeatures) {
            std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> modelAndFormulae = storm::parser::JaniParser::parse(filename);
            
            modelAndFormulae.first.checkValid();
            // TODO: properties
            auto nonEliminatedFeatures = modelAndFormulae.first.restrictToFeatures(allowedFeatures);
            STORM_LOG_THROW(nonEliminatedFeatures.empty(), storm::exceptions::NotSupportedException, "The used model feature(s) " << nonEliminatedFeatures.toString() << " is/are not in the list of allowed features.");
            return modelAndFormulae;
        }
        
        
    }
}
