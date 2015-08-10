#include "src/solver/SymbolicGameSolver.h"

#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddAdd.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType Type>
        SymbolicGameSolver<Type>::SymbolicGameSolver(storm::dd::Add<Type> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) : gameMatrix(gameMatrix), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables) {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<storm::dd::DdType Type>
        SymbolicGameSolver<Type>::SymbolicGameSolver(storm::dd::Add<Type> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : gameMatrix(gameMatrix), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Add<Type> SymbolicGameSolver<Type>::solveGame(bool player1Min, bool player2Min, storm::dd::Add<Type> const& x, storm::dd::Add<Type> const& b) const {
            // Set up the environment.
            storm::dd::Add<Type> xCopy = x;
            uint_fast64_t iterations = 0;
            bool converged = false;
            
            while (!converged && iterations < maximalNumberOfIterations) {
                // Compute tmp = A * x + b
                storm::dd::Add<Type> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<Type> tmp = this->gameMatrix.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp += b;
                
                // Now abstract from player 2 and player 1 variables.
                if (player2Min) {
                    tmp = tmp.minAbstract(this->player2Variables);
                } else {
                    tmp = tmp.maxAbstract(this->player2Variables);
                }
                
                if (player1Min) {
                    tmp = tmp.minAbstract(this->player1Variables);
                } else {
                    tmp = tmp.maxAbstract(this->player1Variables);
                }
                
                // Now check if the process already converged within our precision.
                converged = xCopy.equalModuloPrecision(tmp, precision, relative);
                
                // If the method did not converge yet, we prepare the x vector for the next iteration.
                if (!converged) {
                    xCopy = tmp;
                }
                
                ++iterations;
            }
            
            return xCopy;
        }
        
        template class SymbolicGameSolver<storm::dd::DdType::CUDD>;
        
    }
}