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
                
                minMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> numericalExpression >> qi::lit(",") >> numericalExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::minimum, qi::_1, qi::_2)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::maximum, qi::_1, qi::_2)]];
                minMaxExpression.name("min/max expression");
                
                floorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> numericalExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::floor, qi::_1)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::ceil, qi::_1)]];
                floorCeilExpression.name("integer floor/ceil expression");

                numericalVariableExpression = identifier[qi::_val = phoenix::bind(&storm::expressions::Expression::createDoubleVariable, qi::_1)];
                numericalVariableExpression.name("numerical variable");
                
                modelTypeDefinition = modelType_;
                modelTypeDefinition.name("model type");
                
                undefinedConstantDefinition = (undefinedBooleanConstantDefinition(qi::_r1) | undefinedIntegerConstantDefinition(qi::_r1) | undefinedDoubleConstantDefinition(qi::_r1));
                undefinedConstantDefinition.name("undefined constant definition");

                undefinedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool")) > identifier > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addUndefinedBooleanConstant, phoenix::bind(&GlobalProgramInformation::undefinedBooleanConstants, qi::_r1), qi::_1)];
                undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");

                undefinedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int")) > identifier > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addUndefinedIntegerConstant, phoenix::bind(&GlobalProgramInformation::undefinedIntegerConstants, qi::_r1), qi::_1)];
                undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
                
                undefinedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double")) > identifier > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addUndefinedDoubleConstant, phoenix::bind(&GlobalProgramInformation::undefinedDoubleConstants, qi::_r1), qi::_1)];
                undefinedDoubleConstantDefinition.name("undefined double constant definition");

                definedConstantDefinition %= (definedBooleanConstantDefinition(qi::_r1) | definedIntegerConstantDefinition(qi::_r1) | definedDoubleConstantDefinition(qi::_r1));
                definedConstantDefinition.name("defined constant definition");

                definedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addDefinedBooleanConstant, phoenix::bind(&GlobalProgramInformation::definedBooleanConstants, qi::_r1), qi::_1, qi::_2)];
                definedBooleanConstantDefinition.name("defined boolean constant declaration");

                definedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int") >> identifier >> qi::lit("=")) > expression >> qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addDefinedIntegerConstant, phoenix::bind(&GlobalProgramInformation::definedIntegerConstants, qi::_r1), qi::_1, qi::_2)];
                definedIntegerConstantDefinition.name("defined integer constant declaration");

                definedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addDefinedDoubleConstant, phoenix::bind(&GlobalProgramInformation::definedDoubleConstants, qi::_r1), qi::_1, qi::_2)];
                definedDoubleConstantDefinition.name("defined double constant declaration");
                
                formulaDefinition = (qi::lit("formula") > identifier > qi::lit("=") > expression > qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addFormula, phoenix::bind(&GlobalProgramInformation::formulas, qi::_r1), qi::_1, qi::_2)];
                formulaDefinition.name("formula definition");
                
                globalVariableDefinition = (qi::lit("global") > (booleanVariableDefinition(phoenix::bind(&GlobalProgramInformation::globalBooleanVariables, qi::_r1)) | integerVariableDefinition(phoenix::bind(&GlobalProgramInformation::globalIntegerVariables, qi::_r1))));
                globalVariableDefinition.name("global variable declaration list");
                
                programHeader =         modelTypeDefinition[phoenix::bind(&GlobalProgramInformation::modelType, qi::_r1) = qi::_1]
                                >   *(  undefinedConstantDefinition(qi::_r1)
                                    |   definedConstantDefinition(qi::_r1)
                                    |   formulaDefinition(qi::_r1)
                                    |   globalVariableDefinition(qi::_r1)
                                    );
                programHeader.name("program header");
 
                rewardModelDefinition = (qi::lit("rewards") > qi::lit("\"") > identifier > qi::lit("\"")
                                         > +(   stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)]
                                            |   transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]
                                            )
                                         >> qi::lit("endrewards"))[phoenix::bind(&PrismGrammar::addRewardModel, phoenix::bind(&GlobalProgramInformation::rewardModels, qi::_r1), qi::_1, qi::_a, qi::_b)];
                rewardModelDefinition.name("reward model definition");
                
                stateRewardDefinition = (booleanExpression > qi::lit(":") > numericalExpression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createStateReward, qi::_1, qi::_2)];
                stateRewardDefinition.name("state reward definition");
                
                transitionRewardDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit(":") > numericalExpression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createTransitionReward, qi::_a, qi::_2, qi::_3)];
                transitionRewardDefinition.name("transition reward definition");

                labelDefinition = (qi::lit("label") > -qi::lit("\"") > identifier > -qi::lit("\"") > qi::lit("=") > booleanExpression >> qi::lit(";"))[qi::_pass = phoenix::bind(&PrismGrammar::addLabel, phoenix::bind(&GlobalProgramInformation::labels, qi::_r1), qi::_1, qi::_2)];
                labelDefinition.name("label definition");

                assignmentDefinition = (qi::lit("(") > identifier > qi::lit("'") > qi::lit("=") > expression > qi::lit(")"))[qi::_pass = phoenix::bind(&PrismGrammar::addAssignment, qi::_r1, qi::_1, qi::_2)];
                assignmentDefinition.name("assignment");
                assignmentDefinitionList = assignmentDefinition(qi::_r1) % "&";
                assignmentDefinitionList.name("assignment list");
                
                moduleDefinitionList %= +(moduleDefinition(qi::_r1) | moduleRenaming(qi::_r1));
                moduleDefinitionList.name("module list");

                updateDefinition = (((numericalExpression >> qi::lit(":")) | qi::attr(storm::expressions::Expression::createDoubleLiteral(1))) >> assignmentDefinitionList(qi::_a))[qi::_val = phoenix::bind(&PrismGrammar::createUpdate, qi::_1, qi::_a)];
                updateDefinition.name("update");
                
                updateListDefinition %= +updateDefinition % "+";
                updateListDefinition.name("update list");
                
                commandDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit("->") > updateListDefinition > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createCommand, qi::_a, qi::_2, qi::_3)];
                commandDefinition.name("command definition");

                booleanVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("bool")) > ((qi::lit("init") > booleanExpression) | qi::attr(storm::expressions::Expression::createFalse())) > qi::lit(";"))[phoenix::bind(&PrismGrammar::addBooleanVariable, qi::_r1, qi::_1, qi::_2)];
                booleanVariableDefinition.name("boolean variable definition");
                
                integerVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("[")) > numericalExpression[qi::_a = qi::_1] > qi::lit("..") > numericalExpression > qi::lit("]") > -(qi::lit("init") > numericalExpression[qi::_a = qi::_1]) > qi::lit(";"))[phoenix::bind(&PrismGrammar::addIntegerVariable, qi::_r1, qi::_1, qi::_2, qi::_3, qi::_a)];
                integerVariableDefinition.name("integer variable definition");
                
                variableDefinition = (booleanVariableDefinition(qi::_r1) | integerVariableDefinition(qi::_r2));
                variableDefinition.name("variable declaration");

                moduleDefinition = ((qi::lit("module") >> identifier >> *(variableDefinition(qi::_a, qi::_b))) > +commandDefinition > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismGrammar::createModule, qi::_1, qi::_a, qi::_b, qi::_2)];
                moduleDefinition.name("module definition");
                
                moduleRenaming = ((qi::lit("module")	>> identifier >> qi::lit("=")) > identifier > qi::lit("[")
                                  > ((identifier > qi::lit("=") > identifier)[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))] % ",") > qi::lit("]")
                                  > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismGrammar::createRenamedModule, qi::_1, qi::_2, qi::_a, qi::_r1)];
                moduleRenaming.name("module definition via renaming");
                
                start = (qi::eps > programHeader(qi::_a) > moduleDefinitionList(qi::_a) > *(rewardModelDefinition(qi::_a) | labelDefinition(qi::_a)))[qi::_val = phoenix::bind(&PrismGrammar::createProgram, qi::_a, qi::_1)];
                start.name("probabilistic program");
            }
        } // namespace prism
    } // namespace parser
} // namespace storm
