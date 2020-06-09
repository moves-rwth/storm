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

// TODO: @Svenja add some more tests & rename
TEST(MonotonicityHelperTest, Derivative_checker) {

    // Create the region
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation lowerBoundaries;
    typename storm::storage::ParameterRegion<storm::RationalFunction>::Valuation upperBoundaries;
    auto region =  storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries), std::move(upperBoundaries));

    // Derivative 0
    auto constFunction = storm::RationalFunction(0);
    auto constFunctionRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(constFunction, region);
    EXPECT_TRUE(constFunctionRes.first);
    EXPECT_TRUE(constFunctionRes.second);

    // Derivative 5
    constFunction = storm::RationalFunction(5);
    constFunctionRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(constFunction, region);
    EXPECT_TRUE(constFunctionRes.first);
    EXPECT_FALSE(constFunctionRes.second);

    // Derivative -4
    constFunction = storm::RationalFunction(storm::RationalFunction(1)-constFunction);
    constFunctionRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(constFunction, region);
    EXPECT_FALSE(constFunctionRes.first);
    EXPECT_TRUE(constFunctionRes.second);

    std::shared_ptr<storm::RawPolynomialCache> cache = std::make_shared<storm::RawPolynomialCache>();
    carl::StringParser parser;
    parser.setVariables({"p", "q"});

    // Create the region
    auto functionP = storm::RationalFunction(storm::Polynomial(parser.template parseMultivariatePolynomial<storm::RationalFunctionCoefficient>("p"), cache));
    auto functionQ = storm::RationalFunction(storm::Polynomial(parser.template parseMultivariatePolynomial<storm::RationalFunctionCoefficient>("q"), cache));

    auto varsP = functionP.gatherVariables();
    auto varsQ = functionQ.gatherVariables();
    storm::utility::parametric::Valuation<storm::RationalFunction> lowerBoundaries2;
    storm::utility::parametric::Valuation<storm::RationalFunction> upperBoundaries2;
    for (auto var : varsP) {
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
        lowerBoundaries2.emplace(std::make_pair(var, lb));
        upperBoundaries2.emplace(std::make_pair(var, ub));
    }
    for (auto var : varsQ) {
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType lb = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(0 + 0.000001);
        typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType ub = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(1 - 0.000001);
        lowerBoundaries2.emplace(std::make_pair(var, lb));
        upperBoundaries2.emplace(std::make_pair(var, ub));
    }
    region = storm::storage::ParameterRegion<storm::RationalFunction>(std::move(lowerBoundaries2), std::move(upperBoundaries2));

    // Derivative p
    auto function = functionP;
    auto functionRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(function, region);
    EXPECT_TRUE(functionRes.first);
    EXPECT_FALSE(functionRes.second);

    // Derivative 1-p
    auto functionDecr = storm::RationalFunction(storm::RationalFunction(1)-function);
    auto functionDecrRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(functionDecr, region);
    EXPECT_TRUE(functionDecrRes.first);
    EXPECT_FALSE(functionDecrRes.second);

    // Derivative 1-2p
    auto functionNonMonotonic = storm::RationalFunction(storm::RationalFunction(1)-storm::RationalFunction(2)*function);
    auto functionNonMonotonicRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(functionNonMonotonic, region);
    EXPECT_FALSE(functionNonMonotonicRes.first);
    EXPECT_FALSE(functionNonMonotonicRes.second);

    // Derivative -p
    functionDecr = storm::RationalFunction(storm::RationalFunction(0)-function);
    functionDecrRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(functionDecr, region);
    EXPECT_FALSE(functionDecrRes.first);
    EXPECT_TRUE(functionDecrRes.second);

    // Derivative p*q
    function = functionP * functionQ ;
    functionRes = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>::checkDerivative(function, region);
    EXPECT_TRUE(functionRes.first);
    EXPECT_FALSE(functionRes.second);
}

TEST(MonotonicityHelperTest, Brp_with_bisimulation_no_samples) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [true U s=4 & i=N ]";
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

    // Apply bisimulation
    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
    if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
        bisimType = storm::storage::BisimulationType::Weak;
    }

    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(model, formulas, bisimType)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();


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

    ASSERT_EQ(dtmc->getNumberOfStates(), 99ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 195ull);

    storm::analysis::MonotonicityHelper<storm::RationalFunction, double> MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);
    EXPECT_EQ(1, result.size());
    auto order = result.begin()->first;
    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr, entry.second);
    }
}

TEST(MonotonicityHelperTest, Brp_with_bisimulation_samples) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [true U s=4 & i=N ]";
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

    // Apply bisimulation
    storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
    if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
        bisimType = storm::storage::BisimulationType::Weak;
    }

    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(model, formulas, bisimType)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

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

    ASSERT_EQ(dtmc->getNumberOfStates(), 99ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 195ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);

    EXPECT_EQ(1, result.size());
    auto order = result.begin()->first;
    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr, entry.second);
    }
}

TEST(MonotonicityHelperTest, zeroconf) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/zeroconf4.pm";
    std::string formulaAsString = "P > 0.5 [ F s=5 ]";
    std::string constantsAsString = "n = 4"; //e.g. pL=0.9,TOACK=0.5

    // Program and formula
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 8ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 13ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);

    EXPECT_EQ(1, result.size());
    auto order = result.begin()->first;
    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr, entry.second);
    }
}

TEST(MonotonicityHelperTest, Simple1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/simple1.pm";
    std::string formulaAsString = "P > 0.5 [ F s=3 ]";
    std::string constantsAsString = "";

    // Program and formula
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 5ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);

    EXPECT_EQ(0, result.size());
}

TEST(MonotonicityHelperTest, Casestudy1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy1.pm";
    std::string formulaAsString = "P > 0.5 [ F s=3 ]";
    std::string constantsAsString = "";

    // Program and formula
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 5ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);
    ASSERT_EQ(1, result.size());

    auto order = result.begin()->first;
    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr, entry.second);
    }
}


TEST(MonotonicityHelperTest, CaseStudy2) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy2.pm";
    std::string formulaAsString = "P > 0.5 [ F s=4 ]";
    std::string constantsAsString = "";

    // Program and formula
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 6ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 12ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);

    ASSERT_EQ(1, result.size());
    auto order = result.begin()->first;
    ASSERT_TRUE(order.get() != nullptr);

    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Incr, entry.second);
    }
}




TEST(MonotonicityHelperTest, Casestudy3) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/casestudy3.pm";
    std::string formulaAsString = "P > 0.5 [ F s=3 ]";
    std::string constantsAsString = "";

    // Program and formula
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

    ASSERT_EQ(dtmc->getNumberOfStates(), 5ull);
    ASSERT_EQ(dtmc->getNumberOfTransitions(), 8ull);

    auto MonotonicityHelper = storm::analysis::MonotonicityHelper<storm::RationalFunction, double>(dtmc, formulas, regions, true, 50);
    auto result = MonotonicityHelper.checkMonotonicityInBuild(std::cout);

    ASSERT_EQ(1, result.size());
    auto order = result.begin()->first;

    auto monotonicityResult = result.begin()->second.first;
    EXPECT_TRUE(monotonicityResult->isDone());
    EXPECT_TRUE(monotonicityResult->isSomewhereMonotonicity());
    EXPECT_TRUE(monotonicityResult->isAllMonotonicity());
    auto assumptions = result.begin()->second.second;
    EXPECT_EQ(0, assumptions.size());

    auto monRes = monotonicityResult->getMonotonicityResult();
    for (auto entry : monRes) {
        EXPECT_EQ(storm::analysis::MonotonicityResult<storm::RationalFunctionVariable>::Monotonicity::Decr, entry.second);
    }
}



