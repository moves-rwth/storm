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

TEST(PrismMenuGame, DieAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(19, abstraction.getNodeCount());
}

TEST(PrismMenuGame, DieAbstractionAndRefinementTest) {
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

TEST(PrismMenuGame, DieFullAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s") == manager.integer(7));

    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("d") == manager.integer(6));

    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(313, abstraction.getNodeCount());
    
    storm::dd::Bdd<storm::dd::DdType::CUDD> reachableStatesInAbstraction;
    ASSERT_NO_THROW(reachableStatesInAbstraction = abstractProgram.getReachableStates());
    
    EXPECT_EQ(13, reachableStatesInAbstraction.getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsAbstractionTest) {
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

TEST(PrismMenuGame, CrowdsAbstractionAndRefinementTest) {
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

TEST(PrismMenuGame, CrowdsFullAbstractionTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("phase") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("phase") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("phase") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("phase") == manager.integer(4));

    initialPredicates.push_back(manager.getVariableExpression("good"));

    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("runCount") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("observe0") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("observe1") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("observe2") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("observe3") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("observe4") == manager.integer(5));

    initialPredicates.push_back(manager.getVariableExpression("lastSeen") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("lastSeen") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("lastSeen") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("lastSeen") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("lastSeen") == manager.integer(4));
    
    storm::prism::menu_games::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program.getManager(), program, initialPredicates, std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::dd::Add<storm::dd::DdType::CUDD> abstraction;
    ASSERT_NO_THROW(abstraction = abstractProgram.getAbstractAdd());
    
    EXPECT_EQ(46, abstraction.getNodeCount());
    
    storm::dd::Bdd<storm::dd::DdType::CUDD> reachableStatesInAbstraction;
    ASSERT_NO_THROW(reachableStatesInAbstraction = abstractProgram.getReachableStates());
    
    EXPECT_EQ(13, reachableStatesInAbstraction.getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceAbstractionTest) {
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

TEST(PrismMenuGame, TwoDiceAbstractionAndRefinementTest) {
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

TEST(PrismMenuGame, WlanAbstractionTest) {
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

TEST(PrismMenuGame, WlanAbstractionAndRefinementTest) {
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