#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/PrismParser.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/builder/ExplicitModelBuilder.h"
#include "src/utility/graph.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/DdManager.h"

TEST(GraphTest, SymbolicProb01_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
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
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
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
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
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
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
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
        
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
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
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
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
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
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
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
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
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
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
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader3.nm");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("elected")));
    EXPECT_EQ(0ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(364ul, statesWithProbability01.second.getNumberOfSetBits());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/coin2-2.nm");
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
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/csma2-2.nm");
    model = storm::builder::ExplicitModelBuilder<double>(program, storm::generator::NextStateGeneratorOptions(false, true)).build();
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Mdp);
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Min(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ul, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01Max(*model->as<storm::models::sparse::Mdp<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("collision_max_backoff")));
    EXPECT_EQ(993ul, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(16ul, statesWithProbability01.second.getNumberOfSetBits());
}