#include "storm-config.h"

#include "storm-pars/api/region.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm/api/builder.h"
#include "storm/api/storm.h"
#include "storm/logic/Formulas.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "test/storm_gtest.h"

TEST(AssumptionCheckerReachabilityMdpTest, simpleCase1_max) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pmdp/simpleCaseTest1Rews.nm";
    std::string formulaAsString = "Rmax=? [F s=5]";
    std::string constantsAsString = "";

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Mdp<storm::RationalFunction>>();

    ASSERT_EQ(7, model->getNumberOfStates());
    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999, 0.00001 <= q <= 0.999999", vars);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(
        model->getTransitionMatrix(), std::make_shared<storm::models::sparse::StandardRewardModel<storm::RationalFunction>>(model->getUniqueRewardModel()));
    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());

    // Assuming order 5,6 < 1 < 2 < 3 < 4
    storm::storage::BitVector bottomStates(7);
    storm::storage::BitVector topStates(7);
    bottomStates.set(5);
    bottomStates.set(6);
    auto order = std::make_shared<storm::analysis::Order>(
        storm::analysis::Order(&topStates, &bottomStates, 7, storm::storage::Decomposition<storm::storage::StronglyConnectedComponent>(), {}, false));
    order->add(1);
    order->add(2);
    order->add(3);
    order->add(4);
    order->addRelation(1, 5);
    order->addRelation(2, 1);
    order->addRelation(3, 2);
    order->addRelation(4, 3);

    // 0 > 1
    auto assumption = storm::analysis::Assumption(true, expressionManager, 0, 1, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));
    // 1 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 1, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 1 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 1, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 2
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 2, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));
    // 2 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 2, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 2 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 2, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 3
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 3, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 3 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 3, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 3 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 3, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 4
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 4, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 4 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 4, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 4 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 4, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    order->addToMdpScheduler(0, 1);
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 1, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));
    // 1 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 1, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 1 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 1, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 2
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 2, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));
    // 2 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 2, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 2 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 2, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 3
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 3, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));
    // 3 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 3, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 3 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 3, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));

    // 0 > 4
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 4, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 4 > 0
    assumption = storm::analysis::Assumption(true, expressionManager, 4, 0, storm::expressions::BinaryRelationExpression::RelationType::Greater);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
    // 4 == 0
    assumption = storm::analysis::Assumption(true, expressionManager, 0, 4, storm::expressions::BinaryRelationExpression::RelationType::Equal);
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, order, region));
}