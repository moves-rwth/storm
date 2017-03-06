#pragma once

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolverSettings {
        public:
            StandardMinMaxLinearEquationSolverSettings();
            
            enum class SolutionMethod {
                ValueIteration, PolicyIteration
            };
            
            void setSolutionMethod(SolutionMethod const& solutionMethod);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setRelativeTerminationCriterion(bool value);
            void setPrecision(ValueType precision);

            SolutionMethod const& getSolutionMethod() const;
            uint64_t getMaximalNumberOfIterations() const;
            ValueType getPrecision() const;
            bool getRelativeTerminationCriterion() const;

        private:
            SolutionMethod solutionMethod;
            uint64_t maximalNumberOfIterations;
            ValueType precision;
            bool relative;
        };
        
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings<ValueType> const& settings = StandardMinMaxLinearEquationSolverSettings<ValueType>());
            StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings<ValueType> const& settings = StandardMinMaxLinearEquationSolverSettings<ValueType>());
            
            virtual bool solveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            virtual void repeatedMultiply(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const override;

            StandardMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            void setSettings(StandardMinMaxLinearEquationSolverSettings<ValueType> const& newSettings);
            
            virtual void clearCache() const override;

            virtual ValueType getPrecision() const override;
            virtual bool getRelative() const override;
        private:
            bool solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;
            
            enum class Status {
                Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress
            };
            
            // possibly cached data
            mutable std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSolverA;
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowVector; // A.rowCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries

            Status updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const;
            void reportStatus(Status status, uint64_t iterations) const;
            
            /// The settings of this solver.
            StandardMinMaxLinearEquationSolverSettings<ValueType> settings;
            
            /// The factory used to obtain linear equation solvers.
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;
            
            // A reference to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the reference refers to localA.
            storm::storage::SparseMatrix<ValueType> const& A;
            
        };
     
        template<typename ValueType>
        class StandardMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
        public:
            StandardMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, bool trackScheduler = false);
            StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, bool trackScheduler = false);
            
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;
            
            StandardMinMaxLinearEquationSolverSettings<ValueType>& getSettings();
            StandardMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
        private:
            StandardMinMaxLinearEquationSolverSettings<ValueType> settings;
            
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
        };
        
        template<typename ValueType>
        class GmmxxMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            GmmxxMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class EigenMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EigenMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class NativeMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            NativeMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

        template<typename ValueType>
        class EliminationMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            EliminationMinMaxLinearEquationSolverFactory(bool trackScheduler = false);
        };

    }
}
