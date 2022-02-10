#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/DdManager.h"
#include "storm/utility/graph.h"

TEST(GraphTest, SymbolicProb01_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model =
        storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(),
                                                                                       model->getReachableStates(), model->getStates("observe0Greater1")));
        EXPECT_EQ(4409ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(1316ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(),
                                                                                       model->getReachableStates(), model->getStates("observeIGreater1")));
        EXPECT_EQ(1091ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(4802ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(),
                                                                                       model->getReachableStates(), model->getStates("observeOnlyTrueSender")));
        EXPECT_EQ(5829ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(1032ull, statesWithProbability01.second.getNonZeroCount());
    }
}

TEST(GraphTest, SymbolicProb01_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model =
        storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(),
                                                                                       model->getReachableStates(), model->getStates("observe0Greater1")));
        EXPECT_EQ(4409ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(1316ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(),
                                                                                       model->getReachableStates(), model->getStates("observeIGreater1")));
        EXPECT_EQ(1091ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(4802ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(),
                                                                                       model->getReachableStates(), model->getStates("observeOnlyTrueSender")));
        EXPECT_EQ(5829ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(1032ull, statesWithProbability01.second.getNonZeroCount());
    }
}

TEST(GraphTest, SymbolicProb01MinMax_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model =
        storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ull, statesWithProbability01.second.getNonZeroCount());
    }

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(77ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(149ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(74ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(198ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(94ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(33ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(83ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(35ull, statesWithProbability01.second.getNonZeroCount());
    }

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 =
                            storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                    model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 =
                            storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(),
                                                                    model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ull, statesWithProbability01.second.getNonZeroCount());
    }
}

TEST(GraphTest, SymbolicProb01MinMax_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model =
        storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ull, statesWithProbability01.second.getNonZeroCount());
    }

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(77ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(149ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(74ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(198ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(94ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(33ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                                          model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(83ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(35ull, statesWithProbability01.second.getNonZeroCount());
    }

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        // This block is necessary, so the BDDs get disposed before the manager (contained in the model).
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 =
                            storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                    model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ull, statesWithProbability01.second.getNonZeroCount());

        ASSERT_NO_THROW(statesWithProbability01 =
                            storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(),
                                                                    model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ull, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ull, statesWithProbability01.second.getNonZeroCount());
    }
}

#ifdef STORM_HAVE_MSAT

#include "storm/abstraction/MenuGameRefiner.h"
#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/storage/expressions/Expression.h"

#include "storm/utility/solver.h"

TEST(GraphTest, SymbolicProb01StochasticGameDieSmall) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    std::vector<storm::expressions::Expression> initialPredicates;
    storm::expressions::ExpressionManager& manager = program.getManager();

    initialPredicates.push_back(manager.getVariableExpression("s") < manager.integer(3));

    auto smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    // The target states are those states where !(s < 3).
    storm::dd::Bdd<storm::dd::DdType::CUDD> targetStates = !abstractor.getStates(initialPredicates[0]) && game.getReachableStates();

    storm::utility::graph::SymbolicGameProb01Result<storm::dd::DdType::CUDD> result =
        storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                            storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, true, true);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, true, true);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    refiner.refine({manager.getVariableExpression("s") < manager.integer(2)});
    game = abstractor.abstract();

    // We need to create a new BDD for the target states since the reachable states might have changed.
    targetStates = !abstractor.getStates(initialPredicates[0]) && game.getReachableStates();

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, true, true);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());
    ASSERT_TRUE(result.hasPlayer1Strategy());
    ASSERT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob0 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb0StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb0StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one action in each state.
    storm::dd::Add<storm::dd::DdType::CUDD, double> stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    EXPECT_EQ(0ull, stateDistributionsUnderStrategies.getNonZeroCount());

    // Check that the number of distributions per state is one (or zero in the case where there are no prob0 states).
    storm::dd::Add<storm::dd::DdType::CUDD> stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(0.0, stateDistributionCount.getMax());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(0ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, true, true);
    EXPECT_EQ(8ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob1 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb1StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb1StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one action in each state.
    stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    EXPECT_EQ(8ull, stateDistributionsUnderStrategies.getNonZeroCount());

    // Check that the number of distributions per state is one (or zero in the case where there are no prob1 states).
    stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(1.0, stateDistributionCount.getMax());
}

TEST(GraphTest, SymbolicProb01StochasticGameTwoDice) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());

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

    auto smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    // The target states are those states where s1 == 7 & s2 == 7 & d1 + d2 == 2.
    storm::dd::Bdd<storm::dd::DdType::CUDD> targetStates = abstractor.getStates(initialPredicates[7]) && abstractor.getStates(initialPredicates[22]) &&
                                                           abstractor.getStates(initialPredicates[9]) && abstractor.getStates(initialPredicates[24]) &&
                                                           game.getReachableStates();

    storm::utility::graph::SymbolicGameProb01Result<storm::dd::DdType::CUDD> result =
        storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                            storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, true, true);
    EXPECT_EQ(153ull, result.getPlayer1States().getNonZeroCount());
    ASSERT_TRUE(result.hasPlayer1Strategy());
    ASSERT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob0 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb0StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb0StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one exaction in each state.
    storm::dd::Add<storm::dd::DdType::CUDD, double> stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    EXPECT_EQ(153ull, stateDistributionsUnderStrategies.getNonZeroCount());

    storm::dd::Add<storm::dd::DdType::CUDD> stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(1.0, stateDistributionCount.getMax());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(1ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(153ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(1ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(153ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(1ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(153ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, true, true);
    EXPECT_EQ(1ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob1 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb1StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb1StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one action in each state.
    stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    EXPECT_EQ(1ull, stateDistributionsUnderStrategies.getNonZeroCount());

    // Check that the number of distributions per state is one (or zero in the case where there are no prob1 states).
    stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(1.0, stateDistributionCount.getMax());
}

TEST(GraphTest, SymbolicProb01StochasticGameWlan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/wlan0-2-4.nm");
    program = program.substituteConstantsFormulas();
    program = program.flattenModules(std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());

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

    auto smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    storm::abstraction::prism::PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double> abstractor(program, smtSolverFactory);
    storm::abstraction::MenuGameRefiner<storm::dd::DdType::CUDD, double> refiner(abstractor, smtSolverFactory->create(manager));
    refiner.refine(initialPredicates);

    storm::abstraction::MenuGame<storm::dd::DdType::CUDD, double> game = abstractor.abstract();

    // The target states are those states where col == 2.
    storm::dd::Bdd<storm::dd::DdType::CUDD> targetStates = abstractor.getStates(initialPredicates[2]) && game.getReachableStates();

    storm::utility::graph::SymbolicGameProb01Result<storm::dd::DdType::CUDD> result =
        storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                            storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, true, true);
    EXPECT_EQ(2831ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob0 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb0StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb0StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one action in each state.
    storm::dd::Add<storm::dd::DdType::CUDD, double> stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    ;
    EXPECT_EQ(2831ull, stateDistributionsUnderStrategies.getNonZeroCount());

    // Check that the number of distributions per state is one (or zero in the case where there are no prob0 states).
    storm::dd::Add<storm::dd::DdType::CUDD> stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(1.0, stateDistributionCount.getMax());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(2692ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(2831ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(2692ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(2064ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize);
    EXPECT_EQ(2884ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb0(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize);
    EXPECT_EQ(2064ull, result.getPlayer1States().getNonZeroCount());

    result = storm::utility::graph::performProb1(game, game.getQualitativeTransitionMatrix(), game.getReachableStates(), targetStates,
                                                 storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, true, true);
    EXPECT_EQ(2884ull, result.getPlayer1States().getNonZeroCount());
    EXPECT_TRUE(result.hasPlayer1Strategy());
    EXPECT_TRUE(result.hasPlayer2Strategy());

    // Check the validity of the strategies. Start by checking whether only prob1 states have a strategy.
    storm::dd::Bdd<storm::dd::DdType::CUDD> nonProb1StatesWithStrategy = !result.getPlayer1States() && result.player1Strategy.get();
    EXPECT_TRUE(nonProb1StatesWithStrategy.isZero());

    // Proceed by checking whether they select exactly one action in each state.
    stateDistributionsUnderStrategies =
        (game.getTransitionMatrix() * result.player1Strategy.get().template toAdd<double>() * result.player2Strategy.get().template toAdd<double>())
            .sumAbstract(game.getColumnVariables());
    EXPECT_EQ(2884ull, stateDistributionsUnderStrategies.getNonZeroCount());

    // Check that the number of distributions per state is one (or zero in the case where there are no prob1 states).
    stateDistributionCount = stateDistributionsUnderStrategies.sumAbstract(game.getNondeterminismVariables());
    EXPECT_EQ(1.0, stateDistributionCount.getMax());
}

#endif

TEST(GraphTest, ExplicitProb01) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::sparse::Model<double>> model =
        storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);

    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(),
                                                                                   storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                   model->getStates("observe0Greater1")));
    EXPECT_EQ(4409ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1316ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(),
                                                                                   storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                   model->getStates("observeIGreater1")));
    EXPECT_EQ(1091ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(4802ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(),
                                                                                   storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                   model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1032ull, statesWithProbability01.second.getNumberOfSetBits());
}

TEST(GraphTest, ExplicitProb01MinMax) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::sparse::Model<double>> model =
        storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;

    ASSERT_NO_THROW(statesWithProbability01 =
                        storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 =
                        storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ull, statesWithProbability01.second.getNumberOfSetBits());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("all_coins_equal_0")));
    EXPECT_EQ(77ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(149ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("all_coins_equal_0")));
    EXPECT_EQ(74ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(198ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("all_coins_equal_1")));
    EXPECT_EQ(94ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(33ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("all_coins_equal_1")));
    EXPECT_EQ(83ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(35ull, statesWithProbability01.second.getNumberOfSetBits());

    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ull, statesWithProbability01.second.getNumberOfSetBits());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(),
                                                                                      storm::storage::BitVector(model->getNumberOfStates(), true),
                                                                                      model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ull, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ull, statesWithProbability01.second.getNumberOfSetBits());
}
