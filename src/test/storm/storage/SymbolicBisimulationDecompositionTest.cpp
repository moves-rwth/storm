#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/parser/PrismParser.h"
#include "storm/parser/FormulaParser.h"

#include "storm/builder/DdPrismModelBuilder.h"

#include "storm/storage/dd/BisimulationDecomposition.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/models/symbolic/Mdp.h"
#include "storm/models/symbolic/StandardRewardModel.h"

TEST(SymbolicModelBisimulationDecomposition, Die_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(11ul, quotient->getNumberOfStates());
    EXPECT_EQ(17ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(5ul, quotient->getNumberOfStates());
    EXPECT_EQ(8ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
}

TEST(SymbolicModelBisimulationDecomposition, DiePartialQuotient_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
}

TEST(SymbolicModelBisimulationDecomposition, Die_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(11ul, quotient->getNumberOfStates());
    EXPECT_EQ(17ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"two\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(5ul, quotient->getNumberOfStates());
    EXPECT_EQ(8ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
}

TEST(SymbolicModelBisimulationDecomposition, Crowds_Cudd) {
    storm::storage::SymbolicModelDescription smd = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds5_5.pm");
    
    // Preprocess model to substitute all constants.
    smd = smd.preprocess();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(smd.asPrismProgram());
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(2007ul, quotient->getNumberOfStates());
    EXPECT_EQ(3738ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(65ul, quotient->getNumberOfStates());
    EXPECT_EQ(105ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
}

TEST(SymbolicModelBisimulationDecomposition, Crowds_Sylvan) {
    storm::storage::SymbolicModelDescription smd = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/crowds5_5.pm");
    
    // Preprocess model to substitute all constants.
    smd = smd.preprocess();
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan, double>().build(smd.asPrismProgram());
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(2007ul, quotient->getNumberOfStates());
    EXPECT_EQ(3738ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(65ul, quotient->getNumberOfStates());
    EXPECT_EQ(105ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Dtmc, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
}

TEST(SymbolicModelBisimulationDecomposition, TwoDice_Cudd) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");

    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();

    EXPECT_EQ(77ul, quotient->getNumberOfStates());
    EXPECT_EQ(210ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(116ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>()->getNumberOfChoices()));

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(11ul, quotient->getNumberOfStates());
    EXPECT_EQ(34ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(19ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>()->getNumberOfChoices()));
}

TEST(SymbolicModelBisimulationDecomposition, TwoDice_Sylvan) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan, double>().build(program);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(77ul, quotient->getNumberOfStates());
    EXPECT_EQ(210ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(116ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>()->getNumberOfChoices()));
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Pmin=? [F \"two\"]");
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(11ul, quotient->getNumberOfStates());
    EXPECT_EQ(34ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(19ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>()->getNumberOfChoices()));
}

TEST(SymbolicModelBisimulationDecomposition, AsynchronousLeader_Cudd) {
    storm::storage::SymbolicModelDescription smd = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader4.nm");
    
    // Preprocess model to substitute all constants.
    smd = smd.preprocess();

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD, double>().build(smd.asPrismProgram(), *formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(252ul, quotient->getNumberOfStates());
    EXPECT_EQ(624ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(500ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>()->getNumberOfChoices()));
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::CUDD, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(1107ul, quotient->getNumberOfStates());
    EXPECT_EQ(2684ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(2152ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>()->getNumberOfChoices()));
}

TEST(SymbolicModelBisimulationDecomposition, AsynchronousLeader_Sylvan) {
    storm::storage::SymbolicModelDescription smd = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/leader4.nm");
    
    // Preprocess model to substitute all constants.
    smd = smd.preprocess();
    
    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("Rmax=? [F \"elected\"]");
    
    std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan, double>().build(smd.asPrismProgram(), *formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition(*model, storm::storage::BisimulationType::Strong);
    decomposition.compute();
    std::shared_ptr<storm::models::Model<double>> quotient = decomposition.getQuotient();
    
    EXPECT_EQ(252ul, quotient->getNumberOfStates());
    EXPECT_EQ(624ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(500ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>()->getNumberOfChoices()));
    
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
    formulas.push_back(formula);
    
    storm::dd::BisimulationDecomposition<storm::dd::DdType::Sylvan, double> decomposition2(*model, formulas, storm::storage::BisimulationType::Strong);
    decomposition2.compute();
    quotient = decomposition2.getQuotient();
    
    EXPECT_EQ(1107ul, quotient->getNumberOfStates());
    EXPECT_EQ(2684ul, quotient->getNumberOfTransitions());
    EXPECT_EQ(storm::models::ModelType::Mdp, quotient->getType());
    EXPECT_TRUE(quotient->isSymbolicModel());
    EXPECT_EQ(2152ul, (quotient->as<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>()->getNumberOfChoices()));
}
