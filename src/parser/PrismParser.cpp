/*
 * PrismParser.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

#include "PrismParser.h"

#include "src/utility/OsDetection.h"

#include "src/parser/PrismParser/Includes.h"
#include "src/parser/PrismParser/BooleanExpressionGrammar.h"
#include "src/parser/PrismParser/ConstBooleanExpressionGrammar.h"
#include "src/parser/PrismParser/ConstDoubleExpressionGrammar.h"
#include "src/parser/PrismParser/ConstIntegerExpressionGrammar.h"
#include "src/parser/PrismParser/IntegerExpressionGrammar.h"
#include "src/parser/PrismParser/IdentifierGrammars.h"
#include "src/parser/PrismParser/VariableState.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

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

	void dump(const std::string& s) {
		std::cerr << "Dump: " << s << std::endl;
	}

	std::shared_ptr<BaseExpression> PrismParser::PrismGrammar::addIntegerConstant(const std::string& name, const std::shared_ptr<BaseExpression> value) {
		this->state->integerConstants_.add(name, value);
		this->state->allConstantNames_.add(name, name);
		return value;
	}

	void PrismParser::PrismGrammar::addLabel(const std::string& name, std::shared_ptr<BaseExpression> value, std::map<std::string, std::shared_ptr<BaseExpression>>& mapping) {
		this->state->labelNames_.add(name, name);
		mapping[name] = value;
	}
	void PrismParser::PrismGrammar::addIntAssignment(const std::string& variable, std::shared_ptr<BaseExpression> value, std::map<std::string, Assignment>& mapping) {
		this->state->assignedLocalIntegerVariables_.add(variable, variable);
		mapping[variable] = Assignment(variable, value);
	}
	void PrismParser::PrismGrammar::addBoolAssignment(const std::string& variable, std::shared_ptr<BaseExpression> value, std::map<std::string, Assignment>& mapping) {
		this->state->assignedLocalBooleanVariables_.add(variable, variable);
		mapping[variable] = Assignment(variable, value);
	}
	Module PrismParser::PrismGrammar::renameModule(const std::string& name, const std::string& oldname, std::map<std::string, std::string>& mapping) {
		this->state->moduleNames_.add(name, name);
		Module* old = this->state->moduleMap_.find(oldname);
		if (old == nullptr) {
			std::cerr << "Renaming module failed: module " << oldname << " does not exist!" << std::endl;
			throw "Renaming module failed";
		}
		Module res(*old, name, mapping, this->state);
		this->state->moduleMap_.add(name, res);
		return res;
	}
	Module PrismParser::PrismGrammar::createModule(const std::string name, std::vector<BooleanVariable>& bools, std::vector<IntegerVariable>& ints, std::map<std::string, uint_fast64_t>& boolids, std::map<std::string, uint_fast64_t> intids, std::vector<storm::ir::Command> commands) {
		this->state->moduleNames_.add(name, name);
		Module res(name, bools, ints, boolids, intids, commands);
		this->state->moduleMap_.add(name, res);
		return res;
	}

	StateReward createStateReward(std::shared_ptr<BaseExpression> guard, std::shared_ptr<BaseExpression> reward) {
		return StateReward(guard, reward);
	}
	TransitionReward createTransitionReward(std::string label, std::shared_ptr<BaseExpression> guard, std::shared_ptr<BaseExpression> reward) {
		return TransitionReward(label, guard, reward);
	}
	void createRewardModel(std::string name, std::vector<StateReward>& stateRewards, std::vector<TransitionReward>& transitionRewards, std::map<std::string, RewardModel>& mapping) {
		mapping[name] = RewardModel(name, stateRewards, transitionRewards);
	}
	Update createUpdate(std::shared_ptr<BaseExpression> likelihood, std::map<std::string, Assignment>& bools, std::map<std::string, Assignment> ints) {
		return Update(likelihood, bools, ints);
	}
	Command createCommand(std::string& label, std::shared_ptr<BaseExpression> guard, std::vector<Update>& updates) {
		return Command(label, guard, updates);
	}
	Program createProgram(
			Program::ModelType modelType,
			std::map<std::string, std::shared_ptr<BooleanConstantExpression>> undefBoolConst,
			std::map<std::string, std::shared_ptr<IntegerConstantExpression>> undefIntConst,
			std::map<std::string, std::shared_ptr<DoubleConstantExpression>> undefDoubleConst,
			std::vector<Module> modules,
			std::map<std::string, RewardModel> rewards,
			std::map<std::string, std::shared_ptr<BaseExpression>> labels) {
		return Program(modelType, undefBoolConst, undefIntConst, undefDoubleConst, modules, rewards, labels);
	}

PrismParser::PrismGrammar::PrismGrammar() : PrismParser::PrismGrammar::base_type(start), state(new storm::parser::prism::VariableState()) {
		
		labelDefinition = (qi::lit("label") >> -qi::lit("\"") >> prism::FreeIdentifierGrammar::instance(this->state) >> -qi::lit("\"") >> qi::lit("=") >> prism::BooleanExpressionGrammar::instance(this->state) >> qi::lit(";"))
				[phoenix::bind(&PrismParser::PrismGrammar::addLabel, this, qi::_1, qi::_2, qi::_r1)];
		labelDefinition.name("label declaration");
		labelDefinitionList %= *labelDefinition(qi::_r1);
		labelDefinitionList.name("label declaration list");

		// This block defines all entities that are needed for parsing a reward model.
		stateRewardDefinition = (prism::BooleanExpressionGrammar::instance(this->state) > qi::lit(":") > prism::ConstDoubleExpressionGrammar::instance(this->state) >> qi::lit(";"))[qi::_val = phoenix::bind(&createStateReward, qi::_1, qi::_2)];
		stateRewardDefinition.name("state reward definition");
		transitionRewardDefinition = (qi::lit("[") > -(commandName[qi::_a = qi::_1]) > qi::lit("]") > prism::BooleanExpressionGrammar::instance(this->state) > qi::lit(":") > prism::ConstDoubleExpressionGrammar::instance(this->state) > qi::lit(";"))[qi::_val = phoenix::bind(&createTransitionReward, qi::_a, qi::_2, qi::_3)];
		transitionRewardDefinition.name("transition reward definition");
		rewardDefinition = (qi::lit("rewards") > qi::lit("\"") > prism::FreeIdentifierGrammar::instance(this->state) > qi::lit("\"") > +(stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)] | transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> qi::lit("endrewards"))
				[phoenix::bind(&createRewardModel, qi::_1, qi::_a, qi::_b, qi::_r1)];
		rewardDefinition.name("reward definition");
		rewardDefinitionList = *rewardDefinition(qi::_r1);
		rewardDefinitionList.name("reward definition list");

		commandName %= this->state->commandNames_;
		commandName.name("command name");
		unassignedLocalBooleanVariableName %= this->state->localBooleanVariables_ - this->state->assignedLocalBooleanVariables_;
		unassignedLocalBooleanVariableName.name("unassigned local boolean variable");
		unassignedLocalIntegerVariableName %= this->state->localIntegerVariables_ - this->state->assignedLocalIntegerVariables_;
		unassignedLocalIntegerVariableName.name("unassigned local integer variable");

		// This block defines all entities that are needed for parsing a single command.
		assignmentDefinition = 
				(qi::lit("(") >> unassignedLocalIntegerVariableName > qi::lit("'") > qi::lit("=") > prism::IntegerExpressionGrammar::instance(this->state) > qi::lit(")"))[phoenix::bind(&PrismParser::PrismGrammar::addIntAssignment, this, qi::_1, qi::_2, qi::_r2)] |
				(qi::lit("(") >> unassignedLocalBooleanVariableName > qi::lit("'") > qi::lit("=") > prism::BooleanExpressionGrammar::instance(this->state) > qi::lit(")"))[phoenix::bind(&PrismParser::PrismGrammar::addBoolAssignment, this, qi::_1, qi::_2, qi::_r1)];
		assignmentDefinition.name("assignment");
		assignmentDefinitionList = assignmentDefinition(qi::_r1, qi::_r2) % "&";
		assignmentDefinitionList.name("assignment list");
		updateDefinition = (prism::ConstDoubleExpressionGrammar::instance(this->state) > qi::lit(":")[phoenix::clear(phoenix::ref(this->state->assignedLocalBooleanVariables_)), phoenix::clear(phoenix::ref(this->state->assignedLocalIntegerVariables_))] > assignmentDefinitionList(qi::_a, qi::_b))[qi::_val = phoenix::bind(&createUpdate, qi::_1, qi::_a, qi::_b)];
		updateDefinition.name("update");
		updateListDefinition = +updateDefinition % "+";
		updateListDefinition.name("update list");
		commandDefinition = (
					qi::lit("[") > -(
						(prism::FreeIdentifierGrammar::instance(this->state)[phoenix::bind(this->state->commandNames_.add, qi::_1, qi::_1)] | commandName)[qi::_a = qi::_1]
					) > qi::lit("]") > prism::BooleanExpressionGrammar::instance(this->state) > qi::lit("->") > updateListDefinition > qi::lit(";")
				)[qi::_val = phoenix::bind(&createCommand, qi::_a, qi::_2, qi::_3)];
		commandDefinition.name("command");

		// This block defines all entities that are needed for parsing variable definitions.
		booleanVariableDefinition = (prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit(":") >> qi::lit("bool") > -(qi::lit("init") > prism::ConstBooleanExpressionGrammar::instance(this->state)[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
			[
				//qi::_a = phoenix::bind(&VariableState<Iterator,Skipper>::addBooleanVariable, *this->state.get(), qi::_1),
				qi::_a = phoenix::bind(&storm::parser::prism::VariableState::addBooleanVariable, *this->state, qi::_1, qi::_b),
				phoenix::push_back(qi::_r1, phoenix::construct<BooleanVariable>(qi::_a, phoenix::val(qi::_1), qi::_b)),
				phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, uint_fast64_t>>(qi::_1, qi::_a)),
				phoenix::bind(this->state->localBooleanVariables_.add, qi::_1, qi::_1)
			];
		booleanVariableDefinition.name("boolean variable declaration");

		integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<IntegerLiteral>(qi::_1))];
		integerLiteralExpression.name("integer literal");
		integerVariableDefinition = (prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit(":") >> qi::lit("[") > integerLiteralExpression > qi::lit("..") > integerLiteralExpression > qi::lit("]") > -(qi::lit("init") > prism::ConstIntegerExpressionGrammar::instance(this->state)[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
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
		moduleDefinition = (qi::lit("module") >> prism::FreeIdentifierGrammar::instance(this->state)[phoenix::bind(&prism::VariableState::startModule, *this->state)]
				>> *(variableDefinition(qi::_a, qi::_b, qi::_c, qi::_d)) >> +commandDefinition > qi::lit("endmodule"))
				[qi::_val = phoenix::bind(&PrismParser::PrismGrammar::createModule, this, qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2)];

		moduleDefinition.name("module");
		moduleRenaming = (qi::lit("module")	>> prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit("=")
				> this->state->moduleNames_ > qi::lit("[") > *(
						(prism::IdentifierGrammar::instance(this->state) > qi::lit("=") > prism::IdentifierGrammar::instance(this->state) >> -qi::lit(","))[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))]
				) > qi::lit("]") > qi::lit("endmodule"))
				[qi::_val = phoenix::bind(&PrismParser::PrismGrammar::renameModule, this, qi::_1, qi::_2, qi::_a)];
		moduleRenaming.name("renamed module");
		moduleDefinitionList %= +(moduleDefinition | moduleRenaming);
		moduleDefinitionList.name("module list");
		
		// This block defines all entities that are needed for parsing constant definitions.
		definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") > prism::ConstBooleanExpressionGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedBooleanConstantDefinition.name("defined boolean constant declaration");
		definedIntegerConstantDefinition = (
				qi::lit("const") >> qi::lit("int") >> prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >>
				prism::ConstIntegerExpressionGrammar::instance(this->state) >> qi::lit(";")
			)[ qi::_val = phoenix::bind(&PrismParser::PrismGrammar::addIntegerConstant, this, qi::_1, qi::_2) ];
		definedIntegerConstantDefinition.name("defined integer constant declaration");
		definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> prism::FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") > prism::ConstDoubleExpressionGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(this->state->doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedDoubleConstantDefinition.name("defined double constant declaration");
		undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") > prism::FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<BooleanConstantExpression>>(phoenix::new_<BooleanConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<BooleanConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
		undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
		undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") > prism::FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<IntegerConstantExpression>>(phoenix::new_<IntegerConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<IntegerConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->integerConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
		undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
		undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") > prism::FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<DoubleConstantExpression>>(phoenix::new_<DoubleConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<DoubleConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(this->state->doubleConstants_.add, qi::_1, qi::_a), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1)];
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
		start = (
				modelTypeDefinition >
				constantDefinitionList(qi::_a, qi::_b, qi::_c) >
				moduleDefinitionList >
				rewardDefinitionList(qi::_d) >
				labelDefinitionList(qi::_e)
			)[qi::_val = phoenix::bind(&createProgram, qi::_1, qi::_a, qi::_b, qi::_c, qi::_2, qi::_d, qi::_e)];
		start.name("probabilistic program declaration");
	}
	
	void PrismParser::PrismGrammar::prepareForSecondRun() {
		this->state->prepareForSecondRun();
	}

/*!
 * Opens the given file for parsing, invokes the helper function to parse the actual content and
 * closes the file properly, even if an exception is thrown in the parser. In this case, the
 * exception is passed on to the caller.
 */
storm::ir::Program PrismParser::parseFile(std::string const& filename) const {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);
	storm::ir::Program result;

	// Now try to parse the contents of the file.
	try {
		result = parse(inputFileStream, filename);
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
storm::ir::Program PrismParser::parse(std::istream& inputStream, std::string const& filename) const {
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
	storm::ir::Program result;

	// In order to instantiate the grammar, we have to pass the type of the skipping parser.
	// As this is more complex, we let Boost figure out the actual type for us.
	PrismGrammar grammar;
	try {
		// Now parse the content using phrase_parse in order to be able to supply a skipping parser.
		// First run.
		qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
		grammar.prepareForSecondRun();
		result = storm::ir::Program();
		std::cout << "Now we start the second run..." << std::endl;
		// Second run.
		qi::phrase_parse(positionIteratorBegin2, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
		std::cout << "Here is the parsed grammar: " << std::endl << result.toString() << std::endl;
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
