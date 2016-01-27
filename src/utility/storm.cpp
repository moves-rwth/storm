#include "storm.h"

// Headers related to parsing.
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"



namespace storm {
   
     storm::prism::Program parseProgram(std::string const& path) {
        storm::prism::Program program= storm::parser::PrismParser::parse(path).simplify().simplify();
        program.checkValidity();
        return program;
    }
     
    /**
     * Helper
     * @param FormulaParser
     * @return The formulas.
     */
    std::vector<std::shared_ptr<const storm::logic::Formula>> parseFormulas(storm::parser::FormulaParser & formulaParser, std::string const& inputString) {
        // If the given property looks like a file (containing a dot and there exists a file with that name),
        // we try to parse it as a file, otherwise we assume it's a property.
        if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
            return formulaParser.parseFromFile(inputString);
        } else {
            return formulaParser.parseFromString(inputString);
        }
    }

    std::vector<std::shared_ptr<const storm::logic::Formula>> parseFormulasForExplicit(std::string const& inputString) {
        storm::parser::FormulaParser formulaParser;
        return parseFormulas(formulaParser, inputString);
    }

    std::vector<std::shared_ptr<const storm::logic::Formula>> parseFormulasForProgram(std::string const& inputString, storm::prism::Program const& program) {
        storm::parser::FormulaParser formulaParser(program);
        return parseFormulas(formulaParser, inputString);
    } 
}