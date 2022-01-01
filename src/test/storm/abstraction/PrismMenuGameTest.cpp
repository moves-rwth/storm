#include "storm-config.h"
#include "test/storm_gtest.h"

#ifdef STORM_HAVE_MSAT

#include "storm-parsers/parser/PrismParser.h"

#include "storm/abstraction/MenuGameRefiner.h"
#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/storage/expressions/Expression.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/utility/solver.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

TEST(PrismMenuGame, DieAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(26ull, game.getNumberOfTransitions());
    EXPECT_EQ(4ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, DieAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(26ull, game.getNumberOfTransitions());
    EXPECT_EQ(4ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

#ifdef STORM_HAVE_CARL
// Commented out due to incompatibility with new refiner functionality.
// This functionality depends on some operators being available on the value type which are not there for rational functions.
// TEST(PrismMenuGame, DieAbstractionTest_SylvanWithRationalFunction) {
//    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

//    std::vector<storm::expressions::Expression> initialPredicates;
//    storm::expressions::ExpressionManager& manager = program.getManager();

//    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));

//    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

//    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, storm::RationalFunction> abstractor(program, smtSolverFactory);
//    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction> refiner(abstractor, smtSolverFactory->create(manager));
//    refiner.refine(initialPredicates);

//    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, storm::RationalFunction> game = abstractor.abstract();

//    EXPECT_EQ(26, game.getNumberOfTransitions());
//    EXPECT_EQ(4, game.getNumberOfStates());
//    EXPECT_EQ(2, game.getBottomStates().getNonZeroCount());
//}
#endif

TEST(PrismMenuGame, DieAbstractionAndRefinementTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("s") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(24ull, game.getNumberOfTransitions());
    EXPECT_EQ(5ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, DieAbstractionAndRefinementTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("s") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(24ull, game.getNumberOfTransitions());
    EXPECT_EQ(5ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, DieFullAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(20ull, game.getNumberOfTransitions());
    EXPECT_EQ(13ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, DieFullAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(20ull, game.getNumberOfTransitions());
    EXPECT_EQ(13ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(31ull, game.getNumberOfTransitions());
    EXPECT_EQ(4ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(31ull, game.getNumberOfTransitions());
    EXPECT_EQ(4ull, game.getNumberOfStates());
    EXPECT_EQ(2ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsAbstractionAndRefinementTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(
        refiner.refine({manager.getVariableExpression("observe0") + manager.getVariableExpression("observe1") + manager.getVariableExpression("observe2") +
                            manager.getVariableExpression("observe3") + manager.getVariableExpression("observe4") <=
                        manager.getVariableExpression("runCount")}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(68ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsAbstractionAndRefinementTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("phase") < manager.integer(3));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(
        refiner.refine({manager.getVariableExpression("observe0") + manager.getVariableExpression("observe1") + manager.getVariableExpression("observe2") +
                            manager.getVariableExpression("observe3") + manager.getVariableExpression("observe4") <=
                        manager.getVariableExpression("runCount")}));

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(68ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsFullAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(15113ull, game.getNumberOfTransitions());
    EXPECT_EQ(8607ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, CrowdsFullAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    program = program.substituteConstantsFormulas();

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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(15113ull, game.getNumberOfTransitions());
    EXPECT_EQ(8607ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(90ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(90ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceAbstractionAndRefinementTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("d1") + manager.getVariableExpression("d2") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(276ull, game.getNumberOfTransitions());
    EXPECT_EQ(16ull, game.getNumberOfStates());
    EXPECT_EQ(8ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceAbstractionAndRefinementTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(3));
    initialPredicates.push_back(manager.getVariableExpression("s2") == manager.integer(0));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("d1") + manager.getVariableExpression("d2") == manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(276ull, game.getNumberOfTransitions());
    EXPECT_EQ(16ull, game.getNumberOfStates());
    EXPECT_EQ(8ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceFullAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(436ull, game.getNumberOfTransitions());
    EXPECT_EQ(169ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, TwoDiceFullAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(436ull, game.getNumberOfTransitions());
    EXPECT_EQ(169ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(903ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(903ull, game.getNumberOfTransitions());
    EXPECT_EQ(8ull, game.getNumberOfStates());
    EXPECT_EQ(4ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanAbstractionAndRefinementTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("backoff1") < manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(1800ull, game.getNumberOfTransitions());
    EXPECT_EQ(16ull, game.getNumberOfStates());
    EXPECT_EQ(8ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanAbstractionAndRefinementTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s1") < manager.integer(5));
    initialPredicates.push_back(manager.getVariableExpression("bc1") == manager.integer(0));
    initialPredicates.push_back(manager.getVariableExpression("c1") == manager.getVariableExpression("c2"));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    ASSERT_NO_THROW(refiner.refine({manager.getVariableExpression("backoff1") < manager.integer(7)}));

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(1800ull, game.getNumberOfTransitions());
    EXPECT_EQ(16ull, game.getNumberOfStates());
    EXPECT_EQ(8ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanFullAbstractionTest_Cudd) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    EXPECT_EQ(9503ull, game.getNumberOfTransitions());
    EXPECT_EQ(5523ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

TEST(PrismMenuGame, WlanFullAbstractionTest_Sylvan) {
    auto& settings = storm::settings::mutableAbstractionSettings();
    settings.setAddAllGuards(false);
    settings.setAddAllInitialExpressions(false);

    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
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

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();

    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::Sylvan, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::Sylvan, double> game = abstractor.abstract();

    EXPECT_EQ(9503ull, game.getNumberOfTransitions());
    EXPECT_EQ(5523ull, game.getNumberOfStates());
    EXPECT_EQ(0ull, game.getBottomStates().getNonZeroCount());

    storm::settings::mutableAbstractionSettings().restoreDefaults();
}

#endif
