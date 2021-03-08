#include "DerivativeEvaluationHelper.h"
#include "analysis/GraphConditions.h"
#include "environment/Environment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/GmmxxSolverEnvironment.h"
#include "environment/solver/MinMaxSolverEnvironment.h"
#include "environment/solver/NativeSolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/TopologicalSolverEnvironment.h"
#include "settings/modules/CoreSettings.h"
#include "solver/GmmxxLinearEquationSolver.h"
#include "solver/GmmxxMultiplier.h"
#include "solver/SolverSelectionOptions.h"
#include "solver/helper/SoundValueIterationHelper.h"
#include "modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "settings/SettingsManager.h"
#include "settings/modules/GeneralSettings.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm/solver/EliminationLinearEquationSolver.h"
#include "storm/solver/LinearEquationSolver.h"
#include "utility/graph.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/WrongFormatException.h"
#include "utility/logging.h"

namespace storm {
    namespace derivative {

        template<typename ValueType>
        using VariableType = typename utility::parametric::VariableType<ValueType>::type;
        template<typename ValueType>
        using CoefficientType = typename utility::parametric::CoefficientType<ValueType>::type;

        template<typename ValueType, typename ConstantType>
        ConstantType DerivativeEvaluationHelper<ValueType, ConstantType>::calculateDerivative(
                const VariableType<ValueType> parameter,
                const std::map<VariableType<ValueType>, CoefficientType<ValueType>> &substitutions,
                const std::vector<ConstantType> reachabilityProbabilities) {
            STORM_LOG_ASSERT(reachabilityProbabilities.size() == model->getNumberOfStates(),
                    "Size of reachability probability vector must be equal to the size of the model.");
            // Convert reachabilityProbabilities into our format - we only care for the states of which the
            // bits are 1 in the next vector. The order is kept, so doing this is fine:
            std::vector<ConstantType> interestingReachabilityProbabilities;
            for (uint64_t i = 0; i < reachabilityProbabilities.size(); i++) {
                if (next.get(i)) {
                    interestingReachabilityProbabilities.push_back(reachabilityProbabilities[i]);
                }
            }

            // Instantiate the matrices with the given instantiation
            
            instantiationWatch.start();

            // Write results into the placeholders
            for(auto& functionResult : this->functions) {
                functionResult.second=storm::utility::convertNumber<ConstantType>(
                        storm::utility::parametric::evaluate(functionResult.first, substitutions));
            }

            auto deltaConstrainedMatrixInstantiated = deltaConstrainedMatricesInstantiated->at(parameter);

            // Write the instantiated values to the matrices and vectors according to the stored mappings
            for(auto& entryValuePair : this->matrixMapping){
                entryValuePair.first->setValue(*(entryValuePair.second));
            }

            std::vector<ConstantType> instantiatedDerivedOutputVec(derivedOutputVecs->at(parameter).size());
            for (uint_fast64_t i = 0; i < derivedOutputVecs->at(parameter).size(); i++) {
                instantiatedDerivedOutputVec[i] = utility::convertNumber<ConstantType>(derivedOutputVecs->at(parameter)[i].evaluate(substitutions));
            }

            instantiationWatch.stop();

            approximationWatch.start();

            std::vector<ConstantType> resultVec(interestingReachabilityProbabilities.size());
            deltaConstrainedMatrixInstantiated.multiplyWithVector(interestingReachabilityProbabilities, resultVec);
            for (uint_fast64_t i = 0; i < instantiatedDerivedOutputVec.size(); ++i) {
                resultVec[i] += instantiatedDerivedOutputVec[i];
            }

            // Here's where the real magic happens - the solver call!

            // Calculate (1-M)^-1 * resultVec
            std::vector<ConstantType> finalResult(resultVec.size());
            /* std::cout << constrainedMatrixInstantiated << std::endl; */
            linearEquationSolvers[parameter]->setMatrix(constrainedMatrixInstantiated);
            /* for (auto const& entry : resultVec) { */
            /*     std::cout << entry << " "; */
            /* } */
            /* std::cout << std::endl; */
            /* std::cout << constrainedMatrixInstantiated << std::endl; */
            linearEquationSolvers[parameter]->solveEquations(*this->environment, finalResult, resultVec);
            /* for (auto const& entry : finalResult) { */
            /*     std::cout << entry << " "; */
            /* } */
            /* std::cout << std::endl; */

/*             std::stringstream ss; */
/*             ss << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); */
/*             std::ofstream matrixFileOut; */
/*             matrixFileOut.open("matrices/" + ss.str() + ".matrix.txt"); */
/*             for (uint64_t i = 0; i < constrainedMatrixInstantiated.getRowCount(); i++) { */
/*                 auto row = constrainedMatrixInstantiated.getRow(i); */
/*                 for (auto entry : row) { */
/*                     matrixFileOut << i + 1; */
/*                     matrixFileOut << "    "; */
/*                     matrixFileOut << entry.getColumn() + 1; */
/*                     matrixFileOut << "    "; */
/*                     matrixFileOut << entry.getValue() << std::setprecision(16); */
/*                     matrixFileOut << "\n"; */
/*                 } */
/*             } */
/*             matrixFileOut.close(); */

/*             std::ofstream vectorFileOut; */
/*             vectorFileOut.open("matrices/" + ss.str() + ".vector.txt"); */
/*             for (uint64_t i = 0; i < resultVec.size(); i++) { */
/*                 vectorFileOut << resultVec[i] << std::setprecision(16); */
/*                 vectorFileOut << "\n"; */
/*             } */
/*             vectorFileOut.close(); */

            ConstantType derivative = finalResult[initialState];

            approximationWatch.stop();

            return derivative;
        }

        template<typename ValueType, typename ConstantType>
        void DerivativeEvaluationHelper<ValueType, ConstantType>::setup(
                    std::set<typename utility::parametric::VariableType<ValueType>::type> const& parameters,
                    std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas)  {
            this->formula = formulas[0];
            storm::logic::OperatorInformation opInfo(boost::none, boost::none);
            switch (mode) {
                case ResultType::PROBABILITY: {
                    STORM_LOG_ASSERT(formula->isProbabilityOperatorFormula(), "The DerivativeEvaluationHelper is set to PROBABILITY, but the input formula is not a probability operator formula!");
                    STORM_LOG_ASSERT(rewardModelName == boost::none, "Solving for a probability in the DerivativeEvaluationHelper, but the rewardModelName is not set to boost::none.");
                    this->formulaWithoutBound = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                            formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo);
                    break;
                }
                case ResultType::REWARD: {
                    STORM_LOG_ASSERT(formula->isRewardOperatorFormula(), "The DerivativeEvaluationHelper is set to REWARD, but the input formula is not a reward operator formula!");
                    STORM_LOG_ASSERT(rewardModelName != boost::none, "Solving for a reward in the DerivativeEvaluationHelper, but the rewardModelName is boost::none.");
                    this->formulaWithoutBound = std::make_shared<storm::logic::RewardOperatorFormula>(
                            formula->asRewardOperatorFormula().getSubformula().asSharedPointer());
                    model->reduceToStateBasedRewards();
                    break;
                 }
            }

            generalSetupWatch.start();

            storm::solver::GeneralLinearEquationSolverFactory<ConstantType> factory;

            Environment newEnv;
            // If the environment was set explicitely to the user to something else than topological+eigen, we will respect that.
            // Otherwise, set it to topological+eigen.
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            // TODO this doesn't work yet
            /* if (!coreSettings.isEquationSolverSetFromDefaultValue()) { */
            /*     newEnv.solver().setLinearEquationSolverPrecision( */
            /*             storm::utility::convertNumber<storm::RationalNumber>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()), true); */
            /*     newEnv.solver().setLinearEquationSolverType(solver::EquationSolverType::Topological); */
            /*     newEnv.solver().topological().setUnderlyingEquationSolverType(solver::EquationSolverType::Eigen); */
            /* } */
            this->environment = std::make_unique<Environment>(newEnv);
            bool convertToEquationSystem = factory.getEquationProblemFormat(*environment) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;

            std::map<VariableType<ValueType>, storage::SparseMatrix<ValueType>> equationSystems;
            // Get initial and target states
            storage::BitVector target = model->getStates("target");
            initialState = model->getStates("init").getNextSetIndex(0);

            switch (mode) {
                case ResultType::PROBABILITY: {
                    next = target;
                    next.complement();
                    storm::storage::BitVector atSomePointTarget = storm::utility::graph::performProbGreater0(model->getBackwardTransitions(), storm::storage::BitVector(model->getNumberOfStates(), true), target);
                    next &= atSomePointTarget;
                    break;
                }
                case ResultType::REWARD: {
                    next = target;
                    next.complement();
                    storm::storage::BitVector targetProbOne = storm::utility::graph::performProb1(model->getBackwardTransitions(), storm::storage::BitVector(model->getNumberOfStates(), true), target);
                    next &= targetProbOne;
                    break;
                }
            }

            auto transitionMatrix = model->getTransitionMatrix();
            std::map<uint_fast64_t, uint_fast64_t> stateNumToEquationSystemRow;
            uint_fast64_t newRow = 0;
            for (uint_fast64_t row = 0; row < transitionMatrix.getRowCount(); ++row) {
                if (!next.get(row)) continue;
                stateNumToEquationSystemRow[row] = newRow;
                newRow++;
            }
            storage::SparseMatrix<ValueType> constrainedMatrix = transitionMatrix.getSubmatrix(false, next, next, true);
            // If necessary, convert the matrix from the fixpoint notation to the form needed for the equation system.
            this->constrainedMatrixEquationSystem = constrainedMatrix;
            if (convertToEquationSystem) {
                // go from x = A*x + b to (I-A)x = b.
                constrainedMatrixEquationSystem.convertToEquationSystem();
            }

            // Setup instantiated constrained matrix
            storage::SparseMatrixBuilder<ConstantType> instantiatedSystemBuilder;
            const ConstantType dummyValue = storm::utility::one<ConstantType>();
            for (uint_fast64_t row = 0; row < constrainedMatrixEquationSystem.getRowCount(); ++row) {
                for (auto const& entry : constrainedMatrixEquationSystem.getRow(row)) {
                    instantiatedSystemBuilder.addNextValue(row, entry.getColumn(), dummyValue);
                }
            }
            constrainedMatrixInstantiated = instantiatedSystemBuilder.build();
            initializeInstantiatedMatrix(constrainedMatrixEquationSystem, constrainedMatrixInstantiated);
            // The resulting equation systems
            this->deltaConstrainedMatrices = std::make_unique<std::map<VariableType<ValueType>, storage::SparseMatrix<ValueType>>>();
            this->deltaConstrainedMatricesInstantiated = std::make_unique<std::map<VariableType<ValueType>, storage::SparseMatrix<ConstantType>>>();
            this->derivedOutputVecs = std::make_unique<std::map<VariableType<ValueType>, std::vector<ValueType>>>();

            // The following is a nessecary optimization for deriving the constrainedMatrix w.r.t. all parameters.
            // We will traverse the constrainedMatrix once and inspect all entries, counting their number of parameters.
            // Then, we will insert the correct polynomials into the correct matrix builders.
            std::map<VariableType<ValueType>, storage::SparseMatrixBuilder<ValueType>> matrixBuilders;
            std::map<VariableType<ValueType>, storage::SparseMatrixBuilder<ConstantType>> instantiatedMatrixBuilders;
            for (auto const& var : parameters) {
                matrixBuilders[var] = storage::SparseMatrixBuilder<ValueType>(constrainedMatrix.getRowCount());
                instantiatedMatrixBuilders[var] = storage::SparseMatrixBuilder<ConstantType>(constrainedMatrix.getRowCount());
            }

            for (uint_fast64_t row = 0; row < constrainedMatrix.getRowCount(); ++row) {
                for (storage::MatrixEntry<uint_fast64_t, storm::RationalFunction> const& entry : constrainedMatrix.getRow(row)) {
                    const storm::RationalFunction rationalFunction = entry.getValue();
                    auto variables = rationalFunction.gatherVariables();
                    for (auto const& var : variables) {
                        matrixBuilders.at(var).addNextValue(row, entry.getColumn(), rationalFunction.derivative(var));
                        instantiatedMatrixBuilders.at(var).addNextValue(row, entry.getColumn(), dummyValue);
                    }
                }
            }

            for (auto const& var : parameters) {
                auto builtMatrix = matrixBuilders[var].build();
                auto builtMatrixInstantiated = instantiatedMatrixBuilders[var].build();
                initializeInstantiatedMatrix(builtMatrix, builtMatrixInstantiated);
                deltaConstrainedMatrices->emplace(var, builtMatrix);
                deltaConstrainedMatricesInstantiated->emplace(var, builtMatrixInstantiated);
            }

            for (auto const& var : parameters) {
                (*derivedOutputVecs)[var] = std::vector<ValueType>(constrainedMatrix.getRowCount());
            }

            for (uint_fast64_t x = 0; x < constrainedMatrix.getRowCount(); ++x) {
                    if (!stateNumToEquationSystemRow.count(x)) continue;
                    uint_fast64_t state = stateNumToEquationSystemRow[x];
                    // PROBABILITY -> For every state, the one-step probability to reach the target goes into the output vector
                    // REWARD -> For every state, the reward goes into the output vector
                    ValueType rationalFunction;
                    switch(mode) {
                        case ResultType::PROBABILITY: {
                            ValueType vectorValue = utility::zero<ValueType>();
                            for (auto const& entry : transitionMatrix.getRow(state)) {
                                if (target.get(entry.getColumn())) {
                                    vectorValue += entry.getValue();
                                }
                            }
                            rationalFunction = vectorValue;
                            break;
                        }
                        case ResultType::REWARD: {
                            auto stateRewards = model->getRewardModel(*rewardModelName).getStateRewardVector();
                            rationalFunction = stateRewards[state];
                            break;
                        }
                    }
                    for (auto const& var : rationalFunction.gatherVariables()) {
                        (*derivedOutputVecs)[var][x] = rationalFunction.derivative(var);
                    }
            }

            generalSetupWatch.stop();
            

            for (auto const& param : parameters) {
                this->linearEquationSolvers[param] = factory.create(newEnv);
                this->linearEquationSolvers[param]->setCachingEnabled(true);
            }
        }


        template<typename ValueType, typename ConstantType>
        void DerivativeEvaluationHelper<ValueType, ConstantType>::initializeInstantiatedMatrix(
            storage::SparseMatrix<ValueType> &matrix,
            storage::SparseMatrix<ConstantType> &matrixInstantiated
        )  {
            ConstantType dummyValue = storm::utility::one<ConstantType>();
            auto constantEntryIt = matrixInstantiated.begin();
            auto parametricEntryIt = matrix.begin();
            while(parametricEntryIt != matrix.end()) {
                STORM_LOG_ASSERT(parametricEntryIt->getColumn() == constantEntryIt->getColumn(), "Entries of parametric and constant matrix are not at the same position");
                if(storm::utility::isConstant(parametricEntryIt->getValue())){
                    //Constant entries can be inserted directly
                    constantEntryIt->setValue(storm::utility::convertNumber<ConstantType>(parametricEntryIt->getValue()));
                    /* std::cout << "Setting constant entry" << std::endl; */
                } else {
                    //insert the new function and store that the current constantMatrix entry needs to be set to the value of this function
                    auto functionsIt = functions.insert(std::make_pair(parametricEntryIt->getValue(), dummyValue)).first;
                    matrixMapping.emplace_back(std::make_pair(constantEntryIt, &(functionsIt->second)));
                    //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                    /* std::cout << "Setting non-constant entry" << std::endl; */
                }
                ++constantEntryIt;
                ++parametricEntryIt;
            }
            STORM_LOG_ASSERT(constantEntryIt == matrixInstantiated.end(), "Parametric matrix seems to have more or less entries then the constant matrix");
        }

        template class DerivativeEvaluationHelper<RationalFunction, RationalNumber>;
        template class DerivativeEvaluationHelper<RationalFunction, double>;
    }
}
