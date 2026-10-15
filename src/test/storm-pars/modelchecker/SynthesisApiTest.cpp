#include "storm-config.h"
#include "test/storm_gtest.h"

#include <type_traits>

#include "storm-pars/api/storm-pars.h"
#include "storm-parsers/api/properties.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/api/builder.h"
#include "storm/api/properties.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/constants.h"

namespace {

using Engine = storm::modelchecker::RegionCheckEngine;
using Method = storm::pars::FeasibilityMethod;
using Status = storm::pars::FeasibilityStatus;
using Region = storm::storage::ParameterRegion<storm::RationalFunction>;

class SynthesisApiTest : public ::testing::Test {
   protected:
    void SetUp() override {
        storm::clearRFVariablePool();
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }

    void build(std::string const& probability = "p", std::string const& declarations = "const double p;", std::string const& rewards = "") {
        program = storm::parser::PrismParser::parseFromString("dtmc\n" + declarations +
                                                                  "\nmodule simple\n s : [0..2] init 0;\n"
                                                                  " [] s=0 -> " +
                                                                  probability + " : (s'=1) + (1-(" + probability +
                                                                  ")) : (s'=2);\n"
                                                                  " [] s=1 -> (s'=1);\n [] s=2 -> (s'=2);\nendmodule\nlabel \"target\" = s=1;\n" +
                                                                  rewards,
                                                              "SynthesisApiTest");
        program = program.preprocess("");
        storm::builder::BuilderOptions options({formula("P=? [F \"target\"]")}, program);
        options.setBuildAllRewardModels(true);
        model = storm::api::buildSparseModel<storm::RationalFunction>(program, options);
    }

    std::shared_ptr<storm::logic::Formula const> formula(std::string const& text) const {
        return storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(text, program)).at(0);
    }

    Region region(std::string const& text) const {
        return storm::api::parseRegion<storm::RationalFunction>(text, *model);
    }

    storm::Environment environment;
    storm::prism::Program program;
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model;
};

TEST_F(SynthesisApiTest, VerificationEngineSelectionAndStrictBounds) {
    build();
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, engine));
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P>=0.6 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, engine));
        // The center is a counterexample at exactly the threshold. Strictness must be preserved.
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P<0.5 [F \"target\"]"), region("0.4<=p<=0.6"), std::nullopt, engine));
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P>0.5 [F \"target\"]"), region("0.4<=p<=0.6"), std::nullopt, engine));
    }
}

TEST_F(SynthesisApiTest, PlaFeasibilityAndInfeasibility) {
    build();
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        auto result =
            storm::api::checkFeasibility(environment, model, formula("P>=0.6 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, Method::PLA, engine);
        ASSERT_EQ(Status::Feasible, result.status);
        ASSERT_TRUE(result.witness);
        EXPECT_TRUE(region("0.2<=p<=0.8").contains(result.witness->valuation));
        EXPECT_TRUE(formula("P>=0.6 [F \"target\"]")->asOperatorFormula().getBound().isSatisfied(result.witness->value));

        result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, Method::PLA, engine);
        EXPECT_EQ(Status::Infeasible, result.status);
        EXPECT_FALSE(result.witness);

        result = storm::api::checkFeasibility(environment, model, formula("P<=0.4 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, Method::PLA, engine);
        EXPECT_EQ(Status::Feasible, result.status);
    }
}

TEST_F(SynthesisApiTest, RegionBoundMatchesExplicitRegion) {
    build();
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto box = region("0.2<=p<=0.8");
    auto property = formula("P>=0.6 [F \"target\"]");
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        EXPECT_EQ(storm::api::verify(environment, model, property, box, std::nullopt, engine),
                  storm::api::verify(environment, model, property, std::nullopt, bound, engine));
        auto explicitResult = storm::api::checkFeasibility(environment, model, property, box, std::nullopt, Method::PLA, engine);
        auto boundResult = storm::api::checkFeasibility(environment, model, property, std::nullopt, bound, Method::PLA, engine);
        EXPECT_EQ(explicitResult.status, boundResult.status);
        ASSERT_TRUE(boundResult.witness);
        EXPECT_TRUE(box.contains(boundResult.witness->valuation));
    }
}

TEST_F(SynthesisApiTest, RegionBoundAppliesToEveryParameterWithoutInferringDomains) {
    build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto property = formula("P>=0.6 [F \"target\"]");
    auto result = storm::api::checkFeasibility(environment, model, property, std::nullopt, bound, Method::PLA, Engine::ParameterLifting);
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_EQ(2u, result.witness->valuation.size());
    EXPECT_TRUE(region("0.2<=p<=0.8,0.2<=r<=0.8").contains(result.witness->valuation));

    // For p/2, the user still chose p in [0.2,0.8]; the API does not infer or scale it to [0,2].
    build("p/2");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.4 [F \"target\"]"), std::nullopt, bound, Engine::ExactParameterLifting));
    result =
        storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), std::nullopt, bound, Method::PLA, Engine::ExactParameterLifting);
    EXPECT_EQ(Status::Infeasible, result.status);
    EXPECT_FALSE(result.witness);
}

TEST_F(SynthesisApiTest, RegionBoundIncludesClosedEndpointsAndAllowsSingletons) {
    build();
    auto zero = storm::utility::zero<Region::CoefficientType>();
    auto half = storm::utility::one<Region::CoefficientType>() / 2;
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), std::nullopt, zero, Engine::RobustParameterLifting));
    auto fullRegion =
        storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), std::nullopt, zero, Method::PLA, Engine::RobustParameterLifting);
    EXPECT_EQ(Status::Feasible, fullRegion.status);
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P>=0.5 [F \"target\"]"), std::nullopt, half, Engine::ParameterLifting));
    auto singleton =
        storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), std::nullopt, half, Method::PLA, Engine::ParameterLifting);
    ASSERT_EQ(Status::Feasible, singleton.status);
    ASSERT_TRUE(singleton.witness);
    EXPECT_EQ(half, singleton.witness->valuation.begin()->second);
}

TEST_F(SynthesisApiTest, RejectsMissingConflictingAndInvalidRegionBounds) {
    build();
    auto property = formula("P>=0.5 [F \"target\"]");
    auto one = storm::utility::one<Region::CoefficientType>();
    auto bound = one / 5;
    EXPECT_THROW(storm::api::verify(environment, model, property), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::verify(environment, model, property, region("0.2<=p<=0.8"), bound, Engine::ParameterLifting),
                 storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, region("0.2<=p<=0.8"), bound, Method::PLA, Engine::ParameterLifting),
                 storm::exceptions::InvalidArgumentException);
    for (Region::CoefficientType const& invalid : {Region::CoefficientType(-one / 10), Region::CoefficientType(one * 3 / 5)}) {
        EXPECT_THROW(storm::api::verify(environment, model, property, std::nullopt, invalid, Engine::ParameterLifting),
                     storm::exceptions::InvalidArgumentException);
        EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, std::nullopt, invalid, Method::PLA, Engine::ParameterLifting),
                     storm::exceptions::InvalidArgumentException);
    }
}

TEST_F(SynthesisApiTest, ConstructedRegionMustPassWellformednessAndGraphChecks) {
    build();
    auto zero = storm::utility::zero<Region::CoefficientType>();
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto property = formula("P<=1 [F \"target\"]");
    EXPECT_THROW(storm::api::verify(environment, model, property, std::nullopt, zero, Engine::ParameterLifting), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, std::nullopt, zero, Method::GD, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);

    // [0.2,0.8] would make some probabilities p+4/5 greater than one.
    build("p+4/5");
    EXPECT_THROW(storm::api::verify(environment, model, property, std::nullopt, bound, Engine::ParameterLifting), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, std::nullopt, bound, Method::PLA, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, RobustPlaDoesNotRequireAnAffineWellformednessProof) {
    build("p*p");
    auto box = region("0.1<=p<=0.9");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), box, std::nullopt, Engine::RobustParameterLifting));
    auto result =
        storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), box, std::nullopt, Method::PLA, Engine::RobustParameterLifting);
    EXPECT_EQ(Status::Feasible, result.status);
    EXPECT_THROW(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), box, std::nullopt, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, GradientDescentRespectsExplicitRegionOutsideUnitInterval) {
    build("p/2");
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.85 [F \"target\"]"), region("1.1<=p<=1.9"), std::nullopt, Method::GD);
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(region("1.1<=p<=1.9").contains(result.witness->valuation));
    EXPECT_TRUE(formula("P>=0.85 [F \"target\"]")->asOperatorFormula().getBound().isSatisfied(result.witness->value));
}

TEST_F(SynthesisApiTest, ScpReturnsUnknownRatherThanInfeasible) {
#if defined STORM_HAVE_GLPK
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Glpk);
#elif defined STORM_HAVE_HIGHS
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Highs);
#elif defined STORM_HAVE_SOPLEX
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Soplex);
#else
    GTEST_SKIP() << "No LP solver available for SCP.";
#endif
    build();
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.51 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, Method::SCP);
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(region("0.2<=p<=0.8").contains(result.witness->valuation));

    result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), std::nullopt, Method::SCP);
    EXPECT_EQ(Status::Unknown, result.status);
    EXPECT_FALSE(result.witness);
}

TEST_F(SynthesisApiTest, SingletonAndParameterFreeModels) {
    build();
    auto point = region("0.5<=p<=0.5");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P>=0.5 [F \"target\"]"), point));
    EXPECT_FALSE(storm::api::verify(environment, model, formula("P>0.5 [F \"target\"]"), point));
    auto result = storm::api::checkFeasibility(environment, model, formula("P>0.5 [F \"target\"]"), point);
    EXPECT_EQ(Status::Infeasible, result.status);

    build("1/2", "");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.5 [F \"target\"]")));
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"));
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(result.witness->valuation.empty());
}

TEST_F(SynthesisApiTest, RejectsInvalidInput) {
    build();
    auto bounded = formula("P>=0.5 [F \"target\"]");
    EXPECT_THROW(storm::api::verify(environment, nullptr, bounded), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, nullptr), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::verify(environment, model, formula("P=? [F \"target\"]")), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, formula("P=? [F \"target\"]")), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::verify(environment, model, bounded, Region()), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::verify(environment, model, bounded, region("0.2<=p<=1.2")), storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, MdpSchedulerQuantificationIsPreserved) {
    program = storm::parser::PrismParser::parseFromString(
        "mdp\nconst double p;\nmodule choices\ns : [0..2] init 0;\n"
        "[] s=0 -> p : (s'=1) + (1-p) : (s'=2);\n"
        "[] s=0 -> 1/4 : (s'=1) + 3/4 : (s'=2);\n"
        "[] s=1 -> (s'=1);\n[] s=2 -> (s'=2);\nendmodule\nlabel \"target\" = s=1;\n",
        "SynthesisApiTestMdp");
    program = program.preprocess("");
    model = storm::api::buildSparseModel<storm::RationalFunction>(program, {formula("Pmax=? [F \"target\"]")});
    auto box = region("0.4<=p<=0.8");
    // Pmax can meet the bound via the parametric choice; Pmin cannot because of the fixed 1/4 choice.
    auto existential = formula("Pmax>=0.6 [F \"target\"]");
    auto universal = formula("Pmin>=0.6 [F \"target\"]");
    EXPECT_EQ(Status::Feasible,
              storm::api::checkFeasibility(environment, model, existential, box, std::nullopt, Method::PLA, Engine::ExactParameterLifting).status);
    EXPECT_EQ(Status::Infeasible,
              storm::api::checkFeasibility(environment, model, universal, box, std::nullopt, Method::PLA, Engine::ExactParameterLifting).status);
    EXPECT_FALSE(storm::api::verify(environment, model, universal, box, std::nullopt, Engine::ExactParameterLifting));
    EXPECT_TRUE(storm::api::verify(environment, model, formula("Pmax>=0.3 [F \"target\"]"), box, std::nullopt, Engine::ExactParameterLifting));
    EXPECT_EQ(Status::Infeasible,
              storm::api::checkFeasibility(environment, model, formula("P>=0.6 [F \"target\"]"), box, std::nullopt, Method::PLA, Engine::ExactParameterLifting)
                  .status);
    EXPECT_THROW(storm::api::verify(environment, model, existential, box, std::nullopt, Engine::RobustParameterLifting),
                 storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, existential, box, std::nullopt, Method::GD), storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, RewardThresholdsAndFixedDimensions) {
    build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto box = region("0.5<=p<=0.5,2<=r<=4");
    auto reward = formula("R{\"cost\"}>=3 [F !\"init\"]");
    auto result = storm::api::checkFeasibility(environment, model, reward, box, std::nullopt, Method::PLA, Engine::ExactParameterLifting);
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(box.contains(result.witness->valuation));
    EXPECT_TRUE(reward->asOperatorFormula().getBound().isSatisfied(result.witness->value));
    EXPECT_TRUE(storm::api::verify(environment, model, formula("R{\"cost\"}<=4 [F !\"init\"]"), box, std::nullopt, Engine::ExactParameterLifting));
}

TEST_F(SynthesisApiTest, LegacyVerificationPreservesStrictThresholds) {
    build();
    std::set<Region::VariableType> discreteVariables;
    for (auto const& text : {"P<0.5 [F \"target\"]", "P>0.5 [F \"target\"]"}) {
        auto property = formula(text);
        storm::api::RefinementOptions<storm::RationalFunction> options(model, *property, Engine::ExactParameterLifting,
                                                                       storm::modelchecker::RegionSplittingStrategy(), storm::api::MonotonicitySetting(),
                                                                       discreteVariables);
        // The interior center is a counterexample at equality. Exercise the existing refinement API directly.
        EXPECT_FALSE(storm::api::verifyRegion(environment, options, region("0.4<=p<=0.6")));
    }
}

TEST_F(SynthesisApiTest, InteriorWitnessAndCounterexample) {
    program = storm::parser::PrismParser::parseFromString(
        "dtmc\nconst double p;\nmodule interior\ns : [0..3] init 0;\n"
        "[] s=0 -> p : (s'=1) + (1-p) : (s'=3);\n"
        "[] s=1 -> (1-p) : (s'=2) + p : (s'=3);\n"
        "[] s=2 -> (s'=2);\n[] s=3 -> (s'=3);\nendmodule\nlabel \"target\" = s=2;\n",
        "SynthesisApiTestInterior");
    program = program.preprocess("");
    model = storm::api::buildSparseModel<storm::RationalFunction>(program, {formula("P=? [F \"target\"]")});
    auto box = region("0.1<=p<=0.9");
    // Reachability is p(1-p): both endpoints have value 0.09, while the center has value 0.25.
    auto result =
        storm::api::checkFeasibility(environment, model, formula("P>=0.2 [F \"target\"]"), box, std::nullopt, Method::PLA, Engine::ExactParameterLifting);
    ASSERT_EQ(Status::Feasible, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(formula("P>=0.2 [F \"target\"]")->asOperatorFormula().getBound().isSatisfied(result.witness->value));
    EXPECT_FALSE(storm::api::verify(environment, model, formula("P<=0.2 [F \"target\"]"), box, std::nullopt, Engine::ExactParameterLifting));
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.3 [F \"target\"]"), box, std::nullopt, Method::PLA, Engine::ExactParameterLifting);
    EXPECT_EQ(Status::Infeasible, result.status);
}

TEST_F(SynthesisApiTest, WitnessValuesAreDoubleForEveryMethod) {
    std::vector<Method> methods{Method::PLA, Method::GD};
#if defined STORM_HAVE_GLPK
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Glpk);
    methods.push_back(Method::SCP);
#elif defined STORM_HAVE_HIGHS
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Highs);
    methods.push_back(Method::SCP);
#elif defined STORM_HAVE_SOPLEX
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Soplex);
    methods.push_back(Method::SCP);
#endif
    build();
    auto property = formula("P>=0.51 [F \"target\"]");
    auto box = region("0.2<=p<=0.8");
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    for (auto method : methods) {
        auto numerical = storm::api::checkFeasibility(environment, model, property, std::nullopt, bound, method, Engine::ParameterLifting);
        static_assert(std::is_same_v<decltype(numerical), storm::pars::FeasibilitySynthesisResult<double>>);
        static_assert(std::is_same_v<decltype(numerical.witness->value), double>);
        ASSERT_EQ(Status::Feasible, numerical.status);
        ASSERT_TRUE(numerical.witness);
        EXPECT_TRUE(box.contains(numerical.witness->valuation));
        EXPECT_TRUE(property->asOperatorFormula().getBound().isSatisfied(numerical.witness->value));
    }
}

}  // namespace
