#include "gtest/gtest.h"
#include "storm-config.h"

#ifdef STORM_HAVE_MSAT

#include "src/parser/PrismParser.h"

#include "src/abstraction/prism/AbstractProgram.h"

#include "src/storage/expressions/Expression.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/utility/solver.h"

#include "src/adapters/CarlAdapter.h"

TEST(PrismMenuGame, DieAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(26, game.getNumberOfTransitions());
    EXPECT_EQ(4, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, DieAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(26, game.getNumberOfTransitions());
    EXPECT_EQ(4, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

#ifdef STORM_HAVE_CARL
TEST(PrismMenuGame, DieAbstractionTest_SylvanWithRationalFunction) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, storm::RationalFunction> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, storm::RationalFunction> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(10, game.getNumberOfTransitions());
    EXPECT_EQ(2, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}
#endif

TEST(PrismMenuGame, DieAbstractionAndRefinementTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("s") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(24, game.getNumberOfTransitions());
    EXPECT_EQ(5, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, DieAbstractionAndRefinementTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("s") == manager.integer(7)}));
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(24, game.getNumberOfTransitions());
    EXPECT_EQ(5, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, DieFullAbstractionTest_Cudd) {
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

    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(20, game.getNumberOfTransitions());
    EXPECT_EQ(13, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, DieFullAbstractionTest_Sylvan) {
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
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(20, game.getNumberOfTransitions());
    EXPECT_EQ(13, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(31, game.getNumberOfTransitions());
    EXPECT_EQ(4, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(31, game.getNumberOfTransitions());
    EXPECT_EQ(4, game.getNumberOfStates());
    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsAbstractionAndRefinementTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("observe0") + manager.getVariableExpression("observe1") + manager.getVariableExpression("observe2") + manager.getVariableExpression("observe3") + manager.getVariableExpression("observe4") <= manager.getVariableExpression("runCount")}));
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(84, game.getNumberOfTransitions());
    EXPECT_EQ(8, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsAbstractionAndRefinementTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    program = program.substituteConstants();
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("observe0") + manager.getVariableExpression("observe1") + manager.getVariableExpression("observe2") + manager.getVariableExpression("observe3") + manager.getVariableExpression("observe4") <= manager.getVariableExpression("runCount")}));
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(84, game.getNumberOfTransitions());
    EXPECT_EQ(8, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsFullAbstractionTest_Cudd) {
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
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(15113, game.getNumberOfTransitions());
    EXPECT_EQ(8607, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, CrowdsFullAbstractionTest_Sylvan) {
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
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(15113, game.getNumberOfTransitions());
    EXPECT_EQ(8607, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(90, game.getNumberOfTransitions());
    EXPECT_EQ(8, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(90, game.getNumberOfTransitions());
    EXPECT_EQ(8, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceAbstractionAndRefinementTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("d1") + manager.getVariableExpression("d2") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();

    EXPECT_EQ(324, game.getNumberOfTransitions());
    EXPECT_EQ(16, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceAbstractionAndRefinementTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("d1") + manager.getVariableExpression("d2") == manager.integer(7)}));
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(324, game.getNumberOfTransitions());
    EXPECT_EQ(16, game.getNumberOfStates());
    EXPECT_EQ(4, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceFullAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(7));

    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(6));
    
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(7));

    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(6));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(436, game.getNumberOfTransitions());
    EXPECT_EQ(169, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, TwoDiceFullAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(7));
    
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("d1") == manager.integer(6));
    
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(7));
    
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("d2") == manager.integer(6));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(436, game.getNumberOfTransitions());
    EXPECT_EQ(169, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(2323, game.getNumberOfTransitions());
    EXPECT_EQ(12, game.getNumberOfStates());
    EXPECT_EQ(8, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(2323, game.getNumberOfTransitions());
    EXPECT_EQ(12, game.getNumberOfStates());
    EXPECT_EQ(8, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanAbstractionAndRefinementTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("backoff1") < manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();

    EXPECT_EQ(4600, game.getNumberOfTransitions());
    EXPECT_EQ(24, game.getNumberOfStates());
    EXPECT_EQ(16, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanAbstractionAndRefinementTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    ASSERT_NO_THROW(abstractProgram.refine({manager.getVariableExpression("backoff1") < manager.integer(7)}));
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(4600, game.getNumberOfTransitions());
    EXPECT_EQ(24, game.getNumberOfStates());
    EXPECT_EQ(16, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanFullAbstractionTest_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(2));

    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(2));

    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(2));
    
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(7));

    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(12));
    
    initialPredicates.push_back(manager.getVariableExpression("slot1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("slot1") == manager.integer(1));

    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(12));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(13));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(14));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(15));

    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(1));
    
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(7));
    
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(12));
    
    initialPredicates.push_back(manager.getVariableExpression("slot2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("slot2") == manager.integer(1));
    
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(12));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(13));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(14));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(15));
    
    initialPredicates.push_back(manager.getVariableExpression("bc2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("bc2") == manager.integer(1));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::CUDD, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(9503, game.getNumberOfTransitions());
    EXPECT_EQ(5523, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

TEST(PrismMenuGame, WlanFullAbstractionTest_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/wlan0-2-4.nm");
    program = program.substituteConstants();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
    
    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();
    
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("col") == manager.integer(2));
    
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.integer(2));
    
    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("c2") == manager.integer(2));
    
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("x1") == manager.integer(7));
    
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("s1") == manager.integer(12));
    
    initialPredicates.push_back(manager.getVariableExpression("slot1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("slot1") == manager.integer(1));
    
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(12));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(13));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(14));
    initialPredicates.push_back(manager.getVariableExpression("backoff1") == manager.integer(15));
    
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(1));
    
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("x2") == manager.integer(7));
    
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(12));
    
    initialPredicates.push_back(manager.getVariableExpression("slot2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("slot2") == manager.integer(1));
    
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(1));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(2));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(4));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(6));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(7));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(8));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(9));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(10));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(11));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(12));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(13));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(14));
    initialPredicates.push_back(manager.getVariableExpression("backoff2") == manager.integer(15));
    
    initialPredicates.push_back(manager.getVariableExpression("bc2") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("bc2") == manager.integer(1));
    
    storm::abstraction::prism::AbstractProgram<storm::dd::DdType::Sylvan, double> abstractProgram(program, initialPredicates, std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), false);
    
    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractProgram.getAbstractGame();
    
    EXPECT_EQ(9503, game.getNumberOfTransitions());
    EXPECT_EQ(5523, game.getNumberOfStates());
    EXPECT_EQ(0, game.getBottomStates().getNonZeroCount());
}

#endif
