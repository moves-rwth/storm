#include "storm/solver/SymbolicGameSolver.h"

#include "storm/storage/dd/Bdd.h"
#include "storm/storage/dd/Add.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicGameSolver<Type, ValueType>::SymbolicGameSolver(storm::dd::Add<Type, ValueType> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) : gameMatrix(gameMatrix), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicGameSolver<Type, ValueType>::SymbolicGameSolver(storm::dd::Add<Type, ValueType> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : AbstractGameSolver<ValueType>(precision, maximalNumberOfIterations, relative), gameMatrix(gameMatrix), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> SymbolicGameSolver<Type, ValueType>::solveGame(OptimizationDirection player1Goal, OptimizationDirection player2Goal, storm::dd::Add<Type, ValueType> const& x, storm::dd::Add<Type, ValueType> const& b) const {
            // Set up the environment.
            storm::dd::Add<Type, ValueType> xCopy = x;
            uint_fast64_t iterations = 0;
            bool converged = false;
            
            do {
                // Compute tmp = A * x + b
                storm::dd::Add<Type, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<Type, ValueType> tmp = this->gameMatrix.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp += b;
                
                // Now abstract from player 2 and player 1 variables.
                switch (player2Goal) {
                    case OptimizationDirection::Minimize: tmp = tmp.minAbstract(this->player2Variables); break;
                    case OptimizationDirection::Maximize: tmp = tmp.maxAbstract(this->player2Variables); break;
                }
                
                switch (player1Goal) {
                    case OptimizationDirection::Minimize: tmp = tmp.minAbstract(this->player1Variables); break;
                    case OptimizationDirection::Maximize: tmp = tmp.maxAbstract(this->player1Variables); break;
                }
                
                // Now check if the process already converged within our precision.
                converged = xCopy.equalModuloPrecision(tmp, this->precision, this->relative);
                
                // If the method did not converge yet, we prepare the x vector for the next iteration.
                if (!converged) {
                    xCopy = tmp;
                }
                
                ++iterations;
            } while (!converged && iterations < this->maximalNumberOfIterations);
            
            return xCopy;
        }
        
        template class SymbolicGameSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicGameSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}
