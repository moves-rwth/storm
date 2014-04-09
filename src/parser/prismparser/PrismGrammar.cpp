#include "src/parser/prismparser/PrismGrammar.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace parser {
        namespace prism {
            PrismGrammar::PrismGrammar() : PrismGrammar::base_type(start) {
                // Parse simple identifier.
                identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]] - keywords_;
                identifier.name("identifier");
                
                // Parse a composed expression.
                expression %= (booleanExpression | numericalExpression);
                expression.name("expression");
                
                booleanExpression %= orExpression;
                expression.name("boolean expression");
                
                orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = qi::_val * qi::_1];
                orExpression.name("boolean expression");
                
                andExpression = notExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> notExpression)[qi::_val = qi::_val * qi::_1];
                andExpression.name("boolean expression");
                
                notExpression = atomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicBooleanExpression)[qi::_val = !qi::_1];
                notExpression.name("boolean expression");
                
                atomicBooleanExpression %= relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")");
                atomicBooleanExpression.name("boolean expression");
                
                relativeExpression = ((numericalExpression >> ">") > numericalExpression)[qi::_val = qi::_1 > qi::_2] | ((numericalExpression >> ">=") > numericalExpression)[qi::_val = qi::_1 >= qi::_2] | ((numericalExpression >> "<") > numericalExpression)[qi::_val = qi::_1 < qi::_2] | ((numericalExpression >> "<=") > numericalExpression)[qi::_val = qi::_1 <= qi::_2];
                relativeExpression.name("relative expression");
                
                booleanVariableExpression = identifier[qi::_val = phoenix::bind(&storm::expressions::Expression::createBooleanVariable, qi::_1)];
                booleanVariableExpression.name("boolean variable");
                
                numericalExpression %= plusExpression;
                numericalExpression.name("numerical expression");
                
                plusExpression = multiplicationExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> multiplicationExpression)[phoenix::if_(qi::_a) [qi::_val = qi::_val + qi::_1] .else_ [qi::_val = qi::_val - qi::_1]];
                plusExpression.name("numerical expression");
                
                multiplicationExpression = atomicNumericalExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicNumericalExpression[qi::_val = qi::_val * qi::_1]);
                multiplicationExpression.name("numerical expression");
                
                atomicNumericalExpression %= minMaxExpression | floorCeilExpression | numericalVariableExpression | qi::lit("(") >> numericalExpression >> qi::lit(")");
                atomicNumericalExpression.name("numerical expression");
                
                minMaxExpression %= ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> numericalExpression >> qi::lit(",") >> numericalExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::minimum, qi::_1, qi::_2)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::maximum, qi::_1, qi::_2)]];
                minMaxExpression.name("min/max expression");
                
                floorCeilExpression %= ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> numericalExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::floor, qi::_1)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::ceil, qi::_1)]];
                floorCeilExpression.name("integer floor/ceil expression");

                numericalVariableExpression = identifier[qi::_val = phoenix::bind(&storm::expressions::Expression::createDoubleVariable, qi::_1)];
                numericalVariableExpression.name("numerical variable");
                
                // Parse a model type.
                modelTypeDefinition = modelType_;
                modelTypeDefinition.name("model type");

                programHeader =         modelTypeDefinition[phoenix::bind(&GlobalProgramInformation::modelType, qi::_r1) = qi::_1]
                                >   *(  undefinedConstantDefinition(qi::_r1)
                                    |   definedConstantDefinition(qi::_r1)
                                    |   formulaDefinition(qi::_r1)
                                    |   globalVariableDefinition(qi::_r1)
                                    );
                programHeader.name("program header");

                // This block defines all entities that are needed for parsing global variable definitions.
                globalVariableDefinitionList = *(qi::lit("global") > (booleanVariableDefinition(bind(&GlobalVariableInformation::booleanVariables, qi::_r1), bind(&GlobalVariableInformation::booleanVariableToIndexMap, qi::_r1), true) | integerVariableDefinition(bind(&GlobalVariableInformation::integerVariables, qi::_r1), bind(&GlobalVariableInformation::integerVariableToIndexMap, qi::_r1), true)));
                globalVariableDefinitionList.name("global variable declaration list");
                
                // This block defines all entities that are needed for parsing constant definitions.
                definedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[phoenix::bind(this->state->booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(this->state->allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
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
                undefinedDoubleConstantDefinition.name("undefined double constant definition");
                definedConstantDefinition %= (definedBooleanConstantDefinition(qi::_r1) | definedIntegerConstantDefinition(qi::_r2) | definedDoubleConstantDefinition(qi::_r3));
                definedConstantDefinition.name("defined constant definition");
                undefinedConstantDefinition = (undefinedBooleanConstantDefinition(qi::_r1) | undefinedIntegerConstantDefinition(qi::_r2) | undefinedDoubleConstantDefinition(qi::_r3));
                undefinedConstantDefinition.name("undefined constant definition");
                
                // Parse the ingredients of a probabilistic program.
                start = (qi::eps >
                         programHeader(qi::_a) >
                         formulaDefinitionList >
                         globalVariableDefinitionList(qi::_d) >
                         moduleDefinitionList >
                         rewardDefinitionList(qi::_e) >
                         labelDefinitionList(qi::_f))[qi::_val = phoenix::bind(&createProgram, qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2, qi::_e, qi::_f)];
                start.name("probabilistic program declaration");

                
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
                updateDefinition = (((ConstDoubleExpressionGrammar::instance(this->state) >> qi::lit(":"))
                                     | qi::attr(std::shared_ptr<BaseExpression>(new storm::ir::expressions::DoubleLiteralExpression(1))))[phoenix::clear(phoenix::ref(this->state->assignedBooleanVariables_)), phoenix::clear(phoenix::ref(this->state->assignedIntegerVariables_))]
                                    >> assignmentDefinitionList(qi::_a, qi::_b))[qi::_val = phoenix::bind(&PrismGrammar::createUpdate, this, qi::_1, qi::_a, qi::_b)];
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
                
                
                constantBooleanFormulaDefinition = (qi::lit("formula") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> ConstBooleanExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->constantBooleanFormulas_.add, qi::_1, qi::_2)];
                constantBooleanFormulaDefinition.name("constant boolean formula definition");
                booleanFormulaDefinition = (qi::lit("formula") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> BooleanExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->booleanFormulas_.add, qi::_1, qi::_2)];
                booleanFormulaDefinition.name("boolean formula definition");
                constantIntegerFormulaDefinition = (qi::lit("formula") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> ConstIntegerExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->constantIntegerFormulas_.add, qi::_1, qi::_2)];
                constantIntegerFormulaDefinition.name("constant integer formula definition");
                integerFormulaDefinition = (qi::lit("formula") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> IntegerExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->integerFormulas_.add, qi::_1, qi::_2)];
                integerFormulaDefinition.name("integer formula definition");
                constantDoubleFormulaDefinition = (qi::lit("formula") >> FreeIdentifierGrammar::instance(this->state) >> qi::lit("=") >> ConstDoubleExpressionGrammar::instance(this->state) >> qi::lit(";"))[phoenix::bind(this->state->constantDoubleFormulas_.add, qi::_1, qi::_2)];
                constantDoubleFormulaDefinition.name("constant double formula definition");
                formulaDefinition = constantBooleanFormulaDefinition | booleanFormulaDefinition | constantIntegerFormulaDefinition | integerFormulaDefinition | constantDoubleFormulaDefinition;
                formulaDefinition.name("formula definition");
                formulaDefinitionList = *formulaDefinition;
                formulaDefinitionList.name("formula definition list");
            }
        } // namespace prism
    } // namespace parser
} // namespace storm
