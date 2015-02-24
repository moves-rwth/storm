#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/storage/dd/CuddDd.h"
#include "src/parser/PrismParser.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/sparse/Dtmc.h"
#include "src/builder/DdPrismModelBuilder.h"
#include "src/builder/ExplicitPrismModelBuilder.h"
#include "src/utility/graph.h"

TEST(GraphTest, SymbolicProb01) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);
    
    std::pair<storm::dd::Dd<storm::dd::DdType::CUDD>, storm::dd::Dd<storm::dd::DdType::CUDD>> statesWithProbability01;

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observe0Greater1")));
    EXPECT_EQ(4409, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1316, statesWithProbability01.second.getNonZeroCount());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observeIGreater1")));
    EXPECT_EQ(1091, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(4802, statesWithProbability01.second.getNonZeroCount());

    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD>>(), model->getReachableStates(), model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829, statesWithProbability01.first.getNonZeroCount());
    EXPECT_EQ(1032, statesWithProbability01.second.getNonZeroCount());
}

TEST(GraphTest, ExplicitProb01) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program);
    
    ASSERT_TRUE(model->getType() == storm::models::ModelType::Dtmc);
    
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observe0Greater1")));
    EXPECT_EQ(4409, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1316, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observeIGreater1")));
    EXPECT_EQ(1091, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(4802, statesWithProbability01.second.getNumberOfSetBits());
    
    ASSERT_NO_THROW(statesWithProbability01 = storm::utility::graph::performProb01(*model->as<storm::models::sparse::Dtmc<double>>(), storm::storage::BitVector(model->getNumberOfStates(), true), model->getStates("observeOnlyTrueSender")));
    EXPECT_EQ(5829, statesWithProbability01.first.getNumberOfSetBits());
    EXPECT_EQ(1032, statesWithProbability01.second.getNumberOfSetBits());
}