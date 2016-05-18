#include "src/solver/SymbolicLinearEquationSolver.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicLinearEquationSolver<DdType, ValueType>::SymbolicLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : A(A), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicLinearEquationSolver<DdType, ValueType>::SymbolicLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) : A(A), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs) {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicLinearEquationSolver<DdType, ValueType>::solveEquationSystem(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            // Start by computing the Jacobi decomposition of the matrix A.
            storm::dd::Bdd<DdType> diagonal = x.getDdManager().getBddOne();
            for (auto const& pair : rowColumnMetaVariablePairs) {
                diagonal &= x.getDdManager().template getIdentity<ValueType>(pair.first).equals(x.getDdManager().template getIdentity<ValueType>(pair.second));
                diagonal &= x.getDdManager().getRange(pair.first) && x.getDdManager().getRange(pair.second);
            }
            diagonal &= allRows;
            
            storm::dd::Add<DdType, ValueType> lu = diagonal.ite(this->A.getDdManager().template getAddZero<ValueType>(), this->A);
            storm::dd::Add<DdType> diagonalAdd = diagonal.template toAdd<ValueType>();
            storm::dd::Add<DdType, ValueType> dinv = diagonalAdd / (diagonalAdd * this->A);
            
            // Set up additional environment variables.
            storm::dd::Add<DdType, ValueType> xCopy = x;
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < maximalNumberOfIterations) {
                storm::dd::Add<DdType, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);

                storm::dd::Add<DdType, ValueType> tmp = lu.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp = b - tmp;
                tmp = tmp.swapVariables(this->rowColumnMetaVariablePairs);
                tmp = dinv.multiplyMatrix(tmp, this->columnMetaVariables);
                
                // Now check if the process already converged within our precision.
                converged = xCopy.equalModuloPrecision(tmp, precision, relative);
                
                // If the method did not converge yet, we prepare the x vector for the next iteration.
                if (!converged) {
                    xCopy = tmp;
                }
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
                        
            return xCopy;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicLinearEquationSolver<DdType, ValueType>::performMatrixVectorMultiplication(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b, uint_fast64_t n) const {
            storm::dd::Add<DdType, ValueType> xCopy = x;
            
            // Perform matrix-vector multiplication while the bound is met.
            for (uint_fast64_t i = 0; i < n; ++i) {
                xCopy = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                xCopy = this->A.multiplyMatrix(xCopy, this->columnMetaVariables);
                if (b != nullptr) {
                    xCopy += *b;
                }
            }
            
            return xCopy;
        }
        
        template class SymbolicLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}
