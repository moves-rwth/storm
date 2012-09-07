/*
 * read_tra_file_test.cpp
 *
 *  Created on: 16.08.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "src/sparse/static_sparse_matrix.h"
#include "src/parser/read_tra_file.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"
#include <pantheios/pantheios.hpp>

TEST(ReadTraFileTest, NonExistingFileTest) {
   pantheios::log_INFORMATIONAL("Started NonExistingFileTest");
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(mrmc::parser::read_tra_file("nonExistingFile.not"), mrmc::exceptions::file_IO_exception);
}

/* The following test case is based on one of the original MRMC test cases
 */
TEST(ReadTraFileTest, ParseFileTest1) {
   pantheios::log_INFORMATIONAL("Started ParseFileTest1");
   mrmc::sparse::StaticSparseMatrix<double> *result;
   ASSERT_NO_THROW(result = mrmc::parser::read_tra_file("test/parser/csl_general_input_01.tra"));

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
   delete result;
}

TEST(ReadTraFileTest, WrongFormatTestHeader1) {
   pantheios::log_INFORMATIONAL("Started WrongFormatTestHeader1");

   ASSERT_THROW(mrmc::parser::read_tra_file("test/parser/wrong_format_header1.tra"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadTraFileTest, WrongFormatTestHeader2) {
   pantheios::log_INFORMATIONAL("Started WrongFormatTestHeader2");

   ASSERT_THROW(mrmc::parser::read_tra_file("test/parser/wrong_format_header2.tra"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadTraFileTest, WrongFormatTestTransition) {
   pantheios::log_INFORMATIONAL("Started WrongFormatTestTransition");

   ASSERT_THROW(mrmc::parser::read_tra_file("test/parser/wrong_format_transition.tra"), mrmc::exceptions::wrong_file_format);
}
