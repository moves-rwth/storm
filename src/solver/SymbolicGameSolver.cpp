#include "src/solver/SymbolicGameSolver.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/Add.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicGameSolver<Type, ValueType>::SymbolicGameSolver(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalPlayer1Mask, storm::dd::Bdd<Type> const& illegalPlayer2Mask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) : A(A), allRows(allRows), illegalPlayer1Mask(illegalPlayer1Mask.ite(A.getDdManager().getConstant(storm::utility::infinity<ValueType>()), A.getDdManager().template getAddZero<ValueType>())), illegalPlayer2Mask(illegalPlayer2Mask.ite(A.getDdManager().getConstant(storm::utility::infinity<ValueType>()), A.getDdManager().template getAddZero<ValueType>())), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables), generatePlayer1Strategy(false), generatePlayer2Strategy(false) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicGameSolver<Type, ValueType>::SymbolicGameSolver(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalPlayer1Mask, storm::dd::Bdd<Type> const& illegalPlayer2Mask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : AbstractGameSolver(precision, maximalNumberOfIterations, relative), A(A), allRows(allRows), illegalPlayer1Mask(illegalPlayer1Mask.template toAdd<ValueType>()), illegalPlayer2Mask(illegalPlayer2Mask.template toAdd<ValueType>()), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables), generatePlayer1Strategy(false), generatePlayer2Strategy(false) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> SymbolicGameSolver<Type, ValueType>::solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, storm::dd::Add<Type, ValueType> const& x, storm::dd::Add<Type, ValueType> const& b) const {
            // Set up the environment.
            storm::dd::Add<Type, ValueType> xCopy = x;
            uint_fast64_t iterations = 0;
            bool converged = false;
                        
            do {
                // Compute tmp = A * x + b.
                storm::dd::Add<Type, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<Type, ValueType> tmp = this->A.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp += b;
                
                // Now abstract from player 2 and player 1 variables.
                if (player2Goal == storm::OptimizationDirection::Maximize) {
                    tmp = tmp.maxAbstract(this->player2Variables);
                } else {
                    tmp = (tmp + illegalPlayer2Mask).minAbstract(this->player2Variables);
                }

                if (player1Goal == storm::OptimizationDirection::Maximize) {
                    tmp = tmp.maxAbstract(this->player1Variables);
                } else {
                    tmp = (tmp + illegalPlayer1Mask).minAbstract(this->player1Variables);
                }

                // Now check if the process already converged within our precision.
                converged = xCopy.equalModuloPrecision(tmp, precision, relative);
                
                // If the method did not converge yet, we prepare the x vector for the next iteration.
                if (!converged) {
                    xCopy = tmp;
                }
                
                ++iterations;
            } while (!converged && iterations < maximalNumberOfIterations);
            
            return xCopy;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void SymbolicGameSolver<Type, ValueType>::setGeneratePlayer1Strategy(bool value) {
            generatePlayer1Strategy = value;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void SymbolicGameSolver<Type, ValueType>::setGeneratePlayer2Strategy(bool value) {
            generatePlayer2Strategy = value;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void SymbolicGameSolver<Type, ValueType>::setGeneratePlayerStrategies(bool value) {
            setGeneratePlayer1Strategy(value);
            setGeneratePlayer2Strategy(value);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> const& SymbolicGameSolver<Type, ValueType>::getPlayer1Strategy() const {
            STORM_LOG_THROW(player1Strategy, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 1 strategy because none was generated.");
            return player1Strategy.get();
        }

        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> const& SymbolicGameSolver<Type, ValueType>::getPlayer2Strategy() const {
            STORM_LOG_THROW(player2Strategy, storm::exceptions::IllegalFunctionCallException, "Cannot retrieve player 2 strategy because none was generated.");
            return player2Strategy.get();
        }

        
        template class SymbolicGameSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicGameSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}