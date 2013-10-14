/*
 * PrismGrammar.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <limits>

#include "PrismGrammar.h"

#include "src/utility/OsDetection.h"

#include "src/parser/prismparser/Includes.h"
#include "src/parser/prismparser/BooleanExpressionGrammar.h"
#include "src/parser/prismparser/ConstBooleanExpressionGrammar.h"
#include "src/parser/prismparser/ConstDoubleExpressionGrammar.h"
#include "src/parser/prismparser/ConstIntegerExpressionGrammar.h"
#include "src/parser/prismparser/IntegerExpressionGrammar.h"
#include "src/parser/prismparser/IdentifierGrammars.h"
#include "src/parser/prismparser/VariableState.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

// Some typedefs and namespace definitions to reduce code size.
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace storm {
    namespace parser {
        namespace prism {
            
            void PrismGrammar::addLabel(std::string const& name, std::shared_ptr<BaseExpression> const& value, std::map<std::string, std::unique_ptr<BaseExpression>>& nameToExpressionMap) {
                this->state->labelNames_.add(name, name);
                nameToExpressionMap[name] = value->clone();
            }
            
            void PrismGrammar::addIntegerAssignment(std::string const& variable, std::shared_ptr<BaseExpression> const& value, std::map<std::string, Assignment>& variableToAssignmentMap) {
                this->state->assignedIntegerVariables_.add(variable, variable);
                variableToAssignmentMap[variable] = Assignment(variable, value->clone());
            }
            
            void PrismGrammar::addBooleanAssignment(std::string const& variable, std::shared_ptr<BaseExpression> const& value, std::map<std::string, Assignment>& variableToAssigmentMap) {
                this->state->assignedBooleanVariables_.add(variable, variable);
                variableToAssigmentMap[variable] = Assignment(variable, value->clone());
            }
            
            void PrismGrammar::addUndefinedBooleanConstant(std::string const& name, std::map<std::string, std::unique_ptr<BooleanConstantExpression>>& nameToExpressionMap) {
                this->state->booleanConstants_.add(name, std::shared_ptr<BaseExpression>(new BooleanConstantExpression(name)));
                this->state->allConstantNames_.add(name, name);
                nameToExpressionMap.emplace(name, std::unique_ptr<BooleanConstantExpression>(new BooleanConstantExpression(dynamic_cast<BooleanConstantExpression&>(*this->state->booleanConstants_.at(name)))));
            }
            
            void PrismGrammar::addUndefinedIntegerConstant(std::string const& name, std::map<std::string, std::unique_ptr<IntegerConstantExpression>>& nameToExpressionMap) {
                this->state->integerConstants_.add(name, std::shared_ptr<BaseExpression>(new IntegerConstantExpression(name)));
                this->state->allConstantNames_.add(name, name);
                nameToExpressionMap.emplace(name, std::unique_ptr<IntegerConstantExpression>(new IntegerConstantExpression(dynamic_cast<IntegerConstantExpression&>(*this->state->integerConstants_.at(name)))));
            }
            
            void PrismGrammar::addUndefinedDoubleConstant(std::string const& name, std::map<std::string, std::unique_ptr<DoubleConstantExpression>>& nameToExpressionMap) {
                this->state->doubleConstants_.add(name, std::shared_ptr<BaseExpression>(new DoubleConstantExpression(name)));
                this->state->allConstantNames_.add(name, name);
                nameToExpressionMap.emplace(name, std::unique_ptr<DoubleConstantExpression>(new DoubleConstantExpression(dynamic_cast<DoubleConstantExpression&>(*this->state->doubleConstants_.at(name)))));
            }
            
            Module PrismGrammar::renameModule(std::string const& newName, std::string const& oldName, std::map<std::string, std::string> const& renaming) {
                this->state->moduleNames_.add(newName, newName);
                Module* old = this->moduleMap_.find(oldName);
                if (old == nullptr) {
                    LOG4CPLUS_ERROR(logger, "Renaming module failed: module " << oldName << " does not exist.");
                    throw storm::exceptions::InvalidArgumentException() << "Renaming module failed: module " << oldName << " does not exist.";
                }
                Module res(*old, newName, renaming, *this->state);
                this->moduleMap_.at(newName) = res;
                return res;
            }
            
            Module PrismGrammar::createModule(std::string const& name, std::vector<BooleanVariable> const& bools, std::vector<IntegerVariable> const& ints, std::map<std::string, uint_fast64_t> const& boolids, std::map<std::string, uint_fast64_t> const& intids, std::vector<storm::ir::Command> const& commands) {
                this->state->moduleNames_.add(name, name);
                Module res(name, bools, ints, boolids, intids, commands);
                this->moduleMap_.at(name) = res;
                return res;
            }
            
            void PrismGrammar::createIntegerVariable(std::string const& name, std::shared_ptr<BaseExpression> const& lower, std::shared_ptr<BaseExpression> const& upper, std::shared_ptr<BaseExpression> const& init, std::vector<IntegerVariable>& vars, std::map<std::string, uint_fast64_t>& varids, bool isGlobalVariable) {
                uint_fast64_t id = this->state->addIntegerVariable(name);
                uint_fast64_t newLocalIndex = this->state->nextLocalIntegerVariableIndex;
                vars.emplace_back(newLocalIndex, id, name, lower != nullptr ? lower->clone() : nullptr, upper != nullptr ? upper->clone() : nullptr, init != nullptr ? init->clone() : nullptr);
                varids[name] = newLocalIndex;
                ++this->state->nextLocalIntegerVariableIndex;
                this->state->localIntegerVariables_.add(name, name);
                if (isGlobalVariable) {
                    this->state->globalIntegerVariables_.add(name, name);
                }
            }
            
            void PrismGrammar::createBooleanVariable(std::string const& name, std::shared_ptr<BaseExpression> const& init, std::vector<BooleanVariable>& vars, std::map<std::string, uint_fast64_t>& varids, bool isGlobalVariable) {
                uint_fast64_t id = this->state->addBooleanVariable(name);
                uint_fast64_t newLocalIndex = this->state->nextLocalBooleanVariableIndex;
                vars.emplace_back(newLocalIndex, id, name, init != nullptr ? init->clone() : nullptr);
                varids[name] = newLocalIndex;
                ++this->state->nextLocalBooleanVariableIndex;
                this->state->localBooleanVariables_.add(name, name);
                if (isGlobalVariable) {
                    this->state->globalBooleanVariables_.add(name, name);
                }
            }
            
            StateReward createStateReward(std::shared_ptr<BaseExpression> const& guard, std::shared_ptr<BaseExpression> const& reward) {
                return StateReward(guard->clone(), reward->clone());
            }
            TransitionReward createTransitionReward(std::string const& label, std::shared_ptr<BaseExpression> const& guard, std::shared_ptr<BaseExpression> const& reward) {
                return TransitionReward(label, guard->clone(), reward->clone());
            }
            void createRewardModel(std::string const& name, std::vector<StateReward>& stateRewards, std::vector<TransitionReward>& transitionRewards, std::map<std::string, RewardModel>& mapping) {
                mapping[name] = RewardModel(name, stateRewards, transitionRewards);
            }
            Update PrismGrammar::createUpdate(std::shared_ptr<BaseExpression> const& likelihood, std::map<std::string, Assignment> const& bools, std::map<std::string, Assignment> const& ints) {
                this->state->nextGlobalUpdateIndex++;
                return Update(this->state->getNextGlobalUpdateIndex() - 1, likelihood != nullptr ? likelihood->clone() : nullptr, bools, ints);
            }
            Command PrismGrammar::createCommand(std::string const& label, std::shared_ptr<BaseExpression> const& guard, std::vector<Update> const& updates) {
                this->state->nextGlobalCommandIndex++;
                return Command(this->state->getNextGlobalCommandIndex() - 1, label, guard->clone(), updates);
            }
            Program createProgram(
                                  Program::ModelType modelType,
                                  std::map<std::string, std::unique_ptr<BooleanConstantExpression>> const& undefBoolConst,
                                  std::map<std::string, std::unique_ptr<IntegerConstantExpression>> const& undefIntConst,
                                  std::map<std::string, std::unique_ptr<DoubleConstantExpression>> const& undefDoubleConst,
                                  GlobalVariableInformation const& globalVariableInformation,
                                  std::vector<Module> const& modules,
                                  std::map<std::string, RewardModel> const& rewards,
                                  std::map<std::string, std::unique_ptr<BaseExpression>> const& labels) {
                return Program(modelType, undefBoolConst, undefIntConst, undefDoubleConst,
                               globalVariableInformation.booleanVariables, globalVariableInformation.integerVariables,
                               globalVariableInformation.booleanVariableToIndexMap,
                               globalVariableInformation.integerVariableToIndexMap, modules, rewards, labels);
            }
            
            PrismGrammar::PrismGrammar() : PrismGrammar::base_type(start), state(new VariableState()) {
                
                labelDefinition = (qi::lit("label") >> -qi::lit("\"") >> FreeIdentifierGrammar::instance(this->state) >> -qi::lit("\"") >> qi::lit("=") >> BooleanExpressionGrammar::instance(this->state) >> qi::lit(";"))
                [phoenix::bind(&PrismGrammar::addLabel, this, qi::_1, qi::_2, qi::_r1)];
                labelDefinition.name("label declaration");
                labelDefinitionList %= *labelDefinition(qi::_r1);
                labelDefinitionList.name("label declaration list");
                
                // This block defines all entities that are needed for parsing a reward model.
                stateRewardDefinition = (BooleanExpressionGrammar::instance(this->state) > qi::lit(":") > ConstDoubleExpressionGrammar::instance(this->state) >> qi::lit(";"))[qi::_val = phoenix::bind(&createStateReward, qi::_1, qi::_2)];
                stateRewardDefinition.name("state reward definition");
                transitionRewardDefinition = (qi::lit("[") > -(commandName[qi::_a = qi::_1]) > qi::lit("]") > BooleanExpressionGrammar::instance(this->state) > qi::lit(":") > ConstDoubleExpressionGrammar::instance(this->state) > qi::lit(";"))[qi::_val = phoenix::bind(&createTransitionReward, qi::_a, qi::_2, qi::_3)];
                transitionRewardDefinition.name("transition reward definition");
                rewardDefinition = (qi::lit("rewards") > qi::lit("\"") > FreeIdentifierGrammar::instance(this->state) > qi::lit("\"") > +(stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)] | transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> qi::lit("endrewards"))
                [phoenix::bind(&createRewardModel, qi::_1, qi::_a, qi::_b, qi::_r1)];
                rewardDefinition.name("reward definition");
                rewardDefinitionList = *rewardDefinition(qi::_r1);
                rewardDefinitionList.name("reward definition list");
                
                commandName %= this->state->commandNames_;
                commandName.name("command name");
                unassignedLocalBooleanVariableName %= (this->state->localBooleanVariables_ | this->state->globalBooleanVariables_) - this->state->assignedBooleanVariables_;
                unassignedLocalBooleanVariableName.name("unassigned local/global boolean variable");
                unassignedLocalIntegerVariableName %= (this->state->localIntegerVariables_ | this->state->globalIntegerVariables_) - this->state->assignedIntegerVariables_;
                unassignedLocalIntegerVariableName.name("unassigned local/global integer variable");
                
                // This block defines all entities that are needed for parsing a single command.
                assignmentDefinition =
                (qi::lit("(") >> unassignedLocalIntegerVariableName > qi::lit("'") > qi::lit("=") > IntegerExpressionGrammar::instance(this->state) > qi::lit(")"))[phoenix::bind(&PrismGrammar::addIntegerAssignment, this, qi::_1, qi::_2, qi::_r2)] |
                (qi::lit("(") >> unassignedLocalBooleanVariableName > qi::lit("'") > qi::lit("=") > BooleanExpressionGrammar::instance(this->state) > qi::lit(")"))[phoenix::bind(&PrismGrammar::addBooleanAssignment, this, qi::_1, qi::_2, qi::_r1)];
                assignmentDefinition.name("assignment");
                assignmentDefinitionList = assignmentDefinition(qi::_r1, qi::_r2) % "&";
                assignmentDefinitionList.name("assignment list");
                updateDefinition = ((ConstDoubleExpressionGrammar::instance(this->state) | qi::attr(phoenix::construct<std::shared_ptr<BaseExpression>>(phoenix::new_<storm::ir::expressions::DoubleLiteralExpression>(1)))) > qi::lit(":")[phoenix::clear(phoenix::ref(this->state->assignedBooleanVariables_)), phoenix::clear(phoenix::ref(this->state->assignedIntegerVariables_))] > assignmentDefinitionList(qi::_a, qi::_b))[qi::_val = phoenix::bind(&PrismGrammar::createUpdate, this, qi::_1, qi::_a, qi::_b)];
                updateDefinition.name("update");
                updateListDefinition = +updateDefinition % "+";
                updateListDefinition.name("update list");
                commandDefinition = (
                                     qi::lit("[") > -(
                                                      (FreeIdentifierGrammar::instance(this->state)[phoenix::bind(this->state->commandNames_.add, qi::_1, qi::_1)] | commandName)[qi::_a = qi::_1]
                                                      ) > qi::lit("]") > BooleanExpressionGrammar::instance(this->state) > qi::lit("->") > updateListDefinition > qi::lit(";")
                                     )[qi::_val = phoenix::bind(&PrismGrammar::createCommand, this, qi::_a, qi::_2, qi::_3)];
                commandDefinition.name("command");
                
                // This block defines all entities that are needed for parsing variable definitions.
                booleanVariableDefinition = (FreeIdentifierGrammar::instance(this->state) >> qi::lit(":") >> qi::lit("bool") > -(qi::lit("init") > ConstBooleanExpressionGrammar::instance(this->state)[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
                [phoenix::bind(&PrismGrammar::createBooleanVariable, this, qi::_1, qi::_b, qi::_r1, qi::_r2, qi::_r3)];
                booleanVariableDefinition.name("boolean variable declaration");
                
                integerVariableDefinition = (FreeIdentifierGrammar::instance(this->state) >> qi::lit(":") >> qi::lit("[") > ConstIntegerExpressionGrammar::instance(this->state) > qi::lit("..") > ConstIntegerExpressionGrammar::instance(this->state) > qi::lit("]") > -(qi::lit("init") > ConstIntegerExpressionGrammar::instance(this->state)[qi::_b = phoenix::construct<std::shared_ptr<BaseExpression>>(qi::_1)]) > qi::lit(";"))
                [phoenix::bind(&PrismGrammar::createIntegerVariable, this, qi::_1, qi::_2, qi::_3, qi::_b, qi::_r1, qi::_r2, qi::_r3)];
                integerVariableDefinition.name("integer variable declaration");
                variableDefinition = (booleanVariableDefinition(qi::_r1, qi::_r3, false) | integerVariableDefinition(qi::_r2, qi::_r4, false));
                variableDefinition.name("variable declaration");
                
                // This block defines all entities that are needed for parsing a module.
                moduleDefinition = (qi::lit("module") >> FreeIdentifierGrammar::instance(this->state)[phoenix::bind(&VariableState::clearLocalVariables, *this->state)]
                                    >> *(variableDefinition(qi::_a, qi::_b, qi::_c, qi::_d)) >> +commandDefinition > qi::lit("endmodule"))
                [qi::_val = phoenix::bind(&PrismGrammar::createModule, this, qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2)];
                
                moduleDefinition.name("module");
                moduleRenaming = (qi::lit("module")	>> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=")
                                  > this->state->moduleNames_ > qi::lit("[") > *(
                                                                                 (IdentifierGrammar::instance(this->state) > qi::lit("=") > IdentifierGrammar::instance(this->state) >> -qi::lit(","))[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))]
                                                                                 ) > qi::lit("]") > qi::lit("endmodule"))
                [qi::_val = phoenix::bind(&PrismGrammar::renameModule, this, qi::_1, qi::_2, qi::_a)];
                moduleRenaming.name("renamed module");
                moduleDefinitionList %= +(moduleDefinition | moduleRenaming);
                moduleDefinitionList.name("module list");
                
                // This block defines all entities that are needed for parsing global variable definitions.
                globalVariableDefinitionList = *(qi::lit("global") > (booleanVariableDefinition(bind(&GlobalVariableInformation::booleanVariables, qi::_r1), bind(&GlobalVariableInformation::booleanVariableToIndexMap, qi::_r1), true) | integerVariableDefinition(bind(&GlobalVariableInformation::integerVariables, qi::_r1), bind(&GlobalVariableInformation::integerVariableToIndexMap, qi::_r1), true)));
                globalVariableDefinitionList.name("global variable declaration list");
                
                // This block defines all entities that are needed for parsing constant definitions.
                definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") > ConstBooleanExpressionGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
                definedBooleanConstantDefinition.name("defined boolean constant declaration");
                definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> ConstIntegerExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->integerConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
                definedIntegerConstantDefinition.name("defined integer constant declaration");
                definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") > ConstDoubleExpressionGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(this->state->doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
                definedDoubleConstantDefinition.name("defined double constant declaration");
                undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") > FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(&PrismGrammar::addUndefinedBooleanConstant, this, qi::_1, qi::_r1)];
                undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
                undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") > FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(&PrismGrammar::addUndefinedIntegerConstant, this, qi::_1, qi::_r1)]; 
                undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
                undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") > FreeIdentifierGrammar::instance(this->state) > qi::lit(";"))[phoenix::bind(&PrismGrammar::addUndefinedDoubleConstant, this, qi::_1, qi::_r1)];
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
                         globalVariableDefinitionList(qi::_d) >
                         moduleDefinitionList >
                         rewardDefinitionList(qi::_e) >
                         labelDefinitionList(qi::_f))[qi::_val = phoenix::bind(&createProgram, qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2, qi::_e, qi::_f)];
                start.name("probabilistic program declaration");
            }
            
            void PrismGrammar::prepareForSecondRun() {
                LOG4CPLUS_INFO(logger, "Preparing parser for second run.");
                this->state->prepareForSecondRun();
                BooleanExpressionGrammar::secondRun();
                ConstBooleanExpressionGrammar::secondRun();
                ConstDoubleExpressionGrammar::secondRun();
                ConstIntegerExpressionGrammar::secondRun();
                IntegerExpressionGrammar::secondRun();
            }
            
            void PrismGrammar::resetGrammars() {
                LOG4CPLUS_INFO(logger, "Resetting grammars.");
                BooleanExpressionGrammar::resetInstance();
                ConstBooleanExpressionGrammar::resetInstance();
                ConstDoubleExpressionGrammar::resetInstance();
                ConstIntegerExpressionGrammar::resetInstance();
                IntegerExpressionGrammar::resetInstance();
            }
            
        } // namespace prism
    } // namespace parser
} // namespace storm
