//#include "test/storm_gtest.h"
//#include "storm-config.h"
//#include "test/storm_gtest.h"
//#include "storm/storage/SparseMatrix.h"
//#include "storm/adapters/RationalFunctionAdapter.h"
//
//#include "storm-parsers/parser/FormulaParser.h"
//#include "storm/logic/Formulas.h"
//#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
//#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
//#include "storm-parsers/parser/AutoParser.h"
//#include "storm-parsers/parser/PrismParser.h"
//#include "storm/api/builder.h"
//
//#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
//
//#include "storm-pars/api/storm-pars.h"
//#include "storm/api/storm.h"
//
//#include "storm-parsers/api/storm-parsers.h"
//
//TEST(MonotonicityCheckerTest, Simple1_larger_region) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
//    std::string formulaAsString = "P=? [F s=3 ]";
//    std::string constantsAsString = "";
//
//    // model
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Create the region
//    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
//    auto region=storm::api::parseRegion<storm::RationalFunction>("0.1<=p<=0.9", modelParameters);
//    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};
//
//    // For order extender
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//    // OrderExtender
//    storm::storage::SparseMatrix<storm::RationalFunction> matrix =  model->getTransitionMatrix();
//    auto orderExtender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, matrix);
//    // Order
//    auto order = std::get<0>(orderExtender.toOrder(nullptr));
//    // monchecker
//    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(model->getTransitionMatrix());
//
//    //start testing
//    auto var = modelParameters.begin();
//    EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, 1, *var, region));
//    EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Decr, monChecker->checkLocalMonotonicity(order, 2, *var, region));
//}
//
//TEST(MonotonicityCheckerTest, Simple1_small_region) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
//    std::string formulaAsString = "P=? [F s=3 ]";
//    std::string constantsAsString = "";
//
//    // model
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Create the region
//    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
//    auto region=storm::api::parseRegion<storm::RationalFunction>("0.51<=p<=0.9", modelParameters);
//    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};
//
//    // For order extender
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//    // OrderExtender
//    storm::storage::SparseMatrix<storm::RationalFunction> matrix =  model->getTransitionMatrix();
//    auto orderExtender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, matrix);
//    // Order
//    auto order = std::get<0>(orderExtender.toOrder(nullptr));
//    // TODO: this shouldn't be necessary, check orderextender
//    order->addRelation(1,2);
//    // monchecker
//    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(model->getTransitionMatrix());
//
//    //start testing
//    auto var = modelParameters.begin();
//    EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, 0, *var, region));
//    EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, 1, *var, region));
//    EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Decr, monChecker->checkLocalMonotonicity(order, 2, *var, region));
//}
//
//TEST(MonotonicityCheckerTest, Casestudy1) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy1.pm";
//    std::string formulaAsString = "P=? [F s=3 ]";
//    std::string constantsAsString = "";
//
//    // model
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Create the region
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
//    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    for (auto var : vars) {
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
//        lowerBoundaries.emplace(std::make_pair(var, lb));
//        upperBoundaries.emplace(std::make_pair(var, ub));
//    }
//    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
//    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};
//
//    // For order extender
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//    // OrderExtender
//    storm::storage::SparseMatrix<storm::RationalFunction> matrix =  model->getTransitionMatrix();
//    auto orderExtender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, matrix);
//    // Order
//    auto order = std::get<0>(orderExtender.toOrder(nullptr));
//
//    //monchecker
//    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);
//
//    //start testing
//    auto var = vars.begin();
//    for (uint_fast64_t i = 0; i < 3; i++) {
//        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
//    }
//}
//
//TEST(MonotonicityCheckerTest, Casestudy2) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy2.pm";
//    std::string formulaAsString = "P=? [F s=4 ]";
//    std::string constantsAsString = "";
//
//    // model
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Create the region
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
//    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    for (auto var : vars) {
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
//        lowerBoundaries.emplace(std::make_pair(var, lb));
//        upperBoundaries.emplace(std::make_pair(var, ub));
//    }
//    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
//    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};
//
//    // For order extender
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//    // OrderExtender
//    storm::storage::SparseMatrix<storm::RationalFunction> matrix =  model->getTransitionMatrix();
//    auto orderExtender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, matrix);
//    // Order
//    auto order = std::get<0>(orderExtender.toOrder(nullptr));
//
//    //monchecker
//    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);
//
//    //start testing
//    auto var = vars.begin();
//    for (uint_fast64_t i = 0; i < 4; i++) {
//        STORM_PRINT("State " << i << std::endl);
//        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
//    }
//}
//
//
//TEST(MonotonicityCheckerTest, Casestudy3) {
//    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy3.pm";
//    std::string formulaAsString = "P=? [F s=3 ]";
//    std::string constantsAsString = "";
//
//    // model
//    storm::prism::Program program = storm::api::parseProgram(programFile);
//    program = storm::utility::prism::preprocess(program, constantsAsString);
//    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
//    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
//    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
//    model = simplifier.getSimplifiedModel();
//
//    // Create the region
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
//    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
//    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
//    for (auto var : vars) {
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
//        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
//        lowerBoundaries.emplace(std::make_pair(var, lb));
//        upperBoundaries.emplace(std::make_pair(var, ub));
//    }
//    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
//    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};
//
//    // For order extender
//    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<storm::RationalFunction>> propositionalChecker(*model);
//    storm::storage::BitVector phiStates;
//    storm::storage::BitVector psiStates;
//    phiStates = storm::storage::BitVector(model->getTransitionMatrix().getRowCount(), true);
//    storm::logic::EventuallyFormula formula = formulas[0]->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula();
//    psiStates = propositionalChecker.check(formula.getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//    // Get the maybeStates
//    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model->getBackwardTransitions(), phiStates, psiStates);
//    storm::storage::BitVector topStates = statesWithProbability01.second;
//    storm::storage::BitVector bottomStates = statesWithProbability01.first;
//    // OrderExtender
//    storm::storage::SparseMatrix<storm::RationalFunction> matrix =  model->getTransitionMatrix();
//    auto orderExtender = storm::analysis::OrderExtender<storm::RationalFunction, double>(&topStates, &bottomStates, matrix);
//    // Order
//    auto order = std::get<0>(orderExtender.toOrder(nullptr));
//
//    //monchecker
//    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);
//
//    //start testing
//    auto var = vars.begin();
//    for (uint_fast64_t i = 0; i < 3; i++) {
//        STORM_PRINT("State " << i << std::endl);
//        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
//    }
//}