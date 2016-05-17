#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"

#include "src/builder/ExplicitPrismModelBuilder.h"

#include "src/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"

TEST(NondeterministicModelBisimulationDecomposition, TwoDice) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/two_dice.nm");

    // Build the die model without its reward model.
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>(program).translate();

    ASSERT_EQ(model->getType(), storm::models::ModelType::Mdp);
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = model->as<storm::models::sparse::Mdp<double>>();
    
    storm::storage::NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>> bisim(*mdp);
    ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
    std::shared_ptr<storm::models::sparse::Model<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());
    
    EXPECT_EQ(storm::models::ModelType::Mdp, result->getType());
    EXPECT_EQ(77ul, result->getNumberOfStates());
    EXPECT_EQ(183ul, result->getNumberOfTransitions());
    EXPECT_EQ(97ul, result->as<storm::models::sparse::Mdp<double>>()->getNumberOfChoices());

    typename storm::storage::NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>>::Options options;
    options.respectedAtomicPropositions = std::set<std::string>({"two"});
    
    storm::storage::NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>> bisim2(*mdp, options);
    ASSERT_NO_THROW(bisim2.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim2.getQuotient());
    
    EXPECT_EQ(storm::models::ModelType::Mdp, result->getType());
    EXPECT_EQ(11ul, result->getNumberOfStates());
    EXPECT_EQ(26ul, result->getNumberOfTransitions());
    EXPECT_EQ(14ul, result->as<storm::models::sparse::Mdp<double>>()->getNumberOfChoices());

    // A parser that we use for conveniently constructing the formulas.
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");

    typename storm::storage::NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>>::Options options2(*mdp, *formula);
    
    storm::storage::NondeterministicModelBisimulationDecomposition<storm::models::sparse::Mdp<double>> bisim3(*mdp, options2);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());
    
    EXPECT_EQ(storm::models::ModelType::Mdp, result->getType());
    EXPECT_EQ(11ul, result->getNumberOfStates());
    EXPECT_EQ(26ul, result->getNumberOfTransitions());
    EXPECT_EQ(14ul, result->as<storm::models::sparse::Mdp<double>>()->getNumberOfChoices());
}
