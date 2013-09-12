#include "gtest/gtest.h"

#include "Eigen/Sparse"
#include "src/exceptions/InvalidArgumentException.h"
#include <cstdint>

TEST(EigenSparseMatrixTest, BasicReadWriteTest) {
	// 25 rows, 50 non zero entries
	Eigen::SparseMatrix<int, 1> esm(25, 25);
	
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
		2, 2, 2, 2, /* first row empty, one full row 25 minus the diagonal entry */
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

	typedef Eigen::Triplet<double> ETd;
	std::vector<ETd> tripletList;
	tripletList.reserve(50);

	for (int i = 0; i < 50; ++i) {
		ASSERT_NO_THROW(tripletList.push_back(ETd(position_row[i] - 1, position_col[i] - 1, values[i])));
	}

	ASSERT_NO_THROW(esm.setFromTriplets(tripletList.begin(), tripletList.end()));

	for (int i = 0; i < 50; ++i) {
		Eigen::SparseMatrix<int, 1>::InnerIterator it(esm, position_row[i] - 1);
		ASSERT_EQ(values[i], esm.coeff(position_row[i] - 1, position_col[i] - 1));
	}

	// test for a few of the empty rows
	for (int row = 15; row < 24; ++row) {
		for (int col = 1; col <= 25; ++col) {
		   ASSERT_EQ(0, esm.coeff(row - 1, col - 1));
		}
	}
}

TEST(EigenSparseMatrixTest, BoundaryTest) {
	Eigen::SparseMatrix<int, 1> esm(10, 10);
	esm.reserve(100);
	int values[100];
	for (uint_fast32_t i = 0; i < 100; ++i) {
		values[i] = i;
	}

	for (uint_fast32_t row = 0; row < 10; ++row) {
		for (uint_fast32_t col = 0; col < 10; ++col) {
			ASSERT_NO_THROW(esm.insert(row, col) = values[row * 10 + col]);
		}
	}

	for (uint_fast32_t row = 0; row < 10; ++row) {
		for (uint_fast32_t col = 0; col < 10; ++col) {
			ASSERT_EQ(values[row * 10 + col], esm.coeff(row, col));
		}
	}
}
