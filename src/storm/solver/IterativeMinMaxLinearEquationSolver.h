#pragma once

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class IterativeMinMaxLinearEquationSolverSettings {
        public:
            IterativeMinMaxLinearEquationSolverSettings();
            
            enum class SolutionMethod {
                ValueIteration, PolicyIteration, Acyclic
            };
            
            void setSolutionMethod(SolutionMethod const& solutionMethod);
            void setSolutionMethod(MinMaxMethod const& solutionMethod);
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
        class IterativeMinMaxLinearEquationSolver : public StandardMinMaxLinearEquationSolver<ValueType> {
        public:
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings = IterativeMinMaxLinearEquationSolverSettings<ValueType>());
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings = IterativeMinMaxLinearEquationSolverSettings<ValueType>());
            
            virtual bool solveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

            IterativeMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            void setSettings(IterativeMinMaxLinearEquationSolverSettings<ValueType> const& newSettings);
            
            virtual void clearCache() const override;

            ValueType getPrecision() const;
            bool getRelative() const;
            
        private:
            bool solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsAcyclic(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;
            
            void computeOptimalValueForRowGroup(uint_fast64_t group, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, uint_fast64_t* choice = nullptr) const;
            
            enum class Status {
                Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress
            };
            
            // possibly cached data
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<uint64_t>> rowGroupOrdering; // A.rowGroupCount() entries
            
            Status updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const;
            void reportStatus(Status status, uint64_t iterations) const;
            
            /// The settings of this solver.
            IterativeMinMaxLinearEquationSolverSettings<ValueType> settings;
        };
        
        template<typename ValueType>
        class IterativeMinMaxLinearEquationSolverFactory : public StandardMinMaxLinearEquationSolverFactory<ValueType> {
        public:
            IterativeMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            IterativeMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            IterativeMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, MinMaxMethodSelection const& method = MinMaxMethodSelection::FROMSETTINGS, bool trackScheduler = false);
            
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;
            
            IterativeMinMaxLinearEquationSolverSettings<ValueType>& getSettings();
            IterativeMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual void setMinMaxMethod(MinMaxMethodSelection const& newMethod) override;
            virtual void setMinMaxMethod(MinMaxMethod const& newMethod) override;
            
        private:
            IterativeMinMaxLinearEquationSolverSettings<ValueType> settings;
            
        };
    }
}
