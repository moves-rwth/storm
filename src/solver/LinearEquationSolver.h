#ifndef STORM_SOLVER_LINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_LINEAREQUATIONSOLVER_H_

#include <vector>
#include <memory>

#include "src/solver/AbstractEquationSolver.h"

#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace solver {
        
        /*!
         * An interface that represents an abstract linear equation solver. In addition to solving a system of linear
         * equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<class ValueType>
        class LinearEquationSolver : public AbstractEquationSolver<ValueType> {
        public:
            virtual ~LinearEquationSolver() {
                // Intentionally left empty.
            }
            
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param x The solution vector that has to be computed. Its length must be equal to the number of rows of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const = 0;
            
            /*!
             * Performs on matrix-vector multiplication x' = A*x + b.
             *
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param result The target vector into which to write the multiplication result. Its length must be equal
             * to the number of rows of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             */
            virtual void performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType>& result, std::vector<ValueType> const* b = nullptr) const = 0;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x. Note that the
             * matrix A has to be given upon construction time of the solver object.
             *
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            virtual void performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b = nullptr, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const;
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
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_LINEAREQUATIONSOLVER_H_ */
