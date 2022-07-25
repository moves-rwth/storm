#include <string>
#include "adapters/RationalNumberAdapter.h"
#include "environment/Environment.h"
#include "modelchecker/CheckTask.h"
#include "modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "solver/OptimizationDirection.h"
#include "storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "storage/prism/Program.h"
#include "storm-config.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm-pars/api/region.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm-pars/transformer/TimeTravelling.h"
#include "test/storm_gtest.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/api/builder.h"
#include "storm/api/storm.h"
#include "storm/environment/solver/GmmxxSolverEnvironment.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/TopologicalSolverEnvironment.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/EliminationLinearEquationSolver.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm-pars/analysis/OrderExtender.h"
#include "storm-pars/api/storm-pars.h"
#include "storm-pars/derivative/GradientDescentInstantiationSearcher.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "utility/macros.h"

void testModel(std::string programFile, std::string formulaAsString, std::string formulaBoundAsString, std::string constantsAsString) {
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction> const checkTask(*formulas[0]);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulasBound = storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaBoundAsString, program));
    storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction> const checkTaskBound(*formulasBound[0]);
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model = storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>(*dtmc);
    ASSERT_TRUE(simplifier.simplify(*formulas[0]));
    model = simplifier.getSimplifiedModel();
    dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    uint_fast64_t initialStateModel = dtmc->getStates("init").getNextSetIndex(0);
    
    storm::transformer::TimeTravelling timeTravelling;
    auto timeTravelledDtmc = timeTravelling.timeTravel(*dtmc, checkTask);

    storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>> modelChecker(*dtmc);
    storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>> modelCheckerTT(timeTravelledDtmc);
    
    auto result = modelChecker.check(checkTask)->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[initialStateModel];
    auto resultTT = modelCheckerTT.check(checkTask)->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[initialStateModel];
    
    STORM_LOG_ASSERT(result == resultTT, "Time-Travelling did not preserve reachability probability");

    auto region = storm::api::createRegion<storm::RationalFunction>("0.1", *dtmc);
    
    storm::Environment env;
    
    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> pla;
    pla.specify(env, dtmc, checkTaskBound);
    auto resultPLA = pla.computeQuantitativeValues(env, region[0], storm::OptimizationDirection::Minimize, storm::utility::convertNumber<storm::RationalFunction>(0.1), false).first;

    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> plaTT;
    plaTT.specify(env, timeTravelledDtmc.shared_from_this(), checkTaskBound);
    auto resultPLATT = plaTT.check(env, region[0], storm::OptimizationDirection::Minimize)->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[initialStateModel];
    
    
    STORM_LOG_ASSERT(resultPLATT < resultPLA, "Time-Travelling did not make bound better");
}

TEST(TimeTravelling, Brp) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [F \"error\"]";
    std::string formulaBoundAsString = "P>=0.2 [F \"error\"]";
    std::string constantsAsString = ""; //e.g. pL=0.9,TOACK=0.5
    testModel(programFile, formulaAsString, formulaBoundAsString, constantsAsString);
}