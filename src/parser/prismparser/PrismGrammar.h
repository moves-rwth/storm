/*
 * File:   PrismGrammar.h
 * Author: nafur
 *
 * Created on April 30, 2013, 5:20 PM
 */

#ifndef STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H
#define	STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H

// All classes of the intermediate representation are used.
#include "src/ir/IR.h"
#include "src/parser/prismparser/Includes.h"
#include "src/parser/prismparser/Tokens.h"
#include "src/parser/prismparser/IdentifierGrammars.h"
#include "src/parser/prismparser/VariableState.h"
#include "src/parser/prismparser/ConstBooleanExpressionGrammar.h"
#include "src/parser/prismparser/ConstDoubleExpressionGrammar.h"
#include "src/parser/prismparser/ConstIntegerExpressionGrammar.h"
#include "src/parser/prismparser/BooleanExpressionGrammar.h"
#include "src/parser/prismparser/IntegerExpressionGrammar.h"

// Used for file input.
#include <istream>
#include <memory>

namespace storm {
    namespace parser {
        namespace prism {
            
            using namespace storm::ir;
            using namespace storm::ir::expressions;
            
            struct GlobalVariableInformation {
                std::vector<BooleanVariable> booleanVariables;
                std::vector<IntegerVariable> integerVariables;
                std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
                std::map<std::string, uint_fast64_t> integerVariableToIndexMap;
            };
            
            /*!
             * The Boost spirit grammar for the PRISM language. Returns the intermediate representation of
             * the input that complies with the PRISM syntax.
             */
            class PrismGrammar : public qi::grammar<
            Iterator,
            Program(),
            qi::locals<
            std::map<std::string, std::shared_ptr<BooleanConstantExpression>>,
            std::map<std::string, std::shared_ptr<IntegerConstantExpression>>,
            std::map<std::string, std::shared_ptr<DoubleConstantExpression>>,
            GlobalVariableInformation,
            std::map<std::string, RewardModel>,
            std::map<std::string, std::shared_ptr<BaseExpression>>
            >,
            Skipper> {
            public:
                /*!
                 * Default constructor that creates an empty and functional grammar.
                 */
                PrismGrammar();
                
                /*!
                 * Puts all sub-grammars into the mode for performing the second run. A two-run model was chosen
                 * because modules can involve variables that are only declared afterwards, so the first run
                 * creates all variables and the second one tries to parse the full model.
                 */
                void prepareForSecondRun();
                
                /*!
                 * Resets all sub-grammars, i.e. puts them into an initial state.
                 */
                void resetGrammars();
                
            private:
                
                std::shared_ptr<storm::parser::prism::VariableState> state;
                struct qi::symbols<char, Module> moduleMap_;
                
                // The starting point of the grammar.
                qi::rule<
                Iterator,
                Program(),
                qi::locals<
				std::map<std::string, std::shared_ptr<BooleanConstantExpression>>,
				std::map<std::string, std::shared_ptr<IntegerConstantExpression>>,
				std::map<std::string, std::shared_ptr<DoubleConstantExpression>>,
                GlobalVariableInformation,
				std::map<std::string, RewardModel>,
				std::map<std::string, std::shared_ptr<BaseExpression>>
                >,
                Skipper> start;
                qi::rule<Iterator, Program::ModelType(), Skipper> modelTypeDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), Skipper> constantDefinitionList;
                qi::rule<Iterator, std::vector<Module>(), Skipper> moduleDefinitionList;
                
                // Rules for global variable definitions
                qi::rule<Iterator, qi::unused_type(GlobalVariableInformation&), Skipper> globalVariableDefinitionList;
                
                // Rules for module definition.
                qi::rule<Iterator, Module(), qi::locals<std::vector<BooleanVariable>, std::vector<IntegerVariable>, std::map<std::string, uint_fast64_t>, std::map<std::string, uint_fast64_t>>, Skipper> moduleDefinition;
                qi::rule<Iterator, Module(), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;
                
                // Rules for variable definitions.
                qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&, std::map<std::string, uint_fast64_t>&), Skipper> variableDefinition;
                qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::map<std::string, uint_fast64_t>&, bool), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> booleanVariableDefinition;
                qi::rule<Iterator, qi::unused_type(std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&, bool), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> integerVariableDefinition;
                
                // Rules for command definitions.
                qi::rule<Iterator, Command(), qi::locals<std::string>, Skipper> commandDefinition;
                qi::rule<Iterator, std::vector<Update>(), Skipper> updateListDefinition;
                qi::rule<Iterator, Update(), qi::locals<std::map<std::string, Assignment>, std::map<std::string, Assignment>>, Skipper> updateDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinition;
                
                // Rules for variable/command names.
                qi::rule<Iterator, std::string(), Skipper> commandName;
                qi::rule<Iterator, std::string(), Skipper> unassignedLocalBooleanVariableName;
                qi::rule<Iterator, std::string(), Skipper> unassignedLocalIntegerVariableName;
                
                // Rules for reward definitions.
                qi::rule<Iterator, qi::unused_type(std::map<std::string, RewardModel>&), Skipper> rewardDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, RewardModel>&), qi::locals<std::vector<StateReward>, std::vector<TransitionReward>>, Skipper> rewardDefinition;
                qi::rule<Iterator, StateReward(), Skipper> stateRewardDefinition;
                qi::rule<Iterator, TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;
                
                // Rules for label definitions.
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BaseExpression>>&), Skipper> labelDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BaseExpression>>&), Skipper> labelDefinition;
                
                // Rules for constant definitions.
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), Skipper> undefinedConstantDefinition;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&), qi::locals<std::shared_ptr<BooleanConstantExpression>>, Skipper> undefinedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&), qi::locals<std::shared_ptr<IntegerConstantExpression>>, Skipper> undefinedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), qi::locals<std::shared_ptr<DoubleConstantExpression>>, Skipper> undefinedDoubleConstantDefinition;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedBooleanConstantDefinition;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedIntegerConstantDefinition;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedDoubleConstantDefinition;
                
                // Rules for variable recognition.
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanVariableCreatorExpression;
                qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<std::shared_ptr<BaseExpression>>, Skipper> integerVariableCreatorExpression;
                
                storm::parser::prism::keywordsStruct keywords_;
                storm::parser::prism::modelTypeStruct modelType_;
                storm::parser::prism::relationalOperatorStruct relations_;
                
                /*!
                 * Adds a constant of type integer with the given name and value.
                 *
                 * @param name The name of the constant.
                 * @param value An expression definining the value of the constant.
                 */
                std::shared_ptr<BaseExpression> addIntegerConstant(std::string const& name, std::shared_ptr<BaseExpression> const& value);
                
                /*!
                 * Adds a label with the given name and expression to the given label-to-expression map.
                 *
                 * @param name The name of the label.
                 * @param expression The expression associated with the label.
                 * @param nameToExpressionMap The map to which the label is added.
                 */
                void addLabel(std::string const& name, std::shared_ptr<BaseExpression> const& value, std::map<std::string, std::shared_ptr<BaseExpression>>& nameToExpressionMap);
                
                /*!
                 * Adds a boolean assignment for the given variable with the given expression and adds it to the
                 * provided variable-to-assignment map.
                 *
                 * @param variable The name of the variable that the assignment targets.
                 * @param expression The expression that is assigned to the variable.
                 * @param variableToAssignmentMap The map to which the assignment is added.
                 */
                void addBooleanAssignment(std::string const& variable, std::shared_ptr<BaseExpression> const& expression, std::map<std::string, Assignment>& variableToAssignmentMap);
                
                /*!
                 * Adds a boolean assignment for the given variable with the given expression and adds it to the
                 * provided variable-to-assignment map.
                 *
                 * @param variable The name of the variable that the assignment targets.
                 * @param expression The expression that is assigned to the variable.
                 * @param variableToAssignmentMap The map to which the assignment is added.
                 */
                void addIntegerAssignment(std::string const& variable, std::shared_ptr<BaseExpression> const& value, std::map<std::string, Assignment>& variableToAssignmentMap);
                
                /*!
                 * Creates a module by renaming, i.e. takes the module given by the old name, creates a new module
                 * with the given name which renames all identifiers according to the given mapping.
                 *
                 * @param name The name of the new module.
                 * @param oldName The name of the module that is to be copied (modulo renaming).
                 * @param renaming A mapping from identifiers to their new names.
                 */
                Module renameModule(std::string const& name, std::string const& oldName, std::map<std::string, std::string> const& renaming);
                
                /*!
                 * Creates a new module with the given name, boolean and integer variables and commands.
                 *
                 * @param name The name of the module to create.
                 * @param booleanVariables The boolean variables of the module.
                 * @param integerVariables The integer variables of the module.
                 * @param booleanVariableToLocalIndexMap A mapping of boolean variables to module-local indices.
                 * @param integerVariableToLocalIndexMap A mapping of boolean variables to module-local indices.
                 * @param commands The commands associated with this module.
                 */
                Module createModule(std::string const& name, std::vector<BooleanVariable> const& booleanVariables, std::vector<IntegerVariable> const& integerVariables, std::map<std::string, uint_fast64_t> const& booleanVariableToLocalIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToLocalIndexMap, std::vector<storm::ir::Command> const& commands);
                
                /*!
                 * Creates an integer variable with the given name, domain and initial value and adds it to the
                 * provided list of integer variables and the given mappings.
                 *
                 * @param name The name of the integer variable.
                 * @param lower The expression that defines the lower bound of the domain.
                 * @param upper The expression that defines the upper bound of the domain.
                 * @param init The expression that defines the initial value of the variable.
                 * @param integerVariableToGlobalIndexMap A mapping of integer variables to global indices.
                 * @param isGlobalVariable A flag indicating whether the variable is supposed to be global or not.
                 */
                void createIntegerVariable(std::string const& name, std::shared_ptr<BaseExpression> const& lower, std::shared_ptr<BaseExpression> const& upper, std::shared_ptr<BaseExpression> const& init, std::vector<IntegerVariable>& integerVariables, std::map<std::string, uint_fast64_t>& integerVariableToGlobalIndexMap, bool isGlobalVariable);
                
                /*!
                 * Creates an boolean variable with the given name and initial value and adds it to the
                 * provided list of boolean variables and the given mappings.
                 *
                 * @param name The name of the boolean variable.
                 * @param init The expression that defines the initial value of the variable.
                 * @param booleanVariableToGlobalIndexMap A mapping of boolean variables to global indices.
                 * @param isGlobalVariable A flag indicating whether the variable is supposed to be global or not.
                 */
                void createBooleanVariable(std::string const& name, std::shared_ptr<BaseExpression> const& init, std::vector<BooleanVariable>& booleanVariables, std::map<std::string, uint_fast64_t>& booleanVariableToGlobalIndexMap, bool isGlobalVariable);
                
                /*!
                 * Creates a command with the given label, guard and updates.
                 *
                 * @param label The label of the command.
                 * @param guard The guard of the command.
                 * @param updates The updates associated with the command.
                 */
                Command createCommand(std::string const& label, std::shared_ptr<BaseExpression> guard, std::vector<Update> const& updates);
                
                /*!
                 * Creates an update with the given likelihood and the given assignments to boolean and integer variables, respectively.
                 *
                 * @param likelihood The likelihood of this update being executed.
                 * @param booleanAssignments The assignments to boolean variables this update involves.
                 * @param integerAssignments The assignments to integer variables this update involves.
                 */
                Update createUpdate(std::shared_ptr<BaseExpression> likelihood, std::map<std::string, Assignment> const& booleanAssignments, std::map<std::string, Assignment> const& integerAssignments);
                
            };
            
            
        } // namespace prism
    } // namespace parser
} // namespace storm


#endif	/* STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H */

