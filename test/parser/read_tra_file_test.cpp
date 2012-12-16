/*
 * read_tra_file_test.cpp
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "mrmc-config.h"
#include "src/storage/SquareSparseMatrix.h"
#include "src/parser/readTraFile.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFileFormatException.h"

#include "src/utility/IoUtility.h"

TEST(ReadTraFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(mrmc::parser::TraParser(MRMC_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), mrmc::exceptions::FileIoException);
}

/* The following test case is based on one of the original MRMC test cases
 */
TEST(ReadTraFileTest, ParseFileTest1) {
	mrmc::parser::TraParser* parser;
	ASSERT_NO_THROW(parser = new mrmc::parser::TraParser(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/csl_general_input_01.tra"));
	std::shared_ptr<mrmc::storage::SquareSparseMatrix<double>> result = parser->getMatrix();

	if (result != NULL) {
		double val = 0;
		ASSERT_TRUE(result->getValue(1,1,&val));
		ASSERT_EQ(val,0.080645161290322580645161290322581);

		ASSERT_TRUE(result->getValue(1,2,&val));
		ASSERT_EQ(val,0.080645161290322580645161290322581);

		//Transition 1->3 was not set in the file, so it is not to appear in the matrix!
		ASSERT_FALSE(result->getValue(1,3,&val));
		ASSERT_EQ(val,0);

		ASSERT_TRUE(result->getValue(2,1,&val));
		ASSERT_EQ(val,0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2,2,&val));
		ASSERT_EQ(val,0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2,3,&val));
		ASSERT_EQ(val,0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(2,4,&val));
		ASSERT_EQ(val,0.04032258064516129032258064516129);

		ASSERT_TRUE(result->getValue(3,2,&val));
		ASSERT_EQ(val,0.0806451612903225806451612903225812);

		ASSERT_TRUE(result->getValue(3,3,&val));
		ASSERT_EQ(val,0);

		ASSERT_TRUE(result->getValue(3,4,&val));
		ASSERT_EQ(val,0.080645161290322580645161290322581);

		ASSERT_TRUE(result->getValue(4,4,&val));
		ASSERT_EQ(val,0);

		delete parser;
	} else {
		FAIL();
	}
}

TEST(ReadTraFileTest, WrongFormatTestHeader1) {
   ASSERT_THROW(mrmc::parser::TraParser(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header1.tra"), mrmc::exceptions::WrongFileFormatException);
}

TEST(ReadTraFileTest, WrongFormatTestHeader2) {
   ASSERT_THROW(mrmc::parser::TraParser(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header2.tra"), mrmc::exceptions::WrongFileFormatException);
}

TEST(ReadTraFileTest, WrongFormatTestTransition) {
   ASSERT_THROW(mrmc::parser::TraParser(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_transition.tra"), mrmc::exceptions::WrongFileFormatException);
}
