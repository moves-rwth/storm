#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/MaximalEndComponentDecomposition.h"
#include "src/models/MarkovAutomaton.h"

TEST(MaximalEndComponentDecomposition, FullSystem1) {
    std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.lab", "", "");

    std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::MarkovAutomaton<double>>();

    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition;
    ASSERT_NO_THROW(mecDecomposition = storm::storage::MaximalEndComponentDecomposition<double>(*markovAutomaton));
    
    ASSERT_EQ(2, mecDecomposition.size());
    
    // Now, because there is no ordering we have to check the contents of the MECs in a symmetrical way.
    storm::storage::MaximalEndComponent const& mec1 = mecDecomposition[0];
    if (mec1.containsState(3)) {
        ASSERT_TRUE(mec1.containsState(8));
        ASSERT_TRUE(mec1.containsState(9));
        ASSERT_TRUE(mec1.containsState(10));
        ASSERT_FALSE(mec1.containsState(0));
        ASSERT_FALSE(mec1.containsState(1));
        ASSERT_FALSE(mec1.containsState(2));
        ASSERT_FALSE(mec1.containsState(4));
        ASSERT_FALSE(mec1.containsState(5));
        ASSERT_FALSE(mec1.containsState(6));
        ASSERT_FALSE(mec1.containsState(7));
    } else if (mec1.containsState(4)) {
        ASSERT_TRUE(mec1.containsState(5));
        ASSERT_TRUE(mec1.containsState(6));
        ASSERT_TRUE(mec1.containsState(7));
        ASSERT_FALSE(mec1.containsState(0));
        ASSERT_FALSE(mec1.containsState(1));
        ASSERT_FALSE(mec1.containsState(2));
        ASSERT_FALSE(mec1.containsState(3));
        ASSERT_FALSE(mec1.containsState(8));
        ASSERT_FALSE(mec1.containsState(9));
        ASSERT_FALSE(mec1.containsState(10));
    } else {
        // This case must never happen as the only two existing MECs contain either 3 or 4.
        ASSERT_TRUE(false);
    }
    
    storm::storage::MaximalEndComponent const& mec2 = mecDecomposition[1];
    if (mec2.containsState(3)) {
        ASSERT_TRUE(mec2.containsState(8));
        ASSERT_TRUE(mec2.containsState(9));
        ASSERT_TRUE(mec2.containsState(10));
        ASSERT_FALSE(mec2.containsState(0));
        ASSERT_FALSE(mec2.containsState(1));
        ASSERT_FALSE(mec2.containsState(2));
        ASSERT_FALSE(mec2.containsState(4));
        ASSERT_FALSE(mec2.containsState(5));
        ASSERT_FALSE(mec2.containsState(6));
        ASSERT_FALSE(mec2.containsState(7));
    } else if (mec2.containsState(4)) {
        ASSERT_TRUE(mec2.containsState(5));
        ASSERT_TRUE(mec2.containsState(6));
        ASSERT_TRUE(mec2.containsState(7));
        ASSERT_FALSE(mec2.containsState(0));
        ASSERT_FALSE(mec2.containsState(1));
        ASSERT_FALSE(mec2.containsState(2));
        ASSERT_FALSE(mec2.containsState(3));
        ASSERT_FALSE(mec2.containsState(8));
        ASSERT_FALSE(mec2.containsState(9));
        ASSERT_FALSE(mec2.containsState(10));
    } else {
        // This case must never happen as the only two existing MECs contain either 3 or 4.
        ASSERT_TRUE(false);
    }
}

TEST(MaximalEndComponentDecomposition, FullSystem2) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.lab", "", "");
    
    std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::MarkovAutomaton<double>>();
    
    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition;
    ASSERT_NO_THROW(mecDecomposition = storm::storage::MaximalEndComponentDecomposition<double>(*markovAutomaton));
    
    ASSERT_EQ(1, mecDecomposition.size());
    
    // Now, because there is no ordering we have to check the contents of the MECs in a symmetrical way.
    storm::storage::MaximalEndComponent const& mec1 = mecDecomposition[0];
    if (mec1.containsState(4)) {
        ASSERT_TRUE(mec1.containsState(5));
        ASSERT_TRUE(mec1.containsState(6));
        ASSERT_TRUE(mec1.containsState(7));
        ASSERT_FALSE(mec1.containsState(0));
        ASSERT_FALSE(mec1.containsState(1));
        ASSERT_FALSE(mec1.containsState(2));
        ASSERT_FALSE(mec1.containsState(3));
        ASSERT_FALSE(mec1.containsState(8));
        ASSERT_FALSE(mec1.containsState(9));
        ASSERT_FALSE(mec1.containsState(10));
    } else {
        // This case must never happen as the only two existing MECs contain 4.
        ASSERT_TRUE(false);
    }
}

TEST(MaximalEndComponentDecomposition, Subsystem) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.lab", "", "");
    
    std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::MarkovAutomaton<double>>();
    
    storm::storage::BitVector subsystem(markovAutomaton->getNumberOfStates(), true);
    subsystem.set(7, false);
    
    storm::storage::MaximalEndComponentDecomposition<double> mecDecomposition;
    ASSERT_NO_THROW(mecDecomposition = storm::storage::MaximalEndComponentDecomposition<double>(*markovAutomaton, subsystem));
    
    ASSERT_EQ(1, mecDecomposition.size());
    
    storm::storage::MaximalEndComponent const& mec1 = mecDecomposition[0];
    
    if (mec1.containsState(3)) {
        ASSERT_TRUE(mec1.containsState(8));
        ASSERT_TRUE(mec1.containsState(9));
        ASSERT_TRUE(mec1.containsState(10));
        ASSERT_FALSE(mec1.containsState(0));
        ASSERT_FALSE(mec1.containsState(1));
        ASSERT_FALSE(mec1.containsState(2));
        ASSERT_FALSE(mec1.containsState(4));
        ASSERT_FALSE(mec1.containsState(5));
        ASSERT_FALSE(mec1.containsState(6));
        ASSERT_FALSE(mec1.containsState(7));
    } else {
        // This case must never happen as the only two existing MEC contains 3.
        ASSERT_TRUE(false);
    }
}
