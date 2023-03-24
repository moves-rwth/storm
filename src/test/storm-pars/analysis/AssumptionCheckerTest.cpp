#include <storm/storage/StronglyConnectedComponentDecomposition.h>
#include "storm-config.h"

#include "storm-pars/api/analysis.h"
#include "storm-pars/api/region.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm/api/builder.h"
#include "storm/api/storm.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "test/storm_gtest.h"


TEST(AssumptionCheckerTest, Brp_no_bisimulation) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [F s=4 & i=N ]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(193ul, dtmc->getNumberOfStates());
    ASSERT_EQ(383ul, dtmc->getNumberOfTransitions());

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.00001, 0.00001 <= pL <= 0.99999", vars);

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(dtmc->getTransitionMatrix());
    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
    expressionManager->declareRationalVariable("7");
    expressionManager->declareRationalVariable("5");
    storm::storage::BitVector above(193);
    above.set(0);
    storm::storage::BitVector below(193);
    below.set(1);

    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();
    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<storm::RationalFunction>(model->getTransitionMatrix(), options);

    auto statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
    auto dummyOrder = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 193, decomposition, statesSorted));

    auto assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(), expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(), expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(), storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, dummyOrder, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, dummyOrder, region));


    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, dummyOrder, region));


    checker.initializeCheckingOnSamples(formulas[0], dtmc, region, 3);
    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, checker.validateAssumption(assumption, dummyOrder, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, dummyOrder, region));


    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("7").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("5").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, dummyOrder, region));
}

TEST(AssumptionCheckerTest, Simple1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P=? [F s=3]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(5ul, dtmc->getNumberOfStates());
    ASSERT_EQ(8ul, dtmc->getNumberOfTransitions());

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.99999", vars);

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(dtmc->getTransitionMatrix());

    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
    expressionManager->declareRationalVariable("1");
    expressionManager->declareRationalVariable("2");
    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();
    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<storm::RationalFunction>(model->getTransitionMatrix(), options);

    storm::storage::BitVector above(5);
    above.set(3);
    storm::storage::BitVector below(5);
    below.set(4);
    auto statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 5, decomposition, statesSorted));

    // Validating
    auto assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    region = storm::api::parseRegion<storm::RationalFunction>("0.51 <= p <= 0.99", vars);
    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));
}

TEST(AssumptionCheckerTest, Casestudy1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy1.pm";
    std::string formulaAsString = "P=? [F s=3]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(5ul, dtmc->getNumberOfStates());
    ASSERT_EQ(8ul, dtmc->getNumberOfTransitions());

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.99999", vars);

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(dtmc->getTransitionMatrix());

    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
    expressionManager->declareRationalVariable("1");
    expressionManager->declareRationalVariable("2");

    storm::storage::BitVector above(5);
    above.set(3);
    storm::storage::BitVector below(5);
    below.set(4);

    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();
    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<storm::RationalFunction>(model->getTransitionMatrix(), options);
    auto statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 5, decomposition, statesSorted));

    // Validating
    auto assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));


    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    checker.initializeCheckingOnSamples(formulas[0], dtmc, region, 3);
    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));


    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));
}

TEST(AssumptionCheckerTest, Casestudy2) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy2.pm";
    std::string formulaAsString = "P=? [F s=4]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(6ul, dtmc->getNumberOfStates());
    ASSERT_EQ(12ul, dtmc->getNumberOfTransitions());

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.99999", vars);

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(dtmc->getTransitionMatrix());

    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
    expressionManager->declareRationalVariable("1");
    expressionManager->declareRationalVariable("2");

    storm::storage::BitVector above(6);
    above.set(4);
    storm::storage::BitVector below(6);
    below.set(5);

    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();
    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<storm::RationalFunction>(model->getTransitionMatrix(), options);
    auto statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 6, decomposition, statesSorted));
    order->add(3);

    // Checking on samples and validate
    auto assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));
}

TEST(AssumptionCheckerTest, Casestudy3) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy3.pm";
    std::string formulaAsString = "P=? [F s=3]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(5ul, dtmc->getNumberOfStates());
    ASSERT_EQ(8ul, dtmc->getNumberOfTransitions());

    // Create the region
    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.4", vars);

    auto checker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(dtmc->getTransitionMatrix());

    auto expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
    expressionManager->declareRationalVariable("1");
    expressionManager->declareRationalVariable("2");

    // Order
    storm::storage::BitVector above(5);
    above.set(3);
    storm::storage::BitVector below(5);
    below.set(4);
    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();
    auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<storm::RationalFunction>(model->getTransitionMatrix(), options);
    auto statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 5, decomposition, statesSorted));

    auto assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    checker.initializeCheckingOnSamples(formulas[0], dtmc, region, 3);

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Greater));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));

    assumption = std::make_shared<storm::expressions::BinaryRelationExpression>(
            storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                                                         expressionManager->getVariable("1").getExpression().getBaseExpressionPointer(),
                                                         expressionManager->getVariable("2").getExpression().getBaseExpressionPointer(),
                                                         storm::expressions::RelationType::Equal));
    EXPECT_EQ(storm::analysis::AssumptionStatus::INVALID, checker.validateAssumption(assumption, order, region));
}
