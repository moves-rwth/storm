#include "storm/solver/SymbolicLinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/utility/dd.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/adapters/CarlAdapter.h"

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
        storm::dd::Add<DdType, ValueType> SymbolicLinearEquationSolver<DdType, ValueType>::multiply(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b, uint_fast64_t n) const {
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
        
        template class SymbolicLinearEquationSolver<storm::dd::DdType::Sylvan, storm::RationalFunction>;
        
    }
}
