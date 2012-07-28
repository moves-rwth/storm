#include "gtest/gtest.h"
#include "src/sparse/static_sparse_matrix.h"
#include "src/exceptions/invalid_argument.h"

TEST(StaticSparseMatrixTest, ZeroRowsTest) {
	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(0, 50);

	ASSERT_THROW(ssm->initialize(), mrmc::exceptions::invalid_argument);

	delete ssm;
}

TEST(StaticSparseMatrixTest, TooManyEntriesTest) {
	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(2, 10);

	ASSERT_THROW(ssm->initialize(), mrmc::exceptions::invalid_argument);

	delete ssm;
}

TEST(StaticSparseMatrixTest, addNextValueTest) {
	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(5, 1);

	ASSERT_NO_THROW(ssm->initialize());

	ASSERT_THROW(ssm->addNextValue(0, 1, 1), mrmc::exceptions::out_of_range);
	ASSERT_THROW(ssm->addNextValue(1, 0, 1), mrmc::exceptions::out_of_range);
	ASSERT_THROW(ssm->addNextValue(6, 1, 1), mrmc::exceptions::out_of_range);
	ASSERT_THROW(ssm->addNextValue(1, 6, 1), mrmc::exceptions::out_of_range);

	delete ssm;
}

TEST(StaticSparseMatrixTest, finalizeTest) {
	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(5, 5);

	ASSERT_NO_THROW(ssm->initialize());

	ASSERT_NO_THROW(ssm->addNextValue(1, 2, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 3, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 4, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 5, 1));

	ASSERT_THROW(ssm->finalize(), mrmc::exceptions::invalid_state);

	delete ssm;
}

TEST(StaticSparseMatrixTest, Test) {
	// 25 rows, 50 non zero entries
	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(25, 50);

	int values[50] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47, 48, 49
	};
	int position_row[50] = {
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, /* first row empty, one full row à 25 minus the diagonal entry */
		4, 4, /* one empty row, then first and last column */
		13, 13, 13, 13, /* a few empty rows, middle columns */
		24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24, 24, 24 /* second to last row */
	};
	int position_col[50] = {
		1, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, /* first row empty, one full row a 25 */
		1, 25, /* one empty row, then first and last column */
		16, 17, 18, 19, /* a few empty rows, middle columns */
		2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		14, 15, 16, 17, 18, 19, 20, 21, 22, 23 /* second to last row */
	};

	ASSERT_NO_THROW(ssm->initialize());

	for (int i = 0; i < 50; ++i) {
		ASSERT_NO_THROW(ssm->addNextValue(position_row[i], position_col[i], values[i]));
	}

	ASSERT_NO_THROW(ssm->finalize());

	int target;
	for (int i = 0; i < 50; ++i) {
		ASSERT_TRUE(ssm->getValue(position_row[i], position_col[i], &target));
		ASSERT_EQ(values[i], target);
	}

	// test for a few of the empty rows
	for (int row = 14; row < 24; ++row) {
		for (int col = 1; col <= 25; ++col) {
			target = 1;
			ASSERT_FALSE(ssm->getValue(row, col, &target));
			ASSERT_EQ(0, target);
		}
	}

	delete ssm;
}