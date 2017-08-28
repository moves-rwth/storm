#include "gtest/gtest.h"
#include "storm-config.h"
#include "storm/parser/PrismParser.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/storage/dd/BisimulationDecomposition.h"

TEST(SymbolicBisimulationDecompositionTest_Cudd, Die) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::dd::bisimulation::Partition<storm::dd::DdType::CUDD, double>::create(*model, {"one"}));
    decomposition.compute();
}

TEST(SymbolicBisimulationDecompositionTest_Cudd, Crowds) {
    storm::storage::SymbolicModelDescription modelDescription = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds-5-5.pm");
    storm::prism::Program program = modelDescription.preprocess().asPrismProgram();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::dd::bisimulation::Partition<storm::dd::DdType::CUDD, double>::create(*model, {"observe0Greater1"}));
    decomposition.compute();
}
