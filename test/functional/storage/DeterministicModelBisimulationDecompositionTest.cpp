#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "storage/DeterministicModelBisimulationDecomposition.h"

TEST(DeterministicModelBisimulationDecomposition, Die) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim(*dtmc, boost::optional<std::set<std::string>>(), false, true);
    std::shared_ptr<storm::models::AbstractModel<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(13, result->getNumberOfStates());
    EXPECT_EQ(20, result->getNumberOfTransitions());

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim2(*dtmc, std::set<std::string>({"one"}), false, true);
    ASSERT_NO_THROW(result = bisim2.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(5, result->getNumberOfStates());
    EXPECT_EQ(8, result->getNumberOfTransitions());
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim3(*dtmc, std::set<std::string>({"one"}), true, true);
    ASSERT_NO_THROW(result = bisim3.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(5, result->getNumberOfStates());
    EXPECT_EQ(8, result->getNumberOfTransitions());

//	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.coin_flips.trans.rew");
//    
//    ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
//	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();
}

TEST(DeterministicModelBisimulationDecomposition, Crowds) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim(*dtmc, boost::optional<std::set<std::string>>(), false, true);
    std::shared_ptr<storm::models::AbstractModel<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(334, result->getNumberOfStates());
    EXPECT_EQ(546, result->getNumberOfTransitions());
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim2(*dtmc, std::set<std::string>({"observe0Greater1"}), false, true);
    ASSERT_NO_THROW(result = bisim2.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(65, result->getNumberOfStates());
    EXPECT_EQ(105, result->getNumberOfTransitions());

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim3(*dtmc, std::set<std::string>({"observe0Greater1"}), true, true);
    ASSERT_NO_THROW(result = bisim3.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(43, result->getNumberOfStates());
    EXPECT_EQ(83, result->getNumberOfTransitions());
}
