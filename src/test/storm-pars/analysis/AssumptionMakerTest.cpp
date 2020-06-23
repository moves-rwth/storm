//#include "test/storm_gtest.h"
//#include "storm-config.h"
//#include "test/storm_gtest.h"
//#include "storm-parsers/parser/FormulaParser.h"
//#include "storm/logic/Formulas.h"
//#include "storm/models/sparse/StandardRewardModel.h"
//#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
//#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
//#include "storm-parsers/parser/AutoParser.h"
//#include "storm-parsers/parser/PrismParser.h"
//#include "storm/storage/expressions/ExpressionManager.h"
//#include "storm/api/builder.h"
//#include "storm-pars/api/analysis.h"
//#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
//#include "storm-pars/api/storm-pars.h"
//#include "storm/api/storm.h"
//#include "storm-parsers/api/storm-parsers.h"
//
//TEST(AssumptionMakerTest, Brp_without_bisimulation) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
//    std::string formulaAsString = "P=? [F s=4 & i=N ]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    // Program and formula
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    // Create the region
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= pK <= 0.999999, 0.00001 <= pL <= 0.999999", vars);
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 193);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 383);
//
//    auto *extender = new storm::analysis::OrderExtender<storm::RationalFunction, double>(dtmc, formulas[0], region);
//    auto monRes = new storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>;
//    auto criticalTuple = extender->toOrder(make_shared<storm::analysis::MonotonicityResult<typename storm::analysis::OrderExtender<storm::RationalFunction, double>::VariableType>>(*monRes));
//    ASSERT_EQ(183, std::get<1>(criticalTuple));
//    ASSERT_EQ(186, std::get<2>(criticalTuple));
//
//    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(formulas[0], dtmc, region, 3);
//    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction, double>(&assumptionChecker, dtmc->getNumberOfStates());
//    auto result = assumptionMaker.createAndCheckAssumptions(std::get<1>(criticalTuple), std::get<2>(criticalTuple), std::get<0>(criticalTuple));
//
//    EXPECT_EQ(1, result.size());
//
//    auto itr = result.begin();
//    EXPECT_EQ(storm::analysis::AssumptionStatus::UNKNOWN, itr->second);
//    EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
//    EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
//    EXPECT_EQ("186", itr->first->getFirstOperand()->asVariableExpression().getVariable().getName());
//    EXPECT_EQ("183", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
//    EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
//}
//
//
//TEST(AssumptionMakerTest, Simple1) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
//    std::string formulaAsString = "P=? [F s=3]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//
//    // Create the region
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999", vars);
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 5);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8);
//
//    storm::storage::BitVector above(5);
//    above.set(3);
//    storm::storage::BitVector below(5);
//    below.set(4);
//    std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
//
//    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 5, &statesSorted));
//
//    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(formulas[0], dtmc, region, 3);
//    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction, double>(&assumptionChecker, dtmc->getNumberOfStates());
//    auto result = assumptionMaker.createAndCheckAssumptions(1, 2, order);
//
//    EXPECT_EQ(0, result.size());
//}
//
//TEST(AssumptionMakerTest, Casestudy1) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy1.pm";
//    std::string formulaAsString = "P=? [F s=3]";
//    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
//
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    // Create the region
//    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    auto region = storm::api::parseRegion<storm::RationalFunction>("0.00001 <= p <= 0.999999", vars);
//
//    ASSERT_EQ(dtmc->getNumberOfStates(), 5);
//    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8);
//
//    storm::storage::BitVector above(5);
//    above.set(3);
//    storm::storage::BitVector below(5);
//    below.set(4);
//
//    std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(model->getTransitionMatrix());
//
//    auto order = std::shared_ptr<storm::analysis::Order>(new storm::analysis::Order(&above, &below, 5, &statesSorted));
//
//    auto assumptionChecker = storm::analysis::AssumptionChecker<storm::RationalFunction, double>(formulas[0], dtmc, region, 3);
//    auto assumptionMaker = storm::analysis::AssumptionMaker<storm::RationalFunction, double>(&assumptionChecker, dtmc->getNumberOfStates());
//    auto result = assumptionMaker.createAndCheckAssumptions(1, 2, order);
//
//    EXPECT_EQ(1, result.size());
//    auto itr = result.begin();
//    EXPECT_EQ(storm::analysis::AssumptionStatus::VALID, itr->second);
//    EXPECT_EQ(true, itr->first->getFirstOperand()->isVariable());
//    EXPECT_EQ(true, itr->first->getSecondOperand()->isVariable());
//    EXPECT_EQ("2", itr->first->getSecondOperand()->asVariableExpression().getVariable().getName());
//    EXPECT_EQ(storm::expressions::BinaryRelationExpression::RelationType::Greater, itr->first->getRelationType());
//}