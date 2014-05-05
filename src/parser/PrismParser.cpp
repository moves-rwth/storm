#include "src/parser/PrismParser.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        storm::prism::Program PrismParser::parse(std::string const& filename) {
            // Open file and initialize result.
            std::ifstream inputFileStream(filename, std::ios::in);
            LOG_THROW(inputFileStream.good(), storm::exceptions::WrongFormatException, "Unable to read from file '" << filename << "'.");
            
            storm::prism::Program result;
            
            // Now try to parse the contents of the file.
            try {
                std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
                result = parseFromString(fileContent, filename);
            } catch(std::exception& e) {
                // In case of an exception properly close the file before passing exception.
                inputFileStream.close();
                throw e;
            }
            
            // Close the stream in case everything went smoothly and return result.
            inputFileStream.close();
            return result;
        }
        
        storm::prism::Program PrismParser::parseFromString(std::string const& input, std::string const& filename) {
            PositionIteratorType first(input.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(input.end());
            
            // Create empty result;
            storm::prism::Program result;
            
            // Create grammar.
            storm::parser::PrismParser grammar(filename, first);
            try {
                // Start first run.
                bool succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
                LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in first pass.");
                
                // Start second run.
                first = PositionIteratorType(input.begin());
                iter = first;
                last = PositionIteratorType(input.end());
                grammar.moveToSecondRun();
                succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
                LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in second pass.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                // If the parser expected content different than the one provided, display information about the location of the error.
                std::size_t lineNumber = boost::spirit::get_line(e.first);
                
                // Now propagate exception.
                LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << lineNumber << " of file " << filename << ".");
            }
            
            return result;
        }
        
        PrismParser::PrismParser(std::string const& filename, Iterator first) : PrismParser::base_type(start), secondRun(false), allowDoubleLiteralsFlag(true), filename(filename), annotate(first) {
            // Parse simple identifier.
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&PrismParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            floorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&PrismParser::createFloorExpression, phoenix::ref(*this), qi::_1)] .else_ [qi::_val = phoenix::bind(&PrismParser::createCeilExpression, phoenix::ref(*this), qi::_1)]];
            floorCeilExpression.name("floor/ceil expression");
            
            minMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(",") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&PrismParser::createMinimumExpression, phoenix::ref(*this), qi::_1, qi::_2)] .else_ [qi::_val = phoenix::bind(&PrismParser::createMaximumExpression, phoenix::ref(*this), qi::_1, qi::_2)]];
            minMaxExpression.name("min/max expression");
            
            identifierExpression = identifier[qi::_val = phoenix::bind(&PrismParser::getIdentifierExpression, phoenix::ref(*this), qi::_1)];
            identifierExpression.name("identifier expression");
            
            literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&PrismParser::createTrueExpression, phoenix::ref(*this))] | qi::lit("false")[qi::_val = phoenix::bind(&PrismParser::createFalseExpression, phoenix::ref(*this))] | strict_double[qi::_val = phoenix::bind(&PrismParser::createDoubleLiteralExpression, phoenix::ref(*this), qi::_1, qi::_pass)] | qi::int_[qi::_val = phoenix::bind(&PrismParser::createIntegerLiteralExpression, phoenix::ref(*this), qi::_1)];
            literalExpression.name("literal expression");
            
            atomicExpression = minMaxExpression | floorCeilExpression | qi::lit("(") >> expression >> qi::lit(")") | literalExpression | identifierExpression;
            atomicExpression.name("atomic expression");
            
            unaryExpression = atomicExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicExpression)[qi::_val = phoenix::bind(&PrismParser::createNotExpression, phoenix::ref(*this), qi::_1)] | (qi::lit("-") >> atomicExpression)[qi::_val = phoenix::bind(&PrismParser::createMinusExpression, phoenix::ref(*this), qi::_1)];
            unaryExpression.name("unary expression");
            
            multiplicationExpression = unaryExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> unaryExpression[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&PrismParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&PrismParser::createDivExpression, phoenix::ref(*this), qi::_val, qi::_1)]]);
            multiplicationExpression.name("multiplication expression");
            
            plusExpression = multiplicationExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> multiplicationExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&PrismParser::createPlusExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&PrismParser::createMinusExpression, phoenix::ref(*this), qi::_val, qi::_1)]];
            plusExpression.name("plus expression");
            
            relativeExpression = (plusExpression >> qi::lit(">=") >> plusExpression)[qi::_val = phoenix::bind(&PrismParser::createGreaterOrEqualExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit(">") >> plusExpression)[qi::_val = phoenix::bind(&PrismParser::createGreaterExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit("<=") >> plusExpression)[qi::_val = phoenix::bind(&PrismParser::createLessOrEqualExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit("<") >> plusExpression)[qi::_val = phoenix::bind(&PrismParser::createLessExpression, phoenix::ref(*this), qi::_1, qi::_2)] | plusExpression[qi::_val = qi::_1];
            relativeExpression.name("relative expression");
            
            equalityExpression = relativeExpression[qi::_val = qi::_1] >> *((qi::lit("=")[qi::_a = true] | qi::lit("!=")[qi::_a = false]) >> relativeExpression)[phoenix::if_(qi::_a) [ qi::_val = phoenix::bind(&PrismParser::createEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1) ] .else_ [ qi::_val = phoenix::bind(&PrismParser::createNotEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1) ] ];
            equalityExpression.name("equality expression");
            
            andExpression = equalityExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> equalityExpression)[qi::_val = phoenix::bind(&PrismParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1)];
            andExpression.name("and expression");
            
            orExpression = andExpression[qi::_val = qi::_1] >> *((qi::lit("|")[qi::_a = true] | qi::lit("=>")[qi::_a = false]) >> andExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&PrismParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&PrismParser::createImpliesExpression, phoenix::ref(*this), qi::_val, qi::_1)] ];
            orExpression.name("or expression");
            
            iteExpression = orExpression[qi::_val = qi::_1] >> -(qi::lit("?") > orExpression > qi::lit(":") > orExpression)[qi::_val = phoenix::bind(&PrismParser::createIteExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            iteExpression.name("if-then-else expression");
            
            expression %= iteExpression;
            expression.name("expression");
            
            modelTypeDefinition %= modelType_;
            modelTypeDefinition.name("model type");
            
            undefinedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedBooleanConstant, phoenix::ref(*this), qi::_1)];
            undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
            
            undefinedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedIntegerConstant, phoenix::ref(*this), qi::_1)];
            undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
            
            undefinedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedDoubleConstant, phoenix::ref(*this), qi::_1)];
            undefinedDoubleConstantDefinition.name("undefined double constant definition");
            
            undefinedConstantDefinition = (undefinedBooleanConstantDefinition | undefinedIntegerConstantDefinition | undefinedDoubleConstantDefinition);
            undefinedConstantDefinition.name("undefined constant definition");
            
            definedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedBooleanConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedBooleanConstantDefinition.name("defined boolean constant declaration");
            
            definedIntegerConstantDefinition = ((qi::lit("const") >> qi::lit("int") >> identifier >> qi::lit("=")) > expression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedIntegerConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedIntegerConstantDefinition.name("defined integer constant declaration");
            
            definedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double") >> identifier >> qi::lit("=")) > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedDoubleConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedDoubleConstantDefinition.name("defined double constant declaration");
            
            definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
            definedConstantDefinition.name("defined constant definition");
            
            formulaDefinition = (qi::lit("formula") > identifier > qi::lit("=") > expression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            formulaDefinition.name("formula definition");
            
            booleanVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("bool")) > ((qi::lit("init") > expression) | qi::attr(storm::expressions::Expression::createFalse())) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createBooleanVariable, phoenix::ref(*this), qi::_1, qi::_2)];
            booleanVariableDefinition.name("boolean variable definition");
            
            integerVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("[")[phoenix::bind(&PrismParser::allowDoubleLiterals, phoenix::ref(*this), false)]) > expression[qi::_a = qi::_1] > qi::lit("..") > expression > qi::lit("]")[phoenix::bind(&PrismParser::allowDoubleLiterals, phoenix::ref(*this), true)] > -(qi::lit("init") > expression[qi::_a = qi::_1]) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createIntegerVariable, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_a)];
            integerVariableDefinition.name("integer variable definition");
            
            variableDefinition = (booleanVariableDefinition[phoenix::push_back(qi::_r1, qi::_1)] | integerVariableDefinition[phoenix::push_back(qi::_r2, qi::_1)]);
            variableDefinition.name("variable declaration");
            
            globalVariableDefinition = (qi::lit("global") > (booleanVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalBooleanVariables, qi::_r1), qi::_1)] | integerVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalIntegerVariables, qi::_r1), qi::_1)]));
            globalVariableDefinition.name("global variable declaration list");
                        
            stateRewardDefinition = (expression > qi::lit(":") > plusExpression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createStateReward, phoenix::ref(*this), qi::_1, qi::_2)];
            stateRewardDefinition.name("state reward definition");
            
            transitionRewardDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > expression > qi::lit(":") > plusExpression > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createTransitionReward, phoenix::ref(*this), qi::_a, qi::_2, qi::_3)];
            transitionRewardDefinition.name("transition reward definition");
            
            rewardModelDefinition = (qi::lit("rewards") > -(qi::lit("\"") > identifier[qi::_a = qi::_1] > qi::lit("\""))
                                     > +(   stateRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]
                                         |   transitionRewardDefinition[phoenix::push_back(qi::_c, qi::_1)]
                                         )
                                     >> qi::lit("endrewards"))[qi::_val = phoenix::bind(&PrismParser::createRewardModel, phoenix::ref(*this), qi::_a, qi::_b, qi::_c)];
            rewardModelDefinition.name("reward model definition");
            
            initialStatesConstruct = (qi::lit("init") > expression > qi::lit("endinit"))[qi::_pass = phoenix::bind(&PrismParser::addInitialStatesConstruct, phoenix::ref(*this), qi::_1, qi::_r1)];
            initialStatesConstruct.name("initial construct");
            
            labelDefinition = (qi::lit("label") > -qi::lit("\"") > identifier > -qi::lit("\"") > qi::lit("=") > expression >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createLabel, phoenix::ref(*this), qi::_1, qi::_2)];
            labelDefinition.name("label definition");
            
            assignmentDefinition = (qi::lit("(") > identifier > qi::lit("'") > qi::lit("=") > expression > qi::lit(")"))[qi::_val = phoenix::bind(&PrismParser::createAssignment, phoenix::ref(*this), qi::_1, qi::_2)];
            assignmentDefinition.name("assignment");
            
            assignmentDefinitionList %= +assignmentDefinition % "&";
            assignmentDefinitionList.name("assignment list");
            
            updateDefinition = (((plusExpression > qi::lit(":")) | qi::attr(storm::expressions::Expression::createDoubleLiteral(1))) >> assignmentDefinitionList)[qi::_val = phoenix::bind(&PrismParser::createUpdate, phoenix::ref(*this), qi::_1, qi::_2, qi::_r1)];
            updateDefinition.name("update");
            
            updateListDefinition %= +updateDefinition(qi::_r1) % "+";
            updateListDefinition.name("update list");
            
            commandDefinition = (qi::lit("[") > -(identifier[qi::_a = qi::_1]) > qi::lit("]") > expression > qi::lit("->") > updateListDefinition(qi::_r1) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createCommand, phoenix::ref(*this), qi::_a, qi::_2, qi::_3, qi::_r1)];
            commandDefinition.name("command definition");
            
            moduleDefinition = ((qi::lit("module") >> identifier >> *(variableDefinition(qi::_a, qi::_b))) > +commandDefinition(qi::_r1) > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismParser::createModule, phoenix::ref(*this), qi::_1, qi::_a, qi::_b, qi::_2, qi::_r1)];
            moduleDefinition.name("module definition");
            
            moduleRenaming = ((qi::lit("module") >> identifier >> qi::lit("=")) > identifier > qi::lit("[")
                              > ((identifier > qi::lit("=") > identifier)[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))] % ",") > qi::lit("]")
                              > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismParser::createRenamedModule, phoenix::ref(*this), qi::_1, qi::_2, qi::_a, qi::_r1)];
            moduleRenaming.name("module definition via renaming");
            
            moduleDefinitionList %= +(moduleRenaming(qi::_r1) | moduleDefinition(qi::_r1))[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::modules, qi::_r1), qi::_1)];
            moduleDefinitionList.name("module list");
            
            start = (qi::eps
                     > modelTypeDefinition[phoenix::bind(&GlobalProgramInformation::modelType, qi::_a) = qi::_1]
                     > *(definedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_a), qi::_1)] 
                         | undefinedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_a), qi::_1)]
                         | formulaDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::formulas, qi::_a), qi::_1)]
                         | globalVariableDefinition(qi::_a)
                         | (moduleRenaming(qi::_a) | moduleDefinition(qi::_a))[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::modules, qi::_a), qi::_1)]
                         | initialStatesConstruct(qi::_a)
                         | rewardModelDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::rewardModels, qi::_a), qi::_1)] 
                         | labelDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::labels, qi::_a), qi::_1)] 
                         | formulaDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::formulas, qi::_a), qi::_1)]
                     )
                     > qi::eoi)[qi::_val = phoenix::bind(&PrismParser::createProgram, phoenix::ref(*this), qi::_a)];
            start.name("probabilistic program");
            
            // Enable error reporting.
            qi::on_error<qi::fail>(expression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(iteExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(orExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(andExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(equalityExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(relativeExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(plusExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(multiplicationExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(unaryExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(atomicExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(literalExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(identifierExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(minMaxExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(floorCeilExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            
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
        
        void PrismParser::moveToSecondRun() {
            this->secondRun = true;
        }
        
        void PrismParser::allowDoubleLiterals(bool flag) {
            this->allowDoubleLiteralsFlag = flag;
        }
        
        std::string const& PrismParser::getFilename() const {
            return this->filename;
        }
        
        bool PrismParser::isValidIdentifier(std::string const& identifier) {
            if (this->keywords_.find(identifier) != nullptr) {
                return false;
            }
            return true;
        }
        
        bool PrismParser::addInitialStatesConstruct(storm::expressions::Expression initialStatesExpression, GlobalProgramInformation& globalProgramInformation) {
            LOG_THROW(!globalProgramInformation.hasInitialConstruct, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Program must not define two initial constructs.");
            if (globalProgramInformation.hasInitialConstruct) {
                return false;
            }
            globalProgramInformation.hasInitialConstruct = true;
            globalProgramInformation.initialConstruct = storm::prism::InitialConstruct(initialStatesExpression, this->getFilename(), get_line(qi::_3));
            return true;
        }
        
        storm::expressions::Expression PrismParser::createIteExpression(storm::expressions::Expression e1, storm::expressions::Expression e2, storm::expressions::Expression e3) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1.ite(e2, e3);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createImpliesExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1.implies(e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createOrExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 || e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createAndExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try{
                    return e1 && e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createGreaterExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 > e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createGreaterOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 >= e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createLessExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 < e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createLessOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 <= e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    if (e1.hasBooleanReturnType() && e2.hasBooleanReturnType()) {
                        return e1.iff(e2);
                    } else {
                        return e1 == e2;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createNotEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    if (e1.hasBooleanReturnType() && e2.hasBooleanReturnType()) {
                        return e1 ^ e2;
                    } else {
                        return e1 != e2;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createPlusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 + e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createMinusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 - e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createMultExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 * e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createDivExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1 / e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createNotExpression(storm::expressions::Expression e1) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return !e1;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createMinusExpression(storm::expressions::Expression e1) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return -e1;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createTrueExpression() const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                return storm::expressions::Expression::createTrue();
            }
        }
        
        storm::expressions::Expression PrismParser::createFalseExpression() const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression PrismParser::createDoubleLiteralExpression(double value, bool& pass) const {
            // If we are not supposed to accept double expressions, we reject it by setting pass to false.
            if (!this->allowDoubleLiteralsFlag) {
                pass = false;
            }
            
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                return storm::expressions::Expression::createDoubleLiteral(value);
            }
        }
        
        storm::expressions::Expression PrismParser::createIntegerLiteralExpression(int value) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                return storm::expressions::Expression::createIntegerLiteral(static_cast<int_fast64_t>(value));
            }
        }
        
        storm::expressions::Expression PrismParser::createMinimumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return storm::expressions::Expression::minimum(e1, e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createMaximumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return storm::expressions::Expression::maximum(e1, e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createFloorExpression(storm::expressions::Expression e1) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1.floor();
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::createCeilExpression(storm::expressions::Expression e1) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                try {
                    return e1.ceil();
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": " << e.what() << ".");
                }
            }
        }
        
        storm::expressions::Expression PrismParser::getIdentifierExpression(std::string const& identifier) const {
            if (!this->secondRun) {
                return storm::expressions::Expression::createFalse();
            } else {
                storm::expressions::Expression const* expression = this->identifiers_.find(identifier);
                LOG_THROW(expression != nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Undeclared identifier '" << identifier << "'.");
                return *expression;
            }
        }
        
        storm::prism::Constant PrismParser::createUndefinedBooleanConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createBooleanConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Bool, newConstant, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createUndefinedIntegerConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createIntegerConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Int, newConstant, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createUndefinedDoubleConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createDoubleConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Double, newConstant, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedBooleanConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createBooleanConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Bool, newConstant, expression, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedIntegerConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createIntegerConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Int, newConstant, expression, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedDoubleConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(newConstant) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                this->identifiers_.add(newConstant, storm::expressions::Expression::createDoubleConstant(newConstant));
            }
            return storm::prism::Constant(storm::expressions::ExpressionReturnType::Double, newConstant, expression, this->getFilename());
        }
        
        storm::prism::Formula PrismParser::createFormula(std::string const& formulaName, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(formulaName) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << formulaName << "'.");
                this->identifiers_.add(formulaName, expression);
            }
            return storm::prism::Formula(formulaName, expression, this->getFilename());
        }
        
        storm::prism::Label PrismParser::createLabel(std::string const& labelName, storm::expressions::Expression expression) const {
            return storm::prism::Label(labelName, expression, this->getFilename());
        }
        
        storm::prism::RewardModel PrismParser::createRewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards) const {
            return storm::prism::RewardModel(rewardModelName, stateRewards, transitionRewards, this->getFilename());
        }
        
        storm::prism::StateReward PrismParser::createStateReward(storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const {
            return storm::prism::StateReward(statePredicateExpression, rewardValueExpression, this->getFilename());
        }
        
        storm::prism::TransitionReward PrismParser::createTransitionReward(std::string const& actionName, storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const {
            return storm::prism::TransitionReward(actionName, statePredicateExpression, rewardValueExpression, this->getFilename());
        }
        
        storm::prism::Assignment PrismParser::createAssignment(std::string const& variableName, storm::expressions::Expression assignedExpression) const {
            return storm::prism::Assignment(variableName, assignedExpression, this->getFilename());
        }
        
        storm::prism::Update PrismParser::createUpdate(storm::expressions::Expression likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, GlobalProgramInformation& globalProgramInformation) const {
            ++globalProgramInformation.currentUpdateIndex;
            return storm::prism::Update(globalProgramInformation.currentUpdateIndex - 1, likelihoodExpression, assignments, this->getFilename());
        }
        
        storm::prism::Command PrismParser::createCommand(std::string const& actionName, storm::expressions::Expression guardExpression, std::vector<storm::prism::Update> const& updates, GlobalProgramInformation& globalProgramInformation) const {
            ++globalProgramInformation.currentCommandIndex;
            return storm::prism::Command(globalProgramInformation.currentCommandIndex - 1, actionName, guardExpression, updates, this->getFilename());
        }
        
        storm::prism::BooleanVariable PrismParser::createBooleanVariable(std::string const& variableName, storm::expressions::Expression initialValueExpression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(variableName) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << variableName << "'.");
                this->identifiers_.add(variableName, storm::expressions::Expression::createBooleanVariable(variableName));
            }
            return storm::prism::BooleanVariable(variableName, initialValueExpression, this->getFilename());
        }
        
        storm::prism::IntegerVariable PrismParser::createIntegerVariable(std::string const& variableName, storm::expressions::Expression lowerBoundExpression, storm::expressions::Expression upperBoundExpression, storm::expressions::Expression initialValueExpression) const {
            if (!this->secondRun) {
                LOG_THROW(this->identifiers_.find(variableName) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << variableName << "'.");
                this->identifiers_.add(variableName, storm::expressions::Expression::createIntegerVariable(variableName));
            }
            return storm::prism::IntegerVariable(variableName, lowerBoundExpression, upperBoundExpression, initialValueExpression, this->getFilename());
        }
        
        storm::prism::Module PrismParser::createModule(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, GlobalProgramInformation& globalProgramInformation) const {
            globalProgramInformation.moduleToIndexMap[moduleName] = globalProgramInformation.modules.size();
            return storm::prism::Module(moduleName, booleanVariables, integerVariables, commands, this->getFilename());
        }
        
        storm::prism::Module PrismParser::createRenamedModule(std::string const& newModuleName, std::string const& oldModuleName, std::map<std::string, std::string> const& renaming, GlobalProgramInformation& globalProgramInformation) const {
            // Check whether the module to rename actually exists.
            auto const& moduleIndexPair = globalProgramInformation.moduleToIndexMap.find(oldModuleName);
            LOG_THROW(moduleIndexPair != globalProgramInformation.moduleToIndexMap.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": No module named '" << oldModuleName << "' to rename.");
            storm::prism::Module const& moduleToRename = globalProgramInformation.modules[moduleIndexPair->second];
            
            if (!this->secondRun) {
                // Register all (renamed) variables for later use.
                for (auto const& variable : moduleToRename.getBooleanVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Boolean variable '" << variable.getName() << " was not renamed.");
                    this->identifiers_.add(renamingPair->second, storm::expressions::Expression::createBooleanVariable(renamingPair->second));
                }
                for (auto const& variable : moduleToRename.getIntegerVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Integer variable '" << variable.getName() << " was not renamed.");
                    this->identifiers_.add(renamingPair->second, storm::expressions::Expression::createIntegerVariable(renamingPair->second));
                }
                
                // Return a dummy module in the first pass.
                return storm::prism::Module();
            } else {
                // Add a mapping from the new module name to its (future) index.
                globalProgramInformation.moduleToIndexMap[newModuleName] = globalProgramInformation.modules.size();
                
                // Create a mapping from identifiers to the expressions they need to be replaced with.
                std::map<std::string, storm::expressions::Expression> expressionRenaming;
                for (auto const& namePair : renaming) {
                    storm::expressions::Expression const* substitutedExpression = this->identifiers_.find(namePair.second);
                    // If the mapped-to-value is an expression, we need to replace it.
                    if (substitutedExpression != nullptr) {
                        expressionRenaming.emplace(namePair.first, *substitutedExpression);
                    }
                }
                
                // Rename the boolean variables.
                std::vector<storm::prism::BooleanVariable> booleanVariables;
                for (auto const& variable : moduleToRename.getBooleanVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Boolean variable '" << variable.getName() << " was not renamed.");
                    
                    booleanVariables.push_back(storm::prism::BooleanVariable(renamingPair->second, variable.getInitialValueExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1)));
                }
                
                // Rename the integer variables.
                std::vector<storm::prism::IntegerVariable> integerVariables;
                for (auto const& variable : moduleToRename.getIntegerVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Integer variable '" << variable.getName() << " was not renamed.");
                    
                    integerVariables.push_back(storm::prism::IntegerVariable(renamingPair->second, variable.getLowerBoundExpression().substitute(expressionRenaming), variable.getUpperBoundExpression().substitute(expressionRenaming), variable.getInitialValueExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1)));
                }
                
                // Rename commands.
                std::vector<storm::prism::Command> commands;
                for (auto const& command : moduleToRename.getCommands()) {
                    std::vector<storm::prism::Update> updates;
                    for (auto const& update : command.getUpdates()) {
                        std::vector<storm::prism::Assignment> assignments;
                        for (auto const& assignment : update.getAssignments()) {
                            auto const& renamingPair = renaming.find(assignment.getVariableName());
                            if (renamingPair != renaming.end()) {
                                assignments.emplace_back(renamingPair->second, assignment.getExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1));
                            } else {
                                assignments.emplace_back(assignment.getVariableName(), assignment.getExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1));
                            }
                        }
                        updates.emplace_back(globalProgramInformation.currentUpdateIndex, update.getLikelihoodExpression().substitute(expressionRenaming), assignments, this->getFilename(), get_line(qi::_1));
                        ++globalProgramInformation.currentUpdateIndex;
                    }
                    
                    std::string newActionName = command.getActionName();
                    auto const& renamingPair = renaming.find(command.getActionName());
                    if (renamingPair != renaming.end()) {
                        newActionName = renamingPair->second;
                    }
                    
                    commands.emplace_back(globalProgramInformation.currentCommandIndex, newActionName, command.getGuardExpression().substitute(expressionRenaming), updates, this->getFilename(), get_line(qi::_1));
                    ++globalProgramInformation.currentCommandIndex;
                }
                
                return storm::prism::Module(newModuleName, booleanVariables, integerVariables, commands, oldModuleName, renaming);
            }
        }
        
        storm::prism::Program PrismParser::createProgram(GlobalProgramInformation const& globalProgramInformation) const {
            return storm::prism::Program(globalProgramInformation.modelType, globalProgramInformation.constants, globalProgramInformation.globalBooleanVariables, globalProgramInformation.globalIntegerVariables, globalProgramInformation.formulas, globalProgramInformation.modules, globalProgramInformation.rewardModels, this->secondRun && !globalProgramInformation.hasInitialConstruct, globalProgramInformation.initialConstruct, globalProgramInformation.labels, this->getFilename(), 1, this->secondRun);
        }
    } // namespace parser
} // namespace storm
