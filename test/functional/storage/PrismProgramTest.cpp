#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"

#include "src/utility/solver.h"

#include "src/storage/jani/Model.h"

#ifdef STORM_HAVE_MSAT
TEST(PrismProgramTest, FlattenModules) {
    storm::prism::Program program;
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3.nm"));

    std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>();
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(74, program.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/wlan0_collide.nm"));
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(180, program.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/csma2_2.nm"));
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(71, program.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/firewire.nm"));
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(5026, program.getModule(0).getNumberOfCommands());

    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/coin2.nm"));
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(13, program.getModule(0).getNumberOfCommands());
    
    ASSERT_NO_THROW(program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/two_dice.nm"));
    
    ASSERT_NO_THROW(program = program.flattenModules(smtSolverFactory));
    EXPECT_EQ(1, program.getNumberOfModules());
    EXPECT_EQ(16, program.getModule(0).getNumberOfCommands());
}
#endif

TEST(PrismProgramTest, ConvertToJani) {
    storm::prism::Program prismProgram;
    storm::jani::Model janiModel;

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/leader3.nm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/parser/prism/wlan0_collide.nm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
    
    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());

    ASSERT_NO_THROW(prismProgram = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm"));
    ASSERT_NO_THROW(janiModel = prismProgram.toJani());
}
