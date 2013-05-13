/*
 * ParseMdpTest.cpp
 *
 *  Created on: 14.01.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/NondeterministicModelParser.h"

TEST(ParseMdpTest, parseAndOutput) {
	storm::parser::NondeterministicModelParser* mdpParser = nullptr;
	ASSERT_NO_THROW(mdpParser = new storm::parser::NondeterministicModelParser(
			STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/mdp_general_input_01.tra",
			STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	std::shared_ptr<storm::models::Mdp<double>> mdp = mdpParser->getMdp();
	std::shared_ptr<storm::storage::SparseMatrix<double>> matrix = mdp->getTransitionMatrix();

	ASSERT_EQ(mdp->getNumberOfStates(), (uint_fast64_t)3);
	ASSERT_EQ(mdp->getNumberOfTransitions(), (uint_fast64_t)11);
	ASSERT_EQ(matrix->getRowCount(), (uint_fast64_t)(2 * 3));
	ASSERT_EQ(matrix->getColumnCount(), (uint_fast64_t)3);
	

	delete mdpParser;
}


