#include "gtest/gtest.h"

#include "gmm/gmm_matrix.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/exceptions/InvalidArgumentException.h"
#include <cstdint>

#define STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE 5
#define STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE 5

double getValue(gmm::csr_matrix<double> const& gmmSparseMatrix, uint_fast64_t row, uint_fast64_t col) {
	uint_fast64_t rowStart = gmmSparseMatrix.jc.at(row);
	uint_fast64_t rowEnd = gmmSparseMatrix.jc.at(row + 1);
	while (rowStart < rowEnd) {
		if (gmmSparseMatrix.ir.at(rowStart) == col) {
			return gmmSparseMatrix.pr.at(rowStart);
		}
		if (gmmSparseMatrix.ir.at(rowStart) > col) {
			break;
		}
		++rowStart;
	}

	return 0.0;
}

TEST(GmmAdapterTest, SimpleDenseSquareCopy) {
	// 5 rows
	storm::storage::SparseMatrix<double> sm(STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	
	double values[STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE];
	sm.initialize(STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);

	int row = 0;
	int col = 0;
	for (int i = 0; i < STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		
		sm.addNextValue(row, col, values[i]);
		++col;
		if (col == STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	sm.finalize();

	auto gsm = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix(sm);

	ASSERT_EQ(gsm->nrows(), STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(gsm->ncols(), STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(gmm::nnz(*gsm), STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	row = 0;
	col = 0;
	for (int i = 0; i < STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		ASSERT_EQ(values[i], getValue(*gsm, row, col));
		++col;
		if (col == STORM_GMMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
}

TEST(GmmAdapterTest, SimpleSparseSquareCopy) {
	// 5 rows
	storm::storage::SparseMatrix<double> sm(STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	
	double values[STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE];
	sm.initialize((STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE + 1) / 2);

	int row = 0;
	int col = 0;

	bool everySecondElement = true;

	for (int i = 0; i < STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		if (everySecondElement) {
			sm.addNextValue(row, col, values[i]);
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	sm.finalize();

	auto gsm = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix(sm);

	ASSERT_EQ(gsm->nrows(), STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(gsm->ncols(), STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(gmm::nnz(*gsm), (STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE + 1) / 2);

	row = 0;
	col = 0;
	everySecondElement = true;
	for (int i = 0; i < STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		if (everySecondElement) {
			ASSERT_EQ(values[i], getValue(*gsm, row, col));
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_GMMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
}
