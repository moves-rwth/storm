#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/NativeLinearEquationSolver.h"
#include "src/storage/dd/DdType.h"

namespace storm {
    namespace solver {
        template<storm::dd::DdType T>  class SymbolicGameSolver;
        template<storm::dd::DdType T, typename V> class SymbolicLinearEquationSolver;
        template<typename V> class LinearEquationSolver;
        template<typename V> class MinMaxLinearEquationSolver;
        class LpSolver;
    }
    namespace dd {
        template<storm::dd::DdType T> class Add;
        template<storm::dd::DdType T> class Bdd;
    }
    namespace expressions {
        class Variable;
    }
    
    namespace utility {
        namespace solver {
            template<storm::dd::DdType Type, typename ValueType>
            class SymbolicLinearEquationSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<Type, ValueType>> create(storm::dd::Add<Type> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
            };
            
            template<storm::dd::DdType Type>
            class SymbolicGameSolverFactory {
            public:
                virtual std::unique_ptr<storm::solver::SymbolicGameSolver<Type>> create(storm::dd::Add<Type> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) const;
            };
            
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
                NativeLinearEquationSolverFactory();
                NativeLinearEquationSolverFactory(typename storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod method, ValueType omega);
                
                virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
                
            private:
                typename storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod method;
                ValueType omega;
            };
            
            template<typename ValueType>
            class GmmxxLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class MinMaxLinearEquationSolverFactory {
            public:
                /*!
                 * Creates a new nondeterministic linear equation solver instance with the given matrix.
                 */
                virtual std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const;
            };
            
            template<typename ValueType>
            class NativeMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class GmmxxMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            };
            
            template<typename ValueType>
            class TopologicalMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
            public:
                virtual std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
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
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name);
        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
