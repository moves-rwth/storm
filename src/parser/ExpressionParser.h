#ifndef STORM_PARSER_EXPRESSIONPARSER_H_
#define	STORM_PARSER_EXPRESSIONPARSER_H_

#include "src/parser/SpiritParserDefinitions.h"
#include "src/storage/expressions/Expression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        class ExpressionParser : public qi::grammar<Iterator, storm::expressions::Expression(), Skipper> {
        public:
            /*!
             * Creates an expression parser. Initially the parser is set to a mode in which it will not generate the
             * actual expressions but only perform a syntax check and return the expression "false". To make the parser
             * generate the actual expressions, a mapping of valid identifiers to their expressions need to be provided
             * later.
             *
             * @param invalidIdentifiers_ A symbol table of identifiers that are to be rejected.
             */
            ExpressionParser(qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_);
            
            /*!
             * Sets an identifier mapping that is used to determine valid variables in the expression. The mapped-to
             * expressions will be substituted wherever the key value appears in the parsed expression. After setting
             * this, the parser will generate expressions.
             *
             * @param identifiers A pointer to a mapping from identifiers to expressions.
             */
            void setIdentifierMapping(qi::symbols<char, storm::expressions::Expression> const* identifiers_);
            
            /*!
             * Unsets a previously set identifier mapping. This will make the parser not generate expressions any more
             * but merely check for syntactic correctness of an expression.
             */
            void unsetIdentifierMapping();
            
            /*!
             * Sets whether double literals are to be accepted or not.
             *
             * @param flag If set to true, double literals are accepted.
             */
            void setAcceptDoubleLiterals(bool flag);
            
        private:
            // A flag that indicates whether expressions should actually be generated or just a syntax check shall be
            // performed.
            bool createExpressions;
            
            // A flag that indicates whether double literals are accepted.
            bool acceptDoubleLiterals;
            
            // The currently used mapping of identifiers to expressions. This is used if the parser is set to create
            // expressions.
            qi::symbols<char, storm::expressions::Expression> const* identifiers_;
            
            // The symbol table of invalid identifiers.
            qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_;
            
            // Rules for parsing a composed expression.
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> expression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> iteExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> orExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> andExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> relativeExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> equalityExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> plusExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> multiplicationExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> unaryExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> literalExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> identifierExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> minMaxExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> floorCeilExpression;
            qi::rule<Iterator, std::string(), Skipper> identifier;
            
            // Parser that is used to recognize doubles only (as opposed to Spirit's double_ parser).
            boost::spirit::qi::real_parser<double, boost::spirit::qi::strict_real_policies<double>> strict_double;
            
            // Helper functions to create expressions.
            storm::expressions::Expression createIteExpression(storm::expressions::Expression e1, storm::expressions::Expression e2, storm::expressions::Expression e3) const;
            storm::expressions::Expression createImpliesExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createOrExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createAndExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createGreaterExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createGreaterOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createLessExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createLessOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createNotEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createPlusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMinusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMultExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createDivExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createNotExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createMinusExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createTrueExpression() const;
            storm::expressions::Expression createFalseExpression() const;
            storm::expressions::Expression createDoubleLiteralExpression(double value, bool& pass) const;
            storm::expressions::Expression createIntegerLiteralExpression(int value) const;
            storm::expressions::Expression createMinimumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMaximumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createFloorExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createCeilExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression getIdentifierExpression(std::string const& identifier) const;
            
            bool isValidIdentifier(std::string const& identifier);
            
            // Functor used for displaying error information.
            struct ErrorHandler {
                typedef qi::error_handler_result result_type;
                
                template<typename T1, typename T2, typename T3, typename T4>
                qi::error_handler_result operator()(T1 b, T2 e, T3 where, T4 const& what) const {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(where) << ": " << " expecting " << what << ".");
                    return qi::fail;
                }
            };
            
            // An error handler function.
            phoenix::function<ErrorHandler> handler;
        };
    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_EXPRESSIONPARSER_H_ */