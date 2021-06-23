#include "SparseDerivativeInstantiationModelChecker.h"
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

        template<typename FunctionType>
        using VariableType = typename utility::parametric::VariableType<FunctionType>::type;
        template<typename FunctionType>
        using CoefficientType = typename utility::parametric::CoefficientType<FunctionType>::type;

        template<typename FunctionType, typename ConstantType>
        std::unique_ptr<modelchecker::ExplicitQuantitativeCheckResult<ConstantType>> SparseDerivativeInstantiationModelChecker<FunctionType, ConstantType>::check(Environment const& env, storm::utility::parametric::Valuation<FunctionType> const& valuation, VariableType<FunctionType> const& parameter, boost::optional<std::vector<ConstantType>> const& valueVector) {
            storm::solver::GeneralLinearEquationSolverFactory<ConstantType> factory;


            std::vector<ConstantType> reachabilityProbabilities;
            if (!valueVector.is_initialized()) {
                storm::modelchecker::SparseDtmcInstantiationModelChecker<storm::models::sparse::Dtmc<FunctionType>, ConstantType> instantiationModelChecker(model);
                instantiationModelChecker.specifyFormula(*currentCheckTask);
                std::unique_ptr<storm::modelchecker::CheckResult> result = instantiationModelChecker.check(env, valuation);
                reachabilityProbabilities = result->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            } else {
                STORM_LOG_ASSERT(valueVector->size() == model.getNumberOfStates(), "Size of reachability probability vector must be equal to the size of the model.");
                reachabilityProbabilities = *valueVector;
            }

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
                        storm::utility::parametric::evaluate(functionResult.first, valuation));
            }

            auto deltaConstrainedMatrixInstantiated = deltaConstrainedMatricesInstantiated->at(parameter);

            // Write the instantiated values to the matrices and vectors according to the stored mappings
            for(auto& entryValuePair : this->matrixMapping){
                entryValuePair.first->setValue(*(entryValuePair.second));
            }

            std::vector<ConstantType> instantiatedDerivedOutputVec(derivedOutputVecs->at(parameter).size());
            for (uint_fast64_t i = 0; i < derivedOutputVecs->at(parameter).size(); i++) {
                instantiatedDerivedOutputVec[i] = utility::convertNumber<ConstantType>(derivedOutputVecs->at(parameter)[i].evaluate(valuation));
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
            linearEquationSolvers[parameter]->setMatrix(constrainedMatrixInstantiated);
            linearEquationSolvers[parameter]->solveEquations(env, finalResult, resultVec);

            approximationWatch.stop();

            return std::make_unique<modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(finalResult);
        }

        template<typename FunctionType, typename ConstantType>
        void SparseDerivativeInstantiationModelChecker<FunctionType, ConstantType>::specifyFormula(
                Environment const& env, 
                modelchecker::CheckTask<storm::logic::Formula, FunctionType> const& checkTask)  {
            this->currentFormula = checkTask.getFormula().asSharedPointer();
            this->currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, FunctionType>>(checkTask.substituteFormula(*currentFormula).template convertValueType<FunctionType>());
            this->parameters = storm::models::sparse::getProbabilityParameters(model);
            if (checkTask.isRewardModelSet()) {
                for (auto const& rewardParameter : storm::models::sparse::getRewardParameters(model)) {
                    this->parameters.insert(rewardParameter);
                }
            }
            storm::logic::OperatorInformation opInfo(boost::none, boost::none);
            if (checkTask.isRewardModelSet()) {
                model.reduceToStateBasedRewards();
            } else {
                /* this->formulaWithoutBound = std::make_shared<storm::logic::ProbabilityOperatorFormula>( */
                /*         formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo); */
            }

            generalSetupWatch.start();

            storm::solver::GeneralLinearEquationSolverFactory<ConstantType> factory;

            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            bool convertToEquationSystem = factory.getEquationProblemFormat(env) == storm::solver::LinearEquationSolverProblemFormat::EquationSystem;

            std::map<VariableType<FunctionType>, storage::SparseMatrix<FunctionType>> equationSystems;
            // Get initial and target states
            storage::BitVector target = model.getStates("target");
            initialState = model.getStates("init").getNextSetIndex(0);

            if (!checkTask.isRewardModelSet()) {
                next = target;
                next.complement();
                storm::storage::BitVector atSomePointTarget = storm::utility::graph::performProbGreater0(model.getBackwardTransitions(), storm::storage::BitVector(model.getNumberOfStates(), true), target);
                next &= atSomePointTarget;
            } else {
                next = target;
                next.complement();
                storm::storage::BitVector targetProbOne = storm::utility::graph::performProb1(model.getBackwardTransitions(), storm::storage::BitVector(model.getNumberOfStates(), true), target);
                next &= targetProbOne;
            }

            auto transitionMatrix = model.getTransitionMatrix();
            std::map<uint_fast64_t, uint_fast64_t> stateNumToEquationSystemRow;
            uint_fast64_t newRow = 0;
            for (uint_fast64_t row = 0; row < transitionMatrix.getRowCount(); ++row) {
                if (!next.get(row)) continue;
                stateNumToEquationSystemRow[row] = newRow;
                newRow++;
            }
            storage::SparseMatrix<FunctionType> constrainedMatrix = transitionMatrix.getSubmatrix(false, next, next, true);
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
            this->deltaConstrainedMatrices = std::make_unique<std::map<VariableType<FunctionType>, storage::SparseMatrix<FunctionType>>>();
            this->deltaConstrainedMatricesInstantiated = std::make_unique<std::map<VariableType<FunctionType>, storage::SparseMatrix<ConstantType>>>();
            this->derivedOutputVecs = std::make_unique<std::map<VariableType<FunctionType>, std::vector<FunctionType>>>();

            // The following is a nessecary optimization for deriving the constrainedMatrix w.r.t. all parameters.
            // We will traverse the constrainedMatrix once and inspect all entries, counting their number of parameters.
            // Then, we will insert the correct polynomials into the correct matrix builders.
            std::map<VariableType<FunctionType>, storage::SparseMatrixBuilder<FunctionType>> matrixBuilders;
            std::map<VariableType<FunctionType>, storage::SparseMatrixBuilder<ConstantType>> instantiatedMatrixBuilders;
            for (auto const& var : this->parameters) {
                matrixBuilders[var] = storage::SparseMatrixBuilder<FunctionType>(constrainedMatrix.getRowCount());
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

            for (auto const& var : this->parameters) {
                auto builtMatrix = matrixBuilders[var].build();
                auto builtMatrixInstantiated = instantiatedMatrixBuilders[var].build();
                initializeInstantiatedMatrix(builtMatrix, builtMatrixInstantiated);
                deltaConstrainedMatrices->emplace(var, builtMatrix);
                deltaConstrainedMatricesInstantiated->emplace(var, builtMatrixInstantiated);
            }

            for (auto const& var : this->parameters) {
                (*derivedOutputVecs)[var] = std::vector<FunctionType>(constrainedMatrix.getRowCount());
            }

            for (uint_fast64_t x = 0; x < constrainedMatrix.getRowCount(); ++x) {
                    if (!stateNumToEquationSystemRow.count(x)) continue;
                    uint_fast64_t state = stateNumToEquationSystemRow[x];
                    // PROBABILITY -> For every state, the one-step probability to reach the target goes into the output vector
                    // REWARD -> For every state, the reward goes into the output vector
                    FunctionType rationalFunction;
                    if (!checkTask.isRewardModelSet()) {
                        FunctionType vectorValue = utility::zero<FunctionType>();
                        for (auto const& entry : transitionMatrix.getRow(state)) {
                            if (target.get(entry.getColumn())) {
                                vectorValue += entry.getValue();
                            }
                        }
                        rationalFunction = vectorValue;
                    } else {
                        auto stateRewards = model.getRewardModel(checkTask.getRewardModel()).getStateRewardVector();
                        rationalFunction = stateRewards[state];
                    }
                    for (auto const& var : rationalFunction.gatherVariables()) {
                        (*derivedOutputVecs)[var][x] = rationalFunction.derivative(var);
                    }
            }

            generalSetupWatch.stop();
            

            for (auto const& param : this->parameters) {
                this->linearEquationSolvers[param] = factory.create(env);
                this->linearEquationSolvers[param]->setCachingEnabled(true);
            }
        }


        template<typename FunctionType, typename ConstantType>
        void SparseDerivativeInstantiationModelChecker<FunctionType, ConstantType>::initializeInstantiatedMatrix(
            storage::SparseMatrix<FunctionType> &matrix,
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

        template class SparseDerivativeInstantiationModelChecker<RationalFunction, RationalNumber>;
        template class SparseDerivativeInstantiationModelChecker<RationalFunction, double>;
    }
}
