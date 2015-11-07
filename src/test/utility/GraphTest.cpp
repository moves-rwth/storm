#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/parser/PrismParser.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/utility/graph.h"
#include "storm/utility/shortestPaths.cpp"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/DdManager.h"

TEST(GraphTest, SymbolicProb01_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);
    
    std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observe0Greater1")));
    EXPECT_EQ(4409ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1316ul, statesWithProbability01.second.getNonZeroCount());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observeIGreater1")));
    EXPECT_EQ(1091ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(4802ul, statesWithProbability01.second.getNonZeroCount());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1032ul, statesWithProbability01.second.getNonZeroCount());
}

TEST(GraphTest, SymbolicProb01_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);
    
    std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("observe0Greater1")));
    EXPECT_EQ(4409ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1316ul, statesWithProbability01.second.getNonZeroCount());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("observeIGreater1")));
    EXPECT_EQ(1091ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(4802ul, statesWithProbability01.second.getNonZeroCount());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829ul, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1032ul, statesWithProbability01.second.getNonZeroCount());
}

TEST(GraphTest, SymbolicProb01MinMax_Cudd) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ul, statesWithProbability01.second.getNonZeroCount());
    }
    
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(77ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(149ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(74ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(198ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(94ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(33ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(83ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(35ul, statesWithProbability01.second.getNonZeroCount());
    }
        
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::CUDD>, storm::dd::Bdd<storm::dd::DdType::CUDD>> statesWithProbability01;
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ul, statesWithProbability01.second.getNonZeroCount());
    }
}

TEST(GraphTest, SymbolicProb01MinMax_Sylvan) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);

    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("elected")));
        EXPECT_EQ(0ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(364ul, statesWithProbability01.second.getNonZeroCount());
    }
    
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(77ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(149ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("all_coins_equal_0")));
        EXPECT_EQ(74ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(198ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(94ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(33ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("all_coins_equal_1")));
        EXPECT_EQ(83ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(35ul, statesWithProbability01.second.getNonZeroCount());
    }
    
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan>().build(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    {
        std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, storm::dd::Bdd<storm::dd::DdType::Sylvan>> statesWithProbability01;

        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ul, statesWithProbability01.second.getNonZeroCount());
        
        ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan>>(), model->getReachableStates(), model->getStates("collision_max_backoff")));
        EXPECT_EQ(993ul, statesWithProbability01.first.getNonZeroCount());
        EXPECT_EQ(16ul, statesWithProbability01.second.getNonZeroCount());
    }
}

TEST(GraphTest, ExplicitProb01) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observe0Greater1")));
    EXPECT_EQ(4409ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1316ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observeIGreater1")));
    EXPECT_EQ(1091ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(4802ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1032ul, statesWithProbability01.second.getNumberOfSetBits());
}

TEST(GraphTest, ExplicitProb01MinMax) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader3.nm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ul, statesWithProbability01.second.getNumberOfSetBits());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/coin2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("all_coins_equal_0")));
    EXPECT_EQ(77ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(149ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("all_coins_equal_0")));
    EXPECT_EQ(74ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(198ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("all_coins_equal_1")));
    EXPECT_EQ(94ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(33ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("all_coins_equal_1")));
    EXPECT_EQ(83ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(35ul, statesWithProbability01.second.getNumberOfSetBits());
    
    modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/csma2-2.nm");
    program = modelDescription.preprocess().asPrismProgram();
    model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ul, statesWithProbability01.second.getNumberOfSetBits());
}


TEST(GraphTest, kshortest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>().translateProgram(program);

    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);

    storm::storage::sparse::state_type testState = 300;

    storm::utility::shortestPaths::ShortestPathsGenerator<double> shortestPathsGenerator(model);

    // the 1-shortest path is computed as a preprocessing step via Dijkstra;
    // since there were some bugs here in the past, let's test it separately
    double dijkstraSPDistance = shortestPathsGenerator.getKShortest(testState, 1);
    std::cout << "Res: " << dijkstraSPDistance << std::endl;
    //EXPECT_NEAR(0.0158593, dijkstraSPDistance, 0.0000001);
    EXPECT_DOUBLE_EQ(0.015859334652581887, dijkstraSPDistance);

    // main test
    double kSPDistance1 = shortestPathsGenerator.getKShortest(testState, 100);
    EXPECT_DOUBLE_EQ(1.5231305000339649e-06, kSPDistance1);

    // let's test again to ensure re-entry is no problem
    double kSPDistance2 = shortestPathsGenerator.getKShortest(testState, 500);
    EXPECT_DOUBLE_EQ(3.0462610000679282e-08, kSPDistance2);
}