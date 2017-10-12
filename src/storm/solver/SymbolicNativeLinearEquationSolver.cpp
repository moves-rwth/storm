#include "storm/solver/SymbolicNativeLinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/utility/dd.h"
#include "storm/utility/constants.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/utility/KwekMehlhorn.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType>::SymbolicNativeLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            // Get appropriate settings.
            switch (settings.getLinearEquationSystemMethod()) {
                case storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Power:
                    this->method = SolutionMethod::Power;
                    break;
                case storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::RationalSearch:
                    this->method = SolutionMethod::RationalSearch;
                    break;
                case storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi:
                default:
                    this->method = SolutionMethod::Jacobi;
                    break;
            }

            // Adjust the method if none was specified and we are using rational numbers.
            if (std::is_same<ValueType, storm::RationalNumber>::value) {
                if (settings.isLinearEquationSystemTechniqueSetFromDefaultValue() && this->method != SolutionMethod::RationalSearch) {
                    STORM_LOG_INFO("Selecting 'rational search' as the solution technique to guarantee exact results. If you want to override this, please explicitly specify a different method.");
                    this->method = SolutionMethod::RationalSearch;
                }
            }
            
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
        void SymbolicNativeLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
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

        template<typename ValueType>
        typename SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod SymbolicNativeLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return this->method;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(SymbolicNativeLinearEquationSolverSettings<ValueType> const& settings) : SymbolicLinearEquationSolver<DdType, ValueType>(), settings(settings) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, SymbolicNativeLinearEquationSolverSettings<ValueType> const& settings) : SymbolicNativeLinearEquationSolver(allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, settings) {
            this->setMatrix(A);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolver<DdType, ValueType>::SymbolicNativeLinearEquationSolver(storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, SymbolicNativeLinearEquationSolverSettings<ValueType> const& settings) : SymbolicLinearEquationSolver<DdType, ValueType>(allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs), settings(settings) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            if (this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                return solveEquationsJacobi(x, b);
            } else if (this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power) {
                return solveEquationsPower(x, b);
            } else if (this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch) {
                return solveEquationsRationalSearch(x, b);
            }
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The selected solution technique is not supported.");
        }

        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsJacobi(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            storm::dd::DdManager<DdType>& manager = this->getDdManager();
            
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
                STORM_LOG_INFO("Iterative solver (jacobi) converged in " << iterationCount << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver (jacobi) did not converge in " << iterationCount << " iterations.");
            }
            
            return xCopy;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        typename SymbolicNativeLinearEquationSolver<DdType, ValueType>::PowerIterationResult SymbolicNativeLinearEquationSolver<DdType, ValueType>::performPowerIteration(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b, ValueType const& precision, bool relativeTerminationCriterion, uint64_t maximalIterations) const {
            
            // Set up additional environment variables.
            storm::dd::Add<DdType, ValueType> currentX = x;
            uint_fast64_t iterations = 0;
            SolverStatus status = SolverStatus::InProgress;
            
            while (status == SolverStatus::InProgress && iterations < maximalIterations) {
                storm::dd::Add<DdType, ValueType> currentXAsColumn = currentX.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType, ValueType> tmp = this->A.multiplyMatrix(currentXAsColumn, this->columnMetaVariables) + b;
                
                // Now check if the process already converged within our precision.
                if (tmp.equalModuloPrecision(currentX, precision, relativeTerminationCriterion)) {
                    status = SolverStatus::Converged;
                }
                
                // Set up next iteration.
                ++iterations;
                currentX = tmp;
            }

            return PowerIterationResult(status, iterations, currentX);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsPower(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            PowerIterationResult result = performPowerIteration(this->getLowerBoundsVector(), b, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion(), this->getSettings().getMaximalNumberOfIterations());
            
            if (result.status == SolverStatus::Converged) {
                STORM_LOG_INFO("Iterative solver (power iteration) converged in " << result.iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver (power iteration) did not converge in " << result.iterations << " iterations.");
            }
            
            return result.values;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        bool SymbolicNativeLinearEquationSolver<DdType, ValueType>::isSolutionFixedPoint(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            storm::dd::Add<DdType, ValueType> xAsColumn = x.swapVariables(this->rowColumnMetaVariablePairs);
            storm::dd::Add<DdType, ValueType> tmp = this->A.multiplyMatrix(xAsColumn, this->columnMetaVariables);
            tmp += b;

            return x == tmp;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        storm::dd::Add<DdType, RationalType> SymbolicNativeLinearEquationSolver<DdType, ValueType>::sharpen(uint64_t precision, SymbolicNativeLinearEquationSolver<DdType, RationalType> const& rationalSolver, storm::dd::Add<DdType, ImpreciseType> const& x, storm::dd::Add<DdType, RationalType> const& rationalB, bool& isSolution) {
            
            storm::dd::Add<DdType, RationalType> sharpenedX;
            
            for (uint64_t p = 1; p < precision; ++p) {
                sharpenedX = x.sharpenKwekMehlhorn(precision);
                isSolution = rationalSolver.isSolutionFixedPoint(sharpenedX, rationalB);
                
                if (isSolution) {
                    break;
                }
            }
            
            return sharpenedX;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        storm::dd::Add<DdType, RationalType> SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsRationalSearchHelper(SymbolicNativeLinearEquationSolver<DdType, RationalType> const& rationalSolver, SymbolicNativeLinearEquationSolver<DdType, ImpreciseType> const& impreciseSolver, storm::dd::Add<DdType, RationalType> const& rationalB, storm::dd::Add<DdType, ImpreciseType> const& x, storm::dd::Add<DdType, ImpreciseType> const& b) const {
            
            // Storage for the rational sharpened vector.
            storm::dd::Add<DdType, RationalType> sharpenedX;
            
            // The actual rational search.
            uint64_t overallIterations = 0;
            uint64_t powerIterationInvocations = 0;
            ValueType precision = this->getSettings().getPrecision();
            SolverStatus status = SolverStatus::InProgress;
            while (status == SolverStatus::InProgress && overallIterations < this->getSettings().getMaximalNumberOfIterations()) {
                typename SymbolicNativeLinearEquationSolver<DdType, ImpreciseType>::PowerIterationResult result = impreciseSolver.performPowerIteration(x, b, storm::utility::convertNumber<ImpreciseType, ValueType>(precision), this->getSettings().getRelativeTerminationCriterion(), this->settings.getMaximalNumberOfIterations());
                
                ++powerIterationInvocations;
                STORM_LOG_TRACE("Completed " << powerIterationInvocations << " power iteration invocations, the last one with precision " << precision << " completed in " << result.iterations << " iterations.");
                
                // Count the iterations.
                overallIterations += result.iterations;
                
                // Compute maximal precision until which to sharpen.
                uint64_t p = storm::utility::convertNumber<uint64_t>(storm::utility::ceil(storm::utility::log10<ValueType>(storm::utility::one<ValueType>() / precision)));
                
                bool isSolution = false;
                sharpenedX = sharpen<RationalType, ImpreciseType>(p, rationalSolver, result.values, rationalB, isSolution);
                
                if (isSolution) {
                    status = SolverStatus::Converged;
                } else {
                    precision = precision / 100;
                }
            }
            
            if (status == SolverStatus::InProgress) {
                status = SolverStatus::MaximalIterationsExceeded;
            }
            
            if (status == SolverStatus::Converged) {
                STORM_LOG_INFO("Iterative solver (rational search) converged in " << overallIterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver (rational search) did not converge in " << overallIterations << " iterations.");
            }
            
            return sharpenedX;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && storm::NumberTraits<ValueType>::IsExact, storm::dd::Add<DdType, ValueType>>::type SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsRationalSearchHelper(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            return solveEquationsRationalSearchHelper<ValueType, ValueType>(*this, *this, b, this->getLowerBoundsVector(), b);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !storm::NumberTraits<ValueType>::IsExact, storm::dd::Add<DdType, ValueType>>::type SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsRationalSearchHelper(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            
            storm::dd::Add<DdType, storm::RationalNumber> rationalB = b.template toValueType<storm::RationalNumber>();
            SymbolicNativeLinearEquationSolver<DdType, storm::RationalNumber> rationalSolver(this->A.template toValueType<storm::RationalNumber>(), this->allRows, this->rowMetaVariables, this->columnMetaVariables, this->rowColumnMetaVariablePairs);
            
            storm::dd::Add<DdType, storm::RationalNumber> rationalResult = solveEquationsRationalSearchHelper<storm::RationalNumber, ImpreciseType>(rationalSolver, *this, rationalB, this->getLowerBoundsVector(), b);
            return rationalResult.template toValueType<ValueType>();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, storm::dd::Add<DdType, ValueType>>::type SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsRationalSearchHelper(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            
            // First try to find a solution using the imprecise value type.
            storm::dd::Add<DdType, ValueType> rationalResult;
            storm::dd::Add<DdType, ImpreciseType> impreciseX;
            try {
                impreciseX = this->getLowerBoundsVector().template toValueType<ImpreciseType>();
                storm::dd::Add<DdType, ImpreciseType> impreciseB = b.template toValueType<ImpreciseType>();
                SymbolicNativeLinearEquationSolver<DdType, ImpreciseType> impreciseSolver(this->A.template toValueType<ImpreciseType>(), this->allRows, this->rowMetaVariables, this->columnMetaVariables, this->rowColumnMetaVariablePairs);
                
                rationalResult = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(*this, impreciseSolver, b, impreciseX, impreciseB);
            } catch (storm::exceptions::PrecisionExceededException const& e) {
                STORM_LOG_WARN("Precision of value type was exceeded, trying to recover by switching to rational arithmetic.");
                
                // Fall back to precise value type if the precision of the imprecise value type was exceeded.
                rationalResult = solveEquationsRationalSearchHelper<ValueType, ValueType>(*this, *this, b, impreciseX.template toValueType<ValueType>(), b);
            }
            return rationalResult.template toValueType<ValueType>();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicNativeLinearEquationSolver<DdType, ValueType>::solveEquationsRationalSearch(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            return solveEquationsRationalSearchHelper<double>(x, b);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        LinearEquationSolverProblemFormat SymbolicNativeLinearEquationSolver<DdType, ValueType>::getEquationProblemFormat() const {
            if (this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                return LinearEquationSolverProblemFormat::EquationSystem;
            }
            return LinearEquationSolverProblemFormat::FixedPointSystem;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        LinearEquationSolverRequirements SymbolicNativeLinearEquationSolver<DdType, ValueType>::getRequirements() const {
            LinearEquationSolverRequirements requirements;
            
            if (this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power || this->getSettings().getSolutionMethod() == SymbolicNativeLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch) {
                requirements.requireLowerBounds();
            }
            
            return requirements;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicNativeLinearEquationSolverSettings<ValueType> const& SymbolicNativeLinearEquationSolver<DdType, ValueType>::getSettings() const {
            return settings;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<DdType, ValueType>> SymbolicNativeLinearEquationSolverFactory<DdType, ValueType>::create() const {
            return std::make_unique<SymbolicNativeLinearEquationSolver<DdType, ValueType>>(settings);
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
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::CUDD, storm::RationalNumber>;
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        template class SymbolicNativeLinearEquationSolver<storm::dd::DdType::Sylvan, storm::RationalNumber>;

        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::CUDD, storm::RationalNumber>;
        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>;
        template class SymbolicNativeLinearEquationSolverFactory<storm::dd::DdType::Sylvan, storm::RationalNumber>;
    }
}
