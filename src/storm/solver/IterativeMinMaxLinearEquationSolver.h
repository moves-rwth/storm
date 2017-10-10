#pragma once

#include "storm/solver/MultiplicationStyle.h"

#include "storm/utility/NumberTraits.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"

#include "storm/solver/SolverStatus.h"

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
            bool solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;

            bool solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsSoundValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsAcyclic(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsRationalSearch(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            template<typename RationalType, typename ImpreciseType>
            bool solveEquationsRationalSearchHelper(OptimizationDirection dir, IterativeMinMaxLinearEquationSolver<ImpreciseType> const& impreciseSolver, storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX, std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A, std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const;
            template<typename ImpreciseType>
            typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename ImpreciseType>
            typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename ImpreciseType>
            typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename RationalType, typename ImpreciseType>
            static bool sharpen(storm::OptimizationDirection dir, uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x, std::vector<RationalType> const& b, std::vector<RationalType>& tmp);
            static bool isSolution(storm::OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b);

            void computeOptimalValueForRowGroup(uint_fast64_t group, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, uint_fast64_t* choice = nullptr) const;
                        
            struct ValueIterationResult {
                ValueIterationResult(uint64_t iterations, SolverStatus status) : iterations(iterations), status(status) {
                    // Intentionally left empty.
                }
                
                uint64_t iterations;
                SolverStatus status;
            };
            
            template <typename ValueTypePrime>
            friend class IterativeMinMaxLinearEquationSolver;
            
            ValueIterationResult performValueIteration(OptimizationDirection dir, std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations) const;
            
            void createLinearEquationSolver() const;
            
            // possibly cached data
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector2; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<uint64_t>> rowGroupOrdering; // A.rowGroupCount() entries
            
            SolverStatus updateStatusIfNotConverged(SolverStatus status, std::vector<ValueType> const& x, uint64_t iterations, SolverGuarantee const& guarantee) const;
            static void reportStatus(SolverStatus status, uint64_t iterations);
            
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
