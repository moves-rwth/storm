#ifndef STORM_SOLVER_FULLYSYMBOLICGAMESOLVER_H_
#define STORM_SOLVER_FULLYSYMBOLICGAMESOLVER_H_

#include "src/solver/SymbolicGameSolver.h"

namespace storm {
    namespace solver {
        
        /*!
         * A interface that represents an abstract symbolic game solver.
         */
        template<storm::dd::DdType Type>
        class FullySymbolicGameSolver : public SymbolicGameSolver<Type> {
        public:
            /*!
             * Constructs a symbolic game solver with the given meta variable sets and pairs.
             *
             * @param gameMatrix The matrix defining the coefficients of the game.
             * @param allRows A BDD characterizing all rows of the equation system.
             * @param rowMetaVariables The meta variables used to encode the rows of the matrix.
             * @param columnMetaVariables The meta variables used to encode the columns of the matrix.
             * @param rowColumnMetaVariablePairs The pairs of row meta variables and the corresponding column meta
             * variables.
             * @param player1Variables The meta variables used to encode the player 1 choices.
             * @param player2Variables The meta variables used to encode the player 2 choices.
             * @param precision The precision to achieve.
             * @param maximalNumberOfIterations The maximal number of iterations to perform when solving a linear
             * equation system iteratively.
             * @param relative Sets whether or not to use a relativ stopping criterion rather than an absolute one.
             */
            FullySymbolicGameSolver(storm::dd::Add<Type> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables);
            
            /*!
             * Constructs a symbolic game solver with the given meta variable sets and pairs.
             *
             * @param gameMatrix The matrix defining the coefficients of the game.
             * @param allRows A BDD characterizing all rows of the equation system.
             * @param rowMetaVariables The meta variables used to encode the rows of the matrix.
             * @param columnMetaVariables The meta variables used to encode the columns of the matrix.
             * @param rowColumnMetaVariablePairs The pairs of row meta variables and the corresponding column meta
             * variables.
             * @param player1Variables The meta variables used to encode the player 1 choices.
             * @param player2Variables The meta variables used to encode the player 2 choices.
             * @param precision The precision to achieve.
             * @param maximalNumberOfIterations The maximal number of iterations to perform when solving a linear
             * equation system iteratively.
             * @param relative Sets whether or not to use a relativ stopping criterion rather than an absolute one.
             */
            FullySymbolicGameSolver(storm::dd::Add<Type> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables, double precision, uint_fast64_t maximalNumberOfIterations, bool relative);
            
            virtual storm::dd::Add<Type> solveGame(bool player1Min, bool player2Min, storm::dd::Add<Type> const& x, storm::dd::Add<Type> const& b) const override;
            
        private:
            // The precision to achive.
            double precision;
            
            // The maximal number of iterations to perform.
            uint_fast64_t maximalNumberOfIterations;
            
            // A flag indicating whether a relative or an absolute stopping criterion is to be used.
            bool relative;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_FULLYSYMBOLICGAMESOLVER_H_ */
