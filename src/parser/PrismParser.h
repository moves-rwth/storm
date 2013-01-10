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

#include "src/ir/IR.h"

namespace storm {

namespace parser {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

class PrismParser {

public:
	void test() {
		std::string testInput = "" \
				"" \
				"const bool d; const int c; const double x;" \
				"module test " \
				" a : bool;" \
				"endmodule" \
				"module test2 endmodule";
		getProgram(testInput);
	}

	void getProgram(std::string inputString) {
		storm::ir::Program result;
		prismGrammar<std::string::const_iterator> grammar(result);
		std::string::const_iterator it = inputString.begin();
		std::string::const_iterator end = inputString.end();

		storm::ir::Program realResult;
		bool r = phrase_parse(it, end, grammar, ascii::space, realResult);

		if (r && it == end) {
			std::cout << "Parsing successful!" << std::endl;
			std::cout << realResult.toString() << std::endl;
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
	struct prismGrammar : qi::grammar<Iterator, storm::ir::Program(), ascii::space_type> {

		prismGrammar(storm::ir::Program& program) : prismGrammar::base_type(start), program(program) {
			freeIdentifierName %= qi::lexeme[(qi::alpha >> *(qi::alnum)) - allVariables_ - allConstants_];

			booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BooleanLiteral>(qi::_1))];
			integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::IntegerLiteral>(qi::_1))];
			doubleLiteralExpression = qi::double_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::DoubleLiteral>(qi::_1))];
			literalExpression %= (booleanLiteralExpression | integerLiteralExpression | doubleLiteralExpression);

			integerVariableExpression = integerVariables_;
			booleanVariableExpression = booleanVariables_;
			variableExpression = (integerVariableExpression | booleanVariableExpression);

			booleanConstantExpression %= (booleanConstants_ | booleanLiteralExpression);
			integerConstantExpression %= (integerConstants_ | integerLiteralExpression);
			doubleConstantExpression %= (doubleConstants_ | doubleLiteralExpression);
			constantExpression %= (booleanConstantExpression | integerConstantExpression | doubleConstantExpression);

			atomicIntegerExpression %= (integerVariableExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerConstantExpression);
			integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
			integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> integerMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
			integerExpression %= integerPlusExpression;

			atomicDoubleExpression %= (qi::lit("(") >> doubleExpression >> qi::lit(")") | doubleConstantExpression);
			doubleMultExpression %= atomicDoubleExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicDoubleExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
			doublePlusExpression %= doubleMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> doubleMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
			doubleExpression %= doublePlusExpression;

			relativeExpression = (integerExpression >> relations_ >> integerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
			atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | booleanLiteralExpression | booleanConstantExpression);
			andExpression = atomicBooleanExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> atomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND))];
			orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR))];
			booleanExpression %= orExpression;

			expression %= (booleanExpression | integerExpression | doubleExpression);

			booleanVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("bool") >> qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::BooleanVariable>(qi::_1), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), phoenix::bind(booleanVariables_.add, qi::_1, qi::_a), phoenix::bind(booleanVariableNames_.add, qi::_1, qi::_1), phoenix::bind(allVariables_.add, qi::_1, qi::_a), phoenix::bind(booleanVariableInfo_.add, qi::_1, qi::_val)];
			integerVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("[") >> integerConstantExpression >> qi::lit("..") >> integerConstantExpression >> qi::lit("]") >> qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::IntegerVariable>(qi::_1, qi::_2, qi::_3), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), phoenix::bind(integerVariables_.add, qi::_1, qi::_a), phoenix::bind(integerVariableNames_.add, qi::_1, qi::_1), phoenix::bind(allVariables_.add, qi::_1, qi::_a), phoenix::bind(integerVariableInfo_.add, qi::_1, qi::_val)];
			variableDefinition = (booleanVariableDefinition | integerVariableDefinition);

			definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit("=") >> booleanLiteralExpression >> qi::lit(";"))[phoenix::bind(booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit("=") >> integerLiteralExpression >> qi::lit(";"))[phoenix::bind(integerConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit("=") >> doubleLiteralExpression >> qi::lit(";"))[phoenix::bind(doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>(phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)), phoenix::bind(booleanConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(booleanConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>(phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)), phoenix::bind(integerConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(integerConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>(phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)), phoenix::bind(doubleConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(doubleConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
			// undefinedConstantDefinition = (undefinedBooleanConstantDefinition | undefinedIntegerConstantDefinition | undefinedDoubleConstantDefinition);
			// constantDefinitionList = *(definedConstantDefinition | undefinedConstantDefinition);

			integerVariableName %= integerVariableNames_;
			booleanVariableName %= booleanVariableNames_;

			assignmentDefinition = (qi::lit("(") >> integerVariableName >> qi::lit("'") >> integerExpression >> qi::lit(")"))[qi::_val = phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)] | (qi::lit("(") >> booleanVariableName >> qi::lit("'") >> booleanExpression >> qi::lit(")"))[qi::_val = phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)];
			assignmentDefinitionList %= assignmentDefinition % "&";
			updateDefinition = (doubleConstantExpression >> qi::lit(":") >> assignmentDefinitionList)[qi::_val = phoenix::construct<storm::ir::Update>(qi::_1, qi::_2)];
			updateListDefinition = +updateDefinition;
			commandDefinition = (qi::lit("[") >> freeIdentifierName >> qi::lit("]") >> booleanExpression >> qi::lit("->") >> updateListDefinition >> qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::Command>(qi::_1, qi::_2, qi::_3)];

			moduleDefinition = (qi::lit("module") >> freeIdentifierName >> *(booleanVariableDefinition[phoenix::push_back(qi::_a, qi::_1)] | integerVariableDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> *commandDefinition >> qi::lit("endmodule"))[qi::_val = phoenix::construct<storm::ir::Module>(qi::_1, qi::_a, qi::_b, qi::_3)];

			moduleDefinitionList = +moduleDefinition;

			start = (*(definedConstantDefinition | (undefinedBooleanConstantDefinition[phoenix::push_back(qi::_a, qi::_1)] | undefinedIntegerConstantDefinition[phoenix::push_back(qi::_b, qi::_1)] | undefinedDoubleConstantDefinition[phoenix::push_back(qi::_c, qi::_1)])) >> moduleDefinitionList)[qi::_val = phoenix::construct<storm::ir::Program>(storm::ir::Program::ModelType::DTMC, qi::_a, qi::_b, qi::_c, qi::_1)];
		}

		// The starting point of the grammar.
		qi::rule<Iterator, storm::ir::Program(), qi::locals<std::vector<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>,std::vector<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>,std::vector<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>>, ascii::space_type> start;
		qi::rule<Iterator, qi::unused_type(), ascii::space_type> constantDefinitionList;
		qi::rule<Iterator, std::vector<storm::ir::Module>(), ascii::space_type> moduleDefinitionList;

		qi::rule<Iterator, storm::ir::Module(), qi::locals<std::vector<storm::ir::BooleanVariable>, std::vector<storm::ir::IntegerVariable>>, ascii::space_type> moduleDefinition;

		qi::rule<Iterator, qi::unused_type(), ascii::space_type> variableDefinition;
		qi::rule<Iterator, storm::ir::BooleanVariable(), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>>, ascii::space_type> booleanVariableDefinition;
		qi::rule<Iterator, storm::ir::IntegerVariable(), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>>, ascii::space_type> integerVariableDefinition;

		qi::rule<Iterator, storm::ir::Command(), ascii::space_type> commandDefinition;

		qi::rule<Iterator, std::vector<storm::ir::Update>(), ascii::space_type> updateListDefinition;
		qi::rule<Iterator, storm::ir::Update(), ascii::space_type> updateDefinition;
		qi::rule<Iterator, std::vector<storm::ir::Assignment>(), ascii::space_type> assignmentDefinitionList;
		qi::rule<Iterator, storm::ir::Assignment(), ascii::space_type> assignmentDefinition;

		qi::rule<Iterator, std::string(), ascii::space_type> integerVariableName;
		qi::rule<Iterator, std::string(), ascii::space_type> booleanVariableName;

		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> constantDefinition;
		qi::rule<Iterator, qi::unused_type(), ascii::space_type> undefinedConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> definedConstantDefinition;
		qi::rule<Iterator, qi::unused_type(), qi::locals<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, ascii::space_type> undefinedBooleanConstantDefinition;
		qi::rule<Iterator, qi::unused_type(), qi::locals<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, ascii::space_type> undefinedIntegerConstantDefinition;
		qi::rule<Iterator, qi::unused_type(), qi::locals<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, ascii::space_type> undefinedDoubleConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> definedBooleanConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> definedIntegerConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> definedDoubleConstantDefinition;

		qi::rule<Iterator, std::string(), ascii::space_type> freeIdentifierName;

		// The starting point for arbitrary expressions.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> expression;

		// Rules with boolean result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> booleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> orExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> andExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> atomicBooleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> relativeExpression;

		// Rules with integer result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerPlusExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerMultExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> atomicIntegerExpression;

		// Rules with double result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> doubleExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> doublePlusExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> doubleMultExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> atomicDoubleExpression;

		// Rules for variable recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> variableExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> booleanVariableExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerVariableExpression;

		// Rules for constant recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> constantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> booleanConstantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerConstantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> doubleConstantExpression;

		// Rules for literal recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> literalExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> booleanLiteralExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> integerLiteralExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), ascii::space_type> doubleLiteralExpression;

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

		struct variablesStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::BaseExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariables_, booleanVariables_, allVariables_;

		struct variableNamesStruct : qi::symbols<char, std::string> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariableNames_, booleanVariableNames_;

		struct booleanVariableTypesStruct : qi::symbols<char, storm::ir::BooleanVariable> {
			// Intentionally left empty. This map is filled during parsing.
		} booleanVariableInfo_;

		struct integerVariableTypesStruct : qi::symbols<char, storm::ir::IntegerVariable> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariableInfo_;

		struct constantsStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::BaseExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} integerConstants_, booleanConstants_, doubleConstants_, allConstants_;

		struct undefinedBooleanConstantsTypesStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} booleanConstantInfo_;

		struct undefinedIntegerConstantsTypesStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} integerConstantInfo_;

		struct undefinedDoubleConstantsTypesStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} doubleConstantInfo_;

		struct modulesStruct : qi::symbols<char, unsigned> {
			// Intentionally left empty. This map is filled during parsing.
		} modules_;

		storm::ir::Program& program;
	};
};

} // namespace parser

} // namespace storm

#endif /* PRISMPARSER_H_ */
