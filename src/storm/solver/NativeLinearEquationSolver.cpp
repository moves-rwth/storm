#include "storm/solver/NativeLinearEquationSolver.h"

#include "storm/environment/solver/NativeSolverEnvironment.h"

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/KwekMehlhorn.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver() : localA(nullptr), A(nullptr) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : localA(nullptr), A(nullptr) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) : localA(nullptr), A(nullptr) {
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
        bool NativeLinearEquationSolver<ValueType>::solveEquationsSOR(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType const& omega) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Gauss-Seidel, SOR omega = " << omega << ")");

            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();
            bool relative = env.solver().native().getRelativeTerminationCriterion();
            
            // Set up additional environment variables.
            uint_fast64_t iterations = 0;
            bool converged = false;
            bool terminate = false;
            
            this->startMeasureProgress();
            while (!converged && !terminate && iterations < maxIter) {
                A->performSuccessiveOverRelaxationStep(omega, x, b);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*this->cachedRowVector, x, precision, relative);
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
        bool NativeLinearEquationSolver<ValueType>::solveEquationsJacobi(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Jacobi)");
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // Get a Jacobi decomposition of the matrix A.
            if (!jacobiDecomposition) {
                jacobiDecomposition = std::make_unique<std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>>>(A->getJacobiDecomposition());
            }
            
            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();
            bool relative = env.solver().native().getRelativeTerminationCriterion();

            storm::storage::SparseMatrix<ValueType> const& jacobiLU = jacobiDecomposition->first;
            std::vector<ValueType> const& jacobiD = jacobiDecomposition->second;
            
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = this->cachedRowVector.get();
            
            // Set up additional environment variables.
            uint_fast64_t iterations = 0;
            bool converged = false;
            bool terminate = false;

            this->startMeasureProgress();
            while (!converged && !terminate && iterations < maxIter) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                multiplier.multAdd(jacobiLU, *currentX, nullptr, *nextX);

                storm::utility::vector::subtractVectors(b, *nextX, *nextX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiD, *nextX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, precision, relative);
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
        bool NativeLinearEquationSolver<ValueType>::solveEquationsWalkerChae(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (WalkerChae)");
            
            // (1) Compute an equivalent equation system that has only non-negative coefficients.
            if (!walkerChaeData) {
                walkerChaeData = std::make_unique<WalkerChaeData>(*this->A, b);
            }

            // (2) Enlarge the vectors x and b to account for additional variables.
            x.resize(walkerChaeData->matrix.getRowCount());
            
            // Square the error bound, so we can use it to check for convergence. We take the squared error, because we
            // do not want to compute the root in the 2-norm computation.
            ValueType squaredErrorBound = storm::utility::pow(storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision()), 2);
            
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();
            
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
            while (!converged && iterations < maxIter) {
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
        typename NativeLinearEquationSolver<ValueType>::PowerIterationResult NativeLinearEquationSolver<ValueType>::performPowerIteration(std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations, uint64_t maxIterations, storm::solver::MultiplicationStyle const& multiplicationStyle) const {

            bool useGaussSeidelMultiplication = multiplicationStyle == storm::solver::MultiplicationStyle::GaussSeidel;
            
            std::vector<ValueType>* originalX = currentX;
            
            bool converged = false;
            bool terminate = this->terminateNow(*currentX, guarantee);
            uint64_t iterations = currentIterations;
            while (!converged && !terminate && iterations < maxIterations) {
                if (useGaussSeidelMultiplication) {
                    *newX = *currentX;
                    this->multiplier.multAddGaussSeidelBackward(*this->A, *newX, &b);
                } else {
                    this->multiplier.multAdd(*this->A, *currentX, &b, *newX);
                }
                
                // Now check for termination.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, precision, relative);
                terminate = this->terminateNow(*currentX, guarantee);
                
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
            
            return PowerIterationResult(iterations - currentIterations, converged ? SolverStatus::Converged : (terminate ? SolverStatus::TerminatedEarly : SolverStatus::MaximalIterationsExceeded));
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsPower(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Power)");

            // Prepare the solution vectors.
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            std::vector<ValueType>* currentX = &x;
            SolverGuarantee guarantee = SolverGuarantee::None;
            if (this->hasCustomTerminationCondition()) {
                if (this->getTerminationCondition().requiresGuarantee(SolverGuarantee::LessOrEqual) && this->hasLowerBound()) {
                    this->createLowerBoundsVector(*currentX);
                    guarantee = SolverGuarantee::LessOrEqual;
                } else if (this->getTerminationCondition().requiresGuarantee(SolverGuarantee::GreaterOrEqual) && this->hasUpperBound()) {
                    this->createUpperBoundsVector(*currentX);
                    guarantee = SolverGuarantee::GreaterOrEqual;
                }
            }
            std::vector<ValueType>* newX = this->cachedRowVector.get();
            
            // Forward call to power iteration implementation.
            this->startMeasureProgress();
            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            PowerIterationResult result = this->performPowerIteration(currentX, newX, b, precision, env.solver().native().getRelativeTerminationCriterion(), guarantee, 0, env.solver().native().getMaximalNumberOfIterations(), env.solver().native().getPowerMethodMultiplicationStyle());

            // Swap the result in place.
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *newX);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            this->logIterations(result.status == SolverStatus::Converged, result.status == SolverStatus::TerminatedEarly, result.iterations);

            return result.status == SolverStatus::Converged || result.status == SolverStatus::TerminatedEarly;
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
        bool NativeLinearEquationSolver<ValueType>::solveEquationsSoundPower(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(this->hasLowerBound(), storm::exceptions::UnmetRequirementException, "Solver requires lower bound, but none was given.");
            STORM_LOG_THROW(this->hasUpperBound(), storm::exceptions::UnmetRequirementException, "Solver requires upper bound, but none was given.");
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (SoundPower)");

            std::vector<ValueType>* lowerX = &x;
            this->createLowerBoundsVector(*lowerX);
            this->createUpperBoundsVector(this->cachedRowVector, this->getMatrixRowCount());
            std::vector<ValueType>* upperX = this->cachedRowVector.get();
            
            bool useGaussSeidelMultiplication = env.solver().native().getPowerMethodMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;
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
            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            bool relative = env.solver().native().getRelativeTerminationCriterion();
            if (!relative) {
                precision *= storm::utility::convertNumber<ValueType>(2.0);
            }
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();
            this->startMeasureProgress();
            while (!converged && !terminate && iterations < maxIter) {
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
                        converged = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, this->getRelevantValues(), precision, relative);
                    } else {
                        converged = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, precision, relative);
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
            this->overallPerformedIterations += iterations;

            this->logIterations(converged, terminate, iterations);

            return converged;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsQuickSoundPower(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (QuickPower)");
            bool useGaussSeidelMultiplication = env.solver().native().getPowerMethodMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;

            // Prepare the solution vectors.
            assert(x.size() == getMatrixRowCount());
            std::vector<ValueType> *stepBoundedX, *stepBoundedStayProbs, *tmp;
            if (useGaussSeidelMultiplication) {
                stepBoundedX = &x;
                stepBoundedX->assign(getMatrixRowCount(), storm::utility::zero<ValueType>());
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount(), storm::utility::one<ValueType>());
                } else {
                    this->cachedRowVector->assign(getMatrixRowCount(), storm::utility::one<ValueType>());
                }
                stepBoundedStayProbs = this->cachedRowVector.get();
                tmp = nullptr;
            } else {
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount(), storm::utility::zero<ValueType>());
                } else {
                    this->cachedRowVector->assign(getMatrixRowCount(), storm::utility::zero<ValueType>());
                }
                stepBoundedX = this->cachedRowVector.get();
                if (!this->cachedRowVector2) {
                    this->cachedRowVector2 = std::make_unique<std::vector<ValueType>>(getMatrixRowCount(), storm::utility::one<ValueType>());
                } else {
                    this->cachedRowVector2->assign(getMatrixRowCount(), storm::utility::one<ValueType>());
                }
                stepBoundedStayProbs = this->cachedRowVector2.get();
                tmp = &x;
            }

            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            bool relative = env.solver().native().getRelativeTerminationCriterion();
            if (!relative) {
                precision *= storm::utility::convertNumber<ValueType>(2.0);
            }
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();

            //std::cout << *this->A << std::endl;
            //std::cout << storm::utility::vector::toString(b) << std::endl;

            //std::cout << "solving eq sys.. " << std::endl;
            
            uint64_t iterations = 0;
            bool converged = false;
            bool terminate = false;
            uint64_t minIndex(0), maxIndex(0);
            ValueType minValueBound, maxValueBound;
            bool hasMinValueBound(false), hasMaxValueBound(false);
            // Prepare initial bounds for the solution (if given)
            if (this->hasLowerBound()) {
                minValueBound = this->getLowerBound(true);
                hasMinValueBound = true;
            }
            if (this->hasUpperBound()) {
                maxValueBound = this->getUpperBound(true);
                hasMaxValueBound = true;
            }
            
            bool convergencePhase1 = true;
            uint64_t firstIndexViolatingConvergence = 0;
            this->startMeasureProgress();
            while (!converged && !terminate && iterations < maxIter) {
                
                // Apply step
                if (useGaussSeidelMultiplication) {
                    this->multiplier.multAddGaussSeidelBackward(*this->A, *stepBoundedX, &b);
                    this->multiplier.multAddGaussSeidelBackward(*this->A, *stepBoundedStayProbs, nullptr);
                } else {
                    this->multiplier.multAdd(*this->A, *stepBoundedX, &b, *tmp);
                    std::swap(tmp, stepBoundedX);
                    this->multiplier.multAdd(*this->A, *stepBoundedStayProbs, nullptr, *tmp);
                    std::swap(tmp, stepBoundedStayProbs);
                }
                
                // Check for convergence
                if (convergencePhase1) {
                    // Phase 1: the probability to 'stay within the matrix' has to be < 1 at every state
                    for (; firstIndexViolatingConvergence != stepBoundedStayProbs->size(); ++firstIndexViolatingConvergence) {
                        static_assert(NumberTraits<ValueType>::IsExact || std::is_same<ValueType, double>::value, "Considered ValueType not handled.");
                        if (NumberTraits<ValueType>::IsExact) {
                            if (storm::utility::isOne(stepBoundedStayProbs->at(firstIndexViolatingConvergence))) {
                                break;
                            }
                        } else {
                            if (storm::utility::isAlmostOne(storm::utility::convertNumber<double>(stepBoundedStayProbs->at(firstIndexViolatingConvergence)))) {
                                break;
                            }
                        }
                    }
                    if (firstIndexViolatingConvergence == stepBoundedStayProbs->size()) {
                        STORM_LOG_ASSERT(!std::any_of(stepBoundedStayProbs->begin(), stepBoundedStayProbs->end(), [](ValueType value){return storm::utility::isOne(value);}), "Did not expect staying-probability 1 at this point.");
                        convergencePhase1 = false;
                        firstIndexViolatingConvergence = this->hasRelevantValues() ? this->getRelevantValues().getNextSetIndex(0) : 0;
                    }
                }
                if (!convergencePhase1) {
                    // Phase 2: the difference between lower and upper bound has to be < precision at every (relevant) value
                    // First check with (possibly too tight) bounds from a previous iteration. Only compute the actual bounds if this first check passes.
                    ValueType minValueBoundCandidate = stepBoundedX->at(minIndex) / (storm::utility::one<ValueType>() - stepBoundedStayProbs->at(minIndex));
                    ValueType maxValueBoundCandidate = stepBoundedX->at(maxIndex) / (storm::utility::one<ValueType>() - stepBoundedStayProbs->at(maxIndex));
                    if (hasMinValueBound && minValueBound > minValueBoundCandidate) {
                        minValueBoundCandidate = minValueBound;
                    }
                    if (hasMaxValueBound && maxValueBound < maxValueBoundCandidate) {
                        maxValueBoundCandidate = maxValueBound;
                    }
                    ValueType const& stayProb = stepBoundedStayProbs->at(firstIndexViolatingConvergence);
                    // The error made in this iteration
                    ValueType absoluteError = stayProb * (maxValueBoundCandidate - minValueBoundCandidate);
                    // The maximal allowed error (possibly respecting relative precision)
                    // Note: We implement the relative convergence criterion in a way that avoids division by zero in the case where stepBoundedX[i] is zero.
                    ValueType maxAllowedError = relative ? (precision * stepBoundedX->at(firstIndexViolatingConvergence)) : precision;
                    if (absoluteError <= maxAllowedError) {
                        // Compute the actual bounds now
                        auto valIt = stepBoundedX->begin();
                        auto valIte = stepBoundedX->end();
                        auto probIt = stepBoundedStayProbs->begin();
                        for (uint64_t index = 0; valIt != valIte; ++valIt, ++probIt, ++index) {
                            ValueType currentBound = *valIt / (storm::utility::one<ValueType>() - *probIt);
                            if (currentBound < minValueBoundCandidate) {
                                minIndex = index;
                                minValueBoundCandidate = std::move(currentBound);
                            } else if (currentBound > maxValueBoundCandidate) {
                                maxIndex = index;
                                maxValueBoundCandidate = std::move(currentBound);
                            }
                        }
                        if (!hasMinValueBound || minValueBoundCandidate > minValueBound) {
                            minValueBound = minValueBoundCandidate;
                            hasMinValueBound = true;
                        }
                        if (!hasMaxValueBound || maxValueBoundCandidate < maxValueBound) {
                            maxValueBound = maxValueBoundCandidate;
                            hasMaxValueBound = true;
                        }
                        absoluteError = stayProb * (maxValueBound - minValueBound);
                        if (absoluteError <= maxAllowedError) {
                            // The current index satisfies the desired bound. We now move to the next index that violates it
                            while (true) {
                                ++firstIndexViolatingConvergence;
                                if (this->hasRelevantValues()) {
                                    firstIndexViolatingConvergence = this->getRelevantValues().getNextSetIndex(firstIndexViolatingConvergence);
                                }
                                if (firstIndexViolatingConvergence == stepBoundedStayProbs->size()) {
                                    converged = true;
                                    break;
                                } else {
                                    absoluteError = stepBoundedStayProbs->at(firstIndexViolatingConvergence) * (maxValueBound - minValueBound);
                                    maxAllowedError = relative ? (precision * stepBoundedX->at(firstIndexViolatingConvergence)) : precision;
                                    if (absoluteError > maxAllowedError) {
                                        // not converged yet
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Potentially show progress.
                this->showProgressIterative(iterations);
                
                // Set up next iteration.
                ++iterations;

            }
            
            
            // Finally set up the solution vector
            ValueType meanBound = (maxValueBound + minValueBound) / storm::utility::convertNumber<ValueType>(2.0);
            storm::utility::vector::applyPointwise(*stepBoundedX, *stepBoundedStayProbs, x, [&meanBound] (ValueType const& v, ValueType const& p) { return v + p * meanBound; });
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            this->overallPerformedIterations += iterations;

            this->logIterations(converged, terminate, iterations);
            STORM_LOG_WARN_COND(hasMinValueBound && hasMaxValueBound, "Could not compute lower or upper bound within the given number of iterations.");
            STORM_LOG_INFO("Quick Power Iteration terminated with lower value bound " << minValueBound << " and upper value bound " << maxValueBound << ".");

            return converged;

        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearch(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            return solveEquationsRationalSearchHelper<double>(env, x, b);
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
        bool NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(Environment const& env, NativeLinearEquationSolver<ImpreciseType> const& impreciseSolver, storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX, std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A, std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const {
            
            ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().native().getPrecision());
            uint64_t maxIter = env.solver().native().getMaximalNumberOfIterations();
            bool relative = env.solver().native().getRelativeTerminationCriterion();
            auto multiplicationStyle = env.solver().native().getPowerMethodMultiplicationStyle();
            
            std::vector<ImpreciseType>* currentX = &x;
            std::vector<ImpreciseType>* newX = &tmpX;

            SolverStatus status = SolverStatus::InProgress;
            uint64_t overallIterations = 0;
            uint64_t valueIterationInvocations = 0;
            impreciseSolver.startMeasureProgress();
            while (status == SolverStatus::InProgress && overallIterations < maxIter) {
                // Perform value iteration with the current precision.
                typename NativeLinearEquationSolver<ImpreciseType>::PowerIterationResult result = impreciseSolver.performPowerIteration(currentX, newX, b, storm::utility::convertNumber<ImpreciseType, ValueType>(precision), relative, SolverGuarantee::LessOrEqual, overallIterations, maxIter, multiplicationStyle);
                
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
                    status = SolverStatus::Converged;
                    
                    TemporaryHelper<RationalType, ImpreciseType>::swapSolutions(rationalX, temporaryRational, x, currentX, newX);
                } else {
                    // Increase the precision.
                    precision = precision / 10;
                }
            }
            
            if (status == SolverStatus::InProgress && overallIterations == maxIter) {
                status = SolverStatus::MaximalIterationsExceeded;
            }
            
            this->logIterations(status == SolverStatus::Converged, status == SolverStatus::TerminatedEarly, overallIterations);
            
            return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
        }
     
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is imprecise.
            
            // Create a rational representation of the input so we can check for a proper solution later.
            storm::storage::SparseMatrix<storm::RationalNumber> rationalA = this->A->template toValueType<storm::RationalNumber>();
            std::vector<storm::RationalNumber> rationalX(x.size());
            std::vector<storm::RationalNumber> rationalB = storm::utility::vector::convertNumericVector<storm::RationalNumber>(b);
                        
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<storm::RationalNumber, ImpreciseType>(env, *this, rationalA, rationalX, rationalB, *this->A, x, b, *this->cachedRowVector);
            
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
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is exact and the same type is to be used for the imprecise part.
            
            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(env, *this, *this->A, x, b, *this->A, *this->cachedRowVector, b, x);
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }
        
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type NativeLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
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
                converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(env, impreciseSolver, *this->A, x, b, impreciseA, impreciseX, impreciseB, impreciseTmpX);
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
                converged = solveEquationsRationalSearchHelper<ValueType, ValueType>(env, *this, *this->A, x, b, *this->A, *this->cachedRowVector, b, x);
            }
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }

        template<typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        bool NativeLinearEquationSolver<ValueType>::sharpen(uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x, std::vector<RationalType> const& b, std::vector<RationalType>& tmp) {
            for (uint64_t p = 1; p <= precision; ++p) {
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
        NativeLinearEquationSolverMethod NativeLinearEquationSolver<ValueType>::getMethod(Environment const& env, bool isExactMode) const {
            // Adjust the method if none was specified and we want exact or sound computations
            auto method = env.solver().native().getMethod();
            
            if (isExactMode && method != NativeLinearEquationSolverMethod::RationalSearch) {
                if (env.solver().native().isMethodSetFromDefault()) {
                    method = NativeLinearEquationSolverMethod::RationalSearch;
                    STORM_LOG_INFO("Selecting '" + toString(method) + "' as the solution technique to guarantee exact results. If you want to override this, please explicitly specify a different method.");
                } else {
                    STORM_LOG_WARN("The selected solution method does not guarantee exact results.");
                }
            } else if (env.solver().isForceSoundness() && method != NativeLinearEquationSolverMethod::Power && method != NativeLinearEquationSolverMethod::RationalSearch && method != NativeLinearEquationSolverMethod::QuickPower) {
                if (env.solver().native().isMethodSetFromDefault()) {
                    method = NativeLinearEquationSolverMethod::Power;
                    STORM_LOG_INFO("Selecting '" + toString(method) + "' as the solution technique to guarantee sound results. If you want to override this, please explicitly specify a different method.");
                } else {
                    STORM_LOG_WARN("The selected solution method does not guarantee sound results.");
                }
            }
            return method;
        }

        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            switch(getMethod(env, storm::NumberTraits<ValueType>::IsExact)) {
                case NativeLinearEquationSolverMethod::SOR:
                    return this->solveEquationsSOR(env, x, b, storm::utility::convertNumber<ValueType>(env.solver().native().getSorOmega()));
                case NativeLinearEquationSolverMethod::GaussSeidel:
                    return this->solveEquationsSOR(env, x, b, storm::utility::one<ValueType>());
                case NativeLinearEquationSolverMethod::Jacobi:
                    return this->solveEquationsJacobi(env, x, b);
                case NativeLinearEquationSolverMethod::WalkerChae:
                    return this->solveEquationsWalkerChae(env, x, b);
                case NativeLinearEquationSolverMethod::Power:
                    if (env.solver().isForceSoundness()) {
                        return this->solveEquationsSoundPower(env, x, b);
                    } else {
                        return this->solveEquationsPower(env, x, b);
                    }
                case NativeLinearEquationSolverMethod::QuickPower:
                        return this->solveEquationsQuickSoundPower(env, x, b);
                case NativeLinearEquationSolverMethod::RationalSearch:
                    return this->solveEquationsRationalSearch(env, x, b);
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidEnvironmentException, "Unknown solving technique.");
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
        ValueType NativeLinearEquationSolver<ValueType>::multiplyRow(uint64_t const& rowIndex, std::vector<ValueType> const& x) const {
            return multiplier.multiplyRow(*A, rowIndex, x);
        }

        
        template<typename ValueType>
        LinearEquationSolverProblemFormat NativeLinearEquationSolver<ValueType>::getEquationProblemFormat(Environment const& env) const {
            auto method = getMethod(env, storm::NumberTraits<ValueType>::IsExact);
            if (method == NativeLinearEquationSolverMethod::Power || method == NativeLinearEquationSolverMethod::RationalSearch || method == NativeLinearEquationSolverMethod::QuickPower) {
                return LinearEquationSolverProblemFormat::FixedPointSystem;
            } else {
                return LinearEquationSolverProblemFormat::EquationSystem;
            }
        }
        
        template<typename ValueType>
        LinearEquationSolverRequirements NativeLinearEquationSolver<ValueType>::getRequirements(Environment const& env, LinearEquationSolverTask const& task) const {
            LinearEquationSolverRequirements requirements;
            if (task != LinearEquationSolverTask::Multiply) {
                if (env.solver().native().isForceBoundsSet()) {
                    requirements.requiresLowerBounds();
                    requirements.requiresUpperBounds();
                }
                auto method = getMethod(env, storm::NumberTraits<ValueType>::IsExact);
                if (method == NativeLinearEquationSolverMethod::Power && env.solver().isForceSoundness()) {
                    requirements.requireBounds();
                } else if (method == NativeLinearEquationSolverMethod::RationalSearch) {
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
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(Environment const& env, LinearEquationSolverTask const& task) const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>();
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> NativeLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolver<double>;
        template class NativeLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class NativeLinearEquationSolver<storm::RationalNumber>;
        template class NativeLinearEquationSolverFactory<storm::RationalNumber>;

#endif
    }
}
