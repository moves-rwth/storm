#include "src/solver/SymbolicLinearEquationSolver.h"

#include "src/storage/dd/cudd/CuddDdManager.h"
#include "src/storage/dd/cudd/CuddAdd.h"

#include "src/storage/dd/Add.h"


#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicLinearEquationSolver<DdType, ValueType>::SymbolicLinearEquationSolver(storm::dd::Add<DdType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : A(A), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicLinearEquationSolver<DdType, ValueType>::SymbolicLinearEquationSolver(storm::dd::Add<DdType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) : A(A), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs) {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType>  SymbolicLinearEquationSolver<DdType, ValueType>::solveEquationSystem(storm::dd::Add<DdType> const& x, storm::dd::Add<DdType> const& b) const {
            // Start by computing the Jacobi decomposition of the matrix A.
            storm::dd::Add<DdType> diagonal = x.getDdManager()->getAddOne();
            for (auto const& pair : rowColumnMetaVariablePairs) {
                diagonal *= x.getDdManager()->getIdentity(pair.first).equals(x.getDdManager()->getIdentity(pair.second));
                diagonal *= x.getDdManager()->getRange(pair.first).toAdd() * x.getDdManager()->getRange(pair.second).toAdd();
            }
            diagonal *= allRows.toAdd();
            
            storm::dd::Add<DdType> lu = diagonal.ite(this->A.getDdManager()->getAddZero(), this->A);
            storm::dd::Add<DdType> dinv = diagonal / (diagonal * this->A);
            
            // Set up additional environment variables.
            storm::dd::Add<DdType> xCopy = x;
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < maximalNumberOfIterations) {
                storm::dd::Add<DdType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType> tmp = lu.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
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
        storm::dd::Add<DdType> SymbolicLinearEquationSolver<DdType, ValueType>::performMatrixVectorMultiplication(storm::dd::Add<DdType> const& x, storm::dd::Add<DdType> const* b, uint_fast64_t n) const {
            storm::dd::Add<DdType> xCopy = x;
            
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
        
    }
}
