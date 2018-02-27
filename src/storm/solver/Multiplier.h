#pragma once

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MultiplicationStyle.h"

namespace storm {
    
    class Environment;
    
    namespace storage {
        template<typename ValueType>
        class SparseMatrix;
    }
    
    namespace solver {
        
        template<typename ValueType>
        class Multiplier {
        public:
            
            Multiplier(storm::storage::SparseMatrix<ValueType> const& matrix);
            
            /*!
             * Retrieves whether Gauss Seidel style multiplications are allowed.
             */
            bool getAllowGaussSeidelMultiplications() const;
            
            /*!
             * Sets whether Gauss Seidel style multiplications are allowed.
             */
            void setAllowGaussSeidelMultiplications(bool value);
            
            /*!
             * Returns the multiplication style performed by this multiplier
             */
            virtual MultiplicationStyle getMultiplicationStyle() const = 0;
            
            /*
             * Clears the currently cached data of this multiplier in order to free some memory.
             */
            virtual void clearCache() const;
            
            /*!
             * Performs a matrix-vector multiplication x' = A*x + b.
             *
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param result The target vector into which to write the multiplication result. Its length must be equal
             * to the number of rows of A. Can be the same as the x vector.
             */
            virtual void multiply(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const = 0;
            
            /*!
             * Performs a matrix-vector multiplication x' = A*x + b and then minimizes/maximizes over the row groups
             * so that the resulting vector has the size of number of row groups of A.
             *
             * @param dir The direction for the reduction step.
             * @param rowGroupIndices A vector storing the row groups over which to reduce.
             * @param x The input vector with which to multiply the matrix. Its length must be equal
             * to the number of columns of A.
             * @param b If non-null, this vector is added after the multiplication. If given, its length must be equal
             * to the number of rows of A.
             * @param result The target vector into which to write the multiplication result. Its length must be equal
             * to the number of rows of A. Can be the same as the x vector.
             * @param choices If given, the choices made in the reduction process are written to this vector.
             */
            virtual void multiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices = nullptr) const = 0;
            
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
            void repeatedMultiply(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n) const;
            
            /*!
             * Performs repeated matrix-vector multiplication x' = A*x + b and then minimizes/maximizes over the row groups
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
             * @param n The number of times to perform the multiplication.
             */
            void repeatedMultiplyAndReduce(Environment const& env, OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint64_t n) const;
  
            /*!
             * Multiplies the row with the given index with x and adds the given offset
             * @param rowIndex The index of the considered row
             * @param x The input vector with which the row is multiplied
             */
            virtual ValueType multiplyRow(Environment const& env, uint64_t const& rowIndex, std::vector<ValueType> const& x, ValueType const& offset) const = 0;
            
        protected:
            mutable std::unique_ptr<std::vector<ValueType>> cachedVector;
            storm::storage::SparseMatrix<ValueType> const& matrix;
        private:
            bool allowGaussSeidelMultiplications;
        };
        
        template<typename ValueType>
        class MultiplierFactory {
        public:
            MultiplierFactory() = default;
            ~MultiplierFactory() = default;

            std::unique_ptr<Multiplier<ValueType>> create(Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix);
            
            
        };
        
    }
}
