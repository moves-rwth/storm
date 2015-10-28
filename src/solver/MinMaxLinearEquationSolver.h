#ifndef STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_

#include <vector>
#include <cstdint>
#include <memory>
#include "SolverSelectionOptions.h"
#include "src/storage/sparse/StateType.h"
#include "AllowEarlyTerminationCondition.h"
#include "OptimizationDirection.h"
#include "src/utility/vector.h"

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
            
            void setOptimizationDirection(OptimizationDirection d) {
                direction = convert(d);
            }
            
            void resetOptimizationDirection() {
                direction = OptimizationDirectionSetting::Unset;
            }
            
            
        protected:
            AbstractMinMaxLinearEquationSolver(double precision, bool relativeError, uint_fast64_t maximalIterations, bool trackPolicy, MinMaxTechniqueSelection prefTech);
            
            /// The direction in which to optimize, can be unset.
            OptimizationDirectionSetting direction;

            
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
                A(matrix), earlyTermination(new NoEarlyTerminationCondition<ValueType>()) {
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
                assert(isSet(this->direction));
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
            
            void setEarlyTerminationCriterion(std::unique_ptr<AllowEarlyTerminationCondition<ValueType>> v) {
                earlyTermination = std::move(v);
            }
            
            
        protected:
            
            std::vector<storm::storage::sparse::state_type> computePolicy(std::vector<ValueType>& x, std::vector<ValueType> const& b) const{
                std::vector<ValueType> xPrime(this->A.getRowCount());
                this->A.multiplyVectorWithMatrix(x, xPrime);
                storm::utility::vector::addVectors(xPrime, b, xPrime);
                std::vector<storm::storage::sparse::state_type> policy(x.size());
                std::vector<ValueType> reduced(x.size());
                storm::utility::vector::reduceVectorMinOrMax(convert(this->direction), xPrime, reduced, this->A.getRowGroupIndices(), &(policy));
                return policy;
            }
            
            storm::storage::SparseMatrix<ValueType> const& A;
            std::unique_ptr<AllowEarlyTerminationCondition<ValueType>> earlyTermination;
            
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_MINMAXLINEAREQUATIONSOLVER_H_ */
