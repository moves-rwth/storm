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
using Status = storm::modelchecker::RegionResult;
using Region = storm::storage::ParameterRegion<storm::RationalFunction>;

class SynthesisApiTest : public ::testing::Test {
   protected:
    void SetUp() override {
        storm::clearRFVariablePool();
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }

    void build(std::string const& probability = "p", std::string const& declarations = "const double p;", std::string const& rewards = "",
               std::string const& modelType = "dtmc") {
        program = storm::parser::PrismParser::parseFromString(modelType + "\n" + declarations +
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

    void expectInRegion(Region const& region, Region::Valuation const& valuation) const {
        // Compare at LP precision; converting a double to an exact coefficient can put it just outside the region.
        for (auto const& variable : region.getVariables()) {
            ASSERT_TRUE(valuation.count(variable));
            auto const value = storm::utility::convertNumber<double>(valuation.at(variable));
            EXPECT_GE(value, storm::utility::convertNumber<double>(region.getLowerBoundary(variable)));
            EXPECT_LE(value, storm::utility::convertNumber<double>(region.getUpperBoundary(variable)));
        }
    }

    storm::Environment environment;
    storm::prism::Program program;
    std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model;
};

TEST_F(SynthesisApiTest, VerificationEngineSelectionAndStrictBounds) {
    build();
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), engine));
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P>=0.6 [F \"target\"]"), region("0.2<=p<=0.8"), engine));
        // The center is a counterexample at exactly the threshold. Strictness must be preserved.
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P<0.5 [F \"target\"]"), region("0.4<=p<=0.6"), engine));
        EXPECT_FALSE(storm::api::verify(environment, model, formula("P>0.5 [F \"target\"]"), region("0.4<=p<=0.6"), engine));
    }
}

TEST_F(SynthesisApiTest, PlaFeasibilityAndInfeasibility) {
    build();
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.6 [F \"target\"]"), region("0.2<=p<=0.8"), Method::PLA, engine);
        ASSERT_EQ(Status::ExistsSat, result.status);
        ASSERT_TRUE(result.witness);
        EXPECT_TRUE(region("0.2<=p<=0.8").contains(result.witness->valuation));
        EXPECT_TRUE(formula("P>=0.6 [F \"target\"]")->asOperatorFormula().getBound().isSatisfied(result.witness->value));

        result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), Method::PLA, engine);
        EXPECT_EQ(Status::AllViolated, result.status);
        EXPECT_FALSE(result.witness);

        result = storm::api::checkFeasibility(environment, model, formula("P<=0.4 [F \"target\"]"), region("0.2<=p<=0.8"), Method::PLA, engine);
        EXPECT_EQ(Status::ExistsSat, result.status);
    }
}

TEST_F(SynthesisApiTest, RegionBoundMatchesExplicitRegion) {
    build();
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto box = region("0.2<=p<=0.8");
    auto property = formula("P>=0.6 [F \"target\"]");
    EXPECT_EQ(storm::api::verify(environment, model, property, box), storm::api::verify(environment, model, property, bound));
    EXPECT_EQ(storm::api::checkFeasibility(environment, model, property, box).status, storm::api::checkFeasibility(environment, model, property, bound).status);
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting, Engine::RobustParameterLifting}) {
        EXPECT_EQ(storm::api::verify(environment, model, property, box, engine), storm::api::verify(environment, model, property, bound, engine));
        auto explicitResult = storm::api::checkFeasibility(environment, model, property, box, Method::PLA, engine);
        auto boundResult = storm::api::checkFeasibility(environment, model, property, bound, Method::PLA, engine);
        EXPECT_EQ(explicitResult.status, boundResult.status);
        ASSERT_TRUE(boundResult.witness);
        EXPECT_TRUE(box.contains(boundResult.witness->valuation));
    }
}

TEST_F(SynthesisApiTest, RegionBoundAppliesToEveryParameterWithoutInferringDomains) {
    build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto property = formula("P>=0.6 [F \"target\"]");
    auto result = storm::api::checkFeasibility(environment, model, property, bound, Method::PLA, Engine::ParameterLifting);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_EQ(2u, result.witness->valuation.size());
    EXPECT_TRUE(region("0.2<=p<=0.8,0.2<=r<=0.8").contains(result.witness->valuation));

    // For p/2, the user still chose p in [0.2,0.8]; the API does not infer or scale it to [0,2].
    build("p/2");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.4 [F \"target\"]"), bound, Engine::ExactParameterLifting));
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), bound, Method::PLA, Engine::ExactParameterLifting);
    EXPECT_EQ(Status::AllViolated, result.status);
    EXPECT_FALSE(result.witness);
}

TEST_F(SynthesisApiTest, RegionBoundIncludesClosedEndpointsAndAllowsSingletons) {
    build();
    auto zero = storm::utility::zero<Region::CoefficientType>();
    auto half = storm::utility::one<Region::CoefficientType>() / 2;
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), zero, Engine::RobustParameterLifting));
    auto fullRegion = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), zero, Method::PLA, Engine::RobustParameterLifting);
    EXPECT_EQ(Status::ExistsSat, fullRegion.status);
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P>=0.5 [F \"target\"]"), half, Engine::ParameterLifting));
    auto singleton = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), half, Method::PLA, Engine::ParameterLifting);
    ASSERT_EQ(Status::ExistsSat, singleton.status);
    ASSERT_TRUE(singleton.witness);
    EXPECT_EQ(half, singleton.witness->valuation.begin()->second);
}

TEST_F(SynthesisApiTest, RejectsInvalidRegionBounds) {
    build();
    auto property = formula("P>=0.5 [F \"target\"]");
    auto one = storm::utility::one<Region::CoefficientType>();
    for (Region::CoefficientType const& invalid : {Region::CoefficientType(-one / 10), Region::CoefficientType(one * 3 / 5)}) {
        EXPECT_THROW(storm::api::verify(environment, model, property, invalid, Engine::ParameterLifting), storm::exceptions::InvalidArgumentException);
        EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, invalid, Method::PLA, Engine::ParameterLifting),
                     storm::exceptions::InvalidArgumentException);
    }
}

TEST_F(SynthesisApiTest, ConstructedRegionMustPassWellformednessAndGraphChecks) {
    build();
    auto zero = storm::utility::zero<Region::CoefficientType>();
    auto bound = storm::utility::one<Region::CoefficientType>() / 5;
    auto property = formula("P<=1 [F \"target\"]");
    EXPECT_THROW(storm::api::verify(environment, model, property, zero, Engine::ParameterLifting), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, zero, Method::GD, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);

    // [0.2,0.8] would make some probabilities p+4/5 greater than one.
    build("p+4/5");
    EXPECT_THROW(storm::api::verify(environment, model, property, bound, Engine::ParameterLifting), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, bound, Method::PLA, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, RobustPlaDoesNotRequireAnAffineWellformednessProof) {
    build("p*p");
    auto box = region("0.1<=p<=0.9");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), box, Engine::RobustParameterLifting));
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), box, Method::PLA, Engine::RobustParameterLifting);
    EXPECT_EQ(Status::ExistsSat, result.status);
    EXPECT_THROW(storm::api::verify(environment, model, formula("P<=1 [F \"target\"]"), box, Engine::ParameterLifting),
                 storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, RobustPlaRejectsStepBoundedQueries) {
    build("p", "const double p;", "rewards \"cost\"\n s=0 : 1;\nendrewards\n");
    auto box = region("0.2<=p<=0.8");
    for (auto const& text : {"P<=0.9 [F<=2 \"target\"]", "R{\"cost\"}<=2 [C<=2]"}) {
        auto property = formula(text);
        EXPECT_THROW(storm::api::verify(environment, model, property, box, Engine::RobustParameterLifting), storm::exceptions::NotSupportedException);
        EXPECT_THROW(storm::api::checkFeasibility(environment, model, property, box, Method::PLA, Engine::RobustParameterLifting),
                     storm::exceptions::NotSupportedException);
    }
}

TEST_F(SynthesisApiTest, RobustPlaRejectsNonconstantTotalRewards) {
    build("1", "const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto box = region("0.5<=r<=1.5");
    EXPECT_THROW(storm::api::verify(environment, model, formula("R{\"cost\"}<=1.25 [F \"target\"]"), box, Engine::RobustParameterLifting),
                 storm::exceptions::NotSupportedException);
    EXPECT_THROW(
        storm::api::checkFeasibility(environment, model, formula("R{\"cost\"}>=1.25 [F \"target\"]"), box, Method::PLA, Engine::RobustParameterLifting),
        storm::exceptions::NotSupportedException);

    // Only the queried reward model matters; a constant total reward above one remains supported.
    build("1", "const double r;", "rewards \"cost\"\n s=0 : 2;\nendrewards\nrewards \"unused\"\n s=0 : r;\nendrewards\n");
    box = region("0.5<=r<=1.5");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("R{\"cost\"}<=2 [F \"target\"]"), box, Engine::RobustParameterLifting));
    EXPECT_EQ(
        Status::ExistsSat,
        storm::api::checkFeasibility(environment, model, formula("R{\"cost\"}>=2 [F \"target\"]"), box, Method::PLA, Engine::RobustParameterLifting).status);
}

TEST_F(SynthesisApiTest, GradientDescentHandlesSingletonRegions) {
    build();
    auto point = region("0.5<=p<=0.5");
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.4 [F \"target\"]"), point, Method::GD);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_EQ(0.5, result.witness->value);
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), point, Method::GD);
    EXPECT_EQ(Status::AllViolated, result.status);
    EXPECT_FALSE(result.witness);

    build("1/2", "");
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.4 [F \"target\"]"), Region(), Method::GD);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(result.witness->valuation.empty());
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), Region(), Method::GD);
    EXPECT_EQ(Status::AllViolated, result.status);
    EXPECT_FALSE(result.witness);
}

TEST_F(SynthesisApiTest, GradientDescentPreservesUnrelatedRewardParameters) {
    build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto box = region("0.2<=p<=0.8,2<=r<=4");
    auto property = formula("P>=0.7 [F \"target\"]");
    auto result = storm::api::checkFeasibility(environment, model, property, box, Method::GD);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(box.contains(result.witness->valuation));
    for (auto const& variable : box.getVariables()) {
        if (variable.name() == "r") {
            EXPECT_EQ(box.getCenter(variable), result.witness->valuation.at(variable));
        }
    }
    EXPECT_TRUE(property->asOperatorFormula().getBound().isSatisfied(result.witness->value));
}

TEST_F(SynthesisApiTest, ScpPreservesUnrelatedRewardParameters) {
#if defined STORM_HAVE_GLPK
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Glpk);
#elif defined STORM_HAVE_HIGHS
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Highs);
#elif defined STORM_HAVE_SOPLEX
    environment.solver().setLpSolverType(storm::solver::LpSolverType::Soplex);
#else
    GTEST_SKIP() << "No LP solver available for SCP.";
#endif
    for (auto const& type : {"dtmc", "mdp"}) {
        build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n", type);
        auto box = region("0.2<=p<=0.8,2<=r<=4");
        auto property = formula("P>=0.7 [F \"target\"]");
        auto result = storm::api::checkFeasibility(environment, model, property, box, Method::SCP);
        ASSERT_EQ(Status::ExistsSat, result.status);
        ASSERT_TRUE(result.witness);
        expectInRegion(box, result.witness->valuation);
        for (auto const& variable : box.getVariables()) {
            if (variable.name() == "r") {
                EXPECT_EQ(box.getCenter(variable), result.witness->valuation.at(variable));
            }
        }
        EXPECT_TRUE(property->asOperatorFormula().getBound().isSatisfied(result.witness->value));
    }
}

TEST_F(SynthesisApiTest, GradientDescentRespectsExplicitRegionOutsideUnitInterval) {
    build("p/2");
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.85 [F \"target\"]"), region("1.1<=p<=1.9"), Method::GD);
    ASSERT_EQ(Status::ExistsSat, result.status);
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
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.51 [F \"target\"]"), region("0.2<=p<=0.8"), Method::SCP);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    expectInRegion(region("0.2<=p<=0.8"), result.witness->valuation);

    result = storm::api::checkFeasibility(environment, model, formula("P>=0.9 [F \"target\"]"), region("0.2<=p<=0.8"), Method::SCP);
    EXPECT_EQ(Status::Unknown, result.status);
    EXPECT_FALSE(result.witness);
}

TEST_F(SynthesisApiTest, SingletonAndParameterFreeModels) {
    build();
    auto point = region("0.5<=p<=0.5");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P>=0.5 [F \"target\"]"), point));
    EXPECT_FALSE(storm::api::verify(environment, model, formula("P>0.5 [F \"target\"]"), point));
    auto result = storm::api::checkFeasibility(environment, model, formula("P>0.5 [F \"target\"]"), point);
    EXPECT_EQ(Status::AllViolated, result.status);

    build("1/2", "");
    EXPECT_TRUE(storm::api::verify(environment, model, formula("P<=0.5 [F \"target\"]"), Region()));
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.5 [F \"target\"]"), Region());
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(result.witness->valuation.empty());
}

TEST_F(SynthesisApiTest, RejectsInvalidInput) {
    build();
    auto bounded = formula("P>=0.5 [F \"target\"]");
    EXPECT_THROW(storm::api::verify(environment, nullptr, bounded, Region()), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, nullptr, Region()), storm::exceptions::InvalidArgumentException);
    EXPECT_THROW(storm::api::verify(environment, model, formula("P=? [F \"target\"]"), Region()), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, formula("P=? [F \"target\"]"), Region()), storm::exceptions::NotSupportedException);
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
    for (auto engine : {Engine::ParameterLifting, Engine::ExactParameterLifting, Engine::ValidatingParameterLifting}) {
        EXPECT_EQ(Status::ExistsSat, storm::api::checkFeasibility(environment, model, existential, box, Method::PLA, engine).status);
        EXPECT_EQ(Status::AllViolated, storm::api::checkFeasibility(environment, model, universal, box, Method::PLA, engine).status);
        EXPECT_FALSE(storm::api::verify(environment, model, universal, box, engine));
        EXPECT_TRUE(storm::api::verify(environment, model, formula("Pmax>=0.3 [F \"target\"]"), box, engine));
        EXPECT_EQ(Status::AllViolated, storm::api::checkFeasibility(environment, model, formula("P>=0.6 [F \"target\"]"), box, Method::PLA, engine).status);
        // An upper bound without min/max also quantifies universally over schedulers, so it uses Pmax.
        EXPECT_EQ(Status::ExistsSat, storm::api::checkFeasibility(environment, model, formula("Pmin<=0.3 [F \"target\"]"), box, Method::PLA, engine).status);
        EXPECT_EQ(Status::AllViolated, storm::api::checkFeasibility(environment, model, formula("P<=0.3 [F \"target\"]"), box, Method::PLA, engine).status);
    }
    EXPECT_THROW(storm::api::verify(environment, model, existential, box, Engine::RobustParameterLifting), storm::exceptions::NotSupportedException);
    EXPECT_THROW(storm::api::checkFeasibility(environment, model, existential, box, Method::GD), storm::exceptions::NotSupportedException);
}

TEST_F(SynthesisApiTest, ScpRequiresUniversalMdpSchedulerQuantification) {
    program = storm::parser::PrismParser::parseFromString(
        "mdp\nconst double p;\nmodule choices\ns : [0..2] init 0;\n"
        "[] s=0 -> p : (s'=1) + (1-p) : (s'=2);\n"
        "[] s=0 -> (s'=1);\n[] s=0 -> (s'=2);\n"
        "[] s=1 -> (s'=1);\n[] s=2 -> (s'=2);\nendmodule\nlabel \"target\" = s=1;\n",
        "SynthesisApiTestScpMdp");
    program = program.preprocess("");
    model = storm::api::buildSparseModel<storm::RationalFunction>(program, {formula("Pmax=? [F \"target\"]")});
    auto box = region("0.2<=p<=0.8");
    // Pmin is always zero and Pmax is always one. Unsupported quantifiers must be rejected even when
    // SCP's qualitative analysis can return a constant value before setting up the numerical search.
    for (auto const& text : {"Pmax>=0.8 [F \"target\"]", "Pmin<=0.2 [F \"target\"]"}) {
        EXPECT_THROW(storm::api::checkFeasibility(environment, model, formula(text), box, Method::SCP), storm::exceptions::NotSupportedException);
    }
    for (auto const& text : {"Pmin>=0 [F \"target\"]", "Pmax<=1 [F \"target\"]", "P>=0 [F \"target\"]", "P<=1 [F \"target\"]"}) {
        auto result = storm::api::checkFeasibility(environment, model, formula(text), box, Method::SCP);
        EXPECT_EQ(Status::ExistsSat, result.status);
        EXPECT_TRUE(result.witness);
    }
}

TEST_F(SynthesisApiTest, RewardThresholdsAndFixedDimensions) {
    build("p", "const double p; const double r;", "rewards \"cost\"\n s=0 : r;\nendrewards\n");
    auto box = region("0.5<=p<=0.5,2<=r<=4");
    auto reward = formula("R{\"cost\"}>=3 [F !\"init\"]");
    auto result = storm::api::checkFeasibility(environment, model, reward, box, Method::PLA, Engine::ExactParameterLifting);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(box.contains(result.witness->valuation));
    EXPECT_TRUE(reward->asOperatorFormula().getBound().isSatisfied(result.witness->value));
    EXPECT_TRUE(storm::api::verify(environment, model, formula("R{\"cost\"}<=4 [F !\"init\"]"), box, Engine::ExactParameterLifting));
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
    auto result = storm::api::checkFeasibility(environment, model, formula("P>=0.2 [F \"target\"]"), box, Method::PLA, Engine::ExactParameterLifting);
    ASSERT_EQ(Status::ExistsSat, result.status);
    ASSERT_TRUE(result.witness);
    EXPECT_TRUE(formula("P>=0.2 [F \"target\"]")->asOperatorFormula().getBound().isSatisfied(result.witness->value));
    EXPECT_FALSE(storm::api::verify(environment, model, formula("P<=0.2 [F \"target\"]"), box, Engine::ExactParameterLifting));
    result = storm::api::checkFeasibility(environment, model, formula("P>=0.3 [F \"target\"]"), box, Method::PLA, Engine::ExactParameterLifting);
    EXPECT_EQ(Status::AllViolated, result.status);
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
        auto numerical = storm::api::checkFeasibility(environment, model, property, bound, method, Engine::ParameterLifting);
        static_assert(std::is_same_v<decltype(numerical), storm::pars::FeasibilitySynthesisResult<double>>);
        static_assert(std::is_same_v<decltype(numerical.witness->value), double>);
        ASSERT_EQ(Status::ExistsSat, numerical.status);
        ASSERT_TRUE(numerical.witness);
        if (method == Method::SCP) {
            expectInRegion(box, numerical.witness->valuation);
        } else {
            EXPECT_TRUE(box.contains(numerical.witness->valuation));
        }
        EXPECT_TRUE(property->asOperatorFormula().getBound().isSatisfied(numerical.witness->value));
    }
}

}  // namespace
