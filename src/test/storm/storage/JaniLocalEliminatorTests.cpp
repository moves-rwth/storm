#include <storm/builder/ExplicitModelBuilder.h>
#include "test/storm_gtest.h"

#include <storm/api/storm.h>
#include "storm/models/sparse/Model.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/storage/jani/ModelFeatures.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/JaniLocalEliminator.h"
#include "storm/storage/jani/JSONExporter.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/Variable.h"
#include <storm/storage/jani/Property.h>
#include <storm/modelchecker/results/CheckResult.h>
#include <storm/modelchecker/results/ExplicitQuantitativeCheckResult.h>
#include "storm/environment/Environment.h"
#include <boost/variant/variant.hpp>
#include <storm/settings/modules/GeneralSettings.h>


typedef storm::models::sparse::Dtmc<double> Dtmc;
typedef storm::modelchecker::SparseDtmcPrctlModelChecker<Dtmc> DtmcModelChecker;

using storm::jani::JaniLocalEliminator;

std::pair<storm::jani::Model, std::vector<storm::jani::Property>> saveAndReload(storm::jani::Model model, std::vector<storm::jani::Property> props){
    storm::jani::JsonExporter::toFile(model, props, STORM_TEST_RESOURCES_DIR "/dtmc/_temp_model.jani", true, false);
    auto minModelProps = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/_temp_model.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    return minModelProps;
}

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

TEST(JaniLocalEliminator, ExampleTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);

    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::UnfoldAction>("s"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_2"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_3"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::EliminateAction>("l_s_1"));
    eliminator.scheduler.addAction(std::make_unique<JaniLocalEliminator::FinishAction>());

    eliminator.eliminate();
    model = eliminator.getResult();
    model.checkValid();

    auto minModelProps = saveAndReload(model, props);
    auto modelMin = minModelProps.first;
    auto propsMin = minModelProps.second;

    std::map<storm::expressions::Variable, storm::expressions::Expression> consts = {
            {modelMin.getConstant("N").getExpressionVariable(), modelMin.getExpressionManager().integer(20)},
            {modelMin.getConstant("K").getExpressionVariable(), modelMin.getExpressionManager().integer(1)}
    };

    checkModel(modelMin, propsMin, consts, 16182, 58102, 0.28641904638485044);
}