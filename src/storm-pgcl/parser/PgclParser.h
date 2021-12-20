#pragma once

// Includes files for file input.
#include <fstream>
#include <iomanip>
#include <memory>
// Includes files for building and parsing the PGCL program
#include "storm-parsers/parser/ExpressionParser.h"
#include "storm-parsers/parser/SpiritErrorHandler.h"
#include "storm-parsers/parser/SpiritParserDefinitions.h"
#include "storm-pgcl/storage/pgcl/AssignmentStatement.h"
#include "storm-pgcl/storage/pgcl/BooleanExpression.h"
#include "storm-pgcl/storage/pgcl/IfStatement.h"
#include "storm-pgcl/storage/pgcl/LoopStatement.h"
#include "storm-pgcl/storage/pgcl/NondeterministicBranch.h"
#include "storm-pgcl/storage/pgcl/ObserveStatement.h"
#include "storm-pgcl/storage/pgcl/PgclProgram.h"
#include "storm-pgcl/storage/pgcl/ProbabilisticBranch.h"
#include "storm-pgcl/storage/pgcl/Statement.h"
#include "storm-pgcl/storage/pgcl/UniformExpression.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
namespace pgcl {
class IfStatement;
}

namespace parser {

class PgclParser : public qi::grammar<Iterator, storm::pgcl::PgclProgram(), Skipper> {
   public:
    /*!
     * Parses the given file into the PGCL storage classes in case it
     * complies with the PGCL syntax.
     *
     * @param filename the name of the file to parse.
     * @return The resulting PGCL program.
     */
    static storm::pgcl::PgclProgram parse(std::string const& filename);

    /*!
     * Parses the given input stream into the PGCL storage classes in
     * case it complies with the PGCL syntax.
     *
     * @param input The input string to parse.
     * @param filename Name of the file from which the input was read.
     * @return The resulting PGCL program.
     */
    static storm::pgcl::PgclProgram parseFromString(std::string const& input, std::string const& filename);

   private:
    // Internal constructor used by the parseFromString function.
    PgclParser(std::string const& filename, Iterator first);
    // Nonterminals (and their semantic types) of the PGCL grammar.
    qi::rule<Iterator, storm::pgcl::PgclProgram(), Skipper> program;
    qi::rule<Iterator, std::vector<std::shared_ptr<storm::pgcl::Statement>>(), Skipper> sequenceOfStatements;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::Statement>(), Skipper> statement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::Statement>(), Skipper> simpleStatement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::Statement>(), Skipper> compoundStatement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::IfStatement>(), Skipper> ifElseStatement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::BranchStatement>(), Skipper> branchStatement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::LoopStatement>(), Skipper> loopStatement;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::NondeterministicBranch>(), Skipper> nondeterministicBranch;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::ProbabilisticBranch>(), Skipper> probabilisticBranch;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::AssignmentStatement>(), Skipper> assignmentStatement;
    qi::rule<Iterator, storm::expressions::Variable(), Skipper> declaration;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::ObserveStatement>(), Skipper> observeStatement;
    qi::rule<Iterator, storm::expressions::Expression(), Skipper> expression;
    qi::rule<Iterator, storm::pgcl::BooleanExpression(), Skipper> booleanCondition;
    qi::rule<Iterator, storm::pgcl::UniformExpression(), Skipper> uniformExpression;
    qi::rule<Iterator, std::string(), Skipper> variableName;
    qi::rule<Iterator, std::string(), Skipper> programName;

    qi::rule<Iterator, std::vector<std::shared_ptr<storm::pgcl::VariableDeclaration>>(), Skipper> variableDeclarations;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::VariableDeclaration>(), Skipper> integerDeclaration;
    qi::rule<Iterator, std::shared_ptr<storm::pgcl::VariableDeclaration>(), Skipper> booleanDeclaration;
    qi::rule<Iterator, storm::expressions::Variable(), Skipper> doubleDeclaration;

    // An error handler function.
    phoenix::function<SpiritErrorHandler> handler;

    /// Denotes the invalid identifiers, which are later passed to the expression parser.
    struct keywordsStruct : qi::symbols<char, uint_fast64_t> {
        keywordsStruct() {
            add("while", 1)("if", 2)("observe", 3)("int", 4)("bool", 5)("function", 6);
        }
    };

    /// Initializes the invalid identifier struct.
    keywordsStruct invalidIdentifiers;
    /// Is used to store the identifiers of the PGCL program that are found while parsing.
    qi::symbols<char, storm::expressions::Expression> identifiers_;

    /// Functor used for annotating entities with line number information.
    class PositionAnnotation {
       public:
        typedef void result_type;

        PositionAnnotation(Iterator first) : first(first) {
            // Intentionally left empty.
        }

        template<typename Entity, typename First, typename Last>
        result_type operator()(Entity& entity, First f, Last l) const {
            entity.setLineNumber(get_line(f));
        }

       private:
        std::string filename;
        Iterator const first;
    };

    /// A function used for annotating the entities with their position.
    phoenix::function<PositionAnnotation> annotate;

    /// Manages the expressions and their parsing externally.
    std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
    storm::parser::ExpressionParser expressionParser;

    /// Stores a mapping from location numbers to statements.
    std::vector<std::shared_ptr<storm::pgcl::Statement>> locationToStatement;

    /// Saves whether a loop statements was created.
    bool loopCreated = false;
    /// Saves whether a nondet statements was created.
    bool nondetCreated = false;
    /// Saves whether a observe statement was created.
    bool observeCreated = false;

    /// Stores the unique identifier of the currently parsed statement, starting at 1.
    std::size_t currentLocationNumber = 1;

    /// Sets the expression parser to a mode where it actually generates valid expressions.
    void enableExpressions();

    // Constructors for the single program parts. They just wrap the statement constructors and throw exceptions in case something unexpected was parsed.
    storm::pgcl::PgclProgram createProgram(std::string const& programName, boost::optional<std::vector<storm::expressions::Variable>> parameters,
                                           std::vector<std::shared_ptr<storm::pgcl::VariableDeclaration>> const& variableDeclarations,
                                           std::vector<std::shared_ptr<storm::pgcl::Statement>> const& statements);
    bool isValidIdentifier(std::string const& identifier);
    storm::expressions::Variable declareDoubleVariable(std::string const& variableName);
    std::shared_ptr<storm::pgcl::AssignmentStatement> createAssignmentStatement(
        std::string const& variableName, boost::variant<storm::expressions::Expression, storm::pgcl::UniformExpression> const& assignedExpression);
    std::shared_ptr<storm::pgcl::VariableDeclaration> createIntegerDeclarationStatement(std::string const& variableName,
                                                                                        storm::expressions::Expression const& assignedExpression);
    std::shared_ptr<storm::pgcl::VariableDeclaration> createBooleanDeclarationStatement(std::string const& variableName,
                                                                                        storm::expressions::Expression const& assignedExpression);
    std::shared_ptr<storm::pgcl::ObserveStatement> createObserveStatement(storm::pgcl::BooleanExpression const& condition);
    std::shared_ptr<storm::pgcl::IfStatement> createIfElseStatement(storm::pgcl::BooleanExpression const& condition,
                                                                    std::vector<std::shared_ptr<storm::pgcl::Statement>> const& if_body,
                                                                    boost::optional<std::vector<std::shared_ptr<storm::pgcl::Statement>>> const& else_body);
    std::shared_ptr<storm::pgcl::LoopStatement> createLoopStatement(storm::pgcl::BooleanExpression const& condition,
                                                                    std::vector<std::shared_ptr<storm::pgcl::Statement>> const& body);
    std::shared_ptr<storm::pgcl::NondeterministicBranch> createNondeterministicBranch(std::vector<std::shared_ptr<storm::pgcl::Statement>> const& left,
                                                                                      std::vector<std::shared_ptr<storm::pgcl::Statement>> const& right);
    std::shared_ptr<storm::pgcl::ProbabilisticBranch> createProbabilisticBranch(storm::expressions::Expression const& probability,
                                                                                std::vector<std::shared_ptr<storm::pgcl::Statement>> const& left,
                                                                                std::vector<std::shared_ptr<storm::pgcl::Statement>> const& right);
    storm::pgcl::BooleanExpression createBooleanExpression(storm::expressions::Expression const& expression);
    storm::pgcl::UniformExpression createUniformExpression(int const& begin, int const& end);
};
}  // namespace parser
}  // namespace storm
