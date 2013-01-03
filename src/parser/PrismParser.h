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

namespace storm {
namespace parser {
class intConstDef {
public:
	std::string name;
	int value;

	void print() {
		std::cout << "(" << name << ", " << value << ")" << std::endl;
	}
};
}
}

BOOST_FUSION_ADAPT_STRUCT(
    storm::parser::intConstDef,
    (std::string, name)
    (int, value)
)

namespace storm {

namespace parser {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

class PrismParser {

public:
	void test() {
		std::string str = "const int ab = 10;";
		std::string::const_iterator iter = str.begin();
		std::string::const_iterator end = str.end();
		prismGrammar<std::string::const_iterator> grammar;

		intConstDef result;
		bool r = phrase_parse(iter, end, grammar, ascii::space, result);

		std::cout << r << std::endl;

		if (r && iter == end) {
			std::cout << "-------------------------\n";
			std::cout << "Parsing succeeded\n";
			result.print();
			// std::cout << "result = " << result << std::endl;
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
	template<typename Iterator>
	struct prismGrammar : qi::grammar<Iterator, intConstDef(), ascii::space_type> {

		prismGrammar() : prismGrammar::base_type(start) {
			identifierName %= +(qi::char_);
			integerConstantDefinition %= qi::lit("const") >> qi::lit("int") >> identifierName >> "=" >> qi::int_ >> ";";

			start %= integerConstantDefinition;
		}

		qi::rule<Iterator, intConstDef(), ascii::space_type> start;
		qi::rule<Iterator, intConstDef(), ascii::space_type> integerConstantDefinition;
		qi::rule<Iterator, std::string(), ascii::space_type> identifierName;

	};
};

} // namespace parser

} // namespace storm

#endif /* PRISMPARSER_H_ */
