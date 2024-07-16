#include <carl/formula/Constraint.h>
#include <memory>
#include <string>
#include "adapters/RationalFunctionAdapter.h"
#include "adapters/RationalNumberAdapter.h"
#include "environment/Environment.h"
#include "modelchecker/CheckTask.h"
#include "modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "models/sparse/Model.h"
#include "solver/OptimizationDirection.h"
#include "storage/bisimulation/BisimulationType.h"
#include "storage/prism/Program.h"
#include "storm-config.h"
#include "storm-pars/api/region.h"
#include "storm-pars/modelchecker/instantiation/SparseInstantiationModelChecker.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/transformer/BinaryDtmcTransformer.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/api/bisimulation.h"
#include "storm/api/builder.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/prism.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/storm-parsers.h"
#include "utility/constants.h"
#include "utility/logging.h"
#include "utility/macros.h"

void testModelB(std::string programFile, std::string formulaAsString, std::string constantsAsString) {
    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, constantsAsString);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction> const checkTask(*formulas[0]);
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
    uint64_t initialStateModel = dtmc->getStates("init").getNextSetIndex(0);

    dtmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(dtmc, formulas, storm::storage::BisimulationType::Weak)
               ->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    storm::transformer::BinaryDtmcTransformer binaryDtmcTransformer;
    auto simpleDtmc = binaryDtmcTransformer.transform(*dtmc, true);

    storm::modelchecker::SparseDtmcInstantiationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelChecker(*dtmc);
    modelChecker.specifyFormula(checkTask);
    storm::modelchecker::SparseDtmcInstantiationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelCheckerSimple(*simpleDtmc);
    modelCheckerSimple.specifyFormula(checkTask);

    auto parameters = storm::models::sparse::getAllParameters(*dtmc);

    // Check if both DTMCs are equivalent just by sampling.
    std::vector<std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>> testInstantiations;
    std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> emptyInstantiation;
    testInstantiations.push_back(emptyInstantiation);
    for (auto const& param : parameters) {
        std::vector<std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>> newInstantiations;
        for (auto point : testInstantiations) {
            for (storm::RationalNumber x = storm::utility::convertNumber<storm::RationalNumber>(1e-6); x <= 1;
                 x += (1 - storm::utility::convertNumber<storm::RationalNumber>(1e-6)) / 10) {
                std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> newMap(point);
                newMap[param] = storm::utility::convertNumber<storm::RationalFunctionCoefficient>(x);
                newInstantiations.push_back(newMap);
            }
        }
        testInstantiations = newInstantiations;
    }

    storm::Environment env;
    for (auto const& instantiation : testInstantiations) {
        auto result = modelChecker.check(env, instantiation)->asExplicitQuantitativeCheckResult<double>()[initialStateModel];
        auto resultSimple = modelCheckerSimple.check(env, instantiation)->asExplicitQuantitativeCheckResult<double>()[initialStateModel];
        ASSERT_TRUE(storm::utility::isAlmostZero(result - resultSimple))
            << "Results " << result << " and " << resultSimple << " are not the same but should be.";
    }

    auto region = storm::api::createRegion<storm::RationalFunction>("0.4", *dtmc);

    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> pla;
    pla.specify(env, dtmc, checkTask);
    auto resultPLA = pla.getBoundAtInitState(env, region[0], storm::OptimizationDirection::Minimize);

    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> plaSimple;
    plaSimple.specify(env, simpleDtmc, checkTask);
    auto resultPLASimple = plaSimple.getBoundAtInitState(env, region[0], storm::OptimizationDirection::Minimize);

    // no <= defined for RationalFunctions I suppose
    ASSERT_TRUE(resultPLA < resultPLASimple || resultPLA == resultPLASimple) << "Worse PLA result with simplified DTMC";

    // Check that simpleDtmc is in fact simple
    for (uint64_t state = 0; state < simpleDtmc->getTransitionMatrix().getRowCount(); ++state) {
        auto row = simpleDtmc->getTransitionMatrix().getRow(state);

        std::set<storm::RationalFunctionVariable> variables;

        for (auto const& entry : row) {
            for (auto const& variable : entry.getValue().gatherVariables()) {
                variables.emplace(variable);
            }
        }

        if (variables.size() != 0) {
            ASSERT_TRUE(variables.size() == 1);
            auto parameter = *variables.begin();

            auto parameterPol = storm::RawPolynomial(parameter);
            auto parameterRational = storm::RationalFunction(carl::makePolynomial<storm::Polynomial>(parameterPol));
            auto oneMinusParameter = storm::RawPolynomial(1) - parameterPol;
            auto oneMinusParameterRational = storm::RationalFunction(carl::makePolynomial<storm::Polynomial>(oneMinusParameter));

            uint64_t seenP = 0;
            uint64_t seenOneMinusP = 0;

            for (auto const& entry : row) {
                if (!storm::utility::isZero(entry.getValue())) {
                    if (entry.getValue() == parameterRational) {
                        seenP++;
                    } else if (entry.getValue() == oneMinusParameterRational) {
                        seenOneMinusP++;
                    } else {
                        ASSERT_TRUE(false) << "Value " << entry.getValue() << " is not simple";
                    }
                }
            }

            ASSERT_TRUE(seenP == 1 && seenOneMinusP == 1) << "State " << state << " not simple";
        }
    }
}

TEST(BinaryDtmcTransformer, Crowds) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/crowds3_5.pm";
    std::string formulaAsString = "P=? [F \"observeIGreater1\"]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5
    testModelB(programFile, formulaAsString, constantsAsString);
}
TEST(BinaryDtmcTransformer, Nand) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/nand-5-2.pm";
    std::string formulaAsString = "P=? [F \"target\"]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5
    testModelB(programFile, formulaAsString, constantsAsString);
}
TEST(BinaryDtmcTransformer, Brp) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/brp16_2.pm";
    std::string formulaAsString = "P=? [F \"error\"]";
    std::string constantsAsString = "";  // e.g. pL=0.9,TOACK=0.5
    testModelB(programFile, formulaAsString, constantsAsString);
}
