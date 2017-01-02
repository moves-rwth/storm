#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/parser/PrismParser.h"

#include "storm/utility/solver.h"

#include "storm/storage/jani/Model.h"

#ifdef STORM_HAVE_MSAT
TEST(JaniModelTest, FlattenModules) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(74ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Wlan_Mathsat) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0_collide.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(179ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Csma_Mathsat) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2_2.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(70ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Firewire_Mathsat) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(5024ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Coin_Mathsat) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(13ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Dice_Mathsat) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(16ull, janiModel.getAutomaton(0).getNumberOfEdges());
}
#endif

#ifdef STORM_HAVE_Z3
TEST(JaniModelTest, FlattenModules_Leader_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(74ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Wlan_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0_collide.nm"));
    storm::jani::Model janiModel = program.toJani();
    
    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(179ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Csma_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2_2.nm"));
    storm::jani::Model janiModel = program.toJani();

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(70ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Firewire_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/firewire.nm"));
    storm::jani::Model janiModel = program.toJani();

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(5024ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Coin_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2.nm"));
    storm::jani::Model janiModel = program.toJani();

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(13ull, janiModel.getAutomaton(0).getNumberOfEdges());
}

TEST(JaniModelTest, FlattenModules_Dice_Z3) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm"));
    storm::jani::Model janiModel = program.toJani();

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
    
    ASSERT_NO_THROW(janiModel = janiModel.flattenComposition(smtSolverFactory));
    EXPECT_EQ(1ull, janiModel.getNumberOfAutomata());
    EXPECT_EQ(16ull, janiModel.getAutomaton(0).getNumberOfEdges());
}
#endif
