#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/models/MarkovAutomaton.h"

TEST(StronglyConnectedComponentDecomposition, FullSystem1) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.lab", "", "");

	std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::MarkovAutomaton<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton));
    ASSERT_EQ(5, sccDecomposition.size());
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true));
    ASSERT_EQ(2, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, true));
    ASSERT_EQ(2, sccDecomposition.size());
    
    markovAutomaton = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, FullSystem2) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.lab", "", "");

	std::shared_ptr<storm::models::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::MarkovAutomaton<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, false));
    
    ASSERT_EQ(sccDecomposition.size(), 2);
    
    // Now, because there is no ordering we have to check the contents of the MECs in a symmetrical way.
    storm::storage::StateBlock const& scc1 = sccDecomposition[0];
    storm::storage::StateBlock const& scc2 = sccDecomposition[1];

    std::vector<uint_fast64_t> correctScc1 = {1, 3, 8, 9, 10};
    std::vector<uint_fast64_t> correctScc2 = {4, 5, 6, 7};
    ASSERT_TRUE(scc1 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc1 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()));
    ASSERT_TRUE(scc2 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc2 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()));
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, true));
    ASSERT_EQ(1, sccDecomposition.size());

    markovAutomaton = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, MatrixBasedSystem) {
	storm::parser::AutoParser<double> parser(STORM_CPP_BASE_PATH "/examples/dtmc/scc/scc.tra", STORM_CPP_BASE_PATH "/examples/dtmc/scc/scc.lab", "", "");
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = parser.getModel<storm::models::Dtmc<double>>();

	storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
	ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true, false));

	ASSERT_EQ(sccDecomposition.size(), 3);

	// Now, because there is no ordering we have to check the contents of the MECs in a symmetrical way.
	storm::storage::StateBlock const& scc1 = sccDecomposition[0];
	storm::storage::StateBlock const& scc2 = sccDecomposition[1];
	storm::storage::StateBlock const& scc3 = sccDecomposition[2];

	std::vector<uint_fast64_t> correctScc1 = { 1, 2, 3, 4 };
	std::vector<uint_fast64_t> correctScc2 = { 5, 6, 7, 8 };
	std::vector<uint_fast64_t> correctScc3 = { 0 };

	ASSERT_TRUE(scc1 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc1 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()) || scc1 == storm::storage::StateBlock(correctScc3.begin(), correctScc3.end()));
	ASSERT_TRUE(scc2 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc2 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()) || scc2 == storm::storage::StateBlock(correctScc3.begin(), correctScc3.end()));
	ASSERT_TRUE(scc3 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc3 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()) || scc3 == storm::storage::StateBlock(correctScc3.begin(), correctScc3.end()));

	ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*dtmc, true, true));
	ASSERT_EQ(2, sccDecomposition.size());

	dtmc = nullptr;
}
