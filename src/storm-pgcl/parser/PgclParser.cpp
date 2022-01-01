#include "PgclParser.h"
// If the parser fails due to ill-formed data, this exception is thrown.
#include "storm/exceptions/WrongFormatException.h"
#include "storm/io/file.h"

namespace storm {
namespace parser {
storm::pgcl::PgclProgram PgclParser::parse(std::string const& filename) {
    // Create empty program.
    storm::pgcl::PgclProgram result;

    // Open file and initialize result.
    std::ifstream inputFileStream;
    storm::utility::openFile(filename, inputFileStream);

    // Now try to parse the contents of the file.
    try {
        std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
        result = parseFromString(fileContent, filename);
    } catch (std::exception& e) {
        // In case of an exception properly close the file before passing exception.
        storm::utility::closeFile(inputFileStream);
        throw e;
    }

    // Close the stream in case everything went smoothly and return result.
    storm::utility::closeFile(inputFileStream);
    return result;
}

storm::pgcl::PgclProgram PgclParser::parseFromString(std::string const& input, std::string const& filename) {
    PositionIteratorType first(input.begin());
    PositionIteratorType iter = first;
    PositionIteratorType last(input.end());

    // Create empty program.
    storm::pgcl::PgclProgram result;

    // Create grammar.
    storm::parser::PgclParser grammar(filename, first);
    try {
        // Start the parsing run.
        bool succeeded = qi::phrase_parse(
            iter, last, grammar, storm::spirit_encoding::space_type() | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
        STORM_LOG_THROW(succeeded, storm::exceptions::WrongFormatException, "Parsing of PGCL program failed.");
        STORM_LOG_DEBUG("Parse of PGCL program finished.");
    } catch (qi::expectation_failure<PositionIteratorType> const& e) {
        // If the parser expected content different than the one provided, display information about the location of the error.
        std::size_t lineNumber = boost::spirit::get_line(e.first);
        // Now propagate exception.
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << lineNumber << " of file " << filename << ".");
    }

    return result;
}

PgclParser::PgclParser(std::string const& filename, Iterator first)
    : PgclParser::base_type(program, "PGCL grammar"),
      annotate(first),
      expressionManager(std::shared_ptr<storm::expressions::ExpressionManager>(new storm::expressions::ExpressionManager())),
      expressionParser(*expressionManager, invalidIdentifiers) {
    this->enableExpressions();
    /*
     * PGCL grammar is defined here
     */
    // Rough program structure
    program = (qi::lit("function ") > programName > qi::lit("(") > -(doubleDeclaration % qi::lit(",")) > qi::lit(")") > qi::lit("{") > variableDeclarations >
               sequenceOfStatements > qi::lit("}"))[qi::_val = phoenix::bind(&PgclParser::createProgram, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_4)];
    sequenceOfStatements %= +(statement);
    sequenceOfStatements.name("sequence of statements");

    variableDeclarations %= qi::lit("var") > qi::lit("{") > +(integerDeclaration | booleanDeclaration) > qi::lit("}");
    variableDeclarations.name("variable declarations");

    // Statements
    statement %= simpleStatement | compoundStatement;
    simpleStatement %= assignmentStatement | observeStatement;
    compoundStatement %= ifElseStatement | loopStatement | branchStatement;

    // Simple statements
    doubleDeclaration = (qi::lit("double ") >> variableName)[qi::_val = phoenix::bind(&PgclParser::declareDoubleVariable, phoenix::ref(*this), qi::_1)];
    integerDeclaration = (qi::lit("int ") > variableName > qi::lit(":=") > expression >
                          qi::lit(";"))[qi::_val = phoenix::bind(&PgclParser::createIntegerDeclarationStatement, phoenix::ref(*this), qi::_1, qi::_2)];
    integerDeclaration.name("integer declaration");
    booleanDeclaration = (qi::lit("bool ") > variableName > qi::lit(":=") > expression >
                          qi::lit(";"))[qi::_val = phoenix::bind(&PgclParser::createBooleanDeclarationStatement, phoenix::ref(*this), qi::_1, qi::_2)];
    booleanDeclaration.name("boolean declaration");

    assignmentStatement = (variableName > qi::lit(":=") > (expression | uniformExpression) >
                           qi::lit(";"))[qi::_val = phoenix::bind(&PgclParser::createAssignmentStatement, phoenix::ref(*this), qi::_1, qi::_2)];
    observeStatement = (qi::lit("observe") > qi::lit("(") >> booleanCondition >> qi::lit(")") >
                        qi::lit(";"))[qi::_val = phoenix::bind(&PgclParser::createObserveStatement, phoenix::ref(*this), qi::_1)];

    // Compound statements
    ifElseStatement = (qi::lit("if") > qi::lit("(") >> booleanCondition >> qi::lit(")") >> qi::lit("{") >> sequenceOfStatements >> qi::lit("}") >>
                       -(qi::lit("else") >> qi::lit("{") >> sequenceOfStatements >>
                         qi::lit("}")))[qi::_val = phoenix::bind(&PgclParser::createIfElseStatement, phoenix::ref(*this), qi::_1, qi::_2, qi::_3)];
    ifElseStatement.name("if/else statement");
    branchStatement = nondeterministicBranch | probabilisticBranch;
    loopStatement = (qi::lit("while") > qi::lit("(") > booleanCondition > qi::lit(")") > qi::lit("{") >> sequenceOfStatements >>
                     qi::lit("}"))[qi::_val = phoenix::bind(&PgclParser::createLoopStatement, phoenix::ref(*this), qi::_1, qi::_2)];
    loopStatement.name("loop statement");
    nondeterministicBranch = (qi::lit("{") >> sequenceOfStatements >> qi::lit("}") >> qi::lit("[]") >> qi::lit("{") >> sequenceOfStatements >>
                              qi::lit("}"))[qi::_val = phoenix::bind(&PgclParser::createNondeterministicBranch, phoenix::ref(*this), qi::_1, qi::_2)];
    probabilisticBranch =
        (qi::lit("{") >> sequenceOfStatements >> qi::lit("}") >> qi::lit("[") >> expression >> qi::lit("]") >> qi::lit("{") >> sequenceOfStatements >>
         qi::lit("}"))[qi::_val = phoenix::bind(&PgclParser::createProbabilisticBranch, phoenix::ref(*this), qi::_2, qi::_1, qi::_3)];

    // Expression and condition building, and basic identifiers
    expression %= expressionParser;
    expression.name("expression");
    booleanCondition = expressionParser[qi::_val = phoenix::bind(&PgclParser::createBooleanExpression, phoenix::ref(*this), qi::_1)];
    uniformExpression = (qi::lit("unif") >> qi::lit("(") >> qi::int_ >> qi::lit(",") >> qi::int_ >>
                         qi::lit(")"))[qi::_val = phoenix::bind(&PgclParser::createUniformExpression, phoenix::ref(*this), qi::_1, qi::_2)];
    variableName %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]]
                                 [qi::_pass = phoenix::bind(&PgclParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];

    variableName.name("variable name");
    programName %= +(qi::alnum | qi::lit("_"));
    programName.name("program name");

    // Enables location tracking for important entities
    auto setLocationInfoFunction = this->annotate(*qi::_val, qi::_1, qi::_3);
    qi::on_success(assignmentStatement, setLocationInfoFunction);
    qi::on_success(observeStatement, setLocationInfoFunction);
    qi::on_success(nondeterministicBranch, setLocationInfoFunction);
    qi::on_success(probabilisticBranch, setLocationInfoFunction);
    qi::on_success(loopStatement, setLocationInfoFunction);
    qi::on_success(ifElseStatement, setLocationInfoFunction);

    // Enable error reporting.
    qi::on_error<qi::fail>(program, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(variableDeclarations, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(integerDeclaration, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(booleanDeclaration, handler(qi::_1, qi::_2, qi::_3, qi::_4));

    qi::on_error<qi::fail>(assignmentStatement, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(observeStatement, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(ifElseStatement, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(loopStatement, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(branchStatement, handler(qi::_1, qi::_2, qi::_3, qi::_4));

    qi::on_error<qi::fail>(expression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(booleanCondition, handler(qi::_1, qi::_2, qi::_3, qi::_4));
    qi::on_error<qi::fail>(uniformExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));

    // Adds dummy to the 0-th location.
    std::shared_ptr<storm::pgcl::AssignmentStatement> dummy(new storm::pgcl::AssignmentStatement());
    this->locationToStatement.insert(this->locationToStatement.begin(), dummy);
}  // PgclParser()

void PgclParser::enableExpressions() {
    (this->expressionParser).setIdentifierMapping(&this->identifiers_);
}

/*
 * Creators for various program parts. They all follow the basic scheme
 * to retrieve the subparts of the program part and mold them together
 * into one new statement. They wrap the statement constructors and
 * throw excpetions in case something unexpected was parsed, e.g.
 * (x+5) as a boolean expression, or types of assignments don't match.
 */
storm::pgcl::PgclProgram PgclParser::createProgram(std::string const& programName, boost::optional<std::vector<storm::expressions::Variable>> parameters,
                                                   std::vector<std::shared_ptr<storm::pgcl::VariableDeclaration>> const& variableDeclarations,
                                                   std::vector<std::shared_ptr<storm::pgcl::Statement>> const& statements) {
    // Creates an empty paramter list in case no parameters were given.
    std::vector<storm::expressions::Variable> params;
    if (parameters != boost::none) {
        params = *parameters;
    }
    std::vector<storm::pgcl::VariableDeclaration> declarations;
    for (auto const& decl : variableDeclarations) {
        declarations.push_back(*decl);
    }

    // Creates the actual program.
    std::shared_ptr<storm::pgcl::PgclProgram> result(new storm::pgcl::PgclProgram(
        declarations, statements, this->locationToStatement, params, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    result->back()->setLast(true);
    // Sets the current program as a parent to all its direct children statements.
    for (storm::pgcl::iterator it = result->begin(); it != result->end(); ++it) {
        (*it)->setParentBlock(result.get());
    }
    return *result;
}

bool PgclParser::isValidIdentifier(std::string const& identifier) {
    return this->invalidIdentifiers.find(identifier) == nullptr;
}

storm::expressions::Variable PgclParser::declareDoubleVariable(std::string const& variableName) {
    storm::expressions::Variable variable = expressionManager->declareRationalVariable(variableName);
    this->identifiers_.add(variableName, variable.getExpression());
    return variable;
}

std::shared_ptr<storm::pgcl::AssignmentStatement> PgclParser::createAssignmentStatement(
    std::string const& variableName, boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& assignedExpression) {
    storm::expressions::Variable variable;
    if (!(*expressionManager).hasVariable(variableName)) {
        variable = (*expressionManager).declareIntegerVariable(variableName);
    } else {
        variable = (*expressionManager).getVariable(variableName);
        // Checks if assignment types match.
        if (assignedExpression.which() == 0 && !(variable.getType() == boost::get<storm::expressions::Expression>(assignedExpression).getType())) {
            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Wrong type when assigning to " << variableName << ".");
        }
    }
    std::shared_ptr<storm::pgcl::AssignmentStatement> newAssignment(new storm::pgcl::AssignmentStatement(variable, assignedExpression));
    newAssignment->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, newAssignment);
    currentLocationNumber++;
    return newAssignment;
}

std::shared_ptr<storm::pgcl::VariableDeclaration> PgclParser::createIntegerDeclarationStatement(std::string const& variableName,
                                                                                                storm::expressions::Expression const& assignedExpression) {
    storm::expressions::Variable variable;
    if (!(*expressionManager).hasVariable(variableName)) {
        variable = (*expressionManager).declareIntegerVariable(variableName);
        this->identifiers_.add(variableName, variable.getExpression());
    } else {
        // In case that a declaration already happened.
        variable = (*expressionManager).getVariable(variableName);
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException,
                        "Declaration of integer variable " << variableName << " which was already declared previously.");
    }
    // Todo add some checks
    return std::make_shared<storm::pgcl::VariableDeclaration>(variable, assignedExpression);
}

std::shared_ptr<storm::pgcl::VariableDeclaration> PgclParser::createBooleanDeclarationStatement(std::string const& variableName,
                                                                                                storm::expressions::Expression const& assignedExpression) {
    storm::expressions::Variable variable;
    if (!(*expressionManager).hasVariable(variableName)) {
        variable = (*expressionManager).declareBooleanVariable(variableName);
        this->identifiers_.add(variableName, variable.getExpression());
    } else {
        // In case that a declaration already happened.
        variable = (*expressionManager).getVariable(variableName);
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException,
                        "Declaration of boolean variable " << variableName << " which was already declared previously.");
    }
    // TODO add some checks.
    return std::make_shared<storm::pgcl::VariableDeclaration>(variable, assignedExpression);
}

std::shared_ptr<storm::pgcl::ObserveStatement> PgclParser::createObserveStatement(storm::pgcl::BooleanExpression const& condition) {
    std::shared_ptr<storm::pgcl::ObserveStatement> observe(new storm::pgcl::ObserveStatement(condition));
    this->observeCreated = true;
    observe->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, observe);
    currentLocationNumber++;
    return observe;
}

std::shared_ptr<storm::pgcl::IfStatement> PgclParser::createIfElseStatement(
    storm::pgcl::BooleanExpression const& condition, std::vector<std::shared_ptr<storm::pgcl::Statement>> const& ifBody,
    boost::optional<std::vector<std::shared_ptr<storm::pgcl::Statement>>> const& elseBody) {
    std::shared_ptr<storm::pgcl::PgclBlock> ifBodyProgram(
        new storm::pgcl::PgclBlock(ifBody, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::IfStatement> ifElse;
    if (elseBody) {
        std::shared_ptr<storm::pgcl::PgclBlock> elseBodyProgram(
            new storm::pgcl::PgclBlock(*elseBody, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
        ifElse = std::shared_ptr<storm::pgcl::IfStatement>(new storm::pgcl::IfStatement(condition, ifBodyProgram, elseBodyProgram));
        (*(ifElse->getIfBody()->back())).setLast(true);
        (*(ifElse->getElseBody()->back())).setLast(true);
        // Sets the current program as a parent to all its children statements.
        for (storm::pgcl::iterator it = ifElse->getElseBody()->begin(); it != ifElse->getElseBody()->end(); ++it) {
            (*it)->setParentBlock(ifElse->getElseBody().get());
        }
        for (storm::pgcl::iterator it = ifElse->getIfBody()->begin(); it != ifElse->getIfBody()->end(); ++it) {
            (*it)->setParentBlock(ifElse->getIfBody().get());
        }
    } else {
        ifElse = std::shared_ptr<storm::pgcl::IfStatement>(new storm::pgcl::IfStatement(condition, ifBodyProgram));
        (*(ifElse->getIfBody()->back())).setLast(true);
        // Sets the current program as a parent to all its children statements.
        for (storm::pgcl::iterator it = ifElse->getIfBody()->begin(); it != ifElse->getIfBody()->end(); ++it) {
            (*it)->setParentBlock(ifElse->getIfBody().get());
        }
    }
    ifElse->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, ifElse);
    currentLocationNumber++;
    return ifElse;
}

std::shared_ptr<storm::pgcl::LoopStatement> PgclParser::createLoopStatement(storm::pgcl::BooleanExpression const& condition,
                                                                            std::vector<std::shared_ptr<storm::pgcl::Statement>> const& body) {
    std::shared_ptr<storm::pgcl::PgclBlock> bodyProgram(
        new storm::pgcl::PgclBlock(body, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::LoopStatement> loop(new storm::pgcl::LoopStatement(condition, bodyProgram));
    this->loopCreated = true;
    // Sets the current program as a parent to all its children statements.
    for (storm::pgcl::iterator it = loop->getBody()->begin(); it != loop->getBody()->end(); ++it) {
        (*it)->setParentBlock(loop->getBody().get());
    }
    (*(loop->getBody()->back())).setLast(true);
    loop->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, loop);
    currentLocationNumber++;
    return loop;
}

std::shared_ptr<storm::pgcl::NondeterministicBranch> PgclParser::createNondeterministicBranch(
    std::vector<std::shared_ptr<storm::pgcl::Statement>> const& left, std::vector<std::shared_ptr<storm::pgcl::Statement>> const& right) {
    std::shared_ptr<storm::pgcl::PgclBlock> leftProgram(
        new storm::pgcl::PgclBlock(left, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::PgclBlock> rightProgram(
        new storm::pgcl::PgclBlock(right, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::NondeterministicBranch> branch(new storm::pgcl::NondeterministicBranch(leftProgram, rightProgram));
    this->nondetCreated = true;
    // Sets the left program as a parent to all its children statements.
    for (storm::pgcl::iterator it = branch->getLeftBranch()->begin(); it != branch->getLeftBranch()->end(); ++it) {
        (*it)->setParentBlock(branch->getLeftBranch().get());
    }
    // Sets the right program as a parent to all its children statements.
    for (storm::pgcl::iterator it = branch->getRightBranch()->begin(); it != branch->getRightBranch()->end(); ++it) {
        (*it)->setParentBlock(branch->getRightBranch().get());
    }
    (*(branch->getLeftBranch()->back())).setLast(true);
    (*(branch->getRightBranch()->back())).setLast(true);
    branch->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, branch);
    currentLocationNumber++;
    return branch;
}

std::shared_ptr<storm::pgcl::ProbabilisticBranch> PgclParser::createProbabilisticBranch(storm::expressions::Expression const& probability,
                                                                                        std::vector<std::shared_ptr<storm::pgcl::Statement>> const& left,
                                                                                        std::vector<std::shared_ptr<storm::pgcl::Statement>> const& right) {
    std::shared_ptr<storm::pgcl::PgclBlock> leftProgram(
        new storm::pgcl::PgclBlock(left, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::PgclBlock> rightProgram(
        new storm::pgcl::PgclBlock(right, this->expressionManager, this->loopCreated, this->nondetCreated, this->observeCreated));
    std::shared_ptr<storm::pgcl::ProbabilisticBranch> branch(new storm::pgcl::ProbabilisticBranch(probability, leftProgram, rightProgram));
    // Sets the left program as a parent to all its children statements.
    for (storm::pgcl::iterator it = branch->getLeftBranch()->begin(); it != branch->getLeftBranch()->end(); ++it) {
        (*it)->setParentBlock(branch->getLeftBranch().get());
    }
    // Sets the right program as a parent to all its children statements.
    for (storm::pgcl::iterator it = branch->getRightBranch()->begin(); it != branch->getRightBranch()->end(); ++it) {
        (*it)->setParentBlock(branch->getRightBranch().get());
    }
    (*(branch->getLeftBranch()->back())).setLast(true);
    (*(branch->getRightBranch()->back())).setLast(true);
    branch->setLocationNumber(this->currentLocationNumber);
    this->locationToStatement.insert(this->locationToStatement.begin() + this->currentLocationNumber, branch);
    currentLocationNumber++;
    return branch;
}

storm::pgcl::BooleanExpression PgclParser::createBooleanExpression(storm::expressions::Expression const& expression) {
    if (expression.hasBooleanType()) {
        storm::pgcl::BooleanExpression booleanExpression = storm::pgcl::BooleanExpression(expression);
        return booleanExpression;
    } else {
        // In case that a non-boolean expression was used (e.g. (x+5)).
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Non boolean expression was used in a condition.");
    }
}

storm::pgcl::UniformExpression PgclParser::createUniformExpression(int const& begin, int const& end) {
    return storm::pgcl::UniformExpression(begin, end);
}

}  // namespace parser
}  // namespace storm
