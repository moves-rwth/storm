// #define BOOST_SPIRIT_DEBUG
#include "src/parser/prismparser/PrismGrammar.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        namespace prism {
            PrismGrammar::PrismGrammar(std::string const& filename, Iterator first) : PrismGrammar::base_type(start), doExpressionGeneration(false), filename(filename), annotate(first) {
                // Parse simple identifier.
                identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&PrismGrammar::isValidIdentifier, phoenix::ref(*this), qi::_1)];
                identifier.name("identifier");
                
                setExpressionGeneration(doExpressionGeneration);
                
                modelTypeDefinition %= modelType_;
                modelTypeDefinition.name("model type");

                undefinedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createUndefinedBooleanConstant, phoenix::ref(*this), qi::_1)];
                undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");

                undefinedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createUndefinedIntegerConstant, phoenix::ref(*this), qi::_1)];
                undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
                
                undefinedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createUndefinedDoubleConstant, phoenix::ref(*this), qi::_1)];
                undefinedDoubleConstantDefinition.name("undefined double constant definition");

                undefinedConstantDefinition = (undefinedBooleanConstantDefinition | undefinedIntegerConstantDefinition | undefinedDoubleConstantDefinition);
                undefinedConstantDefinition.name("undefined constant definition");

                definedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createDefinedBooleanConstant, phoenix::ref(*this), qi::_1, qi::_2)];
                definedBooleanConstantDefinition.name("defined boolean constant declaration");

                definedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int") >> identifier >> qi::lit("=")) > expression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createDefinedIntegerConstant, phoenix::ref(*this), qi::_1, qi::_2)];
                definedIntegerConstantDefinition.name("defined integer constant declaration");

                definedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createDefinedDoubleConstant, phoenix::ref(*this), qi::_1, qi::_2)];
                definedDoubleConstantDefinition.name("defined double constant declaration");
                
                definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
                definedConstantDefinition.name("defined constant definition");
                
                formulaDefinition = (qi::lit("formula") > identifier > qi::lit("=") > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createFormula, phoenix::ref(*this), qi::_1, qi::_2)];
                formulaDefinition.name("formula definition");
                
                booleanVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("bool")) > ((qi::lit("init") > expression) | qi::attr(storm::expressions::Expression::createFalse())) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createBooleanVariable, phoenix::ref(*this), qi::_1, qi::_2)];
                booleanVariableDefinition.name("boolean variable definition");
                
                integerVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("[")[phoenix::bind(&PrismGrammar::allowDoubleLiterals, phoenix::ref(*this), false)]) > expression[qi::_a = qi::_1] > qi::lit("..") > expression > qi::lit("]")[phoenix::bind(&PrismGrammar::allowDoubleLiterals, phoenix::ref(*this), true)] > -(qi::lit("init") > expression[qi::_a = qi::_1]) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createIntegerVariable, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_a)];
                integerVariableDefinition.name("integer variable definition");
                
                variableDefinition = (booleanVariableDefinition[phoenix::push_back(qi::_r1, qi::_1)] | integerVariableDefinition[phoenix::push_back(qi::_r2, qi::_1)]);
                variableDefinition.name("variable declaration");
                
                globalVariableDefinition = (qi::lit("global") > (booleanVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalBooleanVariables, qi::_r1), qi::_1)] | integerVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalIntegerVariables, qi::_r1), qi::_1)]));
                globalVariableDefinition.name("global variable declaration list");
                
                programHeader = modelTypeDefinition[phoenix::bind(&GlobalProgramInformation::modelType, qi::_r1) = qi::_1]
                                    >   *(definedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_r1), qi::_1)] | undefinedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_r1), qi::_1)])
                                    >   *(formulaDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::formulas, qi::_r1), qi::_1)])
                                    >   *(globalVariableDefinition(qi::_r1));
                programHeader.name("program header");
                
                stateRewardDefinition = (expression > qi::lit(":") > plusExpression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createStateReward, phoenix::ref(*this), qi::_1, qi::_2)];
                stateRewardDefinition.name("state reward definition");
                
                transitionRewardDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > expression > qi::lit(":") > plusExpression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createTransitionReward, phoenix::ref(*this), qi::_a, qi::_2, qi::_3)];
                transitionRewardDefinition.name("transition reward definition");

                rewardModelDefinition = (qi::lit("rewards") > qi::lit("\"") > identifier > qi::lit("\"")
                                         > +(   stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)]
                                             |   transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]
                                             )
                                         >> qi::lit("endrewards"))[qi::_val = phoenix::bind(&PrismGrammar::createRewardModel, phoenix::ref(*this), qi::_1, qi::_a, qi::_b)];
                rewardModelDefinition.name("reward model definition");
                
                initialStatesConstruct = (qi::lit("init") > expression > qi::lit("endinit"))[qi::_pass = phoenix::bind(&PrismGrammar::addInitialStatesExpression, phoenix::ref(*this), qi::_1, qi::_r1)];
                initialStatesConstruct.name("initial construct");
                
                labelDefinition = (qi::lit("label") > -qi::lit("\"") > identifier > -qi::lit("\"") > qi::lit("=") > expression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createLabel, phoenix::ref(*this), qi::_1, qi::_2)];
                labelDefinition.name("label definition");

                assignmentDefinition = (qi::lit("(") > identifier > qi::lit("'") > qi::lit("=") > expression > qi::lit(")"))[qi::_val = phoenix::bind(&PrismGrammar::createAssignment, phoenix::ref(*this), qi::_1, qi::_2)];
                assignmentDefinition.name("assignment");
                
                assignmentDefinitionList %= +assignmentDefinition % "&";
                assignmentDefinitionList.name("assignment list");

                updateDefinition = (((plusExpression > qi::lit(":")) | qi::attr(storm::expressions::Expression::createDoubleLiteral(1))) >> assignmentDefinitionList)[qi::_val = phoenix::bind(&PrismGrammar::createUpdate, phoenix::ref(*this), qi::_1, qi::_2, qi::_r1)];
                updateDefinition.name("update");
                
                updateListDefinition %= +updateDefinition(qi::_r1) % "+";
                updateListDefinition.name("update list");
                
                commandDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > expression > qi::lit("->") > updateListDefinition(qi::_r1) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismGrammar::createCommand, phoenix::ref(*this), qi::_a, qi::_2, qi::_3, qi::_r1)];
                commandDefinition.name("command definition");

                moduleDefinition = ((qi::lit("module") >> identifier >> *(variableDefinition(qi::_a, qi::_b))) > +commandDefinition(qi::_r1) > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismGrammar::createModule, phoenix::ref(*this), qi::_1, qi::_a, qi::_b, qi::_2, qi::_r1)];
                moduleDefinition.name("module definition");
                
                moduleRenaming = ((qi::lit("module") >> identifier >> qi::lit("=")) > identifier > qi::lit("[")
                                  > ((identifier > qi::lit("=") > identifier)[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))] % ",") > qi::lit("]")
                                  > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismGrammar::createRenamedModule, phoenix::ref(*this), qi::_1, qi::_2, qi::_a, qi::_r1)];
                moduleRenaming.name("module definition via renaming");
                
                moduleDefinitionList %= +(moduleRenaming(qi::_r1) | moduleDefinition(qi::_r1))[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::modules, qi::_r1), qi::_1)];
                moduleDefinitionList.name("module list");

                start = (qi::eps > programHeader(qi::_a) > moduleDefinitionList(qi::_a) > *(initialStatesConstruct(qi::_a) | rewardModelDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::rewardModels, qi::_a), qi::_1)] | labelDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::labels, qi::_a), qi::_1)]) > qi::eoi)[qi::_val = phoenix::bind(&PrismGrammar::createProgram, phoenix::ref(*this), qi::_a)];
                start.name("probabilistic program");

                // Enable location tracking for important entities.
                auto setLocationInfoFunction = this->annotate(qi::_val, qi::_1, qi::_3);
                qi::on_success(undefinedBooleanConstantDefinition, setLocationInfoFunction);
                qi::on_success(undefinedIntegerConstantDefinition, setLocationInfoFunction);
                qi::on_success(undefinedDoubleConstantDefinition, setLocationInfoFunction);
                qi::on_success(definedBooleanConstantDefinition, setLocationInfoFunction);
                qi::on_success(definedIntegerConstantDefinition, setLocationInfoFunction);
                qi::on_success(definedDoubleConstantDefinition, setLocationInfoFunction);
                qi::on_success(booleanVariableDefinition, setLocationInfoFunction);
                qi::on_success(integerVariableDefinition, setLocationInfoFunction);
                qi::on_success(moduleDefinition, setLocationInfoFunction);
                qi::on_success(moduleRenaming, setLocationInfoFunction);
                qi::on_success(formulaDefinition, setLocationInfoFunction);
                qi::on_success(rewardModelDefinition, setLocationInfoFunction);
                qi::on_success(labelDefinition, setLocationInfoFunction);
                qi::on_success(commandDefinition, setLocationInfoFunction);
                qi::on_success(updateDefinition, setLocationInfoFunction);
                qi::on_success(assignmentDefinition, setLocationInfoFunction);
            }
            
            void PrismGrammar::setExpressionGeneration(bool doExpressionGeneration) {
                if (doExpressionGeneration) {
                    floorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::floor, qi::_1)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::ceil, qi::_1)]];
                    floorCeilExpression.name("floor/ceil expression");
                    
                    minMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(",") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&storm::expressions::Expression::minimum, qi::_1, qi::_2)] .else_ [qi::_val = phoenix::bind(&storm::expressions::Expression::maximum, qi::_1, qi::_2)]];
                    minMaxExpression.name("min/max expression");
                    
                    identifierExpression = identifier[qi::_val = phoenix::bind(&PrismGrammar::getIdentifierExpression, phoenix::ref(*this), qi::_1)];
                    identifierExpression.name("identifier expression");
                    
                    literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&storm::expressions::Expression::createTrue)] | qi::lit("false")[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)] | strict_double[qi::_val = phoenix::bind(&storm::expressions::Expression::createDoubleLiteral, qi::_1)] | qi::int_[qi::_val = phoenix::bind(&storm::expressions::Expression::createIntegerLiteral, qi::_1)];
                    literalExpression.name("literal expression");
                    
                    atomicExpression = minMaxExpression | floorCeilExpression | qi::lit("(") >> expression >> qi::lit(")") | literalExpression | identifierExpression;
                    atomicExpression.name("atomic expression");
                    
                    unaryExpression = atomicExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicExpression)[qi::_val = !qi::_1] | (qi::lit("-") >> atomicExpression)[qi::_val = -qi::_1];
                    unaryExpression.name("unary expression");
                    
                    multiplicationExpression = unaryExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> unaryExpression[phoenix::if_(qi::_a) [qi::_val = qi::_val * qi::_1] .else_ [qi::_val = qi::_val / qi::_1]]);
                    multiplicationExpression.name("multiplication expression");
                    
                    plusExpression = multiplicationExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> multiplicationExpression)[phoenix::if_(qi::_a) [qi::_val = qi::_val + qi::_1] .else_ [qi::_val = qi::_val - qi::_1]];
                    plusExpression.name("plus expression");
                    
                    relativeExpression = (plusExpression >> qi::lit(">=") >> plusExpression)[qi::_val = qi::_1 >= qi::_1] | (plusExpression >> qi::lit(">") >> plusExpression)[qi::_val = qi::_1 > qi::_2] | (plusExpression >> qi::lit("<=") >> plusExpression)[qi::_val = qi::_1 <= qi::_2] | (plusExpression >> qi::lit("<") >> plusExpression)[qi::_val = qi::_1 < qi::_2] | (plusExpression >> qi::lit("=") >> plusExpression)[qi::_val = qi::_1 == qi::_2] | (plusExpression >> qi::lit("!=") >> plusExpression)[qi::_val = qi::_1 != qi::_2] | plusExpression[qi::_val = qi::_1];
                    relativeExpression.name("relative expression");
                    
                    andExpression = relativeExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> relativeExpression)[qi::_val = qi::_val && qi::_1];
                    andExpression.name("and expression");
                    
                    orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = qi::_val || qi::_1];
                    orExpression.name("or expression");
                    
                    expression %= orExpression;
                    expression.name("expression");
                } else {
                    floorCeilExpression = ((qi::lit("floor") | qi::lit("ceil")) >> qi::lit("(") >> plusExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    floorCeilExpression.name("floor/ceil expression");
                    
                    minMaxExpression = ((qi::lit("min") | qi::lit("max")) >> qi::lit("(") >> plusExpression >> qi::lit(",") >> plusExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    minMaxExpression.name("min/max expression");
                    
                    identifierExpression = identifier[qi::_val = phoenix::construct<storm::expressions::Expression>(), qi::_pass = phoenix::bind(&PrismGrammar::isValidIdentifier, phoenix::ref(*this), qi::_1)];
                    identifierExpression.name("identifier expression");
                    
                    literalExpression = (qi::lit("true") | qi::lit("false") | strict_double | qi::int_)[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    literalExpression.name("literal expression");
                    
                    atomicExpression = (minMaxExpression | floorCeilExpression | qi::lit("(") >> expression >> qi::lit(")") | literalExpression | identifierExpression)[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    atomicExpression.name("atomic expression");
                    
                    unaryExpression = (atomicExpression | (qi::lit("!") >> atomicExpression) | (qi::lit("-") >> atomicExpression))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    unaryExpression.name("unary expression");
                    
                    multiplicationExpression = (unaryExpression >> *((qi::lit("*") | qi::lit("/")) >> unaryExpression))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    multiplicationExpression.name("multiplication expression");
                    
                    plusExpression = (multiplicationExpression >> *((qi::lit("+") | qi::lit("-")) >> multiplicationExpression))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    plusExpression.name("plus expression");
                    
                    relativeExpression = ((plusExpression >> qi::lit(">=") >> plusExpression) | (plusExpression >> qi::lit(">") >> plusExpression) | (plusExpression >> qi::lit("<=") >> plusExpression) | (plusExpression >> qi::lit("<") >> plusExpression) | (plusExpression >> qi::lit("=") >> plusExpression) | (plusExpression >> qi::lit("!=") >> plusExpression) | plusExpression)[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    relativeExpression.name("relative expression");
                    
                    andExpression = (relativeExpression >> *(qi::lit("&") >> relativeExpression))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    andExpression.name("and expression");
                    
                    orExpression = (andExpression >> *(qi::lit("|") >> andExpression))[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                    orExpression.name("or expression");
                    
                    expression %= orExpression;
                    expression.name("expression");
                }
                
                // Enable error reporting.
                qi::on_error<qi::fail>(expression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(orExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(andExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(relativeExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(plusExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(multiplicationExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(unaryExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(atomicExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(literalExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(identifierExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(minMaxExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(floorCeilExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                
                // Finally toggle the internal flag.
                this->doExpressionGeneration = doExpressionGeneration;
            }
            
            void PrismGrammar::toggleExpressionGeneration() {
                setExpressionGeneration(!doExpressionGeneration);
            }
            
            void PrismGrammar::allowDoubleLiterals(bool flag) {
                if (flag) {
                    if (this->doExpressionGeneration) {
                        literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&storm::expressions::Expression::createTrue)] | qi::lit("false")[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)] | strict_double[qi::_val = phoenix::bind(&storm::expressions::Expression::createDoubleLiteral, qi::_1)] | qi::int_[qi::_val = phoenix::bind(&storm::expressions::Expression::createIntegerLiteral, qi::_1)];
                        literalExpression.name("literal expression");
                    } else {
                        literalExpression = (qi::lit("true") | qi::lit("false") | strict_double | qi::int_)[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                        literalExpression.name("literal expression");
                    }
                } else {
                    if (this->doExpressionGeneration) {
                        literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&storm::expressions::Expression::createTrue)] | qi::lit("false")[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)] | qi::int_[qi::_val = phoenix::bind(&storm::expressions::Expression::createIntegerLiteral, qi::_1)];
                        literalExpression.name("literal expression");
                    } else {
                        literalExpression = (qi::lit("true") | qi::lit("false") | qi::int_)[qi::_val = phoenix::bind(&storm::expressions::Expression::createFalse)];
                        literalExpression.name("literal expression");
                    }
                }
            }
            
            std::string const& PrismGrammar::getFilename() const {
                return this->filename;
            }
            
            bool PrismGrammar::isValidIdentifier(std::string const& identifier) {
                if (this->keywords_.find(identifier) != nullptr) {
                    return false;
                }
                return true;
            }
            
            bool PrismGrammar::addInitialStatesExpression(storm::expressions::Expression initialStatesExpression, GlobalProgramInformation& globalProgramInformation) {
                LOG_THROW(!globalProgramInformation.hasInitialStatesExpression, storm::exceptions::InvalidArgumentException, "Program must not define two initial constructs.");
                if (globalProgramInformation.hasInitialStatesExpression) {
                    return false;
                }
                globalProgramInformation.hasInitialStatesExpression = true;
                globalProgramInformation.initialStatesExpression = initialStatesExpression;
                return true;
            }
            
            storm::expressions::Expression PrismGrammar::getIdentifierExpression(std::string const& identifier) const {
                storm::expressions::Expression const* expression = this->identifiers_.find(identifier);
                LOG_THROW(expression != nullptr, storm::exceptions::WrongFormatException, "Undeclared identifier '" << identifier << "'.");
                return *expression;
            }
            
            storm::prism::Constant PrismGrammar::createUndefinedBooleanConstant(std::string const& newConstant) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createBooleanConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Bool, newConstant, this->getFilename());
            }
            
            storm::prism::Constant PrismGrammar::createUndefinedIntegerConstant(std::string const& newConstant) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createIntegerConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Integer, newConstant, this->getFilename());
            }
            
            storm::prism::Constant PrismGrammar::createUndefinedDoubleConstant(std::string const& newConstant) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createDoubleConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Double, newConstant, this->getFilename());
            }
            
            storm::prism::Constant PrismGrammar::createDefinedBooleanConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createBooleanConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Bool, newConstant, expression, this->getFilename());
            }
            
            storm::prism::Constant PrismGrammar::createDefinedIntegerConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createIntegerConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Integer, newConstant, expression, this->getFilename());
            }
            
            storm::prism::Constant PrismGrammar::createDefinedDoubleConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
                this->identifiers_.add(newConstant, storm::expressions::Expression::createDoubleConstant(newConstant));
                return storm::prism::Constant(storm::prism::Constant::ConstantType::Double, newConstant, expression, this->getFilename());
            }
            
            storm::prism::Formula PrismGrammar::createFormula(std::string const& formulaName, storm::expressions::Expression expression) const {
                this->identifiers_.add(formulaName, expression);
                return storm::prism::Formula(formulaName, expression, this->getFilename());
            }
            
            storm::prism::Label PrismGrammar::createLabel(std::string const& labelName, storm::expressions::Expression expression) const {
                return storm::prism::Label(labelName, expression, this->getFilename());
            }
            
            storm::prism::RewardModel PrismGrammar::createRewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards) const {
                return storm::prism::RewardModel(rewardModelName, stateRewards, transitionRewards, this->getFilename());
            }
            
            storm::prism::StateReward PrismGrammar::createStateReward(storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const {
                return storm::prism::StateReward(statePredicateExpression, rewardValueExpression, this->getFilename());
            }
            
            storm::prism::TransitionReward PrismGrammar::createTransitionReward(std::string const& actionName, storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const {
                return storm::prism::TransitionReward(actionName, statePredicateExpression, rewardValueExpression, this->getFilename());
            }
            
            storm::prism::Assignment PrismGrammar::createAssignment(std::string const& variableName, storm::expressions::Expression assignedExpression) const {
                return storm::prism::Assignment(variableName, assignedExpression, this->getFilename());
            }
            
            storm::prism::Update PrismGrammar::createUpdate(storm::expressions::Expression likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, GlobalProgramInformation& globalProgramInformation) const {
                ++globalProgramInformation.currentUpdateIndex;
                return storm::prism::Update(globalProgramInformation.currentUpdateIndex - 1, likelihoodExpression, assignments, this->getFilename());
            }
            
            storm::prism::Command PrismGrammar::createCommand(std::string const& actionName, storm::expressions::Expression guardExpression, std::vector<storm::prism::Update> const& updates, GlobalProgramInformation& globalProgramInformation) const {
                ++globalProgramInformation.currentCommandIndex;
                return storm::prism::Command(globalProgramInformation.currentCommandIndex - 1, actionName, guardExpression, updates, this->getFilename());
            }
            
            storm::prism::BooleanVariable PrismGrammar::createBooleanVariable(std::string const& variableName, storm::expressions::Expression initialValueExpression) const {
                this->identifiers_.add(variableName, storm::expressions::Expression::createBooleanVariable(variableName));
                return storm::prism::BooleanVariable(variableName, initialValueExpression, this->getFilename());
            }
            
            storm::prism::IntegerVariable PrismGrammar::createIntegerVariable(std::string const& variableName, storm::expressions::Expression lowerBoundExpression, storm::expressions::Expression upperBoundExpression, storm::expressions::Expression initialValueExpression) const {
                this->identifiers_.add(variableName, storm::expressions::Expression::createIntegerVariable(variableName));
                return storm::prism::IntegerVariable(variableName, lowerBoundExpression, upperBoundExpression, initialValueExpression, this->getFilename());
            }
            
            storm::prism::Module PrismGrammar::createModule(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, GlobalProgramInformation& globalProgramInformation) const {
                globalProgramInformation.moduleToIndexMap[moduleName] = globalProgramInformation.modules.size();
                return storm::prism::Module(moduleName, booleanVariables, integerVariables, commands, this->getFilename());
            }
            
            storm::prism::Module PrismGrammar::createRenamedModule(std::string const& newModuleName, std::string const& oldModuleName, std::map<std::string, std::string> const& renaming, GlobalProgramInformation& globalProgramInformation) const {
                auto const& moduleIndexPair = globalProgramInformation.moduleToIndexMap.find(oldModuleName);
                LOG_THROW(moduleIndexPair != globalProgramInformation.moduleToIndexMap.end(), storm::exceptions::WrongFormatException, "No module named '" << oldModuleName << "' to rename.");
                globalProgramInformation.moduleToIndexMap[newModuleName] = globalProgramInformation.modules.size();
                uint_fast64_t commandBaseIndex = globalProgramInformation.currentCommandIndex;
                uint_fast64_t updateBaseIndex = globalProgramInformation.currentUpdateIndex;
                storm::prism::Module const& moduleToClone = globalProgramInformation.modules[moduleIndexPair->second];
                globalProgramInformation.currentCommandIndex += moduleToClone.getNumberOfCommands();
                globalProgramInformation.currentUpdateIndex += moduleToClone.getNumberOfUpdates();
                return storm::prism::Module(moduleToClone, newModuleName, commandBaseIndex, updateBaseIndex, renaming, this->getFilename());
            }
            
            storm::prism::Program PrismGrammar::createProgram(GlobalProgramInformation const& globalProgramInformation) const {
                return storm::prism::Program(globalProgramInformation.modelType, globalProgramInformation.constants, globalProgramInformation.globalBooleanVariables, globalProgramInformation.globalIntegerVariables, globalProgramInformation.formulas, globalProgramInformation.modules, globalProgramInformation.rewardModels, globalProgramInformation.hasInitialStatesExpression, globalProgramInformation.initialStatesExpression, globalProgramInformation.labels, this->getFilename());
            }
        } // namespace prism
    } // namespace parser
} // namespace storm
