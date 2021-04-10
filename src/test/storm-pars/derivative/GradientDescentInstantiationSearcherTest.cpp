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

    const float badCValuesRadam[] = {0.5, 0.49060654640197754, 0.48096320033073425, 0.47105303406715393, 0.4608582556247711, 0.45036017894744873, 0.45009857416152954, 0.44976693391799927, 0.4493735134601593, 0.44892504811286926, 0.44842588901519775, 0.4478800892829895, 0.44729089736938477, 0.446660578250885, 0.4459914565086365, 0.4452851712703705, 0.4445434808731079, 0.44376760721206665, 0.44295892119407654, 0.4421185553073883, 0.4412473738193512, 0.4403461515903473, 0.43941572308540344, 0.4384567141532898, 0.43746981024742126, 0.43645551800727844, 0.43541428446769714, 0.43434661626815796, 0.43325290083885193, 0.4321334660053253, 0.43098869919776917, 0.42981886863708496, 0.42862433195114136, 0.4274052083492279, 0.4261617660522461, 0.42489421367645264, 0.42360275983810425, 0.4222874939441681, 0.4209486246109009, 0.41958627104759216, 0.4182005524635315};
    const float pfValuesRadam[] =  {0.5, 0.4985547959804535, 0.4970662295818329, 0.4955315589904785, 0.4939480423927307, 0.49231281876564026, 0.49205103516578674, 0.4917190968990326, 0.491325318813324, 0.4908764064311981, 0.490376740694046, 0.4898303747177124, 0.4892405867576599, 0.4886097013950348, 0.4879400432109833, 0.4872332811355591, 0.48649120330810547, 0.4857150912284851, 0.48490628600120544, 0.4840660095214844, 0.48319515585899353, 0.4822945296764374, 0.48136502504348755, 0.4804072678089142, 0.47942203283309937, 0.4784098267555237, 0.4773711562156677, 0.47630661725997925, 0.4752165973186493, 0.4741014540195465, 0.4729616641998291, 0.4717975854873657, 0.470609575510025, 0.46939781308174133, 0.4681626856327057, 0.4669044017791748, 0.46562328934669495, 0.46431946754455566, 0.46299320459365845, 0.46164470911026, 0.4602741599082947};

    for (uint_fast64_t i = 0; i < 41; i++) {
        ASSERT_NEAR(storm::utility::convertNumber<double>(radamWalk[i].position[badCVar]), badCValuesRadam[i], 1e-5);
        ASSERT_NEAR(storm::utility::convertNumber<double>(radamWalk[i].position[pfVar]), pfValuesRadam[i], 1e-5);
    }
    
    ASSERT_NEAR(doubleInstantiation.second, 0, 1e-6);
}
