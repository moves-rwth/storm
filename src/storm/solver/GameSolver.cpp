#include "storm/solver/GameSolver.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/StandardGameSolver.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GameSolver<ValueType>::GameSolver() : trackSchedulers(false), cachingEnabled(false) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setTrackSchedulers(bool value) {
            trackSchedulers = value;
            if (!trackSchedulers) {
                player1Scheduler = boost::none;
                player2Scheduler = boost::none;
            }
        }
        
        template<typename ValueType>
        bool GameSolver<ValueType>::isTrackSchedulersSet() const {
            return trackSchedulers;
        }
        
        template<typename ValueType>
        bool GameSolver<ValueType>::hasSchedulers() const {
            return player1Scheduler.is_initialized() && player2Scheduler.is_initialized();
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& GameSolver<ValueType>::getPlayer1Scheduler() const {
            STORM_LOG_THROW(player1Scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 1 scheduler, because none was generated.");
            return *player1Scheduler.get();
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& GameSolver<ValueType>::getPlayer2Scheduler() const {
            STORM_LOG_THROW(player2Scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 2 scheduler, because none was generated.");
            return *player2Scheduler.get();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::storage::TotalScheduler> GameSolver<ValueType>::getPlayer1Scheduler() {
            STORM_LOG_THROW(player1Scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 1 scheduler, because none was generated.");
            return std::move(player1Scheduler.get());
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::storage::TotalScheduler> GameSolver<ValueType>::getPlayer2Scheduler() {
            STORM_LOG_THROW(player2Scheduler, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 2 scheduler, because none was generated.");
            return std::move(player2Scheduler.get());
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setSchedulerHints(storm::storage::TotalScheduler&& player1Scheduler, storm::storage::TotalScheduler&& player2Scheduler) {
            this->player1SchedulerHint = std::move(player1Scheduler);
            this->player2SchedulerHint = std::move(player2Scheduler);
        }

        template<typename ValueType>
        bool GameSolver<ValueType>::hasSchedulerHints() const {
            return player1SchedulerHint.is_initialized() && player2SchedulerHint.is_initialized();
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setCachingEnabled(bool value) {
            if(cachingEnabled && !value) {
                // caching will be turned off. Hence we clear the cache at this point
                clearCache();
            }
            cachingEnabled = value;
        }
        
        template<typename ValueType>
        bool GameSolver<ValueType>::isCachingEnabled() const {
            return cachingEnabled;
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::clearCache() const {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setLowerBound(ValueType const& value) {
            lowerBound = value;
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setUpperBound(ValueType const& value) {
            upperBound = value;
        }
        
        template<typename ValueType>
        void GameSolver<ValueType>::setBounds(ValueType const& lower, ValueType const& upper) {
            setLowerBound(lower);
            setUpperBound(upper);
        }
        
        template<typename ValueType>
        GameSolverFactory<ValueType>::GameSolverFactory() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        std::unique_ptr<GameSolver<ValueType>> GameSolverFactory<ValueType>::create(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) const {
            return std::make_unique<StandardGameSolver<ValueType>>(player1Matrix, player2Matrix, std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>());
        }
                
        template<typename ValueType>
        std::unique_ptr<GameSolver<ValueType>> GameSolverFactory<ValueType>::create(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix, storm::storage::SparseMatrix<ValueType>&& player2Matrix) const {
            return std::make_unique<StandardGameSolver<ValueType>>(std::move(player1Matrix), std::move(player2Matrix), std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>());
        }
        
        template class GameSolver<double>;
        template class GameSolver<storm::RationalNumber>;
        
        template class GameSolverFactory<double>;
        template class GameSolverFactory<storm::RationalNumber>;
    }
}
