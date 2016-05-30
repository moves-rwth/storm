#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/models/symbolic/StandardRewardModel.h"
#include "src/parser/PrismParser.h"
#include "src/builder/DdJaniModelBuilder.h"

TEST(DdJaniModelBuilderTest_Sylvan, Dtmc) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
    storm::jani::Model janiModel = program.toJani();
    
    storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double> builder(janiModel);
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan>> model = builder.translate();
    EXPECT_EQ(13ul, model->getNumberOfStates());
    EXPECT_EQ(20ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
    janiModel = program.toJani();
    builder = storm::builder::DdJaniModelBuilder<storm::dd::DdType::Sylvan, double>(janiModel);
    model = builder.translate();
    EXPECT_EQ(677ul, model->getNumberOfStates());
    EXPECT_EQ(867ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
    janiModel = program.toJani();
    model = builder.translate();
    EXPECT_EQ(8607ul, model->getNumberOfStates());
    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
    janiModel = program.toJani();
    model = builder.translate();
    EXPECT_EQ(273ul, model->getNumberOfStates());
    EXPECT_EQ(397ul, model->getNumberOfTransitions());
    
    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
    janiModel = program.toJani();
    model = builder.translate();
    EXPECT_EQ(1728ul, model->getNumberOfStates());
    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
}

//TEST(DdPrismModelBuilderTest_Cudd, Dtmc) {
//    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die.pm");
//    
//    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
//    EXPECT_EQ(13ul, model->getNumberOfStates());
//    EXPECT_EQ(20ul, model->getNumberOfTransitions());
//    
//    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/brp-16-2.pm");
//    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
//    EXPECT_EQ(677ul, model->getNumberOfStates());
//    EXPECT_EQ(867ul, model->getNumberOfTransitions());
//    
//    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-5.pm");
//    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
//    EXPECT_EQ(8607ul, model->getNumberOfStates());
//    EXPECT_EQ(15113ul, model->getNumberOfTransitions());
//    
//    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/leader-3-5.pm");
//    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
//    EXPECT_EQ(273ul, model->getNumberOfStates());
//    EXPECT_EQ(397ul, model->getNumberOfTransitions());
//    
//    program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/nand-5-2.pm");
//    model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>().translateProgram(program);
//    EXPECT_EQ(1728ul, model->getNumberOfStates());
//    EXPECT_EQ(2505ul, model->getNumberOfTransitions());
//}