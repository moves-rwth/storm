#include <storm/builder/ExplicitModelBuilder.h>
#include "test/storm_gtest.h"

#include "storm/models/sparse/Model.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/storage/jani/ModelFeatures.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/JaniLocalEliminator.h"
#include "storm/storage/jani/JSONExporter.h"
#include "storm/storage/expressions/ExpressionManager.h"

using storm::jani::JaniLocalEliminator;

TEST(JaniLocalEliminator, ExampleTest) {
    auto janiModelProperties = storm::api::parseJaniModel(STORM_TEST_RESOURCES_DIR "/dtmc/nand.v1.jani", storm::jani::getAllKnownModelFeatures(), boost::none);
    auto model = janiModelProperties.first;
    auto props = janiModelProperties.second;

    JaniLocalEliminator eliminator = JaniLocalEliminator(model, props);
    eliminator.eliminate();
    auto result = eliminator.getResult();

    result.checkValid();

    storm::jani::JsonExporter::toFile(result, props, STORM_TEST_RESOURCES_DIR "/dtmc/nandmin.jani", true, false);


    result = result.defineUndefinedConstants({
        {result.getConstant("N").getExpressionVariable(), result.getExpressionManager().integer(20)},
        {result.getConstant("K").getExpressionVariable(), result.getExpressionManager().integer(1)}
    });
    std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> explicitModel = storm::builder::ExplicitModelBuilder<storm::RationalNumber>(result).build();
    EXPECT_EQ(5ul, explicitModel->getNumberOfStates());
    EXPECT_EQ(8ul, explicitModel->getNumberOfTransitions());

    EXPECT_EQ(1, 2);
}