#include "src/parser/FormulaParser.h"

#include <fstream>

#include "src/parser/SpiritErrorHandler.h"

#include "src/storage/prism/Program.h"
#include "src/storage/jani/Model.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFormatException.h"

#include "src/storage/expressions/ExpressionEvaluator.h"
#include "FormulaParserGrammar.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace parser {
        
        FormulaParser::FormulaParser(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : manager(manager), grammar(new FormulaParserGrammar(manager)) {
            // Intentionally left empty.
        }
        
        FormulaParser::FormulaParser(storm::prism::Program const& program) : manager(program.getManager().getSharedPointer()), grammar(new FormulaParserGrammar(manager)) {
            // Make the formulas of the program available to the parser.
            for (auto const& formula : program.getFormulas()) {
                this->addIdentifierExpression(formula.getName(), formula.getExpression());
            }
        }
        
        FormulaParser::FormulaParser(FormulaParser const& other) : FormulaParser(other.manager) {
            other.identifiers_.for_each([=] (std::string const& name, storm::expressions::Expression const& expression) { this->addIdentifierExpression(name, expression); });
        }
        
        FormulaParser& FormulaParser::operator=(FormulaParser const& other) {
            this->manager = other.manager;
            this->grammar = std::shared_ptr<FormulaParserGrammar>(new FormulaParserGrammar(this->manager));
            other.identifiers_.for_each([=] (std::string const& name, storm::expressions::Expression const& expression) { this->addIdentifierExpression(name, expression); });
            return *this;
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParser::parseSingleFormulaFromString(std::string const& formulaString) const {
            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = parseFromString(formulaString);
            STORM_LOG_THROW(formulas.size() == 1, storm::exceptions::WrongFormatException, "Expected exactly one formula, but found " << formulas.size() << " instead.");
            return formulas.front();
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> FormulaParser::parseFromFile(std::string const& filename) const {
            // Open file and initialize result.
            std::ifstream inputFileStream(filename, std::ios::in);
            STORM_LOG_THROW(inputFileStream.good(), storm::exceptions::WrongFormatException, "Unable to read from file '" << filename << "'.");
            
            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
            
            // Now try to parse the contents of the file.
            try {
                std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
                formulas = parseFromString(fileContent);
            } catch(std::exception& e) {
                // In case of an exception properly close the file before passing exception.
                inputFileStream.close();
                throw e;
            }
            
            // Close the stream in case everything went smoothly and return result.
            inputFileStream.close();
            return formulas;
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> FormulaParser::parseFromString(std::string const& formulaString) const {
            PositionIteratorType first(formulaString.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(formulaString.end());
            
            // Create empty result;
            std::vector<std::shared_ptr<storm::logic::Formula const>> result;
            
            // Create grammar.
            try {
                // Start parsing.
                bool succeeded = qi::phrase_parse(iter, last, *grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded, storm::exceptions::WrongFormatException, "Could not parse formula.");
                STORM_LOG_DEBUG("Parsed formula successfully.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, e.what_);
            }
            
            return result;
        }
        
        void FormulaParser::addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression) {
            // Record the mapping and hand it over to the grammar.
            this->identifiers_.add(identifier, expression);
            grammar->addIdentifierExpression(identifier, expression);
        }
                
    } // namespace parser
} // namespace storm
