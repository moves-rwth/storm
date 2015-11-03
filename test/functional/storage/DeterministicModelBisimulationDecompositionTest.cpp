#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "src/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"

TEST(DeterministicModelBisimulationDecomposition, Die) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", "");

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

    options.bounded = false;
    options.type = storm::storage::BisimulationType::Weak;

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());

    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("one");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *eventuallyFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *eventuallyFormula);
#endif

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(bisim4.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim4.getQuotient());
    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());
}

TEST(DeterministicModelBisimulationDecomposition, Crowds) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");

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

    options.bounded = false;
    options.type = storm::storage::BisimulationType::Weak;

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(43ul, result->getNumberOfStates());
    EXPECT_EQ(83ul, result->getNumberOfTransitions());

    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observe0Greater1");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *eventuallyFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *eventuallyFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(bisim4.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim4.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(64ul, result->getNumberOfStates());
    EXPECT_EQ(104ul, result->getNumberOfTransitions());

    auto probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(eventuallyFormula);

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options3(*dtmc, *probabilityOperatorFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options3(*dtmc, *probabilityOperatorFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim5(*dtmc, options3);
    ASSERT_NO_THROW(bisim5.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim5.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(64ul, result->getNumberOfStates());
    EXPECT_EQ(104ul, result->getNumberOfTransitions());

    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), labelFormula, 50);

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options4(*dtmc, *boundedUntilFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options4(*dtmc, *boundedUntilFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim6(*dtmc, options4);
    ASSERT_NO_THROW(bisim6.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim6.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(65ul, result->getNumberOfStates());
    EXPECT_EQ(105ul, result->getNumberOfTransitions());
}
