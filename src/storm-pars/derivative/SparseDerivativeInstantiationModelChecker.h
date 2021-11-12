#ifndef STORM_DERIVATIVEEVALUATIONHELPER_H
#define STORM_DERIVATIVEEVALUATIONHELPER_H

#include <map>
#include "analysis/GraphConditions.h"
#include "logic/Formula.h"
#include "modelchecker/CheckTask.h"
#include "solver/LinearEquationSolver.h"
#include "storm-pars/utility/parametric.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/Stopwatch.h"
#include "storm/modelchecker/results/CheckResult.h"

namespace storm {
    namespace derivative {
        template <typename FunctionType, typename ConstantType>
        class SparseDerivativeInstantiationModelChecker {
        public:
            /**
             * Instantiates a new SparseDerivativeInstantiationModelChecker.
             * @param model The Dtmc to compute the derivatives of.
             */
            SparseDerivativeInstantiationModelChecker(storm::models::sparse::Dtmc<FunctionType> const& model) : model(model) {
                // Intentionally left empty.
            }
            virtual ~SparseDerivativeInstantiationModelChecker() = default;

            /**
             * specifyFormula specifies a CheckTask.
             * The SparseDerivativeInstantiationModelChecker will need to setup the matrices for it.
             * Note this can consume a substantial amount of memory if the model is big and there's a large number of parameters.
             * @param env The environment. Pass the same environment as to check. We need this because we need to know what kind of
             * equation solver we are dealing with.
             * @param checkTask The CheckTask.
             */
            void specifyFormula(Environment const& env, modelchecker::CheckTask<logic::Formula, FunctionType> const& checkTask);
            
            /**
             * check calculates the deriative of the model w.r.t. a parameter at an instantiation.
             * Call specifyFormula first!
             * @param env The environment.
             */
            std::unique_ptr<modelchecker::ExplicitQuantitativeCheckResult<ConstantType>> check(Environment const& env, storm::utility::parametric::Valuation<FunctionType> const& valuation, typename utility::parametric::VariableType<FunctionType>::type const& parameter, boost::optional<std::vector<ConstantType>> const& valueVector = boost::none);

        private:
            models::sparse::Dtmc<FunctionType> model;
            std::unique_ptr<modelchecker::CheckTask<storm::logic::Formula, FunctionType>> currentCheckTask;
            // store the current formula. Note that currentCheckTask only stores a reference to the formula.
            std::shared_ptr<storm::logic::Formula const> currentFormula;

            std::set<typename utility::parametric::VariableType<FunctionType>::type> parameters;
            std::map<typename utility::parametric::VariableType<FunctionType>::type, std::unique_ptr<storm::solver::LinearEquationSolver<ConstantType>>> linearEquationSolvers;
            std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>> matrixMapping; 
            std::unordered_map<FunctionType, ConstantType> functions; 
            storage::SparseMatrix<FunctionType> constrainedMatrixEquationSystem;
            storage::SparseMatrix<ConstantType> constrainedMatrixInstantiated;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<FunctionType>::type, storage::SparseMatrix<FunctionType>>> deltaConstrainedMatrices;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<FunctionType>::type, storage::SparseMatrix<ConstantType>>> deltaConstrainedMatricesInstantiated;
            std::unique_ptr<std::map<typename utility::parametric::VariableType<FunctionType>::type, std::vector<FunctionType>>> derivedOutputVecs;

            // next states: states that have a relevant successor
            storage::BitVector next;
            uint_fast64_t initialState;

            void initializeInstantiatedMatrix(
                storage::SparseMatrix<FunctionType> &matrix,
                storage::SparseMatrix<ConstantType> &matrixInstantiated
            );
            void setup(
                Environment const& env,
                modelchecker::CheckTask<storm::logic::Formula, FunctionType> const& checkTask
            );

            utility::Stopwatch instantiationWatch;
            utility::Stopwatch approximationWatch;
            utility::Stopwatch generalSetupWatch;
        };
    }
}

#endif // STORM_DERIVATIVEEVALUATIONHELPER_H
