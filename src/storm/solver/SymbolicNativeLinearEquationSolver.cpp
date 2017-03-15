#include "storm/solver/SymbolicNativeLinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/utility/dd.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : SymbolicLinearEquationSolver<DdType, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, precision, maximalNumberOfIterations, relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) : SymbolicLinearEquationSolver<DdType, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            // Start by computing the Jacobi decomposition of the matrix A.
            storm::dd::Bdd<DdType> diagonal = storm::utility::dd::getRowColumnDiagonal(x.getDdManager(), this->rowColumnMetaVariablePairs);
            diagonal &= this->allRows;
            
            storm::dd::Add<DdType, ValueType> lu = diagonal.ite(this->A.getDdManager().template getAddZero<ValueType>(), this->A);
            storm::dd::Add<DdType> diagonalAdd = diagonal.template toAdd<ValueType>();
            storm::dd::Add<DdType, ValueType> diag = diagonalAdd.multiplyMatrix(this->A, this->columnMetaVariables);
            
            storm::dd::Add<DdType, ValueType> scaledLu = lu / diag;
            storm::dd::Add<DdType, ValueType> scaledB = b / diag;
            
            // Set up additional environment variables.
            storm::dd::Add<DdType, ValueType> xCopy = x;
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->maximalNumberOfIterations) {
                storm::dd::Add<DdType, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType, ValueType> tmp = scaledB - scaledLu.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                
                // Now check if the process already converged within our precision.
                converged = tmp.equalModuloPrecision(xCopy, this->precision, this->relative);
                
                xCopy = tmp;
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            if (converged) {
                STORM_LOG_TRACE("Iterative solver converged in " << iterationCount << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterationCount << " iterations.");
            }
            
            return xCopy;
        }
        
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}
