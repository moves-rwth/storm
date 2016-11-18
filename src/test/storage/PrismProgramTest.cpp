#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/parser/PrismParser.h"

#include "storm/utility/solver.h"

#include "storm/storage/jani/Model.h"

#ifdef STORM_HAVE_MSAT
TEST(PrismProgramTest, FlattenModules) {
    storm::prism::Program result;
    ASSERT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/parser/prism/leader3.nm"));

    std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory(new storm::utility::solver::MathsatSmtSolverFactory());
    
    ASSERT_NO_THROW(result = result.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(74, result.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(result = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/parser/prism/wlan0_collide.nm"));
    
    ASSERT_NO_THROW(result = result.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, result.getNumberOfModules());
    EXPECT_EQ(180, result.getModule(0).getNumberOfCommands());
}
#endif

TEST(PrismProgramTest, ConvertToJani) {
    storm::prism::Program prismProgram;
    storm::jani::Model janiModel;

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/parser/prism/leader3.nm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/parser/prism/wlan0_collide.nm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/brp-16-2.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/crowds-5-5.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/leader-3-5.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/builder/nand-5-2.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
}
