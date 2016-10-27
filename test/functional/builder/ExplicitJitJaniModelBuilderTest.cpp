#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"
#include "src/storage/jani/Model.h"
#include "src/builder/jit/ExplicitJitJaniModelBuilder.h"

TEST(ExplicitJitJaniModelBuilderTest, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::jani::Model janiModel = program.toJani();
    
    storm::builder::jit::ExplicitJitJaniModelBuilder<double>(janiModel).build();
}
