/*
 * read_tra_file_test.cpp
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "mrmc-config.h"
#include "src/sparse/static_sparse_matrix.h"
#include "src/parser/read_tra_file.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"

TEST(ReadTraFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(mrmc::parser::read_tra_file(MRMC_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), mrmc::exceptions::file_IO_exception);
}

/* The following test case is based on one of the original MRMC test cases
 */
TEST(ReadTraFileTest, ParseFileTest1) {
	mrmc::sparse::StaticSparseMatrix<double> *result = NULL;
	ASSERT_NO_THROW(result = mrmc::parser::read_tra_file(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/csl_general_input_01.tra"));

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

		// FIXME: adapt this test case to the new dot-output routines
		/* result->toDOTFile("output.dot"); */
		delete result;
	} else {
		FAIL();
	}
}

TEST(ReadTraFileTest, WrongFormatTestHeader1) {
   ASSERT_THROW(mrmc::parser::read_tra_file(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header1.tra"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadTraFileTest, WrongFormatTestHeader2) {
   ASSERT_THROW(mrmc::parser::read_tra_file(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_header2.tra"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadTraFileTest, WrongFormatTestTransition) {
   ASSERT_THROW(mrmc::parser::read_tra_file(MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/wrong_format_transition.tra"), mrmc::exceptions::wrong_file_format);
}
