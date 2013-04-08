/*
 * ReadTraFileTest.cpp
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/storage/SparseMatrix.h"
#include "src/parser/DeterministicSparseTransitionParser.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

TEST(ReadTraFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

/* The following test case is based on one of the original STORM test cases
 */
TEST(ReadTraFileTest, ParseFileTest1) {
	storm::parser::DeterministicSparseTransitionParser* parser = nullptr;
	ASSERT_NO_THROW(parser = new storm::parser::DeterministicSparseTransitionParser(STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/csl_general_input_01.tra"));
	std::shared_ptr<storm::storage::SparseMatrix<double>> result = parser->getMatrix();

	if (result != nullptr) {
		double val = 0;
		ASSERT_TRUE(result->getValue(0, 0, &val));
		ASSERT_EQ(val, 0.0);

		ASSERT_TRUE(result->getValue(0, 1, &val));
		ASSERT_EQ(val, 1.0);

		ASSERT_TRUE(result->getValue(1, 1, &val));
		ASSERT_EQ(val, 0.080645161290322580645161290322581);

		ASSERT_TRUE(result->getValue(1, 2, &val));
		ASSERT_EQ(val, 0.080645161290322580645161290322581);

		//Transition 1->3 was not set in the file, so it is not to appear in the matrix!
		ASSERT_FALSE(result->getValue(1, 3, &val));
		ASSERT_EQ(val, 0);

		ASSERT_TRUE(result->getValue(2, 1, &val));
		ASSERT_EQ(val, 0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2, 2, &val));
		ASSERT_EQ(val, 0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2, 3, &val));
		ASSERT_EQ(val, 0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2, 4, &val));
		ASSERT_EQ(val, 0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(3, 2, &val));
		ASSERT_EQ(val, 0.0806451612903225806451612903225812);

		ASSERT_TRUE(result->getValue(3, 3, &val));
		ASSERT_EQ(val, 0.0);

		ASSERT_TRUE(result->getValue(3, 4, &val));
		ASSERT_EQ(val, 0.080645161290322580645161290322581);

		ASSERT_TRUE(result->getValue(4, 4, &val));
		ASSERT_EQ(val, 0.0);

		delete parser;
	} else {
		FAIL();
	}
}

TEST(ReadTraFileTest, WrongFormatTestHeader1) {
   ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser(STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header1.tra"), storm::exceptions::WrongFormatException);
}

TEST(ReadTraFileTest, WrongFormatTestHeader2) {
   ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser(STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header2.tra"), storm::exceptions::WrongFormatException);
}

TEST(ReadTraFileTest, WrongFormatTestTransition) {
   ASSERT_THROW(storm::parser::DeterministicSparseTransitionParser(STORM_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_transition.tra"), storm::exceptions::WrongFormatException);
}
