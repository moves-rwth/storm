#pragma once

#include "storm/solver/MultiplicationStyle.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class IterativeMinMaxLinearEquationSolverSettings {
        public:
            IterativeMinMaxLinearEquationSolverSettings();
            
            enum class SolutionMethod {
                ValueIteration, PolicyIteration, Acyclic, RationalSearch
            };
            
            void setSolutionMethod(SolutionMethod const& solutionMethod);
            void setSolutionMethod(MinMaxMethod const& solutionMethod);
            void setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations);
            void setRelativeTerminationCriterion(bool value);
            void setPrecision(ValueType precision);
            void setValueIterationMultiplicationStyle(MultiplicationStyle value);
            void setForceSoundness(bool value);
            
            SolutionMethod const& getSolutionMethod() const;
            uint64_t getMaximalNumberOfIterations() const;
            ValueType getPrecision() const;
            bool getRelativeTerminationCriterion() const;
            MultiplicationStyle getValueIterationMultiplicationStyle() const;
            bool getForceSoundness() const;
            
        private:
            bool forceSoundness;
            SolutionMethod solutionMethod;
            uint64_t maximalNumberOfIterations;
            ValueType precision;
            bool relative;
            MultiplicationStyle valueIterationMultiplicationStyle;
        };
        
        template<typename ValueType>
        class IterativeMinMaxLinearEquationSolver : public StandardMinMaxLinearEquationSolver<ValueType> {
        public:
            IterativeMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings = IterativeMinMaxLinearEquationSolverSettings<ValueType>());
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings = IterativeMinMaxLinearEquationSolverSettings<ValueType>());
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings = IterativeMinMaxLinearEquationSolverSettings<ValueType>());
            
            virtual bool internalSolveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

            IterativeMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            void setSettings(IterativeMinMaxLinearEquationSolverSettings<ValueType> const& newSettings);
            
            virtual void clearCache() const override;

            ValueType getPrecision() const;
            bool getRelative() const;
            
            virtual MinMaxLinearEquationSolverRequirements getRequirements(EquationSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none) const override;
            
        private:
            static bool isSolution(storm::OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b);
            
            bool solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;

            bool solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsSoundValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsAcyclic(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsRationalSearch(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            static bool sharpen(storm::OptimizationDirection dir, uint64_t precision, storm::storage::SparseMatrix<storm::RationalNumber> const& A, std::vector<double> const& x, std::vector<storm::RationalNumber> const& b, std::vector<storm::RationalNumber>& tmp);

            template<typename ImpreciseValueType>
            std::enable_if<std::is_same<ValueType, ImpreciseValueType>::value, bool> solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename ImpreciseValueType>
            std::enable_if<!std::is_same<ValueType, ImpreciseValueType>::value, bool> solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

            void computeOptimalValueForRowGroup(uint_fast64_t group, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, uint_fast64_t* choice = nullptr) const;
            
            enum class Status {
                Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress
            };
            
            struct ValueIterationResult {
                ValueIterationResult(uint64_t iterations, Status status) : iterations(iterations), status(status) {
                    // Intentionally left empty.
                }
                
                uint64_t iterations;
                Status status;
            };
            
            template <typename ValueTypePrime>
            friend class IterativeMinMaxLinearEquationSolver;
            
            ValueIterationResult performValueIteration(OptimizationDirection dir, std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations) const;
            
            void createLinearEquationSolver() const;
            
            // possibly cached data
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector2; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<uint64_t>> rowGroupOrdering; // A.rowGroupCount() entries
            
            Status updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations, SolverGuarantee const& guarantee) const;
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
            
            IterativeMinMaxLinearEquationSolverSettings<ValueType>& getSettings();
            IterativeMinMaxLinearEquationSolverSettings<ValueType> const& getSettings() const;
            
            virtual void setMinMaxMethod(MinMaxMethodSelection const& newMethod) override;
            virtual void setMinMaxMethod(MinMaxMethod const& newMethod) override;

            // Make the other create methods visible.
            using MinMaxLinearEquationSolverFactory<ValueType>::create;

            virtual std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> create() const override;
            
        private:
            IterativeMinMaxLinearEquationSolverSettings<ValueType> settings;
        };
    }
}
