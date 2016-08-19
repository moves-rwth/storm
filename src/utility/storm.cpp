#include <src/parser/JaniParser.h>
#include "storm.h"

// Headers related to parsing.
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/utility/macros.h"


namespace storm {
   
     storm::prism::Program parseProgram(std::string const& path) {
        storm::prism::Program program = storm::parser::PrismParser::parse(path).simplify().simplify();
        program.checkValidity();
        return program;
    }

    storm::jani::Model parseJaniModel(std::string const& path) {
        storm::jani::Model model = storm::parser::JaniParser::parse(path);
        if(!model.checkValidity(true)) {
            STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Jani file parsing yields invalid model.");
        }
        return model;
    }

    /**
     * Helper
     * @param FormulaParser
     * @return The formulas.
     */
    std::vector<std::shared_ptr<storm::logic::Formula const>> parseFormulas(storm::parser::FormulaParser & formulaParser, std::string const& inputString) {
        // If the given property looks like a file (containing a dot and there exists a file with that name),
        // we try to parse it as a file, otherwise we assume it's a property.
        if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
            return formulaParser.parseFromFile(inputString);
        } else {
            return formulaParser.parseFromString(inputString);
        }
    }

    std::vector<std::shared_ptr<storm::logic::Formula const>> parseFormulasForExplicit(std::string const& inputString) {
        storm::parser::FormulaParser formulaParser;
        return parseFormulas(formulaParser, inputString);
    }

    std::vector<std::shared_ptr<storm::logic::Formula const>> parseFormulasForProgram(std::string const& inputString, storm::prism::Program const& program) {
        storm::parser::FormulaParser formulaParser(program);
        return parseFormulas(formulaParser, inputString);
    } 
}
