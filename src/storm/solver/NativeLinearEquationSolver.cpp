#include "storm/solver/NativeLinearEquationSolver.h"

#include <utility>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/KwekMehlhorn.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>::NativeLinearEquationSolverSettings() {
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel) {
                method = SolutionMethod::GaussSeidel;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi) {
                method = SolutionMethod::Jacobi;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::SOR) {
                method = SolutionMethod::SOR;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::WalkerChae) {
                method = SolutionMethod::WalkerChae;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Power) {
                method = SolutionMethod::Power;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::RationalSearch) {
                method = SolutionMethod::RationalSearch;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The selected solution technique is invalid for this solver.");
            }
        
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            omega = settings.getOmega();
            multiplicationStyle = settings.getPowerMethodMultiplicationStyle();
                                    
            // Finally force soundness and potentially overwrite some other settings.
            this->setForceSoundness(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet());
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
            
            // Make sure we switch the method if we have to guarantee soundness.
            this->setForceSoundness(forceSoundness);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setOmega(ValueType omega) {
            this->omega = omega;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setPowerMethodMultiplicationStyle(MultiplicationStyle value) {
            this->multiplicationStyle = value;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setForceSoundness(bool value) {
            forceSoundness = value;
            if (forceSoundness && method != SolutionMethod::Power && method != SolutionMethod::RationalSearch) {
                STORM_LOG_WARN("To guarantee soundness, the equation solving technique has been switched to '" << storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Power << "'.");
                method = SolutionMethod::Power;
            }
        }
        
        template<typename ValueType>
        typename NativeLinearEquationSolverSettings<ValueType>::SolutionMethod NativeLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return method;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getOmega() const {
            return omega;
        }
        
        template<typename ValueType>
        MultiplicationStyle NativeLinearEquationSolverSettings<ValueType>::getPowerMethodMultiplicationStyle() const {
            return multiplicationStyle;
        }

        template<typename ValueType>
        bool NativeLinearEquationSolverSettings<ValueType>::getForceSoundness() const {
            return forceSoundness;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
            clearCache();
        }

        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A));
            this->A = localA.get();
            clearCache();
        }

        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsSOR(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType const& omega) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Gauss-Seidel, SOR omega = " << omega << ")");

            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // Set up additional environment variables.
            uint_fast64_t iterations = 0;
            bool converged = false;
            bool terminate = false;
            
            this->startMeasureProgress();
            while (!converged && !terminate && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                A->performSuccessiveOverRelaxationStep(omega, x, b);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*this->cachedRowVector, x, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                terminate = this->terminateNow(x, SolverGuarantee::None);
                
                // If we did not yet converge, we need to backup the contents of x.
                if (!converged) {
                    *this->cachedRowVector = x;
                }
                
                // Potentially show progress.
                this->showProgressIterative(iterations);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterations;
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            this->logIterations(converged, terminate, iterations);
            
            return converged;
        }
    
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsJacobi(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Jacobi)");
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // Get a Jacobi decomposition of the matrix A.
            if (!jacobiDecomposition) {
                jacobiDecomposition = std::make_unique<std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>>>(A->getJacobiDecomposition());
            }
            storm::storage::SparseMatrix<ValueType> const& jacobiLU = jacobiDecomposition->first;
            std::vector<ValueType> const& jacobiD = jacobiDecomposition->second;
            
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = this->cachedRowVector.get();
            
            // Set up additional environment variables.
            uint_fast64_t iterations = 0;
            bool converged = false;
            bool terminate = false;

            this->startMeasureProgress();
            while (!converged && !terminate && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                multiplier.multAdd(jacobiLU, *currentX, nullptr, *nextX);

                storm::utility::vector::subtractVectors(b, *nextX, *nextX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiD, *nextX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                terminate = this->terminateNow(*currentX, SolverGuarantee::None);
                
                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);
                
                // Potentially show progress.
                this->showProgressIterative(iterations);
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterations;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *currentX);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            this->logIterations(converged, terminate, iterations);

            return converged;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::WalkerChaeData::WalkerChaeData(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalB) : t(storm::utility::convertNumber<ValueType>(1000.0)) {
            computeWalkerChaeMatrix(originalMatrix);
            computeNewB(originalB);
            precomputeAuxiliaryData();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::computeWalkerChaeMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix) {
            storm::storage::BitVector columnsWithNegativeEntries(originalMatrix.getColumnCount());
            ValueType zero = storm::utility::zero<ValueType>();
            for (auto const& e : originalMatrix) {
                if (e.getValue() < zero) {
                    columnsWithNegativeEntries.set(e.getColumn());
                }
            }
            std::vector<uint64_t> columnsWithNegativeEntriesBefore = columnsWithNegativeEntries.getNumberOfSetBitsBeforeIndices();
            
            // We now build an extended equation system matrix that only has non-negative coefficients.
            storm::storage::SparseMatrixBuilder<ValueType> builder;
            
            uint64_t row = 0;
            for (; row < originalMatrix.getRowCount(); ++row) {
                for (auto const& entry : originalMatrix.getRow(row)) {
                    if (entry.getValue() < zero) {
                        builder.addNextValue(row, originalMatrix.getRowCount() + columnsWithNegativeEntriesBefore[entry.getColumn()], -entry.getValue());
                    } else {
                        builder.addNextValue(row, entry.getColumn(), entry.getValue());
                    }
                }
            }
            ValueType one = storm::utility::one<ValueType>();
            for (auto column : columnsWithNegativeEntries) {
                builder.addNextValue(row, column, one);
                builder.addNextValue(row, originalMatrix.getRowCount() + columnsWithNegativeEntriesBefore[column], one);
                ++row;
            }
            
            matrix = builder.build();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::computeNewB(std::vector<ValueType> const& originalB) {
            b = std::vector<ValueType>(originalB);
            b.resize(matrix.getRowCount());
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::precomputeAuxiliaryData() {
            columnSums = std::vector<ValueType>(matrix.getColumnCount());
            for (auto const& e : matrix) {
                columnSums[e.getColumn()] += e.getValue();
            }
            
            newX.resize(matrix.getRowCount());
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsWalkerChae(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (WalkerChae)");
            
            // (1) Compute an equivalent equation system that has only non-negative coefficients.
            if (!walkerChaeData) {
                walkerChaeData = std::make_unique<WalkerChaeData>(*this->A, b);
            }

            // (2) Enlarge the vectors x and b to account for additional variables.
            x.resize(walkerChaeData->matrix.getRowCount());
            
            // Square the error bound, so we can use it to check for convergence. We take the squared error, because we
            // do not want to compute the root in the 2-norm computation.
            ValueType squaredErrorBound = storm::utility::pow(this->getSettings().getPrecision(), 2);
            
            // Set up references to the x-vectors used in the iteration loop.
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = &walkerChaeData->newX;

            std::vector<ValueType> tmp = walkerChaeData->matrix.getRowSumVector();
            storm::utility::vector::applyPointwise(tmp, walkerChaeData->b, walkerChaeData->b, [this] (ValueType const& first, ValueType const& second) { return walkerChaeData->t * first + second; } );
            
            // Add t to all entries of x.
            storm::utility::vector::applyPointwise(x, x, [this] (ValueType const& value) { return value + walkerChaeData->t; });

            // Create a vector that always holds Ax.
            std::vector<ValueType> currentAx(x.size());
            multiplier.multAdd(walkerChaeData->matrix, *currentX, nullptr, currentAx);
            
            // (3) Perform iterations until convergence.
            bool converged = false;
            uint64_t iterations = 0;
            this->startMeasureProgress();
            while (!converged && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Perform one Walker-Chae step.
                walkerChaeData->matrix.performWalkerChaeStep(*currentX, walkerChaeData->columnSums, walkerChaeData->b, currentAx, *nextX);
                
                // Compute new Ax.
                multiplier.multAdd(walkerChaeData->matrix, *nextX, nullptr, currentAx);
                
                // Check for convergence.
                converged = storm::utility::vector::computeSquaredNorm2Difference(currentAx, walkerChaeData->b) <= squaredErrorBound;
                
                // Swap the x vectors for the next iteration.
                std::swap(currentX, nextX);
                
                // Potentially show progress.
                this->showProgressIterative(iterations);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterations;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == &walkerChaeData->newX) {
                std::swap(x, *currentX);
            }

            // Resize the solution to the right size.
            x.resize(this->A->getRowCount());
            
            // Finalize solution vector.
            storm::utility::vector::applyPointwise(x, x, [this] (ValueType const& value) { return value - walkerChaeData->t; } );
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }

            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterations << " iterations.");
            }

            return converged;
        }
        
        template<typename ValueType>
        typename NativeLinearEquationSolver<ValueType>::PowerIterationResult NativeLinearEquationSolver<ValueType>::performPowerIteration(std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations) const {
            
            bool useGaussSeidelMultiplication = this->getSettings().getPowerMethodMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;
            
            std::vector<ValueType>* originalX = currentX;
            
            bool converged = false;
            bool terminate = this->terminateNow(*currentX, guarantee);
            uint64_t iterations = currentIterations;
            while (!converged && !terminate && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                if (useGaussSeidelMultiplication) {
                    *newX = *currentX;
                    this->multiplier.multAddGaussSeidelBackward(*this->A, *newX, &b);
                } else {
                    this->multiplier.multAdd(*this->A, *currentX, &b, *newX);
                }
                
                // Now check for termination.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                terminate = this->terminateNow(*currentX, SolverGuarantee::LessOrEqual);
                
                // Potentially show progress.
                this->showProgressIterative(iterations);
                
                // Set up next iteration.
                std::swap(currentX, newX);
                ++iterations;
            }
            
            // Swap the pointers so that the output is always in currentX.
            if (originalX == newX) {
                std::swap(currentX, newX);
            }
            
            return PowerIterationResult(iterations - currentIterations, converged ? Status::Converged : (terminate ? Status::TerminatedEarly : Status::MaximalIterationsExceeded));
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsPower(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(this->hasLowerBound(), storm::exceptions::UnmetRequirementException, "Solver requires upper bound, but none was given.");
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Power)");

            // Prepare the solution vectors.
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            std::vector<ValueType>* currentX = &x;
            this->createLowerBoundsVector(*currentX);
            std::vector<ValueType>* newX = this->cachedRowVector.get();
            
            // Forward call to power iteration implementation.
            this->startMeasureProgress();
            PowerIterationResult result = this->performPowerIteration(currentX, newX, b, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion(), SolverGuarantee::LessOrEqual, 0);

            // Swap the result in place.
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *newX);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            this->logIterations(result.status == Status::Converged, result.status == Status::TerminatedEarly, result.iterations);

            return result.status == Status::Converged || result.status == Status::TerminatedEarly;
        }
        
        template<typename ValueType>
        void preserveOldRelevantValues(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType>& oldValues) {
            storm::utility::vector::selectVectorValues(oldValues, relevantValues, allValues);
        }
        
        template<typename ValueType>
        ValueType computeMaxAbsDiff(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType> const& oldValues) {
            ValueType result = storm::utility::zero<ValueType>();
            auto oldValueIt = oldValues.begin();
            for (auto value : relevantValues) {
                result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allValues[value] - *oldValueIt));
            }
            return result;
        }
        
        template<typename ValueType>
        ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
            ValueType result = storm::utility::zero<ValueType>();
            for (auto value : relevantValues) {
                result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[value] - allOldValues[value]));
            }
            return result;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsSoundPower(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(this->hasLowerBound(), storm::exceptions::UnmetRequirementException, "Solver requires upper bound, but none was given.");
            STORM_LOG_THROW(this->hasUpperBound(), storm::exceptions::UnmetRequirementException, "Solver requires upper bound, but none was given.");
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (SoundPower)");
            
            std::vector<ValueType>* lowerX = &x;
            this->createLowerBoundsVector(*lowerX);
            this->createUpperBoundsVector(this->cachedRowVector, this->getMatrixRowCount());
            std::vector<ValueType>* upperX = this->cachedRowVector.get();
            
            bool useGaussSeidelMultiplication = this->getSettings().getPowerMethodMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;
            std::vector<ValueType>* tmp;
            if (!useGaussSeidelMultiplication) {
                cachedRowVector2 = std::make_unique<std::vector<ValueType>>(x.size());
                tmp = cachedRowVector2.get();
            }
            
            bool converged = false;
            bool terminate = false;
            uint64_t iterations = 0;
            bool doConvergenceCheck = true;
            bool useDiffs = this->hasRelevantValues();
            std::vector<ValueType> oldValues;
            if (useGaussSeidelMultiplication && useDiffs) {
                oldValues.resize(this->getRelevantValues().getNumberOfSetBits());
            }
            ValueType maxLowerDiff = storm::utility::zero<ValueType>();
            ValueType maxUpperDiff = storm::utility::zero<ValueType>();
            ValueType precision = static_cast<ValueType>(this->getSettings().getPrecision());
            if (!this->getSettings().getRelativeTerminationCriterion()) {
                precision *= storm::utility::convertNumber<ValueType>(2.0);
            }
            this->startMeasureProgress();
            while (!converged && !terminate && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Remember in which directions we took steps in this iteration.
                bool lowerStep = false;
                bool upperStep = false;
                
                // In every thousandth iteration or if the differences are the same, we improve both bounds.
                if (iterations % 1000 == 0 || maxLowerDiff == maxUpperDiff) {
                    lowerStep = true;
                    upperStep = true;
                    if (useGaussSeidelMultiplication) {
                        if (useDiffs) {
                            preserveOldRelevantValues(*lowerX, this->getRelevantValues(), oldValues);
                        }
                        this->multiplier.multAddGaussSeidelBackward(*this->A, *lowerX, &b);
                        if (useDiffs) {
                            maxLowerDiff = computeMaxAbsDiff(*lowerX, this->getRelevantValues(), oldValues);
                            preserveOldRelevantValues(*upperX, this->getRelevantValues(), oldValues);
                        }
                        this->multiplier.multAddGaussSeidelBackward(*this->A, *upperX, &b);
                        if (useDiffs) {
                            maxUpperDiff = computeMaxAbsDiff(*upperX, this->getRelevantValues(), oldValues);
                        }
                    } else {
                        this->multiplier.multAdd(*this->A, *lowerX, &b, *tmp);
                        if (useDiffs) {
                            maxLowerDiff = computeMaxAbsDiff(*lowerX, *tmp, this->getRelevantValues());
                        }
                        std::swap(tmp, lowerX);
                        this->multiplier.multAdd(*this->A, *upperX, &b, *tmp);
                        if (useDiffs) {
                            maxUpperDiff = computeMaxAbsDiff(*upperX, *tmp, this->getRelevantValues());
                        }
                        std::swap(tmp, upperX);
                    }
                } else {
                    // In the following iterations, we improve the bound with the greatest difference.
                    if (useGaussSeidelMultiplication) {
                        if (maxLowerDiff >= maxUpperDiff) {
                            if (useDiffs) {
                                preserveOldRelevantValues(*lowerX, this->getRelevantValues(), oldValues);
                            }
                            this->multiplier.multAddGaussSeidelBackward(*this->A, *lowerX, &b);
                            if (useDiffs) {
                                maxLowerDiff = computeMaxAbsDiff(*lowerX, this->getRelevantValues(), oldValues);
                            }
                            lowerStep = true;
                        } else {
                            if (useDiffs) {
                                preserveOldRelevantValues(*upperX, this->getRelevantValues(), oldValues);
                            }
                            this->multiplier.multAddGaussSeidelBackward(*this->A, *upperX, &b);
                            if (useDiffs) {
                                maxUpperDiff = computeMaxAbsDiff(*upperX, this->getRelevantValues(), oldValues);
                            }
                            upperStep = true;
                        }
                    } else {
                        if (maxLowerDiff >= maxUpperDiff) {
                            this->multiplier.multAdd(*this->A, *lowerX, &b, *tmp);
                            if (useDiffs) {
                                maxLowerDiff = computeMaxAbsDiff(*lowerX, *tmp, this->getRelevantValues());
                            }
                            std::swap(tmp, lowerX);
                            lowerStep = true;
                        } else {
                            this->multiplier.multAdd(*this->A, *upperX, &b, *tmp);
                            if (useDiffs) {
                                maxUpperDiff = computeMaxAbsDiff(*upperX, *tmp, this->getRelevantValues());
                            }
                            std::swap(tmp, upperX);
                            upperStep = true;
                        }
                    }
                }
                STORM_LOG_ASSERT(maxLowerDiff >= storm::utility::zero<ValueType>(), "Expected non-negative lower diff.");
                STORM_LOG_ASSERT(maxUpperDiff >= storm::utility::zero<ValueType>(), "Expected non-negative upper diff.");
                if (iterations % 1000 == 0) {
                    STORM_LOG_TRACE("Iteration " << iterations << ": lower difference: " << maxLowerDiff << ", upper difference: " << maxUpperDiff << ".");
                }
                
                if (doConvergenceCheck) {
                    // Now check if the process already converged within our precision. Note that we double the target
                    // precision here. Doing so, we need to take the means of the lower and upper values later to guarantee
                    // the original precision.
                    if (this->hasRelevantValues()) {
                        converged = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, this->getRelevantValues(), precision, this->getSettings().getRelativeTerminationCriterion());
                    } else {
                        converged = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, precision, this->getSettings().getRelativeTerminationCriterion());
                    }
                    if (lowerStep) {
                        terminate |= this->terminateNow(*lowerX, SolverGuarantee::LessOrEqual);
                    }
                    if (upperStep) {
                        terminate |= this->terminateNow(*upperX, SolverGuarantee::GreaterOrEqual);
                    }
                }
                
                // Potentially show progress.
                this->showProgressIterative(iterations);
                
                // Set up next iteration.
                ++iterations;
                doConvergenceCheck = !doConvergenceCheck;
            }
            
            // We take the means of the lower and upper bound so we guarantee the desired precision.
            storm::utility::vector::applyPointwise(*lowerX, *upperX, *lowerX, [] (ValueType const& a, ValueType const& b) { return (a + b) / storm::utility::convertNumber<ValueType>(2.0); });

            // Since we shuffled the pointer around, we need to write the actual results to the input/output vector x.
            if (&x == tmp) {
                std::swap(x, *tmp);
            } else if (&x == this->cachedRowVector.get()) {
                std::swap(x, *this->cachedRowVector);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            this->logIterations(converged, terminate, iterations);

            return converged;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearch(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            return solveEquationsRationalSearchHelper<double>(x, b);
        }
        
        template<typename RationalType, typename ImpreciseType>
        struct TemporaryHelper {
            static std::vector<RationalType>* getTemporary(std::vector<RationalType>& rationalX, std::vector<ImpreciseType>*& currentX, std::vector<ImpreciseType>*& newX) {
                return &rationalX;
            }
            
            static void swapSolutions(std::vector<RationalType>& rationalX, std::vector<RationalType>*& rationalSolution, std::vector<ImpreciseType>& x, std::vector<ImpreciseType>*& currentX, std::vector<ImpreciseType>*& newX) {
                // Nothing to do.
            }
        };
        
        template<typename RationalType>
        struct TemporaryHelper<RationalType, RationalType> {
            static std::vector<RationalType>* getTemporary(std::vector<RationalType>& rationalX, std::vector<RationalType>*& currentX, std::vector<RationalType>*& newX) {
                return newX;
            }
            
            static void swapSolutions(std::vector<RationalType>& rationalX, std::vector<RationalType>*& rationalSolution, std::vector<RationalType>& x, std::vector<RationalType>*& currentX, std::vector<RationalType>*& newX) {
                if (&rationalX == rationalSolution) {
                    // In this case, the rational solution is in place.
                    
                    // However, since the rational solution is no alias to current x, the imprecise solution is stored
                    // in current x and and rational x is not an alias to x, we can swap the contents of currentX to x.
                    std::swap(x, *currentX);
                } else {
                    // Still, we may assume that the rational solution is not current x and is therefore new x.
                    std::swap(rationalX, *rationalSolution);
                    std::swap(x, *currentX);
                }
            }
        };
        
        template<typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(NativeLinearEquationSolver<ImpreciseType> const& impreciseSolver, storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX, std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A, std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const {
            
            std::vector<ImpreciseType>* currentX = &x;
            std::vector<ImpreciseType>* newX = &tmpX;
            
            Status status = Status::InProgress;
            uint64_t overallIterations = 0;
            uint64_t valueIterationInvocations = 0;
            ValueType precision = this->getSettings().getPrecision();
            impreciseSolver.startMeasureProgress();
            while (status == Status::InProgress && overallIterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Perform value iteration with the current precision.
                typename NativeLinearEquationSolver<ImpreciseType>::PowerIterationResult result = impreciseSolver.performPowerIteration(currentX, newX, b, storm::utility::convertNumber<ImpreciseType, ValueType>(precision), this->getSettings().getRelativeTerminationCriterion(), SolverGuarantee::LessOrEqual, overallIterations);
                
                // At this point, the result of the imprecise value iteration is stored in the (imprecise) current x.
                
                ++valueIterationInvocations;
                STORM_LOG_TRACE("Completed " << valueIterationInvocations << " power iteration invocations, the last one with precision " << precision << " completed in " << result.iterations << " iterations.");
                
                // Count the iterations.
                overallIterations += result.iterations;
                
                // Compute maximal precision until which to sharpen.
                uint64_t p = storm::utility::convertNumber<uint64_t>(storm::utility::ceil(storm::utility::log10<ValueType>(storm::utility::one<ValueType>() / precision)));
                
                // Make sure that currentX and rationalX are not aliased.
                std::vector<RationalType>* temporaryRational = TemporaryHelper<RationalType, ImpreciseType>::getTemporary(rationalX, currentX, newX);
                
                // Sharpen solution and place it in the temporary rational.
                bool foundSolution = sharpen(p, rationalA, *currentX, rationalB, *temporaryRational);
                
                // After sharpen, if a solution was found, it is contained in the free rational.
                
                if (foundSolution) {
                    status = Status::Converged;
                    
                    TemporaryHelper<RationalType, ImpreciseType>::swapSolutions(rationalX, temporaryRational, x, currentX, newX);
                } else {
                    // Increase the precision.
                    precision = precision / 100;
                }
            }
            
            if (status == Status::InProgress && overallIterations == this->getSettings().getMaximalNumberOfIterations()) {
                status = Status::MaximalIterationsExceeded;
            }
            
            this->logIterations(status == Status::Converged, status == Status::TerminatedEarly, overallIterations);
            
            return status == Status::Converged || status == Status::TerminatedEarly;
        }
     
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is imprecise.
            
            // Create a rational representation of the input so we can check for a proper solution later.
            storm::storage::SparseMatrix<storm::RationalNumber> rationalA = this->A->template toValueType<storm::RationalNumber>();
            std::vector<storm::RationalNumber> rationalX(x.size());
            std::vector<storm::RationalNumber> rationalB = storm::utility::vector::convertNumericVector<storm::RationalNumber>(b);
                        
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<storm::RationalNumber, ImpreciseType>(*this, rationalA, rationalX, rationalB, *this->A, x, b, *this->cachedRowVector);
            
            // Translate back rational result to imprecise result.
            auto targetIt = x.begin();
            for (auto it = rationalX.begin(), ite = rationalX.end(); it != ite; ++it, ++targetIt) {
                *targetIt = storm::utility::convertNumber<ValueType>(*it);
            }
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }
        
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is exact and the same type is to be used for the imprecise part.
            
            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(*this, *this->A, x, b, *this->A, *this->cachedRowVector, b, x);
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }
        
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is exact and the imprecise one is not. We first try to solve the
            // problem using the imprecise data type and fall back to the exact type as needed.
            
            // Translate A to its imprecise version.
            storm::storage::SparseMatrix<ImpreciseType> impreciseA = this->A->template toValueType<ImpreciseType>();
            
            // Translate x to its imprecise version.
            std::vector<ImpreciseType> impreciseX(x.size());
            {
                std::vector<ValueType> tmp(x.size());
                this->createLowerBoundsVector(tmp);
                auto targetIt = impreciseX.begin();
                for (auto sourceIt = tmp.begin(); targetIt != impreciseX.end(); ++targetIt, ++sourceIt) {
                    *targetIt = storm::utility::convertNumber<ImpreciseType, ValueType>(*sourceIt);
                }
            }
            
            // Create temporary storage for an imprecise x.
            std::vector<ImpreciseType> impreciseTmpX(x.size());
            
            // Translate b to its imprecise version.
            std::vector<ImpreciseType> impreciseB(b.size());
            auto targetIt = impreciseB.begin();
            for (auto sourceIt = b.begin(); targetIt != impreciseB.end(); ++targetIt, ++sourceIt) {
                *targetIt = storm::utility::convertNumber<ImpreciseType, ValueType>(*sourceIt);
            }
            
            // Create imprecise solver from the imprecise data.
            NativeLinearEquationSolver<ImpreciseType> impreciseSolver;
            impreciseSolver.setMatrix(impreciseA);
            impreciseSolver.setCachingEnabled(true);
            
            bool converged = false;
            try {
                // Forward the call to the core rational search routine.
                converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(impreciseSolver, *this->A, x, b, impreciseA, impreciseX, impreciseB, impreciseTmpX);
            } catch (storm::exceptions::PrecisionExceededException const& e) {
                STORM_LOG_WARN("Precision of value type was exceeded, trying to recover by switching to rational arithmetic.");
                
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
                }
                
                // Translate the imprecise value iteration result to the one we are going to use from now on.
                auto targetIt = this->cachedRowVector->begin();
                for (auto it = impreciseX.begin(), ite = impreciseX.end(); it != ite; ++it, ++targetIt) {
                    *targetIt = storm::utility::convertNumber<ValueType>(*it);
                }
                
                // Get rid of the superfluous data structures.
                impreciseX = std::vector<ImpreciseType>();
                impreciseTmpX = std::vector<ImpreciseType>();
                impreciseB = std::vector<ImpreciseType>();
                impreciseA = storm::storage::SparseMatrix<ImpreciseType>();
                
                // Forward the call to the core rational search routine, but now with our value type as the imprecise value type.
                converged = solveEquationsRationalSearchHelper<ValueType, ValueType>(*this, *this->A, x, b, *this->A, *this->cachedRowVector, b, x);
            }
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }

        template<typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        bool NativeLinearEquationSolver<ValueType>::sharpen(uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x, std::vector<RationalType> const& b, std::vector<RationalType>& tmp) {
            for (uint64_t p = 0; p <= precision; ++p) {
                storm::utility::kwek_mehlhorn::sharpen(p, x, tmp);
                
                if (NativeLinearEquationSolver<RationalType>::isSolution(A, tmp, b)) {
                    return true;
                }
            }
            return false;
        }

        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::isSolution(storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b) {
            storm::utility::ConstantsComparator<ValueType> comparator;
            
            auto valueIt = values.begin();
            auto bIt = b.begin();
            for (uint64_t row = 0; row < matrix.getRowCount(); ++row, ++valueIt, ++bIt) {
                ValueType rowValue = *bIt + matrix.multiplyRowWithVector(row, values);
                
                // If the value does not match the one in the values vector, the given vector is not a solution.
                if (!comparator.isEqual(rowValue, *valueIt)) {
                    return false;
                }
            }
            
            // Checked all values at this point.
            return true;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::logIterations(bool converged, bool terminate, uint64_t iterations) const {
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterations << " iterations.");
            } else if (terminate) {
                STORM_LOG_INFO("Iterative solver terminated after " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterations << " iterations.");
            }
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::internalSolveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR || this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::GaussSeidel) {
                return this->solveEquationsSOR(x, b, this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR ? this->getSettings().getOmega() : storm::utility::one<ValueType>());
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                return this->solveEquationsJacobi(x, b);
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::WalkerChae) {
                return this->solveEquationsWalkerChae(x, b);
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power) {
                if (this->getSettings().getForceSoundness()) {
                    return this->solveEquationsSoundPower(x, b);
                } else {
                    return this->solveEquationsPower(x, b);
                }
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch) {
                return this->solveEquationsRationalSearch(x, b);
            }
            
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unknown solving technique.");
            return false;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            if (&x != &result) {
                multiplier.multAdd(*A, x, b, result);
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
                }
                
                multiplier.multAdd(*A, x, b, *this->cachedRowVector);
                result.swap(*this->cachedRowVector);
                
                if (!this->isCachingEnabled()) {
                    clearCache();
                }
            }
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            if (&x != &result) {
                multiplier.multAddReduce(dir, rowGroupIndices, *A, x, b, result, choices);
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
                }
            
                multiplier.multAddReduce(dir, rowGroupIndices, *A, x, b, *this->cachedRowVector, choices);
                result.swap(*this->cachedRowVector);
                
                if (!this->isCachingEnabled()) {
                    clearCache();
                }
            }
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::supportsGaussSeidelMultiplication() const {
            return true;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            STORM_LOG_ASSERT(this->A->getRowCount() == this->A->getColumnCount(), "This function is only applicable for square matrices.");
            multiplier.multAddGaussSeidelBackward(*A, x, b);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            multiplier.multAddReduceGaussSeidelBackward(dir, rowGroupIndices, *A, x, b, choices);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setSettings(NativeLinearEquationSolverSettings<ValueType> const& newSettings) {
            settings = newSettings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        LinearEquationSolverProblemFormat NativeLinearEquationSolver<ValueType>::getEquationProblemFormat() const {
            if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power || this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch) {
                return LinearEquationSolverProblemFormat::FixedPointSystem;
            } else {
                return LinearEquationSolverProblemFormat::EquationSystem;
            }
        }
        
        template<typename ValueType>
        LinearEquationSolverRequirements NativeLinearEquationSolver<ValueType>::getRequirements() const {
            LinearEquationSolverRequirements requirements;
            if (this->getSettings().getForceSoundness()) {
                if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power) {
                    requirements.requireBounds();
                } else {
                    STORM_LOG_WARN("Forcing soundness, but selecting a method other than the power iteration is not supported.");
                }
            } else {
                if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power) {
                    requirements.requireLowerBounds();
                }
            }
            return requirements;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::clearCache() const {
            jacobiDecomposition.reset();
            cachedRowVector2.reset();
            walkerChaeData.reset();
            LinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return this->A->getRowCount();
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return this->A->getColumnCount();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create() const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(settings);
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>& NativeLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> NativeLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolverSettings<double>;
        template class NativeLinearEquationSolver<double>;
        template class NativeLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class NativeLinearEquationSolverSettings<storm::RationalNumber>;
        template class NativeLinearEquationSolver<storm::RationalNumber>;
        template class NativeLinearEquationSolverFactory<storm::RationalNumber>;

#endif
    }
}
