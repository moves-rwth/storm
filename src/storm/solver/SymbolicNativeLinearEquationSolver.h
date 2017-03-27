#pragma once

#include "storm/solver/SymbolicLinearEquationSolver.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        class SymbolicNativeLinearEquationSolverSettings {
        public:
            SymbolicNativeLinearEquationSolverSettings();
            
            void setPrecision(ValueType precision);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setRelativeTerminationCriterion(bool value);
            
            ValueType getPrecision() const;
            uint64_t getMaximalNumberOfIterations() const;
            bool getRelativeTerminationCriterion() const;
            
        private:
            // The required precision for the iterative methods.
            ValueType precision;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;
            
            // Sets whether the relative or absolute error is to be considered for convergence detection. Note that this
            // only applies to the Jacobi method for this solver.
            bool relative;
        };
        
        /*!
         * An interface that represents an abstract symbolic linear equation solver. In addition to solving a system of
         * linear equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<storm::dd::DdType DdType, typename ValueType = double>
        class SymbolicNativeLinearEquationSolver : public SymbolicLinearEquationSolver<DdType, ValueType> {
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
             * @param settings The settings to use.
             */
            SymbolicNativeLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, SymbolicNativeLinearEquationSolverSettings<ValueType> const& settings = SymbolicNativeLinearEquationSolverSettings<ValueType>());
            
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param x The initial guess for the solution vector. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             * @return The solution of the equation system.
             */
            virtual storm::dd::Add<DdType, ValueType> solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const;

            SymbolicNativeLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
        private:
            // The settings to use.
            SymbolicNativeLinearEquationSolverSettings<ValueType> settings;
        };
        
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicNativeLinearEquationSolverFactory {
        public:
            virtual std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const;
            
            SymbolicNativeLinearEquationSolverSettings<ValueType>& getSettings();
            SymbolicNativeLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
        private:
            SymbolicNativeLinearEquationSolverSettings<ValueType> settings;
        };
        
    } // namespace solver
} // namespace storm
