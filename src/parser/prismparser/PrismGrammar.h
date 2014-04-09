#ifndef STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_
#define	STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_

// Include files for file input.
#include <istream>
#include <memory>

// Include boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
typedef PositionIteratorType Iterator;
typedef BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol) Skipper;
typedef BOOST_TYPEOF(qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol | boost::spirit::ascii::space) Skipper2;
typedef boost::spirit::unused_type Unused;

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace parser {
        namespace prism {
            
            struct modelTypeStruct : qi::symbols<char, storm::prism::Program::ModelType> {
                modelTypeStruct() {
                    add
                    ("dtmc", storm::prism::Program::ModelType::DTMC)
                    ("ctmc", storm::prism::Program::ModelType::CTMC)
                    ("mdp", storm::prism::Program::ModelType::MDP)
                    ("ctmdp", storm::prism::Program::ModelType::CTMDP)
                    ("ma", storm::prism::Program::ModelType::MA);
                }
            };
            
            struct keywordsStruct : qi::symbols<char, unsigned> {
                keywordsStruct() {
                    add
                    ("dtmc", 1)
                    ("ctmc", 2)
                    ("mdp", 3)
                    ("ctmdp", 4)
                    ("ma", 5)
                    ("const", 6)
                    ("int", 7)
                    ("bool", 8)
                    ("module", 9)
                    ("endmodule", 10)
                    ("rewards", 11)
                    ("endrewards", 12)
                    ("true", 13)
                    ("false", 14);
                }
            };
            
            class GlobalProgramInformation {
            public:
                // Default construct the header information.
                GlobalProgramInformation() = default;
                
                // Members for all essential information that needs to be collected.
                storm::prism::Program::ModelType modelType;
                std::set<std::string> undefinedBooleanConstants;
                std::set<std::string> undefinedIntegerConstants;
                std::set<std::string> undefinedDoubleConstants;
                std::map<std::string, storm::expressions::Expression> definedBooleanConstants;
                std::map<std::string, storm::expressions::Expression> definedIntegerConstants;
                std::map<std::string, storm::expressions::Expression> definedDoubleConstants;
                std::map<std::string, storm::expressions::Expression> formulas;
                std::map<std::string, storm::prism::BooleanVariable> globalBooleanVariables;
                std::map<std::string, storm::prism::IntegerVariable> globalIntegerVariables;
                std::map<std::string, storm::prism::Module> modules;
                std::map<std::string, storm::prism::RewardModel> rewardModels;
                std::map<std::string, storm::expressions::Expression> labels;
                
                // Counters to provide unique indexing for commands and updates.
                uint_fast64_t currentCommandIndex;
                uint_fast64_t currentUpdateIndex;
            };
            
            class PrismGrammar : public qi::grammar<Iterator, storm::prism::Program(), qi::locals<GlobalProgramInformation>, Skipper> {
            public:
                /*!
                 * Default constructor that creates an empty and functional grammar.
                 */
                PrismGrammar();
                
            private:
                // The starting point of the grammar.
                qi::rule<Iterator, storm::prism::Program(), qi::locals<GlobalProgramInformation>, Skipper> start;
                
                // Rules for model type.
                qi::rule<Iterator, storm::prism::Program::ModelType(), Skipper> modelTypeDefinition;
                
                // Rules for parsing the program header.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> programHeader;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedDoubleConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedDoubleConstantDefinition;
                
                // Rules for global variable definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalVariableDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalBooleanVariableDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalIntegerVariableDefinition;
                
                // Rules for modules definition.
                qi::rule<Iterator, std::vector<storm::prism::Module>(GlobalProgramInformation&), Skipper> moduleDefinitionList;
                qi::rule<Iterator, storm::prism::Module(GlobalProgramInformation&), qi::locals<std::map<std::string, storm::prism::BooleanVariable>, std::map<std::string, storm::prism::IntegerVariable>>, Skipper> moduleDefinition;
                qi::rule<Iterator, storm::prism::Module(GlobalProgramInformation&), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;
                
                // Rules for variable definitions.
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::prism::BooleanVariable>&, std::map<std::string, storm::prism::IntegerVariable>&), Skipper> variableDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::prism::BooleanVariable>&), Skipper> booleanVariableDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::prism::IntegerVariable>&), qi::locals<storm::expressions::Expression>, Skipper> integerVariableDefinition;
                
                // Rules for command definitions.
                qi::rule<Iterator, storm::prism::Command(), qi::locals<std::string>, Skipper> commandDefinition;
                qi::rule<Iterator, std::vector<storm::prism::Update>(), Skipper> updateListDefinition;
                qi::rule<Iterator, storm::prism::Update(), qi::locals<std::map<std::string, storm::prism::Assignment>>, Skipper> updateDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::prism::Assignment>&), Skipper> assignmentDefinitionList;
                qi::rule<Iterator, storm::prism::Assignment(std::map<std::string, storm::prism::Assignment>&), Skipper> assignmentDefinition;
                
                // Rules for reward definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), qi::locals<std::vector<storm::prism::StateReward>, std::vector<storm::prism::TransitionReward>>, Skipper> rewardModelDefinition;
                qi::rule<Iterator, storm::prism::StateReward(), Skipper> stateRewardDefinition;
                qi::rule<Iterator, storm::prism::TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;
                
                // Rules for label definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> labelDefinition;
                
                // Rules for formula definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> formulaDefinition;
                
                // Rules for identifier parsing.
                qi::rule<Iterator, std::string(), Skipper> identifier;
                
                // Rules for parsing a composed expression.
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> expression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> booleanExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> orExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> andExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> notExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicBooleanExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> relativeExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> booleanVariableExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> numericalExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> plusExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> multiplicationExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicNumericalExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> numericalVariableExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> minMaxExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> floorCeilExpression;
                
                // Parsers that recognize special keywords and model types.
                storm::parser::prism::keywordsStruct keywords_;
                storm::parser::prism::modelTypeStruct modelType_;
                
                // Helper methods that add data to data structures.
                static bool addUndefinedBooleanConstant(std::set<std::string>& undefinedBooleanConstants, std::string const& newUndefinedBooleanConstant);
                static bool addUndefinedIntegerConstant(std::set<std::string>& undefinedIntegerConstants, std::string const& newUndefinedIntegerConstant);
                static bool addUndefinedDoubleConstant(std::set<std::string>& undefinedDoubleConstants, std::string const& newUndefinedDoubleConstant);
                
                static bool addDefinedBooleanConstant(std::map<std::string, storm::expressions::Expression>& definedBooleanConstants, std::string const& newDefinedBooleanConstant, storm::expressions::Expression expression);
                static bool addDefinedIntegerConstant(std::map<std::string, storm::expressions::Expression>& definedIntegerConstants, std::string const& newDefinedIntegerConstant, storm::expressions::Expression expression);
                static bool addDefinedDoubleConstant(std::map<std::string, storm::expressions::Expression>& definedDoubleConstants, std::string const& newDefinedDoubleConstant, storm::expressions::Expression expression);

                static bool addFormula(std::map<std::string, storm::expressions::Expression>& formulas, std::string const& formulaName, storm::expressions::Expression expression);
                
                static storm::prism::RewardModel addRewardModel(std::map<std::string, storm::prism::RewardModel>& rewardModels, std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards);
                static storm::prism::StateReward createStateReward(storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression);
                static storm::prism::TransitionReward createTransitionReward(std::string const& actionName, storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression);
                
                static bool addLabel(std::map<std::string, storm::expressions::Expression>& labels, std::string const& labelName, storm::expressions::Expression expression);
                
                static bool addAssignment(std::map<std::string, storm::prism::Assignment>& assignments, std::string const& variableName, storm::expressions::Expression assignedExpression);
                static storm::prism::Update createUpdate(storm::expressions::Expression likelihoodExpression, std::map<std::string, storm::prism::Assignment>& assignments);
                static storm::prism::Command createCommand(std::string const& actionName, storm::expressions::Expression guardExpression, std::vector<storm::prism::Update> const& updates);
                static bool addBooleanVariable(std::map<std::string, storm::prism::BooleanVariable>& booleanVariables, std::string const& variableName, storm::expressions::Expression initialValueExpression);
                static bool addIntegerVariable(std::map<std::string, storm::prism::IntegerVariable>& integerVariables, std::string const& variableName, storm::expressions::Expression lowerBoundExpression, storm::expressions::Expression upperBoundExpression, storm::expressions::Expression initialValueExpression);
                static storm::prism::Module createModule(std::string const& moduleName, std::map<std::string, storm::prism::BooleanVariable> const& booleanVariables, std::map<std::string, storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands);
                static storm::prism::Module createRenamedModule(std::string const& newModuleName, std::string const& oldModuleName, std::map<std::string, std::string> const& renaming, GlobalProgramInformation const& globalProgramInformation);
                static storm::prism::Program createProgram(GlobalProgramInformation const& globalProgramInformation, std::vector<storm::prism::Module> const& modules);
            };
        } // namespace prism
    } // namespace parser
} // namespace storm


#endif	/* STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_ */

