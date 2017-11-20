#ifndef STORM_SOLVER_LINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_LINEAREQUATIONSOLVER_H_

#include <vector>
#include <memory>

#include "storm/solver/AbstractEquationSolver.h"
#include "storm/solver/MultiplicationStyle.h"
#include "storm/solver/LinearEquationSolverProblemFormat.h"
#include "storm/solver/LinearEquationSolverRequirements.h"
#include "storm/solver/OptimizationDirection.h"

#include "storm/utility/VectorHelper.h"

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
             * If the solver expects the equation system format, it solves Ax = b. If it it expects a fixed point
             * format, it solves Ax + b = x. In both versions, the matrix A is required to be square and the problem
             * is required to have a unique solution. The solution will be written to the vector x. Note that the matrix
             * A has to be given upon construction time of the solver object.
             *
             * @param x The solution vector that has to be computed. Its length must be equal to the number of rows of A.
             * @param b The vector b. Its length must be equal to the number of rows of A.
             *
             * @return true
             */
            bool solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

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
             * Retrieves whether this solver offers the gauss-seidel style multiplications.
             */
            virtual bool supportsGaussSeidelMultiplication() const;
            
            /*!
             * Performs on matrix-vector multiplication x' = A*x + b. It does so in a gauss-seidel style, i.e. reusing
             * the new x' components in the further multiplication.
             *
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             */
            virtual void multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const;
            
            /*!
             * Performs on matrix-vector multiplication x' = A*x + b and then minimizes/maximizes over the row groups
             * so that the resulting vector has the size of number of row groups of A. It does so in a gauss-seidel
             * style, i.e. reusing the new x' components in the further multiplication.
             *
             * @param dir The direction for the reduction step.
             * @param rowGroupIndices A vector storing the row groups over which to reduce.
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param choices If given, the choices made in the reduction process are written to this vector.
             */
            virtual void multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices = nullptr) const;
            
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
             * Retrieves the format in which this solver expects to solve equations. If the solver expects the equation
             * system format, it solves Ax = b. If it it expects a fixed point format, it solves Ax + b = x.
             */
            virtual LinearEquationSolverProblemFormat getEquationProblemFormat() const = 0;
            
            /*!
             * Retrieves the requirements of the solver under the current settings. Note that these requirements only
             * apply to solving linear equations and not to the matrix vector multiplications.
             */
            virtual LinearEquationSolverRequirements getRequirements() const;
            
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
            
        protected:
            virtual bool internalSolveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const = 0;
                        
            // auxiliary storage. If set, this vector has getMatrixRowCount() entries.
            mutable std::unique_ptr<std::vector<ValueType>> cachedRowVector;
            
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
            
            /// An object that can be used to reduce vectors.
            storm::utility::VectorHelper<ValueType> vectorHelper;
        };
        
        enum class EquationSolverType;
        
        template<typename ValueType>
        class LinearEquationSolverFactory {
        public:
            virtual ~LinearEquationSolverFactory() = default;

            /*!
             * Creates a new linear equation solver instance with the given matrix.
             *
             * @param matrix The matrix that defines the equation system.
             * @return A pointer to the newly created solver.
             */
            std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType> const& matrix) const;

            /*!
             * Creates a new linear equation solver instance with the given matrix. The caller gives up posession of the
             * matrix by calling this function.
             *
             * @param matrix The matrix that defines the equation system.
             * @return A pointer to the newly created solver.
             */
            std::unique_ptr<LinearEquationSolver<ValueType>> create(storm::storage::SparseMatrix<ValueType>&& matrix) const;

            /*!
             * Creates an equation solver with the current settings, but without a matrix.
             */
            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create() const = 0;

            /*!
             * Creates a copy of this factory.
             */
            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const = 0;

            /*!
             * Retrieves the problem format that the solver expects if it was created with the current settings.
             */
            virtual LinearEquationSolverProblemFormat getEquationProblemFormat() const;
            
            /*!
             * Retrieves the requirements of the solver if it was created with the current settings. Note that these
             * requirements only apply to solving linear equations and not to the matrix vector multiplications.
             */
            LinearEquationSolverRequirements getRequirements() const;
        };

        template<typename ValueType>
        class GeneralLinearEquationSolverFactory : public LinearEquationSolverFactory<ValueType> {
        public:
            GeneralLinearEquationSolverFactory();
            GeneralLinearEquationSolverFactory(EquationSolverType const& equationSolver);
            
            using LinearEquationSolverFactory<ValueType>::create;

            virtual std::unique_ptr<LinearEquationSolver<ValueType>> create() const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<ValueType>> clone() const override;
            
        private:
            /*!
             * Sets the equation solver type.
             */
            void setEquationSolverType(EquationSolverType const& equationSolver);
            
            // The equation solver type.
            EquationSolverType equationSolver;
        };

#ifdef STORM_HAVE_CARL
        template<>
        class GeneralLinearEquationSolverFactory<storm::RationalNumber> : public LinearEquationSolverFactory<storm::RationalNumber> {
        public:
            using LinearEquationSolverFactory<storm::RationalNumber>::create;

            virtual std::unique_ptr<LinearEquationSolver<storm::RationalNumber>> create() const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<storm::RationalNumber>> clone() const override;
        };

        template<>
        class GeneralLinearEquationSolverFactory<storm::RationalFunction> : public LinearEquationSolverFactory<storm::RationalFunction> {
        public:
            using LinearEquationSolverFactory<storm::RationalFunction>::create;

            virtual std::unique_ptr<LinearEquationSolver<storm::RationalFunction>> create() const override;

            virtual std::unique_ptr<LinearEquationSolverFactory<storm::RationalFunction>> clone() const override;
        };
#endif
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_LINEAREQUATIONSOLVER_H_ */
