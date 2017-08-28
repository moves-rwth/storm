#include "storm/solver/SymbolicNativeLinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/utility/dd.h"
#include "storm/utility/constants.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType>::SymbolicNativeLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(settings.getPrecision());
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<typename ValueType>
        void SymbolicNativeLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void SymbolicNativeLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void SymbolicNativeLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        ValueType SymbolicNativeLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        uint64_t SymbolicNativeLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        bool SymbolicNativeLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, SymbolicNativeLinearEquationSolverSettings<ValueType> const& settings) : SymbolicLinearEquationSolver<DdType, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs), settings(settings) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            storm::dd::DdManager<DdType>& manager = this->A.getDdManager();
            
            // Start by computing the Jacobi decomposition of the matrix A.
            storm::dd::Bdd<DdType> diagonal = storm::utility::dd::getRowColumnDiagonal(x.getDdManager(), this->rowColumnMetaVariablePairs);
            diagonal &= this->allRows;
            
            storm::dd::Add<DdType, ValueType> lu = diagonal.ite(manager.template getAddZero<ValueType>(), this->A);
            storm::dd::Add<DdType, ValueType> diagonalAdd = diagonal.template toAdd<ValueType>();
            storm::dd::Add<DdType, ValueType> diag = diagonalAdd.multiplyMatrix(this->A, this->columnMetaVariables);

            storm::dd::Add<DdType, ValueType> scaledLu = lu / diag;
            storm::dd::Add<DdType, ValueType> scaledB = b / diag;
            
            // Set up additional environment variables.
            storm::dd::Add<DdType, ValueType> xCopy = x;
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations()) {
                storm::dd::Add<DdType, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType, ValueType> tmp = scaledB - scaledLu.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                
                // Now check if the process already converged within our precision.
                converged = tmp.equalModuloPrecision(xCopy, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion());
                
                xCopy = tmp;
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterationCount << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterationCount << " iterations.");
            }
            
            return xCopy;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType> const& SymbolicNativeLinearEquationSolver<DdType, ValueType>::getSettings() const {
            return settings;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> SymbolicNativeLinearEquationSolverFactory<DdType, ValueType>::create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
            return std::make_unique<SymbolicNativeLinearEquationSolver<DdType, ValueType>>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, settings);
        }
    
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType>& SymbolicNativeLinearEquationSolverFactory<DdType, ValueType>::getSettings() {
            return settings;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType> const& SymbolicNativeLinearEquationSolverFactory<DdType, ValueType>::getSettings() const {
            return settings;
        }

        template class SymbolicNativeLinearEquationSolverSettings<double>;
        template class SymbolicNativeLinearEquationSolverSettings<storm::RationalNumber>;

        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::Sylvan, storm::RationalNumber>;

        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>;
        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::Sylvan, storm::RationalNumber>;
    }
}
