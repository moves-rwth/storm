#pragma once

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/GameSolver.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        class StandardGameSolverSettings {
        public:
            StandardGameSolverSettings();
            
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
        class StandardGameSolver : public GameSolver<ValueType> {
        public:
            StandardGameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardGameSolverSettings<ValueType> const& settings = StandardGameSolverSettings<ValueType>());
            
            StandardGameSolver(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix, storm::storage::SparseMatrix<ValueType>&& player2Matrix, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardGameSolverSettings<ValueType> const& settings = StandardGameSolverSettings<ValueType>());
            
            virtual bool solveGame(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;
            virtual void repeatedMultiply(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1) const override;

            StandardGameSolverSettings<ValueType> const& getSettings() const;
            void setSettings(StandardGameSolverSettings<ValueType> const& newSettings);
            
            virtual void clearCache() const override;

            virtual ValueType getPrecision() const override;
            virtual bool getRelative() const override;
            
        private:
            bool solveGamePolicyIteration(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveGameValueIteration(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

            // Computes p2Matrix * x + b, reduces the result w.r.t. player 2 choices, and then reduces the result w.r.t. player 1 choices.
            void multiplyAndReduce(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b,
                                   storm::solver::LinearEquationSolver<ValueType> const& linEqSolver, std::vector<ValueType>& multiplyResult, std::vector<ValueType>& p2ReducedMultiplyResult, std::vector<ValueType>& p1ReducedMultiplyResult) const;
            
            // Solves the equation system given by the two choice selections
            void getInducedMatrixVector(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<uint_fast64_t> const& player1Choices, std::vector<uint_fast64_t> const& player2Choices, storm::storage::SparseMatrix<ValueType>& inducedMatrix, std::vector<ValueType>& inducedVector) const;
            
            // Extracts the choices of the different players for the given solution x.
            // Returns true iff the newly extracted choices yield "better" values then the given choices for one of the players.
            bool extractChoices(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType> const& x, std::vector<ValueType> const& b, std::vector<ValueType>& player2ChoiceValues, std::vector<uint_fast64_t>& player1Choices, std::vector<uint_fast64_t>& player2Choices) const;
            
            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;
            
            enum class Status {
                Converged, TerminatedEarly, MaximalIterationsExceeded, InProgress
            };
            
            // possibly cached data
            mutable std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> linEqSolverPlayer2Matrix;
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryP2RowVector; // player2Matrix.rowCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryP2RowGroupVector; // player2Matrix.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryP1RowGroupVector; // player1Matrix.rowGroupCount() entries

            Status updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const;
            void reportStatus(Status status, uint64_t iterations) const;
            
            /// The settings of this solver.
            StandardGameSolverSettings<ValueType> settings;
            
            /// The factory used to obtain linear equation solvers.
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<storm::storage::sparse::state_type>> localP1Matrix;
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localP2Matrix;
            
            // A reference to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the reference refers to localA.
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix;
            storm::storage::SparseMatrix<ValueType> const& player2Matrix;
            
        };
    }
}
