#pragma once

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/solver/AbstractEquationSolver.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/storage/Scheduler.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace storage {
        template<typename T> class SparseMatrix;
    }
    
    namespace solver {
        
        /*!
         * A class representing the interface that all game solvers shall implement.
         */
        template<class ValueType>
        class GameSolver : public AbstractEquationSolver<ValueType> {
        public:
            
            virtual ~GameSolver() = default;

            /*!
             * Solves the equation system defined by the game matrices. Note that the game matrices have to be given upon
             * construction time of the solver object.
             *
             * @param player1Dir Sets whether player 1 wants to minimize or maximize.
             * @param player2Dir Sets whether player 2 wants to minimize or maximize.
             * @param x The initial guess of the solution. For correctness, the guess has to be less (or equal) to the final solution (unless both players minimize)
             * @param b The vector to add after matrix-vector multiplication.
             */
            virtual bool solveGame(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const = 0;
            
            /*!
             * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes
             * x[i+1] = min/max(player1Matrix*(min/max(player2Matrix*x[i] + b))) until x[n], where x[0] = x. After each multiplication and addition, the
             * minimal/maximal value out of each row group is selected to reduce the resulting vector to obtain the
             * vector for the next iteration. Note that the player1Matrix and the player2Matrix has to be given upon construction time of the
             * solver object.
             *
             * @param player1Dir Sets whether player 1 wants to minimize or maximize.
             * @param player2Dir Sets whether player 2 wants to minimize or maximize.
             * @param x The initial vector that is to be multiplied with the matrix. This is also the output parameter,
             * i.e. after the method returns, this vector will contain the computed values.
             * @param b If not null, this vector is added after each multiplication.
             * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
             */
            virtual void repeatedMultiply(OptimizationDirection player1Dir, OptimizationDirection player2Dir, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1) const = 0;
            
            /*!
             * Sets whether schedulers are generated when solving equation systems. If the argument is false, the currently
             * stored schedulers (if any) are deleted.
             */
            void setTrackSchedulers(bool value = true);

            /*!
             * Retrieves whether this solver is set to generate schedulers.
             */
            bool isTrackSchedulersSet() const;

            /*!
             * Retrieves whether the solver generated a scheduler.
             */
            bool hasSchedulers() const;

            /*!
             * Retrieves the generated scheduler. Note: it is only legal to call this function if schedulers were generated.
             */
            storm::storage::Scheduler<ValueType> computePlayer1Scheduler() const;
            storm::storage::Scheduler<ValueType> computePlayer2Scheduler() const;
            
            /*!
             * Retrieves the generated (deterministic) choices of the optimal scheduler. Note: it is only legal to call this function if schedulers were generated.
             */
            std::vector<uint_fast64_t> const& getPlayer1SchedulerChoices() const;
            std::vector<uint_fast64_t> const& getPlayer2SchedulerChoices() const;

            /*!
             * Sets scheduler hints that might be considered by the solver as an initial guess
             */
            void setSchedulerHints(std::vector<uint_fast64_t>&& player1Choices, std::vector<uint_fast64_t>&& player2Choices);
            
            /*!
             * Returns whether Scheduler hints are available
             */
            bool hasSchedulerHints() const;
            
            /**
             * Gets the precision after which the solver takes two numbers as equal.
             *
             * @see getRelative()
             */
            virtual ValueType getPrecision() const = 0;

            /**
             *  Gets whether the precision is taken to be absolute or relative
             */
            virtual bool getRelative() const = 0;
            
            /*!
             * Sets whether some of the generated data during solver calls should be cached.
             * This possibly decreases the runtime of subsequent calls but also increases memory consumption.
             */
            void setCachingEnabled(bool value);
            
            /*!
             * Retrieves whether some of the generated data during solver calls should be cached.
             */
            bool isCachingEnabled() const;

            /*
             * Clears the currently cached data that has been stored during previous calls of the solver.
             */
            virtual void clearCache() const;

            /*!
             * Sets a lower bound for the solution that can potentially used by the solver.
             */
            void setLowerBound(ValueType const& value);
            
            /*!
             * Sets an upper bound for the solution that can potentially used by the solver.
             */
            void setUpperBound(ValueType const& value);
            
            /*!
             * Sets bounds for the solution that can potentially used by the solver.
             */
            void setBounds(ValueType const& lower, ValueType const& upper);

        protected:
            
            GameSolver();
            
            /// Whether we generate schedulers during solving.
            bool trackSchedulers;

            /// The scheduler choices that induce the optimal values (if they could be successfully generated).
            mutable boost::optional<std::vector<uint_fast64_t>> player1SchedulerChoices;
            mutable boost::optional<std::vector<uint_fast64_t>> player2SchedulerChoices;
            
            // A lower bound if one was set.
            boost::optional<ValueType> lowerBound;
            
            // An upper bound if one was set.
            boost::optional<ValueType> upperBound;
            
            // scheduler choices that might be considered by the solver as an initial guess
            boost::optional<std::vector<uint_fast64_t>> player1ChoicesHint;
            boost::optional<std::vector<uint_fast64_t>> player2ChoicesHint;
            
        private:
            /// Whether some of the generated data during solver calls should be cached.
            bool cachingEnabled;
            
        };

        template<typename ValueType>
        class GameSolverFactory {
        public:
            GameSolverFactory();
            virtual ~GameSolverFactory() = default;

            virtual std::unique_ptr<GameSolver<ValueType>> create(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) const;
            virtual std::unique_ptr<GameSolver<ValueType>> create(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix, storm::storage::SparseMatrix<ValueType>&& player2Matrix) const;

        private:
            bool trackScheduler;

        };
        
    } // namespace solver
} // namespace storm

