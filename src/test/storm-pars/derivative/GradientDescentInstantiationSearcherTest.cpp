#include "test/storm_gtest.h"
#include "environment/solver/GmmxxSolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "solver/EliminationLinearEquationSolver.h"
#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm/api/builder.h"
#include "storm/api/storm.h"

#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm-pars/api/storm-pars.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "storm-pars/analysis/OrderExtender.h"
#include "storm-pars/derivative/GradientDescentInstantiationSearcher.h"
#include "gtest/gtest.h"

TEST(GradientDescentInstantiationSearcherTest, Simple) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/gradient1.pm";
    std::string formulaAsString = "Pmax=? [F s=2]";
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

    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);

    storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, double> doubleChecker(dtmc, vars, formulas);
    auto doubleInstantiation = doubleChecker.gradientDescent(false);
    ASSERT_NEAR(doubleInstantiation.second, 0.25, 1e-6);

    /* storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, storm::RationalNumber> rationalChecker(dtmc, vars, formulas); */
    /* auto rationalInstantiation = rationalChecker.gradientDescent(false); */
    /* ASSERT_NEAR(rationalInstantiation.second, 0.25, 1e-6); */
}

TEST(GradientDescentInstantiationSearcherTest, Crowds) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "Pmin=? [F \"observe0Greater1\"]";
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

    auto vars = storm::models::sparse::getProbabilityParameters(*dtmc);

    /* ASSERT_EQ(dtmc->getNumberOfStates(), 193ull); */
    /* ASSERT_EQ(dtmc->getNumberOfTransitions(), 383ull); */

    // First, test an ADAM instance. We will check that we have implemented ADAM correctly by comparing our results to results gathered by an ADAM implementation in tensorflow :)
    storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, double> doubleChecker(
            dtmc, 
            vars, 
            formulas,
            storm::derivative::GradientDescentMethod::ADAM,
            0.01,
            0.9,
            0.999,
            2,
            1e-6,
            true
    );
    auto doubleInstantiation = doubleChecker.gradientDescent(false);
    doubleChecker.printRunAsJson();
    auto walk = doubleChecker.getVisualizationWalk();

    carl::Variable badCVar;
    carl::Variable pfVar;
    for (auto parameter : storm::models::sparse::getProbabilityParameters(*dtmc)) {
        if (parameter.name() == "badC") {
            badCVar = parameter;
        } else  if (parameter.name() == "PF") {
            pfVar = parameter;
        }
    }
    std::shared_ptr<storm::RawPolynomialCache> cache = std::make_shared<storm::RawPolynomialCache>();
    auto const badC = storm::RationalFunction(storm::Polynomial(storm::RawPolynomial(badCVar), cache));
    auto const pf = storm::RationalFunction(storm::Polynomial(storm::RawPolynomial(pfVar), cache));

    const float badCValues[] = { 0.5, 0.49000033736228943, 0.4799894690513611, 0.4699603021144867, 0.4599062204360962, 0.4498208165168762, 0.43969807028770447, 0.4295324981212616, 0.41931894421577454, 0.4090527594089508, 0.39872977137565613, 0.3883463442325592, 0.3778994083404541, 0.3673863708972931, 0.35680532455444336, 0.3461548984050751, 0.3354344069957733, 0.3246437907218933, 0.31378373503685, 0.30285564064979553, 0.2918616533279419, 0.28080472350120544, 0.2696886956691742, 0.258518248796463, 0.24729910492897034, 0.23603792488574982, 0.22474248707294464, 0.21342173218727112, 0.20208582282066345, 0.1907462775707245, 0.17941603064537048, 0.16810956597328186, 0.15684303641319275, 0.14563430845737457, 0.13450318574905396, 0.1234714537858963, 0.11256305128335953, 0.10180411487817764, 0.0912230908870697, 0.080850750207901, 0.07072017341852188 };
    const float pfValues[] = { 0.5, 0.49000218510627747, 0.47999337315559387, 0.46996763348579407, 0.45991936326026917, 0.4498434364795685, 0.4397353231906891, 0.4295912981033325, 0.4194081425666809, 0.4091835021972656, 0.3989158570766449, 0.3886045515537262, 0.3782498240470886, 0.3678528368473053, 0.3574157953262329, 0.3469419777393341, 0.33643582463264465, 0.32590293884277344, 0.3153502345085144, 0.30478596687316895, 0.2942197024822235, 0.28366249799728394, 0.2731269598007202, 0.26262718439102173, 0.25217896699905396, 0.24179960787296295, 0.2315080612897873, 0.2213248461484909, 0.2112719565629959, 0.20137275755405426, 0.1916518211364746, 0.18213464319705963, 0.17284737527370453, 0.1638164073228836, 0.15506798028945923, 0.14662761986255646, 0.13851958513259888, 0.13076619803905487, 0.12338729947805405, 0.11639954894781113, 0.10981585085391998 };

    for (uint_fast64_t i = 0; i < 41; i++) {
        ASSERT_NEAR(storm::utility::convertNumber<double>(walk[i].position[badCVar]), badCValues[i], 1e-4);
        ASSERT_NEAR(storm::utility::convertNumber<double>(walk[i].position[pfVar]), pfValues[i], 1e-4);
    }
    
    ASSERT_NEAR(doubleInstantiation.second, 0, 1e-6);
}
