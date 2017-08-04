#include "storm/parser/PrismParser.h"

#include "storm/storage/prism/Compositions.h"

#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidTypeException.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"
#include "storm/exceptions/WrongFormatException.h"

#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/parser/ExpressionParser.h"

namespace storm {
    namespace parser {
        storm::prism::Program PrismParser::parse(std::string const& filename) {
            // Open file and initialize result.
            std::ifstream inputFileStream;
            storm::utility::openFile(filename, inputFileStream);
            storm::prism::Program result;
            
            // Now try to parse the contents of the file.
            try {
                std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
                result = parseFromString(fileContent, filename);
            } catch(std::exception& e) {
                // In case of an exception properly close the file before passing exception.
                storm::utility::closeFile(inputFileStream);
                throw e;
            }
            
            // Close the stream in case everything went smoothly and return result.
            storm::utility::closeFile(inputFileStream);
            return result;
        }
        
        storm::prism::Program PrismParser::parseFromString(std::string const& input, std::string const& filename) {
            PositionIteratorType first(input.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(input.end());
            STORM_LOG_ASSERT(first != last, "Illegal input to PRISM parser.");
            
            // Create empty result;
            storm::prism::Program result;
            
            // Create grammar.
            storm::parser::PrismParser grammar(filename, first);
            try {
                // Start first run.
                bool succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in first pass.");
                STORM_LOG_DEBUG("First pass of parsing PRISM input finished.");
                
                // Start second run.
                first = PositionIteratorType(input.begin());
                iter = first;
                last = PositionIteratorType(input.end());
                grammar.moveToSecondRun();
                succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in second pass.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                // If the parser expected content different than the one provided, display information about the location of the error.
                std::size_t lineNumber = boost::spirit::get_line(e.first);
                
                // Now propagate exception.
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << lineNumber << " of file " << filename << ".");
            }
            
            STORM_LOG_TRACE("Parsed PRISM input: " << result);
            
            return result;
        }
        
        PrismParser::PrismParser(std::string const& filename, Iterator first) : PrismParser::base_type(start), secondRun(false), filename(filename), annotate(first), manager(new storm::expressions::ExpressionManager()), expressionParser(new ExpressionParser(*manager, keywords_, false, false)) {
            
            ExpressionParser& expression_ = *expressionParser;
            // Parse simple identifier.
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&PrismParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            modelTypeDefinition %= modelType_;
            modelTypeDefinition.name("model type");
            
            undefinedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedBooleanConstant, phoenix::ref(*this), qi::_1)];
            undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
            
            undefinedIntegerConstantDefinition = ((qi::lit("const") >> -qi::lit("int")) >> identifier >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedIntegerConstant, phoenix::ref(*this), qi::_1)];
            undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
            
            undefinedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double")) > identifier > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createUndefinedDoubleConstant, phoenix::ref(*this), qi::_1)];
            undefinedDoubleConstantDefinition.name("undefined double constant definition");
            
            undefinedConstantDefinition = (undefinedBooleanConstantDefinition | undefinedDoubleConstantDefinition | undefinedIntegerConstantDefinition);
            undefinedConstantDefinition.name("undefined constant definition");
            
            definedBooleanConstantDefinition = ((qi::lit("const") >> qi::lit("bool") >> identifier >> qi::lit("=")) > expression_ > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedBooleanConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedBooleanConstantDefinition.name("defined boolean constant declaration");
            
            definedIntegerConstantDefinition = ((qi::lit("const") >> -qi::lit("int") >> identifier >> qi::lit("=")) > expression_ >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedIntegerConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedIntegerConstantDefinition.name("defined integer constant declaration");
            
            definedDoubleConstantDefinition = ((qi::lit("const") >> qi::lit("double") >> identifier >> qi::lit("=")) > expression_ > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDefinedDoubleConstant, phoenix::ref(*this), qi::_1, qi::_2)];
            definedDoubleConstantDefinition.name("defined double constant declaration");
            
            definedConstantDefinition %= (definedBooleanConstantDefinition | definedDoubleConstantDefinition | definedIntegerConstantDefinition);
            definedConstantDefinition.name("defined constant definition");
            
            formulaDefinition = (qi::lit("formula") > identifier > qi::lit("=") > expression_ > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            formulaDefinition.name("formula definition");
            
            booleanVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("bool")) > -((qi::lit("init") > expression_[qi::_a = qi::_1]) | qi::attr(manager->boolean(false))) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createBooleanVariable, phoenix::ref(*this), qi::_1, qi::_a)];
            booleanVariableDefinition.name("boolean variable definition");
            
            integerVariableDefinition = ((identifier >> qi::lit(":") >> qi::lit("[")[phoenix::bind(&PrismParser::allowDoubleLiterals, phoenix::ref(*this), false)]) > expression_ > qi::lit("..") > expression_ > qi::lit("]")[phoenix::bind(&PrismParser::allowDoubleLiterals, phoenix::ref(*this), true)] > -(qi::lit("init") > expression_[qi::_a = qi::_1]) > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createIntegerVariable, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_a)];
            integerVariableDefinition.name("integer variable definition");
            
            variableDefinition = (booleanVariableDefinition[phoenix::push_back(qi::_r1, qi::_1)] | integerVariableDefinition[phoenix::push_back(qi::_r2, qi::_1)]);
            variableDefinition.name("variable declaration");
            
            globalVariableDefinition = (qi::lit("global") > (booleanVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalBooleanVariables, qi::_r1), qi::_1)] | integerVariableDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::globalIntegerVariables, qi::_r1), qi::_1)]));
            globalVariableDefinition.name("global variable declaration list");
                        
            stateRewardDefinition = (expression_ > qi::lit(":") > expression_ >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createStateReward, phoenix::ref(*this), qi::_1, qi::_2)];
            stateRewardDefinition.name("state reward definition");
            
            stateActionRewardDefinition = (qi::lit("[") >> -identifier >> qi::lit("]") >> expression_ >> qi::lit(":") >> expression_ >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createStateActionReward, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_r1)];
            stateActionRewardDefinition.name("state action reward definition");

            transitionRewardDefinition = (qi::lit("[") > -identifier > qi::lit("]") > expression_ > qi::lit("->") > expression_ > qi::lit(":") > expression_ > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createTransitionReward, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_4, qi::_r1)];
            transitionRewardDefinition.name("transition reward definition");
            
            rewardModelDefinition = (qi::lit("rewards") > -(qi::lit("\"") > identifier[qi::_a = qi::_1] > qi::lit("\""))
                                     > +(   stateRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]
                                         |  stateActionRewardDefinition(qi::_r1)[phoenix::push_back(qi::_c, qi::_1)]
                                         |  transitionRewardDefinition(qi::_r1)[phoenix::push_back(qi::_d, qi::_1)]
                                         )
                                     >> qi::lit("endrewards"))[qi::_val = phoenix::bind(&PrismParser::createRewardModel, phoenix::ref(*this), qi::_a, qi::_b, qi::_c, qi::_d)];
            rewardModelDefinition.name("reward model definition");
            
            initialStatesConstruct = (qi::lit("init") > expression_ > qi::lit("endinit"))[qi::_pass = phoenix::bind(&PrismParser::addInitialStatesConstruct, phoenix::ref(*this), qi::_1, qi::_r1)];
            initialStatesConstruct.name("initial construct");
            
            systemCompositionConstruct = (qi::lit("system") > parallelComposition > qi::lit("endsystem"))[phoenix::bind(&PrismParser::addSystemCompositionConstruct, phoenix::ref(*this), qi::_1, qi::_r1)];
            systemCompositionConstruct.name("system composition construct");
            
            actionNameList %= identifier[phoenix::insert(qi::_val, qi::_1)] >> *("," >> identifier[phoenix::insert(qi::_val, qi::_1)]);
            actionNameList.name("action list");
            
            parallelComposition = hidingOrRenamingComposition[qi::_val = qi::_1] >> *((interleavingParallelComposition > hidingOrRenamingComposition)[qi::_val = phoenix::bind(&PrismParser::createInterleavingParallelComposition, phoenix::ref(*this), qi::_val, qi::_1)] |
                                                                                      (synchronizingParallelComposition > hidingOrRenamingComposition)[qi::_val = phoenix::bind(&PrismParser::createSynchronizingParallelComposition, phoenix::ref(*this), qi::_val, qi::_1)] |
                                                                                      (restrictedParallelComposition > hidingOrRenamingComposition)[qi::_val = phoenix::bind(&PrismParser::createRestrictedParallelComposition, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)]
                                                                                      );
            parallelComposition.name("parallel composition");

            synchronizingParallelComposition = qi::lit("||");
            synchronizingParallelComposition.name("synchronizing parallel composition");
            
            interleavingParallelComposition = qi::lit("|||");
            interleavingParallelComposition.name("interleaving parallel composition");

            restrictedParallelComposition = qi::lit("|[") > actionNameList > qi::lit("]|");
            restrictedParallelComposition.name("restricted parallel composition");
            
            hidingOrRenamingComposition = hidingComposition | renamingComposition | atomicComposition;
            hidingOrRenamingComposition.name("hiding/renaming composition");
            
            hidingComposition = (atomicComposition >> (qi::lit("/") > (qi::lit("{") > actionNameList > qi::lit("}"))))[qi::_val = phoenix::bind(&PrismParser::createHidingComposition, phoenix::ref(*this), qi::_1, qi::_2)];
            hidingComposition.name("hiding composition");
            
            actionRenamingList = +(identifier >> (qi::lit("<-") >> identifier))[phoenix::insert(qi::_val, phoenix::construct<std::pair<std::string, std::string>>(qi::_1, qi::_2))];
            actionRenamingList.name("action renaming list");
            
            renamingComposition = (atomicComposition >> (qi::lit("{") > (actionRenamingList > qi::lit("}"))))[qi::_val = phoenix::bind(&PrismParser::createRenamingComposition, phoenix::ref(*this), qi::_1, qi::_2)];
            renamingComposition.name("renaming composition");
            
            atomicComposition = (qi::lit("(") > parallelComposition > qi::lit(")")) | moduleComposition;
            atomicComposition.name("atomic composition");
            
            moduleComposition = identifier[qi::_val = phoenix::bind(&PrismParser::createModuleComposition, phoenix::ref(*this), qi::_1)];
            moduleComposition.name("module composition");
            
            labelDefinition = (qi::lit("label") > -qi::lit("\"") > identifier > -qi::lit("\"") > qi::lit("=") > expression_ >> qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createLabel, phoenix::ref(*this), qi::_1, qi::_2)];
            labelDefinition.name("label definition");
            
            assignmentDefinition = (qi::lit("(") > identifier > qi::lit("'") > qi::lit("=") > expression_ > qi::lit(")"))[qi::_val = phoenix::bind(&PrismParser::createAssignment, phoenix::ref(*this), qi::_1, qi::_2)];
            assignmentDefinition.name("assignment");
            
            assignmentDefinitionList = (assignmentDefinition % "&")[qi::_val = qi::_1] | (qi::lit("true"))[qi::_val = phoenix::construct<std::vector<storm::prism::Assignment>>()];
            assignmentDefinitionList.name("assignment list");
            
            updateDefinition = (((expression_ >> qi::lit(":")) | qi::attr(manager->rational(1))) >> assignmentDefinitionList)[qi::_val = phoenix::bind(&PrismParser::createUpdate, phoenix::ref(*this), qi::_1, qi::_2, qi::_r1)];
            updateDefinition.name("update");
            
            updateListDefinition %= +updateDefinition(qi::_r1) % "+";
            updateListDefinition.name("update list");
            
            // This is a dummy command-definition (it ignores the actual contents of the command) that is overwritten when the parser is moved to the second run.
            commandDefinition = (((qi::lit("[") > -identifier > qi::lit("]"))
                                  |
                                 (qi::lit("<") > -identifier > qi::lit(">")[qi::_a = true]))
                                 > +(qi::char_ - qi::lit(";"))
                                 > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createDummyCommand, phoenix::ref(*this), qi::_1, qi::_r1)];
            commandDefinition.name("command definition");
            
            moduleDefinition = ((qi::lit("module") >> identifier >> *(variableDefinition(qi::_a, qi::_b))) > *commandDefinition(qi::_r1) > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismParser::createModule, phoenix::ref(*this), qi::_1, qi::_a, qi::_b, qi::_2, qi::_r1)];
            moduleDefinition.name("module definition");
            
            moduleRenaming = ((qi::lit("module") >> identifier >> qi::lit("=")) > identifier > qi::lit("[")
                              > ((identifier > qi::lit("=") > identifier)[phoenix::insert(qi::_a, phoenix::construct<std::pair<std::string,std::string>>(qi::_1, qi::_2))] % ",") > qi::lit("]")
                              > qi::lit("endmodule"))[qi::_val = phoenix::bind(&PrismParser::createRenamedModule, phoenix::ref(*this), qi::_1, qi::_2, qi::_a, qi::_r1)];
            moduleRenaming.name("module definition via renaming");
            
            moduleDefinitionList %= +(moduleRenaming(qi::_r1) | moduleDefinition(qi::_r1))[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::modules, qi::_r1), qi::_1)];
            moduleDefinitionList.name("module list");
            
            start = (qi::eps[phoenix::bind(&PrismParser::removeInitialConstruct, phoenix::ref(*this), qi::_a)]
                     > *(modelTypeDefinition[phoenix::bind(&PrismParser::setModelType, phoenix::ref(*this), qi::_a, qi::_1)]
                         | definedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_a), qi::_1)]
                         | undefinedConstantDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::constants, qi::_a), qi::_1)]
                         | formulaDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::formulas, qi::_a), qi::_1)]
                         | globalVariableDefinition(qi::_a)
                         | (moduleRenaming(qi::_a) | moduleDefinition(qi::_a))[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::modules, qi::_a), qi::_1)]
                         | initialStatesConstruct(qi::_a)
                         | rewardModelDefinition(qi::_a)[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::rewardModels, qi::_a), qi::_1)]
                         | labelDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::labels, qi::_a), qi::_1)] 
                         | formulaDefinition[phoenix::push_back(phoenix::bind(&GlobalProgramInformation::formulas, qi::_a), qi::_1)]
                     )
                     > -(systemCompositionConstruct(qi::_a)) > qi::eoi)[qi::_val = phoenix::bind(&PrismParser::createProgram, phoenix::ref(*this), qi::_a)];
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
            
            // Enable error reporting.
            qi::on_error<qi::fail>(start, handler(qi::_1, qi::_2, qi::_3, qi::_4));
        }
        
        void PrismParser::moveToSecondRun() {
            // In the second run, we actually need to parse the commands instead of just skipping them,
            // so we adapt the rule for parsing commands.
            commandDefinition = (((qi::lit("[") > -identifier > qi::lit("]"))
                                 |
                                  (qi::lit("<") > -identifier > qi::lit(">")[qi::_a = true]))
                                 > *expressionParser
                                 > qi::lit("->")
                                 > updateListDefinition(qi::_r1)
                                 > qi::lit(";"))[qi::_val = phoenix::bind(&PrismParser::createCommand, phoenix::ref(*this), qi::_a, qi::_1, qi::_2, qi::_3, qi::_r1)];
            
            this->secondRun = true;
            this->expressionParser->setIdentifierMapping(&this->identifiers_);
        }
        
        void PrismParser::allowDoubleLiterals(bool flag) {
            this->expressionParser->setAcceptDoubleLiterals(flag);
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
        
        bool PrismParser::addInitialStatesConstruct(storm::expressions::Expression const& initialStatesExpression, GlobalProgramInformation& globalProgramInformation) {
            STORM_LOG_THROW(!globalProgramInformation.hasInitialConstruct, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Program must not define two initial constructs.");
            if (globalProgramInformation.hasInitialConstruct) {
                return false;
            }
            globalProgramInformation.hasInitialConstruct = true;
            globalProgramInformation.initialConstruct = storm::prism::InitialConstruct(initialStatesExpression, this->getFilename(), get_line(qi::_3));
            return true;
        }
        
        bool PrismParser::addSystemCompositionConstruct(std::shared_ptr<storm::prism::Composition> const& composition, GlobalProgramInformation& globalProgramInformation) {
            globalProgramInformation.systemCompositionConstruct = storm::prism::SystemCompositionConstruct(composition, this->getFilename(), get_line(qi::_3));
            return true;
        }
        
        void PrismParser::setModelType(GlobalProgramInformation& globalProgramInformation, storm::prism::Program::ModelType const& modelType) {
            STORM_LOG_THROW(globalProgramInformation.modelType == storm::prism::Program::ModelType::UNDEFINED, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Program must not set model type multiple times.");
            globalProgramInformation.modelType = modelType;
        }

        std::shared_ptr<storm::prism::Composition> PrismParser::createModuleComposition(std::string const& moduleName) const {
            return std::make_shared<storm::prism::ModuleComposition>(moduleName);
        }
        
        std::shared_ptr<storm::prism::Composition> PrismParser::createRenamingComposition(std::shared_ptr<storm::prism::Composition> const& subcomposition, std::map<std::string, std::string> const& renaming) const {
            return std::make_shared<storm::prism::RenamingComposition>(subcomposition, renaming);
        }
        
        std::shared_ptr<storm::prism::Composition> PrismParser::createHidingComposition(std::shared_ptr<storm::prism::Composition> const& subcomposition, std::set<std::string> const& actionsToHide) const {
            return std::make_shared<storm::prism::HidingComposition>(subcomposition, actionsToHide);
        }
        
        std::shared_ptr<storm::prism::Composition> PrismParser::createSynchronizingParallelComposition(std::shared_ptr<storm::prism::Composition> const& left, std::shared_ptr<storm::prism::Composition> const& right) const {
            return std::make_shared<storm::prism::SynchronizingParallelComposition>(left, right);
        }
        
        std::shared_ptr<storm::prism::Composition> PrismParser::createInterleavingParallelComposition(std::shared_ptr<storm::prism::Composition> const& left, std::shared_ptr<storm::prism::Composition> const& right) const {
            return std::make_shared<storm::prism::InterleavingParallelComposition>(left, right);
        }
        
        std::shared_ptr<storm::prism::Composition> PrismParser::createRestrictedParallelComposition(std::shared_ptr<storm::prism::Composition> const& left, std::set<std::string> const& synchronizingActions, std::shared_ptr<storm::prism::Composition> const& right) const {
            return std::make_shared<storm::prism::RestrictedParallelComposition>(left, synchronizingActions, right);
        }
        
        storm::prism::Constant PrismParser::createUndefinedBooleanConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareBooleanVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createUndefinedIntegerConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareIntegerVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createUndefinedDoubleConstant(std::string const& newConstant) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareRationalVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedBooleanConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareBooleanVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), expression, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedIntegerConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareIntegerVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), expression, this->getFilename());
        }
        
        storm::prism::Constant PrismParser::createDefinedDoubleConstant(std::string const& newConstant, storm::expressions::Expression expression) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareRationalVariable(newConstant, true);
                    this->identifiers_.add(newConstant, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(newConstant)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << newConstant << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << newConstant << "'.");
                    }
                }
            }
            return storm::prism::Constant(manager->getVariable(newConstant), expression, this->getFilename());
        }
        
        storm::prism::Formula PrismParser::createFormula(std::string const& formulaName, storm::expressions::Expression expression) {
            // Only register formula in second run. This prevents the parser from accepting formulas that depend on future
            // formulas.
            if (this->secondRun) {
                STORM_LOG_THROW(this->identifiers_.find(formulaName) == nullptr, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << formulaName << "'.");
                this->identifiers_.add(formulaName, expression);
            }
            return storm::prism::Formula(formulaName, expression, this->getFilename());
        }
        
        storm::prism::Label PrismParser::createLabel(std::string const& labelName, storm::expressions::Expression expression) const {
            return storm::prism::Label(labelName, expression, this->getFilename());
        }
        
        storm::prism::RewardModel PrismParser::createRewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::StateActionReward> const& stateActionRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards) const {
            return storm::prism::RewardModel(rewardModelName, stateRewards, stateActionRewards, transitionRewards, this->getFilename());
        }
        
        storm::prism::StateReward PrismParser::createStateReward(storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const {
            if (this->secondRun) {
                return storm::prism::StateReward(statePredicateExpression, rewardValueExpression, this->getFilename());
            } else {
                return storm::prism::StateReward();
            }
        }
        
        storm::prism::StateActionReward PrismParser::createStateActionReward(boost::optional<std::string> const& actionName, storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression, GlobalProgramInformation& globalProgramInformation) const {
            if (this->secondRun) {
                std::string realActionName = actionName ? actionName.get() : "";
                
                auto const& nameIndexPair = globalProgramInformation.actionIndices.find(realActionName);
                STORM_LOG_THROW(nameIndexPair != globalProgramInformation.actionIndices.end(), storm::exceptions::WrongFormatException, "Transition reward refers to illegal action '" << realActionName << "'.");
                return storm::prism::StateActionReward(nameIndexPair->second, realActionName, statePredicateExpression, rewardValueExpression, this->getFilename());
            } else {
                return storm::prism::StateActionReward();
            }
        }
        
        storm::prism::TransitionReward PrismParser::createTransitionReward(boost::optional<std::string> const& actionName, storm::expressions::Expression sourceStatePredicateExpression, storm::expressions::Expression targetStatePredicateExpression, storm::expressions::Expression rewardValueExpression, GlobalProgramInformation& globalProgramInformation) const {
            if (this->secondRun) {
                std::string realActionName = actionName ? actionName.get() : "";
    
                auto const& nameIndexPair = globalProgramInformation.actionIndices.find(realActionName);
                STORM_LOG_THROW(nameIndexPair != globalProgramInformation.actionIndices.end(), storm::exceptions::WrongFormatException, "Transition reward refers to illegal action '" << realActionName << "'.");
                return storm::prism::TransitionReward(nameIndexPair->second, realActionName, sourceStatePredicateExpression, targetStatePredicateExpression, rewardValueExpression, this->getFilename());
            } else {
                return storm::prism::TransitionReward();
            }
        }
        
        storm::prism::Assignment PrismParser::createAssignment(std::string const& variableName, storm::expressions::Expression assignedExpression) const {
            return storm::prism::Assignment(manager->getVariable(variableName), assignedExpression, this->getFilename());
        }
        
        storm::prism::Update PrismParser::createUpdate(storm::expressions::Expression likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, GlobalProgramInformation& globalProgramInformation) const {
            ++globalProgramInformation.currentUpdateIndex;
            return storm::prism::Update(globalProgramInformation.currentUpdateIndex - 1, likelihoodExpression, assignments, this->getFilename());
        }
        
        storm::prism::Command PrismParser::createCommand(bool markovian, boost::optional<std::string> const& actionName, storm::expressions::Expression guardExpression, std::vector<storm::prism::Update> const& updates, GlobalProgramInformation& globalProgramInformation) const {
            ++globalProgramInformation.currentCommandIndex;
            std::string realActionName = actionName ? actionName.get() : "";

            uint_fast64_t actionIndex = 0;
            
            // If the action name was not yet seen, record it.
            auto nameIndexPair = globalProgramInformation.actionIndices.find(realActionName);
            if (nameIndexPair == globalProgramInformation.actionIndices.end()) {
                std::size_t nextIndex = globalProgramInformation.actionIndices.size();
                globalProgramInformation.actionIndices.emplace(realActionName, nextIndex);
                actionIndex = nextIndex;
            } else {
                actionIndex = nameIndexPair->second;
            }
            return storm::prism::Command(globalProgramInformation.currentCommandIndex - 1, markovian, actionIndex, realActionName, guardExpression, updates, this->getFilename());
        }
        
        storm::prism::Command PrismParser::createDummyCommand(boost::optional<std::string> const& actionName, GlobalProgramInformation& globalProgramInformation) const {
            STORM_LOG_ASSERT(!this->secondRun, "Dummy procedure must not be called in second run.");
            std::string realActionName = actionName ? actionName.get() : "";

            // Register the action name if it has not appeared earlier.
            auto nameIndexPair = globalProgramInformation.actionIndices.find(realActionName);
            if (nameIndexPair == globalProgramInformation.actionIndices.end()) {
                std::size_t nextIndex = globalProgramInformation.actionIndices.size();
                globalProgramInformation.actionIndices.emplace(realActionName, nextIndex);
            }
            
            return storm::prism::Command();
        }
        
        storm::prism::BooleanVariable PrismParser::createBooleanVariable(std::string const& variableName, storm::expressions::Expression initialValueExpression) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareBooleanVariable(variableName);
                    this->identifiers_.add(variableName, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(variableName)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << variableName << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << variableName << "'.");
                    }
                }
            }
            return storm::prism::BooleanVariable(manager->getVariable(variableName), initialValueExpression, this->getFilename());
        }
        
        storm::prism::IntegerVariable PrismParser::createIntegerVariable(std::string const& variableName, storm::expressions::Expression lowerBoundExpression, storm::expressions::Expression upperBoundExpression, storm::expressions::Expression initialValueExpression) const {
            if (!this->secondRun) {
                try {
                    storm::expressions::Variable newVariable = manager->declareIntegerVariable(variableName);
                    this->identifiers_.add(variableName, newVariable.getExpression());
                } catch (storm::exceptions::InvalidArgumentException const& e) {
                    if (manager->hasVariable(variableName)) {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Duplicate identifier '" << variableName << "'.");
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": illegal identifier '" << variableName << "'.");
                    }
                }
            }
            return storm::prism::IntegerVariable(manager->getVariable(variableName), lowerBoundExpression, upperBoundExpression, initialValueExpression, this->getFilename());
        }
        
        storm::prism::Module PrismParser::createModule(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, GlobalProgramInformation& globalProgramInformation) const {
            globalProgramInformation.moduleToIndexMap[moduleName] = globalProgramInformation.modules.size();
            return storm::prism::Module(moduleName, booleanVariables, integerVariables, commands, this->getFilename());
        }
        
        storm::prism::Module PrismParser::createRenamedModule(std::string const& newModuleName, std::string const& oldModuleName, std::map<std::string, std::string> const& renaming, GlobalProgramInformation& globalProgramInformation) const {
            // Check whether the module to rename actually exists.
            auto const& moduleIndexPair = globalProgramInformation.moduleToIndexMap.find(oldModuleName);
            STORM_LOG_THROW(moduleIndexPair != globalProgramInformation.moduleToIndexMap.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": No module named '" << oldModuleName << "' to rename.");
            storm::prism::Module const& moduleToRename = globalProgramInformation.modules[moduleIndexPair->second];
            
            if (!this->secondRun) {
                // Register all (renamed) variables for later use.
                for (auto const& variable : moduleToRename.getBooleanVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    STORM_LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Boolean variable '" << variable.getName() << " was not renamed.");
                    storm::expressions::Variable renamedVariable = manager->declareBooleanVariable(renamingPair->second);
                    this->identifiers_.add(renamingPair->second, renamedVariable.getExpression());
                }
                for (auto const& variable : moduleToRename.getIntegerVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    STORM_LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Integer variable '" << variable.getName() << " was not renamed.");
                    storm::expressions::Variable renamedVariable = manager->declareIntegerVariable(renamingPair->second);
                    this->identifiers_.add(renamingPair->second, renamedVariable.getExpression());
                }
                
                for (auto const& command : moduleToRename.getCommands()) {
                    std::string newActionName = command.getActionName();
                    auto const& renamingPair = renaming.find(command.getActionName());
                    if (renamingPair != renaming.end()) {
                        newActionName = renamingPair->second;
                    }

                    // Record any newly occurring action names/indices.
                    auto nameIndexPair = globalProgramInformation.actionIndices.find(newActionName);
                    if (nameIndexPair == globalProgramInformation.actionIndices.end()) {
                        std::size_t nextIndex = globalProgramInformation.actionIndices.size();
                        globalProgramInformation.actionIndices.emplace(newActionName, nextIndex);
                    }
                }
                
                // Return a dummy module in the first pass.
                return storm::prism::Module();
            } else {
                // Add a mapping from the new module name to its (future) index.
                globalProgramInformation.moduleToIndexMap[newModuleName] = globalProgramInformation.modules.size();
                
                // Create a mapping from identifiers to the expressions they need to be replaced with.
                std::map<storm::expressions::Variable, storm::expressions::Expression> expressionRenaming;
                for (auto const& namePair : renaming) {
                    storm::expressions::Expression const* substitutedExpression = this->identifiers_.find(namePair.second);
                    // If the mapped-to-value is an expression, we need to replace it.
                    if (substitutedExpression != nullptr) {
                        expressionRenaming.emplace(manager->getVariable(namePair.first), *substitutedExpression);
                    }
                }
                
                // Rename the boolean variables.
                std::vector<storm::prism::BooleanVariable> booleanVariables;
                for (auto const& variable : moduleToRename.getBooleanVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    STORM_LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Boolean variable '" << variable.getName() << " was not renamed.");
                    
                    booleanVariables.push_back(storm::prism::BooleanVariable(manager->getVariable(renamingPair->second), variable.hasInitialValue() ? variable.getInitialValueExpression().substitute(expressionRenaming) : variable.getInitialValueExpression(), this->getFilename(), get_line(qi::_1)));
                }
                
                // Rename the integer variables.
                std::vector<storm::prism::IntegerVariable> integerVariables;
                for (auto const& variable : moduleToRename.getIntegerVariables()) {
                    auto const& renamingPair = renaming.find(variable.getName());
                    STORM_LOG_THROW(renamingPair != renaming.end(), storm::exceptions::WrongFormatException, "Parsing error in " << this->getFilename() << ", line " << get_line(qi::_3) << ": Integer variable '" << variable.getName() << " was not renamed.");
                    
                    integerVariables.push_back(storm::prism::IntegerVariable(manager->getVariable(renamingPair->second), variable.getLowerBoundExpression().substitute(expressionRenaming), variable.getUpperBoundExpression().substitute(expressionRenaming), variable.hasInitialValue() ? variable.getInitialValueExpression().substitute(expressionRenaming) : variable.getInitialValueExpression(), this->getFilename(), get_line(qi::_1)));
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
                                assignments.emplace_back(manager->getVariable(renamingPair->second), assignment.getExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1));
                            } else {
                                assignments.emplace_back(assignment.getVariable(), assignment.getExpression().substitute(expressionRenaming), this->getFilename(), get_line(qi::_1));
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
                    
                    uint_fast64_t actionIndex = 0;
                    auto nameIndexPair = globalProgramInformation.actionIndices.find(newActionName);
                    if (nameIndexPair == globalProgramInformation.actionIndices.end()) {
                        std::size_t nextIndex = globalProgramInformation.actionIndices.size();
                        globalProgramInformation.actionIndices.emplace(newActionName, nextIndex);
                        actionIndex = nextIndex;
                    } else {
                        actionIndex = nameIndexPair->second;
                    }
                    
                    commands.emplace_back(globalProgramInformation.currentCommandIndex, command.isMarkovian(), actionIndex, newActionName, command.getGuardExpression().substitute(expressionRenaming), updates, this->getFilename(), get_line(qi::_1));
                    ++globalProgramInformation.currentCommandIndex;
                }
                
                return storm::prism::Module(newModuleName, booleanVariables, integerVariables, commands, oldModuleName, renaming);
            }
        }
        
        storm::prism::Program PrismParser::createProgram(GlobalProgramInformation const& globalProgramInformation) const {
            storm::prism::Program::ModelType finalModelType = globalProgramInformation.modelType;
            if (globalProgramInformation.modelType == storm::prism::Program::ModelType::UNDEFINED) {
                STORM_LOG_WARN("Program does not specify model type. Implicitly assuming 'mdp'.");
                finalModelType = storm::prism::Program::ModelType::MDP;
            }
            
            return storm::prism::Program(manager, finalModelType, globalProgramInformation.constants, globalProgramInformation.globalBooleanVariables, globalProgramInformation.globalIntegerVariables, globalProgramInformation.formulas, globalProgramInformation.modules, globalProgramInformation.actionIndices, globalProgramInformation.rewardModels, globalProgramInformation.labels, secondRun && !globalProgramInformation.hasInitialConstruct ? boost::none : boost::make_optional(globalProgramInformation.initialConstruct), globalProgramInformation.systemCompositionConstruct, this->getFilename(), 1, this->secondRun);
        }
        
        void PrismParser::removeInitialConstruct(GlobalProgramInformation& globalProgramInformation) const {
            globalProgramInformation.hasInitialConstruct = false;
        }
    } // namespace parser
} // namespace storm
