#include "gtest/gtest.h"
#include "src/storage/SparseMatrix.h"

TEST(SparseMatrix, Iteration) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    for (uint_fast64_t row = 0; row < 10000; ++row) {
        for (uint_fast64_t column = 0; column < row; ++column) {
            ASSERT_NO_THROW(matrixBuilder.addNextValue(row, column, row+column));
        }
    }
    
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());
    
    for (uint_fast64_t row = 0; row < matrix.getRowCount(); ++row) {
        for (auto const& entry : matrix.getRow(row)) {
            // The following can never be true, but prevents the compiler from optimizing away the loop.
            if (entry.getColumn() > matrix.getColumnCount()) {
                ASSERT_TRUE(false);
            }
        }
    }
}

TEST(SparseMatrix, SparseMultiplication) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    for (uint_fast64_t row = 0; row < 100000; ++row) {
        ASSERT_NO_THROW(matrixBuilder.addNextValue(row, 0, storm::utility::one<double>()));
        ASSERT_NO_THROW(matrixBuilder.addNextValue(row, 1, storm::utility::one<double>()));
    }
    
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());
    
    std::vector<double> x(matrix.getColumnCount(), 1.0);
    std::vector<double> result(matrix.getRowCount());
    
    for (uint_fast64_t i = 0; i < 30000; ++i) {
        matrix.multiplyWithVector(x, result);
        
        // The following can never be true, but prevents the compiler from optimizing away the loop.
        if (result.size() > matrix.getRowCount()) {
            ASSERT_TRUE(false);
        }
    }
}

TEST(SparseMatrix, HalfSparseMultiplication) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    for (uint_fast64_t row = 0; row < 2000; ++row) {
        for (uint_fast64_t column = 0; column < row; ++column) {
            ASSERT_NO_THROW(matrixBuilder.addNextValue(row, column, row+column));
        }
    }

    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());
    
    std::vector<double> x(matrix.getColumnCount(), 1.0);
    std::vector<double> result(matrix.getRowCount());
    
    for (uint_fast64_t i = 0; i < 5000; ++i) {
        matrix.multiplyWithVector(x, result);
        
        // The following can never be true, but prevents the compiler from optimizing away the loop.
        if (result.size() > matrix.getRowCount()) {
            ASSERT_TRUE(false);
        }
    }
}

TEST(SparseMatrix, DenseMultiplication) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder;
    uint_fast64_t const size = 2000;
    for (uint_fast64_t row = 0; row < size; ++row) {
        for (uint_fast64_t column = 0; column < size; ++column) {
            ASSERT_NO_THROW(matrixBuilder.addNextValue(row, column, storm::utility::one<double>()));
        }
    }
    
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());
    
    std::vector<double> x(matrix.getColumnCount(), 1.0);
    std::vector<double> result(matrix.getRowCount());
    
    for (uint_fast64_t i = 0; i < 1000; ++i) {
        matrix.multiplyWithVector(x, result);
        
        // The following can never be true, but prevents the compiler from optimizing away the loop.
        if (result.size() > matrix.getRowCount()) {
            ASSERT_TRUE(false);
        }
    }
}