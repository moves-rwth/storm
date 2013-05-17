#include "gtest/gtest.h"

#include "Eigen/Sparse"
#include "src/adapters/EigenAdapter.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "boost/integer/integer_mask.hpp"

#define STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE 5
#define STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE 5

TEST(EigenAdapterTest, SimpleDenseSquareCopy) {
	// 5 rows
	storm::storage::SparseMatrix<double> sm(STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	
	double values[STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE];
	sm.initialize(STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);

	int row = 0;
	int col = 0;
	for (int i = 0; i < STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		
		sm.addNextValue(row, col, values[i]);
		++col;
		if (col == STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	sm.finalize();

	auto esm = storm::adapters::EigenAdapter::toEigenSparseMatrix(sm);

	ASSERT_EQ(esm->rows(), STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(esm->cols(), STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(esm->nonZeros(), STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);

	row = 0;
	col = 0;
	for (int i = 0; i < STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		ASSERT_EQ(values[i], esm->coeff(row, col));
		++col;
		if (col == STORM_EIGENADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}

	delete esm;
}

TEST(EigenAdapterTest, SimpleSparseSquareCopy) {
	// 5 rows
	storm::storage::SparseMatrix<double> sm(STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	
	double values[STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE];
	sm.initialize((STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE + 1) / 2);

	int row = 0;
	int col = 0;

	bool everySecondElement = true;

	for (int i = 0; i < STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		if (everySecondElement) {
			sm.addNextValue(row, col, values[i]);
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	sm.finalize();

	auto esm = storm::adapters::EigenAdapter::toEigenSparseMatrix(sm);

	ASSERT_EQ(esm->rows(), STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(esm->cols(), STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(esm->nonZeros(), (STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE + 1) / 2);

	row = 0;
	col = 0;
	everySecondElement = true;
	for (int i = 0; i < STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		if (everySecondElement) {
			ASSERT_EQ(values[i], esm->coeff(row, col));
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_EIGENADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}

	delete esm;
}
