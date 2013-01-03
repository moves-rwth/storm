/*
 * PrismParser.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Christian Dehnert
 */

#ifndef PRISMPARSER_H_
#define PRISMPARSER_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include "src/ir/expressions/Expressions.h"

namespace storm {

namespace parser {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

class PrismParser {

public:
	void test() {
		std::string str = "const int ab = 10;";
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		prismGrammar<std::string::const_iterator> grammar;

		storm::ir::expressions::BaseExpression* result;
		bool r = phrase_parse(iter, end, grammar, ascii::space, result);

		std::cout << r << std::endl;

		if (r && iter == end) {
			std::cout << "-------------------------\n";
			std::cout << "Parsing succeeded\n";
			std::cout << "result = " << result << std::endl;
			std::cout << "-------------------------\n";
		} else {
			std::string rest(iter, end);
			std::cout << "-------------------------\n";
			std::cout << "Parsing failed\n";
			std::cout << "stopped at: " << rest << "\"\n";
			std::cout << "-------------------------\n";
		}
	}

private:
	struct keywords_ : qi::symbols<char, unsigned> {
		keywords_() {
			add
				("dtmc", 1)
				("ctmc", 2)
				("mdp", 3)
				("ctmdp", 4)
				("const", 5)
				("int", 6)
				("bool", 7)
				("module", 8)
				("endmodule", 9)
				("rewards", 10)
				("endrewards", 11)
				("true", 12)
				("false", 13)
			;
		}
	};

	struct variables_ : qi::symbols<char, std::string> {
		// Intentionally left empty. This map is filled during parsing.
	};

	struct modules_ : qi::symbols<char, unsigned> {
		// Intentionally left empty. This map is filled during parsing.
	};

	template<typename Iterator>
	struct prismGrammar : qi::grammar<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> {

		prismGrammar() : prismGrammar::base_type(start) {
			// identifierName %= qi::lexeme[qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9")];
			// integerConstantDefinition %= qi::lit("const") >> qi::lit("int") >> identifierName >> "=" >> qi::int_ >> ";";

			start %= literalExpression;

			literalExpression %= (booleanLiteralExpression | integerLiteralExpression | doubleLiteralExpression);
			booleanLiteralExpression = qi::bool_[qi::_val = phoenix::new_<storm::ir::expressions::BooleanLiteral>(qi::_1)];
			integerLiteralExpression = qi::int_[qi::_val = phoenix::new_<storm::ir::expressions::IntegerLiteral>(qi::_1)];
			doubleLiteralExpression = qi::double_[qi::_val = phoenix::new_<storm::ir::expressions::DoubleLiteral>(qi::_1)];
		}

		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> start;

		// The expression rules.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> literalExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanLiteralExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerLiteralExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doubleLiteralExpression;

		// qi::rule<Iterator, intConstDef(), ascii::space_type> integerConstantDefinition;
		// qi::rule<Iterator, std::string(), ascii::space_type> identifierName;

	};
};

} // namespace parser

} // namespace storm

#endif /* PRISMPARSER_H_ */
