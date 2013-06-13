#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"
//#include "src/utility/IoUtility.h"
#include "src/ir/Program.h"
#include "src/adapters/ExplicitModelAdapter.h"
#include "src/models/Dtmc.h"
#include "src/models/Mdp.h"

TEST(ParsePrismTest, parseCrowds5_5) {
	storm::ir::Program program;
	ASSERT_NO_THROW(program = storm::parser::PrismParserFromFile(STORM_CPP_BASE_PATH "/examples/dtmc/crowds/crowds5_5.pm"));
	storm::adapters::ExplicitModelAdapter adapter(program);

	std::shared_ptr<storm::models::Dtmc<double>> model = adapter.getModel()->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(model->getNumberOfStates(), (uint_fast64_t)8607);
	ASSERT_EQ(model->getNumberOfTransitions(), (uint_fast64_t)15113);
}

TEST(ParsePrismTest, parseTwoDice) {
	storm::ir::Program program;
	ASSERT_NO_THROW(program = storm::parser::PrismParserFromFile(STORM_CPP_BASE_PATH "/examples/mdp/two_dice/two_dice.nm"));
	storm::adapters::ExplicitModelAdapter adapter(program);

	std::shared_ptr<storm::models::Mdp<double>> model = adapter.getModel()->as<storm::models::Mdp<double>>();
	
	ASSERT_EQ(model->getNumberOfStates(), (uint_fast64_t)169);
	ASSERT_EQ(model->getNumberOfChoices(), (uint_fast64_t)254);
	ASSERT_EQ(model->getNumberOfTransitions(), (uint_fast64_t)436);
}
