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

TEST(PrismMenuGame, DieProgramAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(19, abstraction.getNodeCount());
}

TEST(PrismMenuGame, DieProgramAbstractionAndRefinementTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("s") == manager.integer(7)}));
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(26, abstraction.getNodeCount());
}

TEST(PrismMenuGame, CrowdsProgramAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(46, abstraction.getNodeCount());
}

TEST(PrismMenuGame, CrowdsProgramAbstractionAndRefinementTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(46, abstraction.getNodeCount());
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("observe0") + manager.getVariableExpression("observe1") + manager.getVariableExpression("observe2") + manager.getVariableExpression("observe3") + manager.getVariableExpression("observe4") <= manager.getVariableExpression("runCount")}));
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(75, abstraction.getNodeCount());
}

TEST(PrismMenuGame, TwoDiceProgramAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(38, abstraction.getNodeCount());
}

TEST(PrismMenuGame, TwoDiceProgramAbstractionAndRefinementTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(38, abstraction.getNodeCount());
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("d1") + manager.getVariableExpression("d2") == manager.integer(7)}));
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());

    EXPECT_EQ(107, abstraction.getNodeCount());
}

TEST(PrismMenuGame, WlanProgramAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(219, abstraction.getNodeCount());
}

TEST(PrismMenuGame, WlanProgramAbstractionAndRefinementTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-2.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(219, abstraction.getNodeCount());
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("backoff1") < manager.integer(7)}));
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(292, abstraction.getNodeCount());
}

#endif