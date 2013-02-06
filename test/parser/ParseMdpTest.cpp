/*
 * ParseMdpTest.cpp
 *
 *  Created on: 14.01.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/MdpParser.h"
#include "src/utility/IoUtility.h"

TEST(ParseMdpTest, parseAndOutput) {
	storm::parser::MdpParser* mdpParser = nullptr;
	ASSERT_NO_THROW(mdpParser = new storm::parser::MdpParser(
			STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/mdp_general_input_01.tra",
			STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	std::shared_ptr<storm::models::Mdp<double>> mdp = mdpParser->getMdp();
	std::shared_ptr<storm::storage::SparseMatrix<double>> matrix = mdp->getTransitionProbabilityMatrix();

	ASSERT_EQ(mdp->getNumberOfStates(), 3);
	ASSERT_EQ(mdp->getNumberOfTransitions(), 11);
	ASSERT_EQ(matrix->getRowCount(), 2 * 3);
	ASSERT_EQ(matrix->getColumnCount(), 3);
	

	delete mdpParser;
}


