#include "src/solver/SymbolicMinMaxLinearEquationSolver.h"

#include "src/storage/dd/DdManager.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/utility/constants.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

namespace storm {
    namespace solver {
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::SymbolicMinMaxLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : A(A), allRows(allRows), illegalMaskAdd(illegalMask.ite(A.getDdManager().getConstant(storm::utility::infinity<ValueType>()), A.getDdManager().template getAddZero<ValueType>())), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), choiceVariables(choiceVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), relative(relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::SymbolicMinMaxLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) : A(A), allRows(allRows), illegalMaskAdd(illegalMask.ite(A.getDdManager().getConstant(storm::utility::infinity<ValueType>()), A.getDdManager().template getAddZero<ValueType>())), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), choiceVariables(choiceVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs) {
            // Get the settings object to customize solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquationSystem(bool minimize, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            // Set up the environment.
            storm::dd::Add<DdType, ValueType> xCopy = x;
            uint_fast64_t iterations = 0;
            bool converged = false;
            
            while (!converged && iterations < maximalNumberOfIterations) {
                // Compute tmp = A * x + b
                storm::dd::Add<DdType, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType, ValueType> tmp = this->A.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp += b;
                
                if (minimize) {
                    // This is a hack and only here because of the lack of a suitable minAbstract/maxAbstract function
                    // that can properly deal with a restriction of the choices.
                    tmp += illegalMaskAdd;
                    tmp = tmp.minAbstract(this->choiceVariables);
                } else {
                    tmp = tmp.maxAbstract(this->choiceVariables);
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
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::performMatrixVectorMultiplication(bool minimize, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b, uint_fast64_t n) const {
            storm::dd::Add<DdType, ValueType> xCopy = x;
            
            // Perform matrix-vector multiplication while the bound is met.
            for (uint_fast64_t i = 0; i < n; ++i) {
                xCopy = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                xCopy = this->A.multiplyMatrix(xCopy, this->columnMetaVariables);
                if (b != nullptr) {
                    xCopy += *b;
                }
                
                if (minimize) {
                    // This is a hack and only here because of the lack of a suitable minAbstract/maxAbstract function
                    // that can properly deal with a restriction of the choices.
                    xCopy += illegalMaskAdd;
                    xCopy = xCopy.minAbstract(this->choiceVariables);
                } else {
                    xCopy = xCopy.maxAbstract(this->choiceVariables);
                }
            }
            
            return xCopy;
        }
        
        template class SymbolicMinMaxLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicMinMaxLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}
