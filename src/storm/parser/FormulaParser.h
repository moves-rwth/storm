#ifndef STORM_PARSER_FORMULAPARSER_H_
#define STORM_PARSER_FORMULAPARSER_H_

#include <sstream>

#include "src/parser/SpiritParserDefinitions.h"
#include "src/parser/ExpressionParser.h"
#include "src/logic/Formulas.h"
#include "src/storage/expressions/Expression.h"
#include "src/utility/macros.h"

namespace storm {
    namespace prism {
        class Program;
    }
    
    namespace parser {
        
        // Forward-declare grammar.
        class FormulaParserGrammar;
        
        class FormulaParser {
        public:
            FormulaParser();
            explicit FormulaParser(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager);
            explicit FormulaParser(storm::prism::Program const& program);
            
            FormulaParser(FormulaParser const& other);
            FormulaParser& operator=(FormulaParser const& other);

            /*!
             * Parses the formula given by the provided string.
             *
             * @param formulaString The formula as a string.
             * @return The resulting formula.
             */
            std::shared_ptr<storm::logic::Formula const> parseSingleFormulaFromString(std::string const& formulaString) const;
            
            /*!
             * Parses the formula given by the provided string.
             *
             * @param formulaString The formula as a string.
             * @return The contained formulas.
             */
            std::vector<std::shared_ptr<storm::logic::Formula const>> parseFromString(std::string const& formulaString) const;
            
            /*!
             * Parses the formulas in the given file.
             *
             * @param filename The name of the file to parse.
             * @return The contained formulas.
             */
            std::vector<std::shared_ptr<storm::logic::Formula const>> parseFromFile(std::string const& filename) const;
            
            /*!
             * Adds an identifier and the expression it is supposed to be replaced with. This can, for example be used
             * to substitute special identifiers in the formula by expressions.
             *
             * @param identifier The identifier that is supposed to be substituted.
             * @param expression The expression it is to be substituted with.
             */
            void addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression);
            
        private:
            // The manager used to parse expressions.
            std::shared_ptr<storm::expressions::ExpressionManager const> manager;
            
            // Keep track of added identifier expressions.
            qi::symbols<char, storm::expressions::Expression> identifiers_;
            
            // The grammar used to parse the input.
            std::shared_ptr<FormulaParserGrammar> grammar;
        };
                
    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_FORMULAPARSER_H_ */
