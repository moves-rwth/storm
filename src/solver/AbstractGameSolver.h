#ifndef STORM_SOLVER_ABSTRACTGAMESOLVER_H_
#define STORM_SOLVER_ABSTRACTGAMESOLVER_H_

#include <cstdint>
#include "src/storage/sparse/StateType.h"
#include "src/utility/vector.h"

namespace storm {
    namespace solver {
        /*!
         * The abstract base class for the game solvers.
         */
        class AbstractGameSolver {
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
            
            void setPolicyTracking(bool setToTrue=true);
            
            std::vector<storm::storage::sparse::state_type> getPlayer1Policy() const;
            std::vector<storm::storage::sparse::state_type> getPlayer2Policy() const;
            double getPrecision() const;
            bool getRelative() const;

        protected:
            // The precision to achieve.
            double precision;

            // The maximal number of iterations to perform.
            uint_fast64_t maximalNumberOfIterations;

            // A flag indicating whether a relative or an absolute stopping criterion is to be used.
            bool relative;
            
            // Whether we track the policies we generate.
            bool trackPolicies;
            
            // The policies for the different players
            mutable std::vector<storm::storage::sparse::state_type> player1Policy;
            mutable std::vector<storm::storage::sparse::state_type> player2Policy;
            
        };
    }
}

#endif /* STORM_SOLVER_ABSTRACTGAMESOLVER_H_ */
