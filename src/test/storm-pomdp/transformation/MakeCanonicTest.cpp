#include "storm-config.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm-pomdp/analysis/QualitativeAnalysisOnGraphs.h"
#include "storm-pomdp/transformer/MakePOMDPCanonic.h"
#include "storm/api/storm.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "test/storm_gtest.h"

TEST(MakeCanonic, Simple) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/pomdp/simple.prism");
    program = storm::utility::prism::preprocess(program, "slippery=0.4");
    std::shared_ptr<storm::logic::Formula const> formula = storm::api::parsePropertiesForPrismProgram("Pmax=? [F \"goal\" ]", program).front().getRawFormula();
    std::shared_ptr<storm::models::sparse::Pomdp<double>> pomdp =
        storm::api::buildSparseModel<double>(program, {formula})->as<storm::models::sparse::Pomdp<double>>();
    storm::transformer::MakePOMDPCanonic<double> makeCanonic(*pomdp);
    pomdp = makeCanonic.transform();
}
