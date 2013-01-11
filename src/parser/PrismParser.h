/*
 * PrismParser.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Christian Dehnert
 */

#ifndef PRISMPARSER_H_
#define PRISMPARSER_H_

#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include "src/ir/IR.h"

#include <istream>
#include <fstream>
#include <iomanip>

namespace storm {

namespace parser {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

typedef std::istreambuf_iterator<char> base_iterator_type;
typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
typedef boost::spirit::classic::position_iterator2<forward_iterator_type> pos_iterator_type;

class PrismParser {

public:
	void test(std::string const& fileName) {
		std::ifstream inputFileStream(fileName, std::ios::in);
		getProgram(inputFileStream, fileName);
		inputFileStream.close();
	}

	void getProgram(std::istream& inputStream, std::string const& fileName) {
		base_iterator_type in_begin(inputStream);

		forward_iterator_type fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
		forward_iterator_type fwd_end;

		pos_iterator_type position_begin(fwd_begin, fwd_end, fileName);
		pos_iterator_type position_end;

		storm::ir::Program result;
		prismGrammar<pos_iterator_type,  BOOST_TYPEOF(ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol)> grammar;
		try {
			phrase_parse(position_begin, position_end, grammar, ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
		} catch(const qi::expectation_failure<pos_iterator_type>& e) {
			const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();
			std::stringstream msg;
			msg << pos.file << ", line " << pos.line << ", column " << pos.column << ": parse error: expected " << e.what_ << std::endl << "\t" << e.first.get_currentline() << std::endl << "\t";
			int i = 0;
			for (i = 0; i < pos.column; ++i) {
				msg << "-";
			}
			msg << "^";
			for (; i < 80; ++i) {
				msg << "-";
			}
			msg << std::endl;
			std::cout << msg.str();
			throw storm::exceptions::WrongFileFormatException() << msg.str();
		}

		std::cout << result.toString();
	}

private:
	template<typename Iterator, typename Skipper>
	struct prismGrammar : qi::grammar<Iterator, storm::ir::Program(), qi::locals<std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, std::map<std::string, storm::ir::RewardModel>, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>, Skipper> {

		prismGrammar() : prismGrammar::base_type(start) {
			freeIdentifierName %= qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_'))) - booleanVariables_ - integerVariables_ - allConstants_]];
			freeIdentifierName.name("unused identifier");

			booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BooleanLiteral>(qi::_1))];
			booleanLiteralExpression.name("boolean literal");
			integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::IntegerLiteral>(qi::_1))];
			integerLiteralExpression.name("integer literal");
			doubleLiteralExpression = qi::double_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::DoubleLiteral>(qi::_1))];
			doubleLiteralExpression.name("double literal");
			literalExpression %= (booleanLiteralExpression | integerLiteralExpression | doubleLiteralExpression);
			literalExpression.name("literal");

			integerVariableExpression %= integerVariables_;
			integerVariableExpression.name("integer variable");
			booleanVariableExpression %= booleanVariables_;
			booleanVariableExpression.name("boolean variable");
			variableExpression %= (integerVariableExpression | booleanVariableExpression);
			variableExpression.name("variable");

			booleanConstantExpression %= (booleanConstants_ | booleanLiteralExpression);
			booleanConstantExpression.name("boolean constant or literal");
			integerConstantExpression %= (integerConstants_ | integerLiteralExpression);
			integerConstantExpression.name("integer constant or literal");
			doubleConstantExpression %= (doubleConstants_ | doubleLiteralExpression);
			doubleConstantExpression.name("double constant or literal");
			constantExpression %= (booleanConstantExpression | integerConstantExpression | doubleConstantExpression);
			constantExpression.name("constant or literal");

			atomicIntegerExpression %= (integerVariableExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerConstantExpression);
			atomicIntegerExpression.name("integer expression");
			integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
			integerMultExpression.name("integer expression");
			integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> integerMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
			integerPlusExpression.name("integer expression");
			integerExpression %= integerPlusExpression;
			integerExpression.name("integer expression");

			constantAtomicIntegerExpression %= (qi::lit("(") >> constantIntegerExpression >> qi::lit(")") | integerConstantExpression);
			constantAtomicIntegerExpression.name("constant integer expression");
			constantIntegerMultExpression %= constantAtomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
			constantIntegerMultExpression.name("constant integer expression");
			constantIntegerPlusExpression = constantIntegerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> constantIntegerMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
			constantIntegerPlusExpression.name("constant integer expression");
			constantIntegerExpression %= constantIntegerPlusExpression;
			constantIntegerExpression.name("constant integer expression");

			// This block defines all expressions of type double that are by syntax constant. That is, they are evaluable given the values for all constants.
			constantAtomicDoubleExpression %= (qi::lit("(") >> constantDoubleExpression >> qi::lit(")") | doubleConstantExpression);
			constantAtomicDoubleExpression.name("constant double expression");
			constantDoubleMultExpression %= constantAtomicDoubleExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicDoubleExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
			constantDoubleMultExpression.name("constant double expression");
			constantDoublePlusExpression %= constantDoubleMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> constantDoubleMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
			constantDoublePlusExpression.name("constant double expression");
			constantDoubleExpression %= constantDoublePlusExpression;
			constantDoubleExpression.name("constant double expression");

			// This block defines all expressions of type boolean.
			relativeExpression = (integerExpression >> relations_ >> integerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
			relativeExpression.name("boolean expression");
			atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | booleanConstantExpression);
			atomicBooleanExpression.name("boolean expression");
			notExpression = atomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::UnaryBooleanFunctionExpression>>(phoenix::new_<storm::ir::expressions::UnaryBooleanFunctionExpression>(qi::_1, storm::ir::expressions::UnaryBooleanFunctionExpression::FunctorType::NOT))];
			notExpression.name("boolean expression");
			andExpression = notExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> notExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND))];
			andExpression.name("boolean expression");
			orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR))];
			orExpression.name("boolean expression");
			booleanExpression %= orExpression;
			booleanExpression.name("boolean expression");

			// This block defines all expressions of type boolean that are by syntax constant. That is, they are evaluable given the values for all constants.
			constantRelativeExpression = (constantIntegerExpression >> relations_ >> constantIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
			constantRelativeExpression.name("constant boolean expression");
			constantAtomicBooleanExpression %= (constantRelativeExpression | qi::lit("(") >> constantBooleanExpression >> qi::lit(")") | booleanLiteralExpression | booleanConstantExpression);
			constantAtomicBooleanExpression.name("constant boolean expression");
			constantNotExpression = constantAtomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> constantAtomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::UnaryBooleanFunctionExpression>>(phoenix::new_<storm::ir::expressions::UnaryBooleanFunctionExpression>(qi::_1, storm::ir::expressions::UnaryBooleanFunctionExpression::FunctorType::NOT))];
			constantNotExpression.name("constant boolean expression");
			constantAndExpression = constantNotExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> constantNotExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND))];
			constantAndExpression.name("constant boolean expression");
			constantOrExpression = constantAndExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> constantAndExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR))];
			constantOrExpression.name("constant boolean expression");
			constantBooleanExpression %= constantOrExpression;
			constantBooleanExpression.name("constant boolean expression");

			// This block defines the general root of all expressions. Most of the time, however, you may want to start with a more specialized rule.
			expression %= (booleanExpression | integerExpression | constantDoubleExpression);
			expression.name("expression");

			// This block defines all entities that are needed for parsing labels.
			labelDefinition = (qi::lit("label") >> freeIdentifierName >> qi::lit("=") >> booleanExpression >> qi::lit(";"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>(qi::_1, qi::_2))];
			labelDefinitionList %= *labelDefinition(qi::_r1);

			// This block defines all entities that are needed for parsing a reward model.
			stateRewardDefinition = (booleanExpression > qi::lit(":") > constantDoubleExpression >> qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::StateReward>(qi::_1, qi::_2)];
			stateRewardDefinition.name("state reward definition");
			transitionRewardDefinition = (qi::lit("[") > -(commandName[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit(":") > constantDoubleExpression > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::TransitionReward>(qi::_a, qi::_2, qi::_3)];
			transitionRewardDefinition.name("transition reward definition");
			rewardDefinition = (qi::lit("rewards") > qi::lit("\"") > freeIdentifierName > qi::lit("\"") > +(stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)] | transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> qi::lit("endrewards"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, storm::ir::RewardModel>>(qi::_1, phoenix::construct<storm::ir::RewardModel>(qi::_1, qi::_a, qi::_b)))];
			rewardDefinition.name("reward definition");
			rewardDefinitionList = *rewardDefinition(qi::_r1);
			rewardDefinitionList.name("reward definition list");

			// This block defines auxiliary entities that are used to check whether a certain variable exist.
			booleanVariableName %= booleanVariableNames_;
			booleanVariableName.name("boolean variable");
			integerVariableName %= integerVariableNames_;
			integerVariableName.name("integer variable");
			commandName %= commandNames_;
			commandName.name("command name");

			// This block defines all entities that are needed for parsing a single command.
			assignmentDefinition = (qi::lit("(") >> integerVariableName > qi::lit("'") > qi::lit("=") > integerExpression > qi::lit(")"))[qi::_val = phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)] | (qi::lit("(") > booleanVariableName > qi::lit("'") > qi::lit("=") > booleanExpression > qi::lit(")"))[qi::_val = phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)];
			assignmentDefinition.name("assignment");
			assignmentDefinitionList %= assignmentDefinition % "&";
			assignmentDefinitionList.name("assignment list");
			updateDefinition = (constantDoubleExpression > qi::lit(":") > assignmentDefinitionList)[qi::_val = phoenix::construct<storm::ir::Update>(qi::_1, qi::_2)];
			updateDefinition.name("update");
			updateListDefinition = +updateDefinition % "+";
			updateListDefinition.name("update list");
			commandDefinition = (qi::lit("[") > -((freeIdentifierName[phoenix::bind(commandNames_.add, qi::_1, qi::_1)] | commandName)[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit("->") > updateListDefinition > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::Command>(qi::_a, qi::_2, qi::_3)];
			commandDefinition.name("command");

			// This block defines all entities that are neede for parsing variable definitions.
			booleanVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("bool") > -(qi::lit("init") > constantBooleanExpression[qi::_b = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(qi::_1)]) > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::BooleanVariable>(phoenix::val("hallo"), qi::_b), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), std::cout << phoenix::val("here!"), phoenix::bind(booleanVariables_.add, qi::_1, qi::_a), phoenix::bind(booleanVariableNames_.add, qi::_1, qi::_1)];
			booleanVariableDefinition.name("boolean variable declaration");
			integerVariableDefinition = (freeIdentifierName > qi::lit(":") > qi::lit("[") > constantIntegerExpression > qi::lit("..") > constantIntegerExpression > qi::lit("]") > -(qi::lit("init") > constantIntegerExpression[qi::_b = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(qi::_1)]) > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::IntegerVariable>(qi::_1, qi::_2, qi::_3, qi::_b), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(qi::_1)), phoenix::bind(integerVariables_.add, qi::_1, qi::_a), phoenix::bind(integerVariableNames_.add, qi::_1, qi::_1)];
			integerVariableDefinition.name("integer variable declaration");
			variableDefinition = (booleanVariableDefinition | integerVariableDefinition);
			variableDefinition.name("variable declaration");

			// This block defines all entities that are needed for parsing a module.
			moduleDefinition = (qi::lit("module") > freeIdentifierName > *(booleanVariableDefinition[phoenix::push_back(qi::_a, qi::_1)] | integerVariableDefinition[phoenix::push_back(qi::_b, qi::_1)]) > +commandDefinition > qi::lit("endmodule"))[qi::_val = phoenix::construct<storm::ir::Module>(qi::_1, qi::_a, qi::_b, qi::_3)];
			moduleDefinition.name("module");
			moduleDefinitionList %= +moduleDefinition;
			moduleDefinitionList.name("module list");

			// This block defines all entities that are needed for parsing constant definitions.
			definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit("=") > booleanLiteralExpression > qi::lit(";"))[phoenix::bind(booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedBooleanConstantDefinition.name("defined boolean constant declaration");
			definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit("=") > integerLiteralExpression > qi::lit(";"))[phoenix::bind(integerConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedIntegerConstantDefinition.name("defined integer constant declaration");
			definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit("=") > doubleLiteralExpression > qi::lit(";"))[phoenix::bind(doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstants_.add, qi::_1, qi::_2), qi::_val = qi::_2];
			definedDoubleConstantDefinition.name("defined double constant declaration");
			undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>(phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(booleanConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(booleanConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
			undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>(phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(integerConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(integerConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
			undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>(phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(doubleConstantInfo_.add, qi::_1, qi::_a), phoenix::bind(doubleConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstants_.add, qi::_1, qi::_a)];
			undefinedDoubleConstantDefinition.name("undefined double constant declaration");
			definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
			definedConstantDefinition.name("defined constant declaration");
			undefinedConstantDefinition = (undefinedBooleanConstantDefinition(qi::_r1) | undefinedIntegerConstantDefinition(qi::_r2) | undefinedDoubleConstantDefinition(qi::_r3));
			undefinedConstantDefinition.name("undefined constant declaration");
			constantDefinitionList = *(definedConstantDefinition | undefinedConstantDefinition(qi::_r1, qi::_r2, qi::_r3));
			constantDefinitionList.name("constant declaration list");

			// This block defines all entities that are needed for parsing a program.
			modelTypeDefinition = modelType_;
			modelTypeDefinition.name("model type");
			start = (modelTypeDefinition > constantDefinitionList(qi::_a, qi::_b, qi::_c) > moduleDefinitionList > rewardDefinitionList(qi::_d) > labelDefinitionList(qi::_e))[qi::_val = phoenix::construct<storm::ir::Program>(qi::_1, qi::_a, qi::_b, qi::_c, qi::_2, qi::_d, qi::_e)];
			start.name("probabilistic program declaration");
		}

		// The starting point of the grammar.
		qi::rule<Iterator, storm::ir::Program(), qi::locals<std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, std::map<std::string, storm::ir::RewardModel>, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>, Skipper> start;
		qi::rule<Iterator, storm::ir::Program::ModelType(), Skipper> modelTypeDefinition;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), Skipper> constantDefinitionList;
		qi::rule<Iterator, std::vector<storm::ir::Module>(), Skipper> moduleDefinitionList;

		qi::rule<Iterator, storm::ir::Module(), qi::locals<std::vector<storm::ir::BooleanVariable>, std::vector<storm::ir::IntegerVariable>>, Skipper> moduleDefinition;

		qi::rule<Iterator, qi::unused_type(), Skipper> variableDefinition;
		qi::rule<Iterator, storm::ir::BooleanVariable(), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>, std::shared_ptr<storm::ir::expressions::BaseExpression>>, Skipper> booleanVariableDefinition;
		qi::rule<Iterator, storm::ir::IntegerVariable(), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>, std::shared_ptr<storm::ir::expressions::BaseExpression>>, Skipper> integerVariableDefinition;

		qi::rule<Iterator, storm::ir::Command(), qi::locals<std::string>, Skipper> commandDefinition;

		qi::rule<Iterator, std::vector<storm::ir::Update>(), Skipper> updateListDefinition;
		qi::rule<Iterator, storm::ir::Update(), Skipper> updateDefinition;
		qi::rule<Iterator, std::vector<storm::ir::Assignment>(), Skipper> assignmentDefinitionList;
		qi::rule<Iterator, storm::ir::Assignment(), Skipper> assignmentDefinition;

		qi::rule<Iterator, std::string(), Skipper> integerVariableName;
		qi::rule<Iterator, std::string(), Skipper> booleanVariableName;
		qi::rule<Iterator, std::string(), Skipper> commandName;

		qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::RewardModel>&), Skipper> rewardDefinitionList;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::RewardModel>&), qi::locals<std::vector<storm::ir::StateReward>, std::vector<storm::ir::TransitionReward>>, Skipper> rewardDefinition;
		qi::rule<Iterator, storm::ir::StateReward(), Skipper> stateRewardDefinition;
		qi::rule<Iterator, storm::ir::TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;

		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>&), Skipper> labelDefinitionList;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>&), Skipper> labelDefinition;

		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDefinition;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), Skipper> undefinedConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedConstantDefinition;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, Skipper> undefinedBooleanConstantDefinition;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, Skipper> undefinedIntegerConstantDefinition;
		qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, Skipper> undefinedDoubleConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedBooleanConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedIntegerConstantDefinition;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedDoubleConstantDefinition;

		qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
		qi::rule<Iterator, std::string(), Skipper> identifierName;

		// The starting point for arbitrary expressions.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> expression;

		// Rules with boolean result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> orExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> andExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> notExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> atomicBooleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> relativeExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantBooleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantOrExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAndExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantNotExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicBooleanExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantRelativeExpression;

		// Rules with integer result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerPlusExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerMultExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> atomicIntegerExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerPlusExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerMultExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicIntegerExpression;

		// Rules with double result type.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoubleExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoublePlusExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoubleMultExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicDoubleExpression;

		// Rules for variable recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> variableExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanVariableExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerVariableExpression;

		// Rules for constant recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanConstantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerConstantExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> doubleConstantExpression;

		// Rules for literal recognition.
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> literalExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanLiteralExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerLiteralExpression;
		qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> doubleLiteralExpression;

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

		struct modelTypeStruct : qi::symbols<char, storm::ir::Program::ModelType> {
			modelTypeStruct() {
				add
					("dtmc", storm::ir::Program::ModelType::DTMC)
					("ctmc", storm::ir::Program::ModelType::CTMC)
					("mdp", storm::ir::Program::ModelType::MDP)
					("ctmdp", storm::ir::Program::ModelType::CTMDP)
				;
			}
		} modelType_;

		struct relationalOperatorStruct : qi::symbols<char, storm::ir::expressions::BinaryRelationExpression::RelationType> {
			relationalOperatorStruct() {
				add
					("=", storm::ir::expressions::BinaryRelationExpression::EQUAL)
					("<", storm::ir::expressions::BinaryRelationExpression::LESS)
					("<=", storm::ir::expressions::BinaryRelationExpression::LESS_OR_EQUAL)
					(">", storm::ir::expressions::BinaryRelationExpression::GREATER)
					(">=", storm::ir::expressions::BinaryRelationExpression::GREATER_OR_EQUAL)
				;
			}
		} relations_;

		struct variablesStruct : qi::symbols<char, std::shared_ptr<storm::ir::expressions::BaseExpression>> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariables_, booleanVariables_;

		struct entityNamesStruct : qi::symbols<char, std::string> {
			// Intentionally left empty. This map is filled during parsing.
		} integerVariableNames_, booleanVariableNames_, commandNames_;

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
	};
};

} // namespace parser

} // namespace storm

#endif /* PRISMPARSER_H_ */
