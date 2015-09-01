#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"
#include "src/logic/Formulas.h"
#include "src/permissivesched/PermissiveSchedulers.h"
#include "src/builder/ExplicitPrismModelBuilder.h"


TEST(MilpPermissiveSchedulerTest, DieSelection) {
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/die_selection.nm");
    storm::parser::FormulaParser formulaParser(program.getManager().getSharedPointer());
    std::cout << " We are now here " << std::endl;
    auto formula = formulaParser.parseSingleFormulaFromString("P<=0.2 [ F \"one\"]")->asProbabilityOperatorFormula();
    std::cout << formula << std::endl;
    
    // Customize and perform model-building.
    typename storm::builder::ExplicitPrismModelBuilder<double>::Options options;
    
    options = typename storm::builder::ExplicitPrismModelBuilder<double>::Options(formula);
    options.addConstantDefinitionsFromString(program, "");
    options.buildCommandLabels = true;
    
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = storm::builder::ExplicitPrismModelBuilder<double>::translateProgram(program, options)->as<storm::models::sparse::Mdp<double>>();
    
    storm::ps::computePermissiveSchedulerViaMILP(mdp, formula);
    
 // 
    
}
