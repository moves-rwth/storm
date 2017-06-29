#include "storm/api/model_descriptions.h"

#include "storm/parser/PrismParser.h"
#include "storm/parser/JaniParser.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"

namespace storm {
    namespace api {
        
        storm::prism::Program parseProgram(std::string const& filename) {
            storm::prism::Program program = storm::parser::PrismParser::parse(filename).simplify().simplify();
            program.checkValidity();
            return program;
        }
        
        std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& filename) {
            std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> modelAndFormulae = storm::parser::JaniParser::parse(filename);
            modelAndFormulae.first.checkValid();
            return modelAndFormulae;
        }
        
    }
}
