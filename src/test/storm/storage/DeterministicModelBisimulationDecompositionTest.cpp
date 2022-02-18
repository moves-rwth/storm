#include "storm-config.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "test/storm_gtest.h"

TEST(DeterministicModelBisimulationDecomposition, Die) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/die.tra", STORM_TEST_RESOURCES_DIR "/lab/die.lab", "", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc);
    ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
    std::shared_ptr<storm::models::sparse::Model<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(13ul, result->getNumberOfStates());
    EXPECT_EQ(20ul, result->getNumberOfTransitions());

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"one"});

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim2(*dtmc, options);
    ASSERT_NO_THROW(bisim2.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim2.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());

    options.setType(storm::storage::BisimulationType::Weak);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(bisim4.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim4.getQuotient());
    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());
}

TEST(DeterministicModelBisimulationDecomposition, Crowds) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra", STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab", "", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc);
    std::shared_ptr<storm::models::sparse::Model<double>> result;
    ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(334ul, result->getNumberOfStates());
    EXPECT_EQ(546ul, result->getNumberOfTransitions());

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"observe0Greater1"});

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim2(*dtmc, options);
    ASSERT_NO_THROW(bisim2.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim2.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(65ul, result->getNumberOfStates());
    EXPECT_EQ(105ul, result->getNumberOfTransitions());

    options.setType(storm::storage::BisimulationType::Weak);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(43ul, result->getNumberOfStates());
    EXPECT_EQ(83ul, result->getNumberOfTransitions());

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options3(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim5(*dtmc, options3);
    ASSERT_NO_THROW(bisim5.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim5.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(64ul, result->getNumberOfStates());
    EXPECT_EQ(104ul, result->getNumberOfTransitions());

    formula = formulaParser.parseSingleFormulaFromString("P=? [true U<=50 \"observe0Greater1\"] ");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options4(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim6(*dtmc, options4);
    ASSERT_NO_THROW(bisim6.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim6.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(65ul, result->getNumberOfStates());
    EXPECT_EQ(105ul, result->getNumberOfTransitions());
}
