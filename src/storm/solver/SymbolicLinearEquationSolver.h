#ifndef STORM_SOLVER_SYMBOLICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_SYMBOLICLINEAREQUATIONSOLVER_H_

#include <set>
#include <vector>

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/dd/DdType.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace dd {
        template<storm::dd::DdType Type, typename ValueType>
        class Add;
        
        template<storm::dd::DdType Type>
        class Bdd;
        
    }
    
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType = double>
        class SymbolicLinearEquationSolverSettings {
        public:
            // Currently empty.
        };
        
        /*!
         * An interface that represents an abstract symbolic linear equation solver. In addition to solving a system of
         * linear equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<storm::dd::DdType DdType, typename ValueType = double>
        class SymbolicLinearEquationSolver {
        public:
            /*!
             * Constructs a symbolic linear equation solver with the given meta variable sets and pairs.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param diagonal An ADD characterizing the elements on the diagonal of the matrix.
             * @param allRows A BDD characterizing all rows of the equation system.
             * @param rowMetaVariables The meta variables used to encode the rows of the matrix.
             * @param columnMetaVariables The meta variables used to encode the columns of the matrix.
             * @param rowColumnMetaVariablePairs The pairs of row meta variables and the corresponding column meta
             * variables.
             */
            SymbolicLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);
            
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param x The initial guess for the solution vector. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             * @return The solution of the equation system.
             */
            virtual storm::dd::Add<DdType, ValueType> solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const = 0;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x. Note that the
             * matrix A has to be given upon construction time of the solver object.
             *
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of rows of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @return The solution of the equation system.
             */
            virtual storm::dd::Add<DdType, ValueType> multiply(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b = nullptr, uint_fast64_t n = 1) const;
            
            void setMatrix(storm::dd::Add<DdType, ValueType> const& newA);
            
        protected:
            // The matrix defining the coefficients of the linear equation system.
            storm::dd::Add<DdType, ValueType> A;
            
            // A BDD characterizing all rows of the equation system.
            storm::dd::Bdd<DdType> allRows;
            
            // The row variables.
            std::set<storm::expressions::Variable> rowMetaVariables;
            
            // The column variables.
            std::set<storm::expressions::Variable> columnMetaVariables;
            
            // The pairs of meta variables used for renaming.
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs;
        };
        
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicLinearEquationSolverFactory {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const = 0;
        };
        
        template<storm::dd::DdType DdType, typename ValueType>
        class GeneralSymbolicLinearEquationSolverFactory : public SymbolicLinearEquationSolverFactory<DdType, ValueType> {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
        };

        template<storm::dd::DdType DdType>
        class GeneralSymbolicLinearEquationSolverFactory<DdType, storm::RationalNumber> : public SymbolicLinearEquationSolverFactory<DdType, storm::RationalNumber> {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, storm::RationalNumber>> create(storm::dd::Add<DdType, storm::RationalNumber> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
        };
        
        template<storm::dd::DdType DdType>
        class GeneralSymbolicLinearEquationSolverFactory<DdType, storm::RationalFunction> : public SymbolicLinearEquationSolverFactory<DdType, storm::RationalFunction> {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, storm::RationalFunction>> create(storm::dd::Add<DdType, storm::RationalFunction> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_SYMBOLICLINEAREQUATIONSOLVER_H_ */
