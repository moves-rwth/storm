#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/AutoParser.h"
#include "storage/DeterministicModelBisimulationDecomposition.h"

TEST(DeterministicModelBisimulationDecomposition, Die) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/die/die.tra", STORM_CPP_BASE_PATH "/examples/dtmc/die/die.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim(*dtmc);
    std::shared_ptr<storm::models::AbstractModel<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(13, result->getNumberOfStates());
    EXPECT_EQ(20, result->getNumberOfTransitions());

#ifdef WINDOWS
	storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options;
#else
	typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"one"});
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim2(*dtmc, options);
    ASSERT_NO_THROW(result = bisim2.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(5, result->getNumberOfStates());
    EXPECT_EQ(8, result->getNumberOfTransitions());

    options.bounded = false;
    options.weak = true;
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim3(*dtmc, options);
    ASSERT_NO_THROW(result = bisim3.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(5, result->getNumberOfStates());
    EXPECT_EQ(8, result->getNumberOfTransitions());
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("one");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);

#ifdef WINDOWS
	storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options2(*dtmc, *eventuallyFormula);
#else
	typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options2(*dtmc, *eventuallyFormula);
#endif
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(result = bisim4.getQuotient());
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(5, result->getNumberOfStates());
    EXPECT_EQ(8, result->getNumberOfTransitions());
}

TEST(DeterministicModelBisimulationDecomposition, Crowds) {
	std::shared_ptr<storm::models::AbstractModel<double>> abstractModel = storm::parser::AutoParser::parseModel(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.tra", STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.lab", "", "");
    
    ASSERT_EQ(abstractModel->getType(), storm::models::DTMC);
	std::shared_ptr<storm::models::Dtmc<double>> dtmc = abstractModel->as<storm::models::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim(*dtmc);
    std::shared_ptr<storm::models::AbstractModel<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(334, result->getNumberOfStates());
    EXPECT_EQ(546, result->getNumberOfTransitions());
    
#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options;
#else
	typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"observe0Greater1"});
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim2(*dtmc, options);
    ASSERT_NO_THROW(result = bisim2.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(65, result->getNumberOfStates());
    EXPECT_EQ(105, result->getNumberOfTransitions());

    options.bounded = false;
    options.weak = true;
    
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim3(*dtmc, options);
    ASSERT_NO_THROW(result = bisim3.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(43, result->getNumberOfStates());
    EXPECT_EQ(83, result->getNumberOfTransitions());
    
    auto labelFormula = std::make_shared<storm::logic::AtomicLabelFormula>("observe0Greater1");
    auto eventuallyFormula = std::make_shared<storm::logic::EventuallyFormula>(labelFormula);
    
#ifdef WINDOWS
	storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options2(*dtmc, *eventuallyFormula);
#else
	typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options2(*dtmc, *eventuallyFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(result = bisim4.getQuotient());

    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(64, result->getNumberOfStates());
    EXPECT_EQ(104, result->getNumberOfTransitions());
    
    auto probabilityOperatorFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(eventuallyFormula);
    
#ifdef WINDOWS
	storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options3(*dtmc, *probabilityOperatorFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options3(*dtmc, *probabilityOperatorFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim5(*dtmc, options3);
    ASSERT_NO_THROW(result = bisim5.getQuotient());

    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(64, result->getNumberOfStates());
    EXPECT_EQ(104, result->getNumberOfTransitions());
    
    auto boundedUntilFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), labelFormula, 50);
    
#ifdef WINDOWS
	storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options4(*dtmc, *boundedUntilFormula);
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<double>::Options options4(*dtmc, *boundedUntilFormula);
#endif
    storm::storage::DeterministicModelBisimulationDecomposition<double> bisim6(*dtmc, options4);
    ASSERT_NO_THROW(result = bisim6.getQuotient());
    
    EXPECT_EQ(storm::models::DTMC, result->getType());
    EXPECT_EQ(65, result->getNumberOfStates());
    EXPECT_EQ(105, result->getNumberOfTransitions());
}
