#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"
#include "src/utility/IoUtility.h"
#include "src/ir/Program.h"
#include "src/adapters/ExplicitModelAdapter.h"

TEST(ParsePrismTest, parseAndOutput) {
	storm::parser::PrismParser parser;
	storm::ir::Program program;
	ASSERT_NO_THROW(program = parser.parseFile("examples/dtmc/crowds/crowds5_5.pm"));
	storm::adapters::ExplicitModelAdapter adapter(program);

	std::shared_ptr<storm::models::Dtmc<double>> model = adapter.getModel()->as<storm::models::Dtmc<double>>();

	ASSERT_EQ(model->getNumberOfStates(), (uint_fast64_t)8607);
	ASSERT_EQ(model->getNumberOfTransitions(), (uint_fast64_t)15113);
}


