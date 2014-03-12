#include "gtest/gtest.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/OutOfRangeException.h"

#include "storm-config.h"

#ifdef STORM_HAVE_CUDAFORSTORM

#include "cudaForStorm.h"

TEST(CudaPlugin, CreationWithDimensions) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(4, 4, 10);
    ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 1, 1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 3, -1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 0, 8.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 1, 7.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 2, -5.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 3, 2.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 0, 2.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 1, 2.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 2, 4.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 3, 4.0));

    
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());
    
    ASSERT_EQ(4, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(10, matrix.getEntryCount());

	std::vector<double> x({0, 4, 1, 1});
	std::vector<double> b({0, 0, 0, 0});

	ASSERT_NO_THROW(basicValueIteration_spmv_uint64_double(matrix.getColumnCount(), matrix.__internal_getRowIndications(), matrix.__internal_getColumnsAndValues(), x, b));

	ASSERT_EQ(b.at(0), 3);
	ASSERT_EQ(b.at(1), 25);
	ASSERT_EQ(b.at(2), 16);
	ASSERT_EQ(b.at(3), 0);
}

TEST(CudaPlugin, VerySmall) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder(2, 2, 2);
	ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 0, 1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 1, 2.0));

	storm::storage::SparseMatrix<double> matrix;
	ASSERT_NO_THROW(matrix = matrixBuilder.build());

	ASSERT_EQ(2, matrix.getRowCount());
	ASSERT_EQ(2, matrix.getColumnCount());
	ASSERT_EQ(2, matrix.getEntryCount());

	std::vector<double> x({ 4.0, 8.0 });
	std::vector<double> b({ 0.0, 0.0 });

	ASSERT_NO_THROW(basicValueIteration_spmv_uint64_double(matrix.getColumnCount(), matrix.__internal_getRowIndications(), matrix.__internal_getColumnsAndValues(), x, b));

	ASSERT_EQ(b.at(0), 4.0);
	ASSERT_EQ(b.at(1), 16.0);
}

#endif