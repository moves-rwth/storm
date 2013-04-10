/*
 * PrismParser.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

#include "PrismParser.h"
#include "src/utility/OsDetection.h"
#include "src/parser/PrismParser/VariableState.h"
#include "src/parser/PrismParser/IntegerExpressionGrammar.h"
#include "src/parser/PrismParser/ConstIntegerExpressionGrammar.h"
#include "src/parser/PrismParser/BooleanExpressionGrammar.h"
#include "src/parser/PrismParser/ConstBooleanExpressionGrammar.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

// Used for Boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <limits>

// Some typedefs and namespace definitions to reduce code size.
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace storm {

namespace parser {

PrismParser::PrismGrammar::PrismGrammar() : PrismParser::PrismGrammar::base_type(start) {

		this->state = std::shared_ptr<storm::parser::prism::VariableState>(new storm::parser::prism::VariableState());
		storm::parser::prism::IntegerExpressionGrammar intExpr(this->state);
		storm::parser::prism::ConstIntegerExpressionGrammar constIntExpr(this->state);
		storm::parser::prism::BooleanExpressionGrammar boolExpr(this->state);
		storm::parser::prism::ConstBooleanExpressionGrammar constBoolExpr(this->state);

		// This rule defines all identifiers that have not been previously used.
		identifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&storm::parser::prism::VariableState::isIdentifier, this->state.get(), qi::_1) ];
		identifierName.name("identifier");
		freeIdentifierName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][ qi::_pass = phoenix::bind(&storm::parser::prism::VariableState::isFreeIdentifier, this->state.get(), qi::_1) ];
		freeIdentifierName.name("unused identifier");

		// This block defines all literal expressions.
		booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BooleanLiteral>(qi::_1))];
		booleanLiteralExpression.name("boolean literal");
		doubleLiteralExpression = qi::double_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<DoubleLiteral>(qi::_1))];
		doubleLiteralExpression.name("double literal");
		
		
		// This block defines all expressions that are variables.
		std::shared_ptr<BaseExpression> intvarexpr = std::shared_ptr<BaseExpression>(new VariableExpression(BaseExpression::int_, std::numeric_limits<uint_fast64_t>::max(), "int", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
		std::shared_ptr<BaseExpression> boolvarexpr = std::shared_ptr<BaseExpression>(new VariableExpression(BaseExpression::bool_, std::numeric_limits<uint_fast64_t>::max(), "int", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
		integerVariableExpression = identifierName[qi::_val = intvarexpr];
		integerVariableExpression.name("integer variable");
		booleanVariableExpression = identifierName[qi::_val = boolvarexpr];
		booleanVariableExpression.name("boolean variable");

		// This block defines all atomic expressions that are constant, i.e. literals and constants.
		booleanConstantExpression %= (this->state->booleanConstants_ | booleanLiteralExpression);
		booleanConstantExpression.name("boolean constant or literal");
		doubleConstantExpression %= (this->state->doubleConstants_ | doubleLiteralExpression);
		doubleConstantExpression.name("double constant or literal");

		// This block defines all expressions of type double that are by syntax constant. That is, they are evaluable given the values for all constants.
		constantAtomicDoubleExpression %= (qi::lit("(") >> constantDoubleExpression >> qi::lit(")") | doubleConstantExpression);
		constantAtomicDoubleExpression.name("constant double expression");
		constantDoubleMultExpression %= constantAtomicDoubleExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> constantAtomicDoubleExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::double_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::TIMES)) ] .else_ [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::double_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::DIVIDE))]];
		constantDoubleMultExpression.name("constant double expression");
		constantDoublePlusExpression %= constantDoubleMultExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> constantDoubleMultExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::double_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::PLUS)) ] .else_ [qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<BinaryNumericalFunctionExpression>(BaseExpression::double_, qi::_val, qi::_1, BinaryNumericalFunctionExpression::MINUS))]];
		constantDoublePlusExpression.name("constant double expression");
		constantDoubleExpression %= constantDoublePlusExpression;
		constantDoubleExpression.name("constant double expression");

		// This block defines all entities that are needed for parsing labels.
		labelDefinition = (qi::lit("label") >> -qi::lit("\"") >> freeIdentifierName >> -qi::lit("\"") >> qi::lit("=") >> boolExpr >> qi::lit(";"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<BaseExpression>>>(qi::_1, qi::_2)), phoenix::bind(this->state->labelNames_.add, qi::_1, qi::_1)];
		labelDefinition.name("label declaration");
		labelDefinitionList %= *labelDefinition(qi::_r1);
		labelDefinitionList.name("label declaration list");

		// This block defines all entities that are needed for parsing a reward model.
		stateRewardDefinition = (boolExpr > qi::lit(":") > constantDoubleExpression >> qi::lit(";"))[qi::_val = phoenix::construct<StateReward>(qi::_1, qi::_2)];
		stateRewardDefinition.name("state reward definition");
		transitionRewardDefinition = (qi::lit("[") > -(commandName[qi::_a = qi::_1]) > qi::lit("]") > boolExpr > qi::lit(":") > constantDoubleExpression > qi::lit(";"))[qi::_val = phoenix::construct<TransitionReward>(qi::_a, qi::_2, qi::_3)];
		transitionRewardDefinition.name("transition reward definition");
		rewardDefinition = (qi::lit("rewards") > qi::lit("\"") > freeIdentifierName > qi::lit("\"") > +(stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)] | transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> qi::lit("endrewards"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, RewardModel>>(qi::_1, phoenix::construct<RewardModel>(qi::_1, qi::_a, qi::_b)))];
		rewardDefinition.name("reward definition");
		rewardDefinitionList = *rewardDefinition(qi::_r1);
		rewardDefinitionList.name("reward definition list");

		// This block defines auxiliary entities that are used to check whether a certain variable exist.
		booleanVariableName %= this->state->booleanVariableNames_;
		booleanVariableName.name("boolean variable");
		integerVariableName %= this->state->integerVariableNames_;
		integerVariableName.name("integer variable");
		commandName %= this->state->commandNames_;
		commandName.name("command name");
		unassignedLocalBooleanVariableName %= this->state->localBooleanVariables_ - this->state->assignedLocalBooleanVariables_;
		unassignedLocalBooleanVariableName.name("unassigned local boolean variable");
		unassignedLocalIntegerVariableName %= this->state->localIntegerVariables_ - this->state->assignedLocalIntegerVariables_;
		unassignedLocalIntegerVariableName.name("unassigned local integer variable");

		// This block defines all entities that are needed for parsing a single command.
		assignmentDefinition = (qi::lit("(") >> unassignedLocalIntegerVariableName > qi::lit("'") > qi::lit("=") > intExpr > qi::lit(")"))[phoenix::bind(this->state->assignedLocalIntegerVariables_.add, qi::_1, qi::_1), phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, Assignment>>(qi::_1, phoenix::construct<Assignment>(qi::_1, qi::_2)))] | (qi::lit("(") > unassignedLocalBooleanVariableName > qi::lit("'") > qi::lit("=") > boolExpr > qi::lit(")"))[phoenix::bind(this->state->assignedLocalBooleanVariables_.add, qi::_1, qi::_1), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, Assignment>>(qi::_1, phoenix::construct<Assignment>(qi::_1, qi::_2)))];
		assignmentDefinition.name("assignment");
		assignmentDefinitionList = assignmentDefinition(qi::_r1, qi::_r2) % "&";
		assignmentDefinitionList.name("assignment list");
		updateDefinition = (constantDoubleExpression > qi::lit(":")[phoenix::clear(phoenix::ref(this->state->assignedLocalBooleanVariables_)), phoenix::clear(phoenix::ref(this->state->assignedLocalIntegerVariables_))] > assignmentDefinitionList(qi::_a, qi::_b))[qi::_val = phoenix::construct<Update>(qi::_1, qi::_a, qi::_b)];
		updateDefinition.name("update");
		updateListDefinition = +updateDefinition % "+";
		updateListDefinition.name("update list");
		commandDefinition = (qi::lit("[") > -((freeIdentifierName[phoenix::bind(this->state->commandNames_.add, qi::_1, qi::_1)] | commandName)[qi::_a = qi::_1]) > qi::lit("]") > boolExpr > qi::lit("->") > updateListDefinition > qi::lit(";"))[qi::_val = phoenix::construct<Command>(qi::_a, qi::_2, qi::_3)];
		commandDefinition.name("command");

		// This block defines all entities that are needed for parsing variable definitions.
		booleanVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("bool") > -(qi::lit("init") > constBoolExpr[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
			[
				//qi::_a = phoenix::bind(&VariableState<Iterator,Skipper>::addBooleanVariable, *this->state.get(), qi::_1),
				qi::_a = phoenix::bind(&storm::parser::prism::VariableState::addBooleanVariable, *this->state, qi::_1, qi::_b),
				phoenix::push_back(qi::_r1, phoenix::construct<BooleanVariable>(qi::_a, phoenix::val(qi::_1), qi::_b)),
				phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, uint_fast64_t>>(qi::_1, qi::_a)),
				phoenix::bind(this->state->localBooleanVariables_.add, qi::_1, qi::_1)
			];
		booleanVariableDefinition.name("boolean variable declaration");
		integerVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("[") > constIntExpr > qi::lit("..") > constIntExpr > qi::lit("]") > -(qi::lit("init") > constIntExpr[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
			[
				qi::_a = phoenix::bind(&storm::parser::prism::VariableState::addIntegerVariable, *this->state, qi::_1, qi::_2, qi::_3, qi::_b),
				phoenix::push_back(qi::_r1, phoenix::construct<IntegerVariable>(qi::_a, qi::_1, qi::_2, qi::_3, qi::_b)),
				phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, uint_fast64_t>>(qi::_1, qi::_a)),
				phoenix::bind(this->state->localIntegerVariables_.add, qi::_1, qi::_1)
			];
		integerVariableDefinition.name("integer variable declaration");
		variableDefinition = (booleanVariableDefinition(qi::_r1, qi::_r3) | integerVariableDefinition(qi::_r2, qi::_r4));
		variableDefinition.name("variable declaration");

		// This block defines all entities that are needed for parsing a module.
		moduleDefinition = (qi::lit("module") >> freeIdentifierName
				[phoenix::clear(phoenix::ref(this->state->localBooleanVariables_)), phoenix::clear(phoenix::ref(this->state->localIntegerVariables_))]
				>> *(variableDefinition(qi::_a, qi::_b, qi::_c, qi::_d)) >> +commandDefinition > qi::lit("endmodule"))
				[
					phoenix::bind(this->state->moduleNames_.add, qi::_1, qi::_1),
					qi::_val = phoenix::construct<Module>(qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2),
					phoenix::bind(this->state->moduleMap_.add, qi::_1, qi::_val)
				];

		Module const * (qi::symbols<char, Module>::*moduleFinder)(const std::string&) const = &qi::symbols<char, Module>::find;
		moduleDefinition.name("module");
		moduleRenaming = (qi::lit("module")	>> freeIdentifierName >> qi::lit("=")
				> this->state->moduleNames_ > qi::lit("[") > *(
						(identifierName > qi::lit("=") > identifierName >> -qi::lit(","))[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))]
				) > qi::lit("]") > qi::lit("endmodule"))
				[
					phoenix::bind(this->state->moduleNames_.add, qi::_1, qi::_1),
					qi::_val = phoenix::construct<Module>(*phoenix::bind(moduleFinder, this->state->moduleMap_, qi::_2), qi::_1, qi::_a, this->state),
					phoenix::bind(this->state->moduleMap_.add, qi::_1, qi::_val)

				];
		moduleRenaming.name("renamed module");
		moduleDefinitionList %= +(moduleDefinition | moduleRenaming);
		moduleDefinitionList.name("module list");
		
		integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<IntegerLiteral>(qi::_1))];
		integerLiteralExpression.name("integer literal");
		// This block defines all entities that are needed for parsing constant definitions.
		definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit("=") > booleanLiteralExpression > qi::lit(";"))[phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedBooleanConstantDefinition.name("defined boolean constant declaration");
		definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit("=") > integerLiteralExpression > qi::lit(";"))[phoenix::bind(this->state->integerConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedIntegerConstantDefinition.name("defined integer constant declaration");
		definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit("=") > doubleLiteralExpression > qi::lit(";"))[phoenix::bind(this->state->doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedDoubleConstantDefinition.name("defined double constant declaration");
		undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<BooleanConstantExpression>>(phoenix::new_<BooleanConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<BooleanConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
		undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
		undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<IntegerConstantExpression>>(phoenix::new_<IntegerConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<IntegerConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->integerConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
		undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
		undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<DoubleConstantExpression>>(phoenix::new_<DoubleConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<DoubleConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->doubleConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
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
		start = (modelTypeDefinition > constantDefinitionList(qi::_a, qi::_b, qi::_c) > moduleDefinitionList > rewardDefinitionList(qi::_d) > labelDefinitionList(qi::_e))[qi::_val = phoenix::construct<Program>(qi::_1, qi::_a, qi::_b, qi::_c, qi::_2, qi::_d, qi::_e)];
		start.name("probabilistic program declaration");
	}
	
	void PrismParser::PrismGrammar::prepareForSecondRun() {
		// Clear constants.
		storm::parser::prism::IntegerExpressionGrammar intExpr(this->state);
		storm::parser::prism::ConstIntegerExpressionGrammar constIntExpr(this->state);

		this->state->prepareForSecondRun();
		
		// Override variable expressions: only allow declared variables.
		integerVariableExpression %= this->state->integerVariables_;
		integerVariableExpression.name("integer variable");
		booleanVariableExpression %= this->state->booleanVariables_;
		booleanVariableExpression.name("boolean variable");
	}

/*!
 * Opens the given file for parsing, invokes the helper function to parse the actual content and
 * closes the file properly, even if an exception is thrown in the parser. In this case, the
 * exception is passed on to the caller.
 */
std::shared_ptr<storm::ir::Program> PrismParser::parseFile(std::string const& filename) const {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);
	std::shared_ptr<storm::ir::Program> result(nullptr);

	// Now try to parse the contents of the file.
	try {
		result = std::shared_ptr<storm::ir::Program>(parse(inputFileStream, filename));
	} catch(std::exception& e) {
		// In case of an exception properly close the file before passing exception.
		inputFileStream.close();
		throw e;
	}

	// Close the stream in case everything went smoothly and return result.
	inputFileStream.close();
	return result;
}

/*!
 * Passes iterators to the input stream to the Boost spirit parser and thereby parses the input.
 * If the parser throws an expectation failure exception, i.e. expected input different than the one
 * provided, this is caught and displayed properly before the exception is passed on.
 */
std::shared_ptr<storm::ir::Program> PrismParser::parse(std::istream& inputStream, std::string const& filename) const {
	// Prepare iterators to input.
	// TODO: Right now, this parses the whole contents of the file into a string first.
	// While this is usually not necessary, because there exist adapters that make an input stream
	// iterable in both directions without storing it into a string, using the corresponding
	// Boost classes gives an awful output under valgrind and is thus disabled for the time being.
	std::string fileContent((std::istreambuf_iterator<char>(inputStream)), (std::istreambuf_iterator<char>()));
	BaseIteratorType stringIteratorBegin = fileContent.begin();
	BaseIteratorType stringIteratorEnd = fileContent.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, filename);
	PositionIteratorType positionIteratorBegin2(stringIteratorBegin, stringIteratorEnd, filename);
	PositionIteratorType positionIteratorEnd;

	// Prepare resulting intermediate representation of input.
	std::shared_ptr<storm::ir::Program> result(new storm::ir::Program());

	// In order to instantiate the grammar, we have to pass the type of the skipping parser.
	// As this is more complex, we let Boost figure out the actual type for us.
	PrismGrammar grammar;
	try {
		// Now parse the content using phrase_parse in order to be able to supply a skipping parser.
		// First run.
		qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, *result);
		grammar.prepareForSecondRun();
		result = std::shared_ptr<storm::ir::Program>(new storm::ir::Program());
		std::cout << "Now we start the second run..." << std::endl;
		// Second run.
		qi::phrase_parse(positionIteratorBegin2, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, *result);
		std::cout << "Here is the parsed grammar: " << std::endl << result->toString() << std::endl;
	} catch(const qi::expectation_failure<PositionIteratorType>& e) {
		// If the parser expected content different than the one provided, display information
		// about the location of the error.
		const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();

		// Construct the error message including a caret display of the position in the
		// erroneous line.
		std::stringstream msg;
		std::string line = e.first.get_currentline();
		while (line.find('\t') != std::string::npos) line.replace(line.find('\t'),1," ");
		msg << pos.file << ", line " << pos.line << ", column " << pos.column
				<< ": parse error: expected " << e.what_ << std::endl << "\t"
				<< line << std::endl << "\t";
		int i = 0;
		for (i = 1; i < pos.column; ++i) {
			msg << "-";
		}
		msg << "^";
		for (; i < 80; ++i) {
			msg << "-";
		}
		msg << std::endl;

		std::cerr << msg.str();

		// Now propagate exception.
		throw storm::exceptions::WrongFileFormatException() << msg.str();
	}

	return result;
}

} // namespace parser

} // namespace storm
