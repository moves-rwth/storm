#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_MSAT

#include "src/parser/PrismParser.h"

#include "src/storage/prism/menu_games/AbstractProgram.h"

#include "src/storage/expressions/Expression.h"

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

#include "src/utility/solver.h"

TEST(PrismMenuGame, CommandAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction = abstractProgram.getAbstractAdd();
    abstraction.exportToDot("abstr1.dot");
    
    abstractProgram.refine({manager.getVariableExpression("s") == manager.integer(7)});
    abstraction = abstractProgram.getAbstractAdd();
    abstraction.exportToDot("abstr2.dot");
    
}

#endif