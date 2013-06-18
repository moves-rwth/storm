#include "gtest/gtest.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"
#include "src/adapters/EigenAdapter.h"
#include "src/adapters/StormAdapter.h"

TEST(SparseMatrixTest, ZeroRowsTest) {
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(0);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::UnInitialized);

	ASSERT_THROW(ssm->initialize(50), storm::exceptions::InvalidArgumentException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	delete ssm;
}

TEST(SparseMatrixTest, TooManyEntriesTest) {
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(2);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::UnInitialized);

	ASSERT_THROW(ssm->initialize(10), storm::exceptions::InvalidArgumentException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	delete ssm;
}

TEST(SparseMatrixTest, addNextValueTest) {
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(5);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::UnInitialized);

	ASSERT_NO_THROW(ssm->initialize(1));
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	ASSERT_THROW(ssm->addNextValue(-1, 1, 1), storm::exceptions::OutOfRangeException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	ASSERT_THROW(ssm->addNextValue(1, -1, 1), storm::exceptions::OutOfRangeException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	ASSERT_THROW(ssm->addNextValue(6, 1, 1), storm::exceptions::OutOfRangeException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	ASSERT_THROW(ssm->addNextValue(1, 6, 1), storm::exceptions::OutOfRangeException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	delete ssm;
}

TEST(SparseMatrixTest, finalizeTest) {
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(5);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::UnInitialized);

	ASSERT_NO_THROW(ssm->initialize(5));
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	ASSERT_NO_THROW(ssm->addNextValue(1, 2, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 3, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 4, 1));
	ASSERT_NO_THROW(ssm->addNextValue(1, 5, 1));
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	ASSERT_THROW(ssm->finalize(), storm::exceptions::InvalidStateException);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Error);

	delete ssm;
}

TEST(SparseMatrixTest, Test) {
	// 25 rows, 50 non zero entries
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(25);
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::UnInitialized);

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
	int row_sums[25] = {};
	for (int i = 0; i < 50; ++i) {
		row_sums[position_row[i]] += values[i];
	}

	ASSERT_NO_THROW(ssm->initialize(50));
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	for (int i = 0; i < 50; ++i) {
		ASSERT_NO_THROW(ssm->addNextValue(position_row[i], position_col[i], values[i]));
	}
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	ASSERT_NO_THROW(ssm->finalize());
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	int target;
	for (int i = 0; i < 50; ++i) {
		ASSERT_TRUE(ssm->getValue(position_row[i], position_col[i], &target));
		ASSERT_EQ(values[i], target);
	}

	// test for a few of the empty rows
	for (int row = 15; row < 24; ++row) {
		for (int col = 1; col <= 25; ++col) {
		   target = 1;
		   ASSERT_FALSE(ssm->getValue(row, col, &target));

		   ASSERT_EQ(0, target);
		}
	}
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	// Test Row Sums
	for (int row = 0; row < 25; ++row) {
		ASSERT_EQ(row_sums[row], ssm->getRowSum(row));
	}

	delete ssm;
}

TEST(SparseMatrixTest, ConversionFromDenseEigen_ColMajor_SparseMatrixTest) {
	// 10 rows, 100 non zero entries
	Eigen::SparseMatrix<int> esm(10, 10);
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 10; ++col) {
			esm.insert(row, col) = row * 10 + col;
		}
	}

	// make compressed, important for initialize()
	esm.makeCompressed();

	storm::storage::SparseMatrix<int> *ssm = nullptr;
	ASSERT_NO_THROW(ssm = storm::adapters::StormAdapter::toStormSparseMatrix(esm));

	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	int target = -1;
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 10; ++col) {
			ASSERT_TRUE(ssm->getValue(row, col, &target));
			ASSERT_EQ(target, row * 10 + col);
		}
	}
	
	delete ssm;
}

TEST(SparseMatrixTest, ConversionFromDenseEigen_RowMajor_SparseMatrixTest) {
	// 10 rows, 100 non zero entries
	Eigen::SparseMatrix<int, Eigen::RowMajor> esm(10, 10);
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 10; ++col) {
			esm.insert(row, col) = row * 10 + col;
		}
	}

	// make compressed, important for initialize()
	esm.makeCompressed();

	storm::storage::SparseMatrix<int> *ssm = nullptr;
	ASSERT_NO_THROW(ssm = storm::adapters::StormAdapter::toStormSparseMatrix(esm));

	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	int target = -1;
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 10; ++col) {
			ASSERT_TRUE(ssm->getValue(row, col, &target));
			ASSERT_EQ(target, row * 10 + col);
		}
	}
	
	delete ssm;
}

TEST(SparseMatrixTest, ConversionFromSparseEigen_ColMajor_SparseMatrixTest) {
	// 10 rows, 15 non zero entries
	Eigen::SparseMatrix<int> esm(10, 10);
	
	typedef Eigen::Triplet<int> IntTriplet;
	std::vector<IntTriplet> tripletList;
	tripletList.reserve(15);
	tripletList.push_back(IntTriplet(1, 0, 0));
	tripletList.push_back(IntTriplet(1, 1, 1));
	tripletList.push_back(IntTriplet(1, 2, 2));
	tripletList.push_back(IntTriplet(1, 3, 3));
	tripletList.push_back(IntTriplet(1, 4, 4));
	tripletList.push_back(IntTriplet(1, 5, 5));
	tripletList.push_back(IntTriplet(1, 6, 6));
	tripletList.push_back(IntTriplet(1, 7, 7));
	tripletList.push_back(IntTriplet(1, 8, 8));
	tripletList.push_back(IntTriplet(1, 9, 9));

	tripletList.push_back(IntTriplet(4, 3, 10));
	tripletList.push_back(IntTriplet(4, 6, 11));
	tripletList.push_back(IntTriplet(4, 9, 12));

	tripletList.push_back(IntTriplet(6, 0, 13));
	tripletList.push_back(IntTriplet(8, 9, 14));
	
	esm.setFromTriplets(tripletList.begin(), tripletList.end());

	// make compressed, important for initialize()
	esm.makeCompressed();

	storm::storage::SparseMatrix<int> *ssm = nullptr;
	ASSERT_NO_THROW(ssm = storm::adapters::StormAdapter::toStormSparseMatrix(esm));

	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	int target = -1;

	for (auto &coeff: tripletList) {
		ASSERT_TRUE(ssm->getValue(coeff.row(), coeff.col(), &target));
		ASSERT_EQ(target, coeff.value());
	}
	
	delete ssm;
}

TEST(SparseMatrixTest, ConversionFromSparseEigen_RowMajor_SparseMatrixTest) {
	// 10 rows, 15 non zero entries
	Eigen::SparseMatrix<int, Eigen::RowMajor> esm(10, 10);
	
	typedef Eigen::Triplet<int> IntTriplet;
	std::vector<IntTriplet> tripletList;
	tripletList.reserve(15);
	tripletList.push_back(IntTriplet(1, 0, 15));
	tripletList.push_back(IntTriplet(1, 1, 1));
	tripletList.push_back(IntTriplet(1, 2, 2));
	tripletList.push_back(IntTriplet(1, 3, 3));
	tripletList.push_back(IntTriplet(1, 4, 4));
	tripletList.push_back(IntTriplet(1, 5, 5));
	tripletList.push_back(IntTriplet(1, 6, 6));
	tripletList.push_back(IntTriplet(1, 7, 7));
	tripletList.push_back(IntTriplet(1, 8, 8));
	tripletList.push_back(IntTriplet(1, 9, 9));

	tripletList.push_back(IntTriplet(4, 3, 10));
	tripletList.push_back(IntTriplet(4, 6, 11));
	tripletList.push_back(IntTriplet(4, 9, 12));

	tripletList.push_back(IntTriplet(6, 0, 13));
	tripletList.push_back(IntTriplet(8, 9, 14));
	
	esm.setFromTriplets(tripletList.begin(), tripletList.end());

	// make compressed, important for initialize()
	esm.makeCompressed();

	storm::storage::SparseMatrix<int> *ssm = nullptr;
	ASSERT_NO_THROW(ssm = storm::adapters::StormAdapter::toStormSparseMatrix(esm));

	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	int target = -1;
	for (auto &coeff: tripletList) {
		bool retVal = ssm->getValue(coeff.row(), coeff.col(), &target);
		ASSERT_TRUE(retVal);
		ASSERT_EQ(target, coeff.value());
	}
	
	delete ssm;
}

TEST(SparseMatrixTest, ConversionToSparseEigen_RowMajor_SparseMatrixTest) {
	int values[100];
	storm::storage::SparseMatrix<int> *ssm = new storm::storage::SparseMatrix<int>(10);

	for (uint_fast32_t i = 0; i < 100; ++i) {
		values[i] = i;
	}

	ASSERT_NO_THROW(ssm->initialize(100));
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	for (uint_fast32_t row = 0; row < 10; ++row) {
		for (uint_fast32_t col = 0; col < 10; ++col) {
			ASSERT_NO_THROW(ssm->addNextValue(row, col, values[row * 10 + col]));
		}
	}
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::Initialized);

	ASSERT_NO_THROW(ssm->finalize());
	ASSERT_EQ(ssm->getState(), storm::storage::SparseMatrix<int>::MatrixStatus::ReadReady);

	Eigen::SparseMatrix<int, Eigen::RowMajor, int_fast32_t>* esm = storm::adapters::EigenAdapter::toEigenSparseMatrix<int>(*ssm);

	for (uint_fast32_t row = 0; row < 10; ++row) {
		for (uint_fast32_t col = 0; col < 10; ++col) {
			ASSERT_EQ(values[row * 10 + col], esm->coeff(row, col));
		}
	}
	
	delete esm;
	delete ssm;
}

#ifdef STORM_USE_TBB
TEST(SparseMatrixTest, TBBMatrixMultTest) {
	storm::storage::SparseMatrix<double> matrix(10, 10);
	ASSERT_NO_THROW(matrix.initialize(100));
	double values[100];
	std::vector<double> x;
	x.resize(10);
	for (uint_fast64_t i = 0; i < 100; ++i) {
		values[i] = i + 1.0;
		if (i < 10) {
			x[i] = 1.0;
		}
		matrix.addNextValue(i / 10, i % 10, values[i]);
	}
	ASSERT_NO_THROW(matrix.finalize());
	ASSERT_EQ(matrix.getState(), storm::storage::SparseMatrix<double>::MatrixStatus::ReadReady);

	std::vector<double> result;
	result.resize(10);

	matrix.multiplyWithVector(x, result);

	for (uint_fast64_t i = 0; i < 10; ++i) {
		double rowSum = 0.0;
		for (uint_fast64_t j = 0; j < 10; ++j) {
			rowSum += values[10 * i + j];
		}
		ASSERT_EQ(rowSum, result.at(i));
	}
}

TEST(SparseMatrixTest, TBBSparseMatrixMultTest) {
	storm::storage::SparseMatrix<double> matrix(10, 10);
	ASSERT_NO_THROW(matrix.initialize(50));
	double values[100];
	std::vector<double> x;
	x.resize(10);
	for (uint_fast64_t i = 0; i < 100; ++i) {
		if (i % 2 == 0) {
			values[i] = i + 1.0;
			matrix.addNextValue(i / 10, i % 10, i + 1.0);
		} else {
			values[i] = 0.0;
		}

		if (i < 10) {
			x[i] = 1.0;
		}
		
	}
	ASSERT_NO_THROW(matrix.finalize());
	ASSERT_EQ(matrix.getState(), storm::storage::SparseMatrix<double>::MatrixStatus::ReadReady);

	std::vector<double> result;
	result.resize(10);

	matrix.multiplyWithVector(x, result);

	for (uint_fast64_t i = 0; i < 10; ++i) {
		double rowSum = 0.0;
		for (uint_fast64_t j = 0; j < 10; ++j) {
			rowSum += values[10 * i + j];
		}
		ASSERT_EQ(rowSum, result.at(i));
	}
}

#endif // STORM_USE_TBB
