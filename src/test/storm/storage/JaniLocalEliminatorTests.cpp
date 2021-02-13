#include <storm/builder/ExplicitModelBuilder.h>
#include "test/storm_gtest.h"

#include <storm/api/storm.h>
#include "storm-parsers/api/model_descriptions.h"
#include "storm/storage/jani/ModelFeatures.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/localeliminator/JaniLocalEliminator.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Variable.h"
#include <storm/modelchecker/results/CheckResult.h>
#include <storm/modelchecker/results/ExplicitQuantitativeCheckResult.h>
#include "storm/environment/Environment.h"
#include <storm/settings/modules/GeneralSettings.h>


typedef storm::models::sparse::Dtmc<double> Dtmc;
typedef storm::modelchecker::SparseDtmcPrctlModelChecker<Dtmc> DtmcModelChecker;

using storm::jani::JaniLocalEliminator;

void checkModel(storm::jani::Model model, std::vector<storm::jani::Property> properties, std::map<storm::expressions::Variable, storm::expressions::Expression> consts, int states, int transitions, double expectedValue){
    model = model.defineUndefinedConstants(consts);
    properties[0] = properties[0].substitute(consts);

    auto formulae = storm::api::extractFormulasFromProperties(properties);
    storm::builder::BuilderOptions  options(formulae, model);
    options.setBuildAllLabels(true);
    auto explicitModel = storm::api::buildSparseModel<double>(model, options)->template as<Dtmc>();

    EXPECT_EQ(states, explicitModel->getNumberOfStates());
    EXPECT_EQ(transitions, explicitModel->getNumberOfTransitions());

    auto task = storm::modelchecker::CheckTask<>(*(formulae[0]), true);
    storm::Environment env;
    auto checkResult = storm::api::verifyWithSparseEngine<double>(env, explicitModel, task);
    auto quantResult = checkResult->asExplicitQuantitativeCheckResult<double>();

    auto initialStates = explicitModel->getInitialStates();
    for (auto state = initialStates.begin(); state != initialStates.end(); ++state){
        EXPECT_NEAR(expectedValue, quantResult[*state], storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
    }
}

TEST(JaniLocalEliminator, NandNoActionTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
    };

    checkModel(model, props, consts, 78332, 121512, 0.28641904638485044);
}

TEST(JaniLocalEliminator, NandUnfoldOnlyTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("multiplex", "s"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
    };

    checkModel(model, props, consts, 78332, 121512, 0.28641904638485044);
}

TEST(JaniLocalEliminator, NandSingleElimination) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("multiplex", "s"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_2"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_3"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_1"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
    };

    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
}

TEST(JaniLocalEliminator, NandAutoElimination) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("multiplex", "s"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("multiplex", JaniLocalEliminator::EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount, 5000));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
    };

    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
}

TEST(JaniLocalEliminator, NandAutomatic) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::AutomaticAction>());
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
            {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
    };

    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
}

TEST(JaniLocalEliminator, MultiplicityTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/multiplicity.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("main", "x"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("main", "l_x_2"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::RebuildWithoutUnreachableAction>());
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    EXPECT_EQ(model.getAutomaton("main").getEdges().size(), 0);

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = { };

    checkModel(model, props, consts, 31, 55, 0.04);
}

TEST(JaniLocalEliminator, BrpTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/brp.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    storm::jani::Model model = janiModelProperties.first;
    model = model.flattenComposition();
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "r"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "s"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("brp_flattened"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "l"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("brp_flattened"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "k"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("brp_flattened"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "srep"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("brp_flattened"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("brp_flattened", "s_ab"));
//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("brp_flattened"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(32)},
            {model.getConstant("MAX").getExpressionVariable(), model.getExpressionManager().integer(3)}
    };

    checkModel(model, props, consts, 1766, 2307, 0.000025235372864445436);
}

TEST(JaniLocalEliminator, BluetoothTest) {
    // Doesn't work, even without our unfolding or performing any model checking
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/bluetooth.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    storm::jani::Model model = janiModelProperties.first;
    model = model.flattenComposition();
    auto props = janiModelProperties.second;

    // JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

//    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("bluetooth_flattened", "?"));
//
//    eliminator.eliminate();
//    model = eliminator.getResult();
//    model.checkValid();
//    model.finalize();

//    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
//            {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(32)},
//            {model.getConstant("MAX").getExpressionVariable(), model.getExpressionManager().integer(3)}
//    };

    // checkModel(model, props, consts, 1766, 2307, 0.000025235372864445436);
}

TEST(JaniLocalEliminator, CouponTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/coupon.9-4.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    // eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("main", "draw0"));
    // eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAutomaticallyAction>("main", JaniLocalEliminator::EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();
    model.finalize();

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {model.getConstant("B").getExpressionVariable(), model.getExpressionManager().integer(5)},
    };

    checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);
}


TEST(JaniLocalEliminator, PerformanceTest){
    double timeEliminationTotal = 0;
    double timeCheckingTotal = 0;

    int runs = 10;

    for (int i = 0; i < runs; i++) {
        auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
        auto model = janiModelProperties.first;
        auto props = janiModelProperties.second;

        auto startElimination = clock();

        JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
//
//        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("multiplex", "s"));
//        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_2"));
//        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_3"));
//        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("multiplex", "l_s_1"));
        eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

        eliminator.eliminate();
        model = eliminator.getResult();
        model.checkValid();
        model.finalize();

        auto endElimination = clock();

        std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
                {model.getConstant("N").getExpressionVariable(), model.getExpressionManager().integer(20)},
                {model.getConstant("K").getExpressionVariable(), model.getExpressionManager().integer(1)}
        };

        checkModel(model, props, consts, 16182, 58102, 0.28641904638485044);

        auto endChecking = clock();

        timeEliminationTotal += (double)(endElimination - startElimination) / CLOCKS_PER_SEC;
        timeCheckingTotal += (double)(endChecking - endElimination) / CLOCKS_PER_SEC;
    }

    EXPECT_EQ(timeEliminationTotal / runs, 0);
    EXPECT_EQ(timeCheckingTotal / runs, 0);
}