#include "test/storm_gtest.h"
#include "environment/solver/GmmxxSolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/TopologicalSolverEnvironment.h"
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

namespace {
    class RationalGmmxxEnvironment {
    public:
        typedef storm::RationalFunction ValueType;
        typedef storm::RationalNumber ConstantType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            return env;
        }
    };
    class DoubleGmmxxEnvironment {
    public:
        typedef storm::RationalFunction ValueType;
        typedef double ConstantType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Gmmxx);
            return env;
        }
    };
    class RationalEigenEnvironment {
    public:
        typedef storm::RationalFunction ValueType;
        typedef storm::RationalNumber ConstantType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
            return env;
        }
    };
    class DoubleEigenEnvironment {
    public:
        typedef storm::RationalFunction ValueType;
        typedef double ConstantType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Eigen);
            return env;
        }
    };
    class DoubleEigenTopologicalEnvironment {
    public:
        typedef storm::RationalFunction ValueType;
        typedef double ConstantType;
        static storm::Environment createEnvironment() {
            storm::Environment env;
            env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Topological);
            env.solver().topological().setUnderlyingEquationSolverType(storm::solver::EquationSolverType::Eigen);
            return env;
        }
    };
    template<typename TestType>
    class GradientDescentInstantiationSearcherTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;
        typedef typename TestType::ConstantType ConstantType;
        GradientDescentInstantiationSearcherTest() : _environment(TestType::createEnvironment()) {}
        storm::Environment const& env() const { return _environment; }
        virtual void SetUp() { carl::VariablePool::getInstance().clear(); }
        virtual void TearDown() { carl::VariablePool::getInstance().clear(); }
    private:
        storm::Environment _environment;
    };

    typedef ::testing::Types<
        // The rational environments take ages... sorry, but GD is just not made for rational arithmetic.
        DoubleGmmxxEnvironment,
        DoubleEigenEnvironment,
        DoubleEigenTopologicalEnvironment
    > TestingTypes;
}

TYPED_TEST_SUITE(GradientDescentInstantiationSearcherTest, TestingTypes, );

TYPED_TEST(GradientDescentInstantiationSearcherTest, Simple) {
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

    storm::derivative::GradientDescentInstantiationSearcher<typename TestFixture::ValueType, typename TestFixture::ConstantType> checker(this->env(), dtmc, vars, formulas);
    auto doubleInstantiation = checker.gradientDescent(this->env(), false);
    ASSERT_NEAR(doubleInstantiation.second, 0.25, 1e-6);
}

TYPED_TEST(GradientDescentInstantiationSearcherTest, Crowds) {
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
    storm::derivative::GradientDescentInstantiationSearcher<typename TestFixture::ValueType, typename TestFixture::ConstantType> adamChecker(
            this->env(),
            dtmc, 
            vars, 
            formulas,
            storm::derivative::GradientDescentMethod::ADAM,
            0.01,
            0.9,
            0.999,
            2,
            1e-6,
            boost::none,
            true
    );
    auto doubleInstantiation = adamChecker.gradientDescent(this->env(), false);
    auto walk = adamChecker.getVisualizationWalk();

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

    // Same thing with RAdam
    storm::derivative::GradientDescentInstantiationSearcher<typename TestFixture::ValueType, typename TestFixture::ConstantType> radamChecker(
            this->env(),
            dtmc, 
            vars, 
            formulas,
            storm::derivative::GradientDescentMethod::RADAM,
            0.01,
            0.9,
            0.999,
            2,
            1e-6,
            boost::none,
            true
    );
    auto radamInstantiation = radamChecker.gradientDescent(this->env(), false);
    auto radamWalk = radamChecker.getVisualizationWalk();

    const float badCValuesRadam[] = {0.5, 0.49060654640197754, 0.48096320033073425, 0.47105303406715393, 0.4608582556247711, 0.46068474650382996, 0.4604234993457794, 0.460092693567276, 0.4597006142139435, 0.4592539370059967, 0.4587569832801819, 0.45821380615234375, 0.4576275646686554, 0.4570005536079407, 0.45633500814437866, 0.4556325674057007, 0.45489493012428284, 0.454123318195343, 0.4533190429210663, 0.45248323678970337, 0.45161670446395874, 0.4507202208042145, 0.44979462027549744, 0.448840469121933, 0.44785845279693604, 0.4468490481376648, 0.4458127021789551, 0.4447498917579651, 0.44366100430488586, 0.4425463378429413, 0.4414062798023224, 0.4402410686016083, 0.4390510618686676, 0.43783634901046753, 0.43659722805023193, 0.43533384799957275, 0.4340464472770691, 0.4327350854873657, 0.43139997124671936, 0.43004119396209717, 0.4286588430404663};

    const float pfValuesRadam[] = {0.5, 0.4985547959804535, 0.4970662295818329, 0.4955315589904785, 0.4939480423927307, 0.4937744438648224, 0.4935130178928375, 0.4931819438934326, 0.49278953671455383, 0.492342472076416, 0.49184510111808777, 0.4913014769554138, 0.49071478843688965, 0.4900873303413391, 0.48942139744758606, 0.4887186288833618, 0.48798075318336487, 0.4872090220451355, 0.48640477657318115, 0.48556917905807495, 0.48470309376716614, 0.4838073253631592, 0.48288270831108093, 0.48192986845970154, 0.4809495508670807, 0.4799422323703766, 0.47890838980674744, 0.4778485894203186, 0.4767632484436035, 0.4756527245044708, 0.47451743483543396, 0.4733576774597168, 0.4721738398075104, 0.470966100692749, 0.4697347581386566, 0.46848005056381226, 0.4672022759914398, 0.46590155363082886, 0.46457815170288086, 0.46323221921920776, 0.4618639349937439};

    for (uint_fast64_t i = 0; i < 41; i++) {
        ASSERT_NEAR(storm::utility::convertNumber<double>(radamWalk[i].position[badCVar]), badCValuesRadam[i], 1e-5);
        ASSERT_NEAR(storm::utility::convertNumber<double>(radamWalk[i].position[pfVar]), pfValuesRadam[i], 1e-5);
    }
    
    ASSERT_NEAR(doubleInstantiation.second, 0, 1e-6);
}
