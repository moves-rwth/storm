#ifndef STORM_SOLVER_ABSTRACTGAMESOLVER_H_
#define STORM_SOLVER_ABSTRACTGAMESOLVER_H_

#include <cstdint>
#include <memory>

#include <boost/optional.hpp>

#include "src/storage/sparse/StateType.h"
#include "src/utility/vector.h"
#include "src/solver/AbstractEquationSolver.h"
#include "src/storage/TotalScheduler.h"

namespace storm {
    namespace solver {
        /*!
         * The abstract base class for the game solvers.
         */
        template< typename ValueType>
        class AbstractGameSolver : public AbstractEquationSolver<ValueType>{
        public:
            /*!
             * Creates a game solver with the default parameters.
             */
            AbstractGameSolver();

            /*!
             * Creates a game solver with the given parameters.
             *
             * @param precision The precision to achieve.
             * @param maximalNumberOfIterations The maximal number of iterations to perform.
             * @param relative A flag indicating whether a relative or an absolute stopping criterion is to be used.
             */
            AbstractGameSolver(double precision, uint_fast64_t maximalNumberOfIterations, bool relative);
            
            void setTrackScheduler(bool trackScheduler = true);
            bool isTrackSchedulerSet() const;
            bool hasScheduler() const;
            
            storm::storage::TotalScheduler const& getPlayer1Scheduler() const;
            storm::storage::TotalScheduler const& getPlayer2Scheduler() const;
            
            double getPrecision() const;
            bool getRelative() const;

        protected:
            // The precision to achieve.
            double precision;

            // The maximal number of iterations to perform.
            uint_fast64_t maximalNumberOfIterations;

            // A flag indicating whether a relative or an absolute stopping criterion is to be used.
            bool relative;
            
            // Whether we generate a scheduler during solving.
            bool trackScheduler;
            
            // The scheduler for the corresponding players (if it could be successfully generated).
            mutable boost::optional<std::unique_ptr<storm::storage::TotalScheduler>> player1Scheduler;
            mutable boost::optional<std::unique_ptr<storm::storage::TotalScheduler>> player2Scheduler;
        };
    }
}

#endif /* STORM_SOLVER_ABSTRACTGAMESOLVER_H_ */
