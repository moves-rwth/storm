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
            if (entry.first > matrix.getColumnCount()) {
                ASSERT_TRUE(false);
            }
        }
    }
}

TEST(SparseMatrix, Multiplication) {
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