#include "storm-config.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/api/storm.h"
#include "storm/api/export.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/jani/Property.h"
#include "test/storm_gtest.h"
#include "storm/transformer/AddUncertainty.h"

TEST(AddUncertaintyTransformerTest, BrpTest) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/brp-16-2.pm");
    std::string formulasString =
        "P=? [ F \"target\"]";
    auto formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulasString, program));
    auto model = storm::api::buildSparseModel<double>(program, formulas);

    auto transformer = storm::transformer::AddUncertainty(model);
    auto uncertainModel = transformer.transform(0.01);
    // TODO
}