#include "test/storm_gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm-parsers/parser/FormulaParser.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/api/builder.h"

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm-pars/api/storm-pars.h"
#include "storm/api/storm.h"

#include "storm-parsers/api/storm-parsers.h"

TEST(MonotonicityCheckerTest, Simple1_not_monotone) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P=? [F s=3 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto region=storm::api::parseRegion<storm::RationalFunction>("0.1<=p<=0.9", modelParameters);
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    // order
    auto monHelper = new storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(model, formulas, regions, 0, 0, false);
    auto result = monHelper->checkMonotonicityInBuild(std::cout);
    ASSERT_TRUE(result.size() != 0);
    auto order = result.begin()->first;

    // monchecker
    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(model->getTransitionMatrix());

    //start testing
    auto var = modelParameters.begin();
    for (uint_fast64_t i = 0; i < 3; i++) {
        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Decr, monChecker->checkLocalMonotonicity(order, i, *var, region));
    }

    // TODO add test for p in (0.5,1)
}

TEST(MonotonicityCheckerTest, Casestudy1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy1.pm";
    std::string formulaAsString = "P=? [F s=3 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    for (auto var : vars) {
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
        lowerBoundaries.emplace(std::make_pair(var, lb));
        upperBoundaries.emplace(std::make_pair(var, ub));
    }
    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    //order
    auto monHelper = new storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(model, formulas, regions, 0, 0, false);
    auto result = monHelper->checkMonotonicityInBuild(std::cout);
    ASSERT_TRUE(result.size() != 0);
    auto order = result.begin()->first;


    //monchecker
    auto matrix = model->getTransitionMatrix();
    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);

    //start testing
    auto var = vars.begin();
    for (uint_fast64_t i = 0; i < 3; i++) {
        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
    }
}

TEST(MonotonicityCheckerTest, Casestudy2) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy2.pm";
    std::string formulaAsString = "P=? [F s=4 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    for (auto var : vars) {
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
        lowerBoundaries.emplace(std::make_pair(var, lb));
        upperBoundaries.emplace(std::make_pair(var, ub));
    }
    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};


    //order
    auto monHelper = new storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(model, formulas, regions, 0, 0, false);
    auto result = monHelper->checkMonotonicityInBuild(std::cout);

    ASSERT_TRUE(result.size() != 0);

    auto order = result.begin()->first;


    //monchecker
    auto matrix = model->getTransitionMatrix();
    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);


    //start testing
    auto var = vars.begin();
    for (uint_fast64_t i = 0; i < 4; i++) {
        STORM_PRINT("State " << i << std::endl);
        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
    }
}


TEST(MonotonicityCheckerTest, Casestudy3) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy3.pm";
    std::string formulaAsString = "P=? [F s=3 ]";
    std::string constantsAsString = "";

    // model
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*(formulas[0])));
    model = simplifier.getSimplifiedModel();

    // Create the region
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
    std::set<typename storm::storage::ParameterRegion<storm::RationalFunction>::VariableType> vars = storm::models::sparse::getProbabilityParameters(*dtmc);
    for (auto var : vars) {
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
        lowerBoundaries.emplace(std::make_pair(var, lb));
        upperBoundaries.emplace(std::make_pair(var, ub));
    }
    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));
    std::vector<storm::storage::ParameterRegion<storm::RationalFunction>> regions = {region};

    //order
    auto monHelper = new storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(model, formulas, regions, 0, 0, false);

    auto result = monHelper->checkMonotonicityInBuild(std::cout);
    ASSERT_TRUE(result.size() != 0);
    auto order = result.begin()->first;


    //monchecker
    auto matrix = model->getTransitionMatrix();
    auto monChecker = new storm::analysis::MonotonicityChecker<storm::RationalFunction>(matrix);

    //start testing
    auto var = vars.begin();
    for (uint_fast64_t i = 0; i < 3; i++) {
        STORM_PRINT("State " << i << std::endl);
        EXPECT_EQ(storm::analysis::MonotonicityChecker<storm::RationalFunction>::Monotonicity::Incr, monChecker->checkLocalMonotonicity(order, i, *var, region));
    }
}