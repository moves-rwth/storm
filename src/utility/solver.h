#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/LinearEquationSolver.h"
#include "src/solver/NondeterministicLinearEquationSolver.h"
#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace utility {
        namespace solver {
            template<typename ValueType>
            class LinearEquationSolverFactory {
            public:
                /*!
                 * Creates a new linear equation solver instance with the given matrix.
                 *
                 * @param matrix The matrix that defines the equation system.
                 * @return A pointer to the newly created solver.
                 */
                virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const;
            };
            
            template<typename ValueType>
            class NativeLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class GmmxxLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class NondeterministicLinearEquationSolverFactory {
            public:
                /*!
                 * Creates a new nondeterministic linear equation solver instance with the given matrix.
                 */
                virtual std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const;
            };
            
            template<typename ValueType>
            class NativeNondeterministicLinearEquationSolverFactory : public NondeterministicLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class GmmxxNondeterministicLinearEquationSolverFactory : public NondeterministicLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class TopologicalNondeterministicLinearEquationSolverFactory : public NondeterministicLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };

            class LpSolverFactory {
            public:
                /*!
                 * Creates a new linear equation solver instance with the given name.
                 *
                 * @param name The name of the LP solver.
                 * @return A pointer to the newly created solver.
                 */
                virtual std::unique_ptr<storm::solver::LpSolver> create(std::string const& name) const;
            };
            
            class GlpkLpSolverFactory : public LpSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::LpSolver> create(std::string const& name) const override;
            };
            
            class GurobiLpSolverFactory : public LpSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::LpSolver> create(std::string const& name) const override;
            };
        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
