#ifndef STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_

#include <vector>
#include <cstdint>
#include <memory>

#include <boost/optional.hpp>

#include "src/solver/AbstractEquationSolver.h"
#include "src/solver/SolverSelectionOptions.h"
#include "src/storage/sparse/StateType.h"
#include "src/storage/TotalScheduler.h"
#include "src/solver/OptimizationDirection.h"

#include "src/exceptions/InvalidSettingsException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace storage {
        template<typename T> class SparseMatrix;
    }
    
    namespace solver {
        
        /**
         * Abstract base class of min-max linea equation solvers.
         */
        template<typename ValueType>
        class AbstractMinMaxLinearEquationSolver : public AbstractEquationSolver<ValueType> {
        public:
            void setTrackScheduler(bool trackScheduler = true);
            bool isTrackSchedulerSet() const;
            bool hasScheduler() const;
            
            storm::storage::TotalScheduler const& getScheduler() const;
            storm::storage::TotalScheduler& getScheduler();
            
            void setOptimizationDirection(OptimizationDirection d);
            void resetOptimizationDirection();
                        
        protected:
            AbstractMinMaxLinearEquationSolver(double precision, bool relativeError, uint_fast64_t maximalIterations, bool trackScheduler, MinMaxTechniqueSelection prefTech);
            
            // The direction in which to optimize, can be unset.
            OptimizationDirectionSetting direction;

            // The required precision for the iterative methods.
            double precision;
            
            // Sets whether the relative or absolute error is to be considered for convergence detection.
            bool relative;
            
            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;

            // Whether value iteration or policy iteration is to be used.
            bool useValueIteration;
            
            // Whether we generate a scheduler during solving.
            bool trackScheduler;
            
            // The scheduler (if it could be successfully generated).
            mutable boost::optional<std::unique_ptr<storm::storage::TotalScheduler>> scheduler;
        };
        
        /*!
         * A interface that represents an abstract nondeterministic linear equation solver. In addition to solving
         * linear equation systems involving min/max operators, repeated matrix-vector multiplication functionality is
         * provided.
         */
        template<class ValueType>
        class MinMaxLinearEquationSolver : public AbstractMinMaxLinearEquationSolver<ValueType> {
        protected:
            MinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& matrix, double precision, bool relativeError, uint_fast64_t maxNrIterations, bool trackScheduler, MinMaxTechniqueSelection prefTech) : AbstractMinMaxLinearEquationSolver<ValueType>(precision, relativeError, maxNrIterations, trackScheduler, prefTech), A(matrix) {
                // Intentionally left empty.
            }
        
        public:
            virtual ~MinMaxLinearEquationSolver() {
                // Intentionally left empty.
            }
            
            /*!
             * Solves the equation system x = min/max(A*x + b) given by the parameters. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param d For minimum, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The solution vector x. The initial values of x represent a guess of the real values to the
             * solver, but may be ignored.
             * @param b The vector to add after matrix-vector multiplication.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             * @param newX If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the length of the vector x (and thus to the number of columns of A).
             * @return The solution vector x of the system of linear equations as the content of the parameter x.
             */
            virtual void solveEquationSystem(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const = 0;
            
            /*!
             * As solveEquationSystem with an optimization-direction, but this uses the internally set direction.
             * Can only be called after the direction has been set.
             */
            virtual void solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const {
                STORM_LOG_ASSERT(isSet(this->direction), "Direction not set.");
                solveEquationSystem(convert(this->direction), x, b, multiplyResult, newX);
            }
            
            /*!
             * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes
             * x[i+1] = min/max(A*x[i] + b) until x[n], where x[0] = x. After each multiplication and addition, the
             * minimal/maximal value out of each row group is selected to reduce the resulting vector to obtain the
             * vector for the next iteration. Note that the matrix A has to be given upon construction time of the
             * solver object.
             *
             * @param d For minimum, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The initial vector that is to be multiplied with the matrix. This is also the output parameter,
             * i.e. after the method returns, this vector will contain the computed values.
             * @param b If not null, this vector is added after each multiplication.
             * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             * @return The result of the repeated matrix-vector multiplication as the content of the vector x.
             */
            virtual void performMatrixVectorMultiplication(OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const = 0;
            
            /*!
             * As performMatrixVectorMultiplication with an optimization-direction, but this uses the internally set direction.
             * Can only be called if the internal direction has been set.
             */
            virtual void performMatrixVectorMultiplication( std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const {
                return performMatrixVectorMultiplication(convert(this->direction), x, b, n, multiplyResult);
            }
            
        protected:
            storm::storage::SparseMatrix<ValueType> const& A;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_ */
