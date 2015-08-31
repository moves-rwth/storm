#ifndef STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_

#include <vector>
#include <cstdint>
#include <memory>
#include "SolverSelectionOptions.h"
#include "src/storage/sparse/StateType.h"
#include "AllowEarlyTerminationCondition.h"

namespace storm {
    namespace storage {
        template<typename T> class SparseMatrix;
    }
    
    namespace solver {
        /**
         * Abstract base class which provides value-type independent helpers.
         */
        class AbstractMinMaxLinearEquationSolver {
        
        public:
            void setPolicyTracking(bool setToTrue=true);
            
            std::vector<storm::storage::sparse::state_type> getPolicy() const;
            
        protected:
            AbstractMinMaxLinearEquationSolver(double precision, bool relativeError, uint_fast64_t maximalIterations, bool trackPolicy, MinMaxTechniqueSelection prefTech);
            
            /// The required precision for the iterative methods.
            double precision;
            
            /// Sets whether the relative or absolute error is to be considered for convergence detection.
            bool relative;
            
            /// The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;

            /// Whether value iteration or policy iteration is to be used.
            bool useValueIteration;
            
            /// Whether we track the policy we generate.
            bool trackPolicy;
            
            /// 
            mutable std::vector<storm::storage::sparse::state_type> policy;
        };
        
        /*!
         * A interface that represents an abstract nondeterministic linear equation solver. In addition to solving
         * linear equation systems involving min/max operators, repeated matrix-vector multiplication functionality is
         * provided.
         */
        template<class ValueType>
        class MinMaxLinearEquationSolver : public AbstractMinMaxLinearEquationSolver {
        protected:
            MinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& matrix, double precision, bool relativeError, uint_fast64_t maxNrIterations, bool trackPolicy, MinMaxTechniqueSelection prefTech) :
                AbstractMinMaxLinearEquationSolver(precision, relativeError, maxNrIterations, trackPolicy, prefTech),
                earlyTermination(new NoEarlyTerminationCondition<ValueType>()), A(matrix) {
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
             * @param minimize If set, all the value of a group of rows is the taken as the minimum over all rows and as
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
            virtual void solveEquationSystem(bool minimize, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr, std::vector<ValueType>* newX = nullptr) const = 0;
            
            /*!
             * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes
             * x[i+1] = min/max(A*x[i] + b) until x[n], where x[0] = x. After each multiplication and addition, the
             * minimal/maximal value out of each row group is selected to reduce the resulting vector to obtain the
             * vector for the next iteration. Note that the matrix A has to be given upon construction time of the
             * solver object.
             *
             * @param minimize If set, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The initial vector that is to be multiplied with the matrix. This is also the output parameter,
             * i.e. after the method returns, this vector will contain the computed values.
             * @param b If not null, this vector is added after each multiplication.
             * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             * @return The result of the repeated matrix-vector multiplication as the content of the vector x.
             */
            virtual void performMatrixVectorMultiplication(bool minimize, std::vector<ValueType>& x, std::vector<ValueType>* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const = 0;
            
            
            void setEarlyTerminationCondition(std::unique_ptr<AllowEarlyTerminationCondition<ValueType>> v) {
                earlyTermination = std::move(v);
            }
            
        protected:
            std::unique_ptr<AllowEarlyTerminationCondition<ValueType>> earlyTermination;
            storm::storage::SparseMatrix<ValueType> const& A;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_ */
