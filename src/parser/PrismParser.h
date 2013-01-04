/*
 * PrismParser.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Christian Dehnert
 */

#ifndef PRISMPARSER_H_
#define PRISMPARSER_H_

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include "src/ir/expressions/Expressions.h"
#include "src/ir/Program.h"

namespace storm {

namespace parser {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

class PrismParser {

public:
	void test() {
		std::string testInput = "";
		// storm::ir::Program* result = getProgram(testInput);
		getProgram(testInput);

/*		std::cout << "Resulting program:" << std::endl;
		std::cout << "-------------------------\n";
		std::cout << result->toString() << std::endl;
		std::cout << "-------------------------\n"; */
	}

	void getProgram(std::string inputString) {
		using qi::_val;
		using qi::int_;
		using qi::_r1;
		using qi::_1;
		using phoenix::ref;
		using phoenix::val;

		prismGrammar<std::string::const_iterator> grammar;
		std::string::const_iterator it = inputString.begin();
		std::string::const_iterator end = inputString.end();
		storm::ir::Program result;

		int resultInt = 0;
		std::string input = "asdf";
		bool r = phrase_parse(it, end, grammar(phoenix::val(input)), ascii::space, resultInt);

		if (r && it == end) {
			std::cout << "Parsing successful!" << std::endl;
		} else {
			std::string rest(it, end);
			std::cout << "-------------------------\n";
			std::cout << "Parsing failed\n";
			std::cout << "stopped at: \"" << rest << "\"\n";
			std::cout << "-------------------------\n";
		}
	}

private:
	template<typename Iterator>
	struct prismGrammar : qi::grammar<Iterator, int(std::string), ascii::space_type> {

		prismGrammar() : prismGrammar::base_type(start) {
			using qi::_val;
			using qi::int_;
			using qi::_r1;
			using qi::_1;
			using phoenix::ref;

			// start = constantDefinitionList(phoenix::ref(qi::_r1), phoenix::ref(qi::_r1));
			start = qi::lit(qi::_r1) >> int_;
			constantDefinitionList = qi::int_;

			variableDefinition %= (booleanVariableDefinition | integerVariableDefinition);
			booleanVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("bool") >> qi::lit(";"))[phoenix::bind(booleanVariables_.add, qi::_1, phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), phoenix::bind(allVariables_.add, qi::_1, phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), qi::_val = phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)];
			integerVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("[") >> integerConstantExpression >> qi::lit("..") >> integerConstantExpression >> qi::lit("]") >> qi::lit(";"))[phoenix::bind(integerVariables_.add, qi::_1, phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), phoenix::bind(allVariables_.add, qi::_1, phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), qi::_val = phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)];

			constantDefinition %= (definedConstantDefinition | undefinedConstantDefinition);
			definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
			undefinedConstantDefinition %= (undefinedBooleanConstantDefinition | undefinedIntegerConstantDefinition | undefinedDoubleConstantDefinition);
			definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit("=") >> booleanLiteralExpression >> qi::lit(";"))[phoenix::bind(booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit("=") >> integerLiteralExpression >> qi::lit(";"))[phoenix::bind(integerConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit("=") >> doubleLiteralExpression >> qi::lit(";"))[phoenix::bind(doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit(";"))[phoenix::bind(booleanConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)), phoenix::bind(allConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)), qi::_val = phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)];
			undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit(";"))[phoenix::bind(integerConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)), phoenix::bind(allConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)), qi::_val = phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)];
			undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit(";"))[phoenix::bind(doubleConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)), phoenix::bind(allConstants_.add, qi::_1, phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)), qi::_val = phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)];

			expression %= (booleanExpression | integerExpression | doubleExpression);

			booleanExpression %= orExpression;
			orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR)];
			andExpression = atomicBooleanExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> atomicBooleanExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND)];
			atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | booleanLiteralExpression | booleanConstantExpression);
			relativeExpression = (integerExpression >> relations_ >> integerExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2)];

			integerExpression %= integerPlusExpression;
			integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> integerMultExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS)];
			integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES)];
			atomicIntegerExpression %= (integerVariableExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerConstantExpression);

			doubleExpression %= doublePlusExpression;
			doublePlusExpression = doubleMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> doubleMultExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS)];
			doubleMultExpression %= atomicDoubleExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicDoubleExpression)[qi::_val = phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES)];
			atomicDoubleExpression %= (qi::lit("(") >> doubleExpression >> qi::lit(")") | doubleConstantExpression);

			literalExpression %= (booleanLiteralExpression | integerLiteralExpression | doubleLiteralExpression);
			booleanLiteralExpression = qi::bool_[qi::_val = phoenix::new_<storm::ir::expressions::BooleanLiteral>(qi::_1)];
			integerLiteralExpression = qi::int_[qi::_val = phoenix::new_<storm::ir::expressions::IntegerLiteral>(qi::_1)];
			doubleLiteralExpression = qi::double_[qi::_val = phoenix::new_<storm::ir::expressions::DoubleLiteral>(qi::_1)];

			constantExpression %= (booleanConstantExpression | integerConstantExpression | doubleConstantExpression);
			booleanConstantExpression %= (booleanConstants_ | booleanLiteralExpression);
			integerConstantExpression %= (integerConstants_ | integerLiteralExpression);
			doubleConstantExpression %= (doubleConstants_ | doubleLiteralExpression);

			variableExpression = (integerVariableExpression | booleanVariableExpression);
			integerVariableExpression = integerVariables_;
			booleanVariableExpression = booleanVariables_;

			freeIdentifierName %= qi::lexeme[(qi::alpha >> *(qi::alnum)) - allVariables_ - allConstants_];
		}

		// The starting point of the grammar.
		qi::rule<Iterator, int(std::string), ascii::space_type> start;

		qi::rule<Iterator, int(), ascii::space_type> constantDefinitionList;

		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> variableDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanVariableDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerVariableDefinition;

		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> constantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> undefinedConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> definedConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> undefinedBooleanConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> undefinedIntegerConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> undefinedDoubleConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> definedBooleanConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> definedIntegerConstantDefinition;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> definedDoubleConstantDefinition;

		qi::rule<Iterator, std::string(), ascii::space_type> freeIdentifierName;

		// The starting point for arbitrary expressions.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> expression;

		// Rules with boolean result type.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> orExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> andExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> atomicBooleanExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> relativeExpression;

		// Rules with integer result type.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerPlusExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerMultExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> atomicIntegerExpression;

		// Rules with double result type.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doubleExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doublePlusExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doubleMultExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> atomicDoubleExpression;

		// Rules for variable recognition.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> variableExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanVariableExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerVariableExpression;

		// Rules for constant recognition.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> constantExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanConstantExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerConstantExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doubleConstantExpression;

		// Rules for literal recognition.
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> literalExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> booleanLiteralExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> integerLiteralExpression;
		qi::rule<Iterator, storm::ir::expressions::BaseExpression*(), ascii::space_type> doubleLiteralExpression;

		struct keywordsStruct : qi::symbols<char, unsigned> {
			keywordsStruct() {
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
		} keywords_;

		struct relationalOperatorStruct : qi::symbols<char, storm::ir::expressions::BinaryRelationExpression::RelationType> {
			relationalOperatorStruct() {
				add
					("==", storm::ir::expressions::BinaryRelationExpression::EQUAL)
					("<", storm::ir::expressions::BinaryRelationExpression::LESS)
					("<=", storm::ir::expressions::BinaryRelationExpression::LESS_OR_EQUAL)
					(">", storm::ir::expressions::BinaryRelationExpression::GREATER)
					(">=", storm::ir::expressions::BinaryRelationExpression::GREATER_OR_EQUAL)
				;
			}
		} relations_;

		struct variablesStruct : qi::symbols<char, storm::ir::expressions::BaseExpression*> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariables_, booleanVariables_, allVariables_ ;

		struct constantsStruct : qi::symbols<char, storm::ir::expressions::BaseExpression*> {
			// Intentionally left empty. This map is filled during parsing.
		} integerConstants_, booleanConstants_, doubleConstants_, allConstants_;

		struct modulesStruct : qi::symbols<char, unsigned> {
			// Intentionally left empty. This map is filled during parsing.
		} modules_;
	};
};

} // namespace parser

} // namespace storm

#endif /* PRISMPARSER_H_ */
