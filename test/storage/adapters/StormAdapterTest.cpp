#include "gtest/gtest.h"

#include "gmm/gmm_matrix.h"
#include "src/adapters/StormAdapter.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "boost/integer/integer_mask.hpp"

#define STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE 5
#define STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE 5

TEST(StormAdapterTest, SimpleDenseSquareCopy) {
	// 5 rows
	gmm::csr_matrix<double> gmmSparseMatrix;

	/*
	 * As CSR_Matrix is read-only we have to prepare the data in this row_matrix and then do a copy.
	 */
	gmm::row_matrix<gmm::wsvector<double>> gmmPreMatrix(STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE, STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	
	double values[STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE];
	
	int row = 0;
	int col = 0;
	for (int i = 0; i < STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		
		gmmPreMatrix(row, col) = values[i];
		++col;
		if (col == STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	gmm::copy(gmmPreMatrix, gmmSparseMatrix);

	auto stormSparseMatrix = storm::adapters::StormAdapter::toStormSparseMatrix(gmmSparseMatrix);

	ASSERT_EQ(stormSparseMatrix->getRowCount(), STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(stormSparseMatrix->getColumnCount(), STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	ASSERT_EQ(stormSparseMatrix->getNonZeroEntryCount(), STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE);
	row = 0;
	col = 0;
	for (int i = 0; i < STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE; ++i) {
		ASSERT_EQ(values[i], stormSparseMatrix->getValue(row, col));
		++col;
		if (col == STORM_STORMADAPTERTEST_SIMPLEDENSESQUARECOPY_SIZE) {
			++row;
			col = 0; 
		}
	}
}

TEST(StormAdapterTest, SimpleSparseSquareCopy) {
	// 5 rows
	gmm::csr_matrix<double> gmmSparseMatrix;

	/*
	 * As CSR_Matrix is read-only we have to prepare the data in this row_matrix and then do a copy.
	 */
	gmm::row_matrix<gmm::wsvector<double>> gmmPreMatrix(STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE, STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	
	double values[STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE];

	int row = 0;
	int col = 0;

	bool everySecondElement = true;

	for (int i = 0; i < STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		values[i] = static_cast<double>(i + 1);
		if (everySecondElement) {
			gmmPreMatrix(row, col) = values[i];
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
	gmm::copy(gmmPreMatrix, gmmSparseMatrix);

	auto stormSparseMatrix = storm::adapters::StormAdapter::toStormSparseMatrix(gmmSparseMatrix);

	ASSERT_EQ(stormSparseMatrix->getRowCount(), STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(stormSparseMatrix->getColumnCount(), STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE);
	ASSERT_EQ(stormSparseMatrix->getNonZeroEntryCount(), (STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE + 1) / 2);


	row = 0;
	col = 0;
	everySecondElement = true;
	for (int i = 0; i < STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE * STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE; ++i) {
		if (everySecondElement) {
			ASSERT_EQ(values[i], stormSparseMatrix->getValue(row, col));
		}
		everySecondElement = !everySecondElement;
		++col;
		if (col == STORM_STORMADAPTERTEST_SIMPLESPARSESQUARECOPY_SIZE) {
			++row;
			col = 0;
		}
	}
}
