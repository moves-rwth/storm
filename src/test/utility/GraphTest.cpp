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

    model->printModelInformationToStream(std::cout);

    storm::utility::shortestPaths::ShortestPathsGenerator<double> shortestPathsGenerator(model);

    // TODO: actually write tests here

    /*
    std::queue<uint_fast64_t> nodeQueue;
    for (uint_fast64_t initialNode : model->getInitialStates()) {
        for (uint_fast64_t succ : shortestPathSuccessors[initialNode]) {
            nodeQueue.push(succ);
        }
        storm::storage::sparse::path<double> path;
        path.tail_k = 1;
        path.distance = dijkstraDistance[initialNode];
        assert(path.distance == 1);
        kShortestPaths[initialNode].push_back(path);
    }

    // Dijkstra BFS
    while (!nodeQueue.empty()) {
        uint_fast64_t currentNode = nodeQueue.front();
        nodeQueue.pop();

        for (auto succ : shortestPathSuccessors[currentNode]) {
            nodeQueue.push(succ);
        }

        storm::storage::sparse::path<double> path;
        path.tail = shortestPathPredecessor[currentNode];
        path.tail_k = 1;
        path.distance = dijkstraDistance[currentNode];
        kShortestPaths[currentNode].push_back(path);
    }
    */

    // FIXME: ~~treat starting node(s) separately~~ actually, this whole thing should be done differently:
    // first I need to run over the Dijkstra result and make a tree (as vector of vectors) of successors,
    // then walk that tree DF/BF
    /*
    for (auto node : model->getInitialStates()) {
        storm::storage::sparse::path<double> p = {};
        p.distance = dijkstraDistance[node];
        assert(p.distance == 1);
        kShortestPaths[node].emplace_back(p);
    }
    */

    // shortest paths are stored recursively, so the predecessor must always be dealt with first
    // by considering the nodes in order of distance, we should have roughly the correct order,
    // but not quite: in the case s ~~~> u -1-> v, v might be listed before u, in which case it must be deferred
    /*
    while (!nodeQueue.empty()) {
        std::pair<double, uint_fast64_t> distanceStatePair = nodeQueue.front();
        nodeQueue.pop();

        uint_fast64_t currentNode = distanceStatePair.second;

        uint_fast64_t predecessor = shortestPathPredecessors[currentNode];
        if (kShortestPaths[predecessor].empty) {
            // we need to take care of the predecessor first; defer this one
            nodeQueue.emplace(currentNode);
            continue;
        } else {
            //shortestPaths[currentNode].emplace(predecessor, 1, )
        }
    }
    */

    EXPECT_TRUE(false);
}
