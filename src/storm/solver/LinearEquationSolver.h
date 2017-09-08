#ifndef STORM_SOLVER_LINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_LINEAREQUATIONSOLVER_H_

#include <vector>
#include <memory>

#include "storm/solver/AbstractEquationSolver.h"
#include "storm/solver/OptimizationDirection.h"

#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace solver {
        
        enum class LinearEquationSolverOperation {
            SolveEquations, MultiplyRepeatedly
        };

        /*!
         * An interface that represents an abstract linear equation solver. In addition to solving a system of linear
         * equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<class ValueType>
        class LinearEquationSolver : public AbstractEquationSolver<ValueType> {
        public:
            LinearEquationSolver();

            virtual ~LinearEquationSolver() {
                // Intentionally left empty.
            }

            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) = 0;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) = 0;

            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param x The solution vector that has to be computed. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             *
             * @return true
             */
            virtual bool solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const = 0;

            /*!
             * Performs on matrix-vector multiplication x' = A*x + b.
             *
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param result The target vector into which to write the multiplication result. Its length must be equal
             * to the number of rows of A.
             */
            virtual void multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const = 0;
            
            /*!
             * Performs on matrix-vector multiplication x' = A*x + b and then minimizes/maximizes over the row groups
             * so that the resulting vector has the size of number of row groups of A.
             *
             * @param dir The direction for the reduction step.
             * @param rowGroupIndices A vector storing the row groups over which to reduce.
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param result The target vector into which to write the multiplication result. Its length must be equal
             * to the number of rows of A.
             * @param choices If given, the choices made in the reduction process are written to this vector.
             */
            virtual void multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices = nullptr) const;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x. Note that the
             * matrix A has to be given upon construction time of the solver object.
             *
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param n The number of times to perform the multiplication.
             */
            void repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n) const;
            
            /*!
             * Sets whether some of the generated data during solver calls should be cached.
             * This possibly increases the runtime of subsequent calls but also increases memory consumption.
             */
            void setCachingEnabled(bool value) const;
            
            /*!
             * Retrieves whether some of the generated data during solver calls should be cached.
             */
            bool isCachingEnabled() const;
            
            /*
             * Clears the currently cached data that has been stored during previous calls of the solver.
             */
            virtual void clearCache() const;
            
            /*!
             * Sets a lower bound for the solution that can potentially be used by the solver.
             */
            void setLowerBound(ValueType const& value);

            /*!
             * Sets an upper bound for the solution that can potentially be used by the solver.
             */
            void setUpperBound(ValueType const& value);

            /*!
             * Sets bounds for the solution that can potentially be used by the solver.
             */
            void setBounds(ValueType const& lower, ValueType const& upper);

        protected:
            // auxiliary storage. If set, this vector has getMatrixRowCount() entries.
            mutable std::unique_ptr<std::vector<ValueType>> cachedRowVector;
            
            // A lower bound if one was set.
            boost::optional<ValueType> lowerBound;

            // An upper bound if one was set.
            boost::optional<ValueType> upperBound;

        private:
            /*!
             * Retrieves the row count of the matrix associated with this solver.
             */
            virtual uint64_t getMatrixRowCount() const = 0;

            /*!
             * Retrieves the column count of the matrix associated with this solver.
             */
            virtual uint64_t getMatrixColumnCount() const = 0;
            
            /// Whether some of the generated data during solver calls should be cached.
            mutable bool cachingEnabled;
        };
        
        template<typename ValueType>
        class LinearEquationSolverFactory {
        public:
            /*!
             * Creates a new linear equation solver instance with the given matrix.
             *
             * @param matrix The matrix that defines the equation system.
             * @return A pointer to the newly created solver.
             */
            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const = 0;

            /*!
             * Creates a new linear equation solver instance with the given matrix. The caller gives up posession of the
             * matrix by calling this function.
             *
             * @param matrix The matrix that defines the equation system.
             * @return A pointer to the newly created solver.
             */
            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const;

            /*!
             * Creates a copy of this factory.
             */
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const = 0;
        };

        template<typename ValueType>
        class GeneralLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const override;
            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;

        private:
            template<typename MatrixType>
            std::unique_ptr<LinearEquationSolver<ValueType>> selectSolver(MatrixType&& matrix) const;
        };

#ifdef STORM_HAVE_CARL
        template<>
        class GeneralLinearEquationSolverFactory<storm::RationalNumber> : public LinearEquationSolverFactory<storm::RationalNumber> {
        public:
            virtual std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> create(storm::storage::SparseMatrix<storm::RationalNumber> const& matrix) const override;
            virtual std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> create(storm::storage::SparseMatrix<storm::RationalNumber>&& matrix) const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<storm::RationalNumber>> clone() const override;

        private:
            template<typename MatrixType>
            std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> selectSolver(MatrixType&& matrix) const;
        };

        template<>
        class GeneralLinearEquationSolverFactory<storm::RationalFunction> : public LinearEquationSolverFactory<storm::RationalFunction> {
        public:
            virtual std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> create(storm::storage::SparseMatrix<storm::RationalFunction> const& matrix) const override;
            virtual std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> create(storm::storage::SparseMatrix<storm::RationalFunction>&& matrix) const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<storm::RationalFunction>> clone() const override;

        private:
            template<typename MatrixType>
            std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> selectSolver(MatrixType&& matrix) const;
        };
#endif
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_LINEAREQUATIONSOLVER_H_ */
