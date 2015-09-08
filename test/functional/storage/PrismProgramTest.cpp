#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"

#include "src/utility/solver.h"

#ifdef STORM_HAVE_MSAT
TEST(PrismProgramTest, FlattenModules) {
    storm::prism::Program result;
    ASSERT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3.nm"));

    std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory(new storm::utility::solver::MathsatSmtSolverFactory());
    
    ASSERT_NO_THROW(result = result.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(74, result.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/wlan0_collide.nm"));
    
    ASSERT_NO_THROW(result = result.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(180, result.getModule(0).getNumberOfCommands());
}
#endif
