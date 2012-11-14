/*
 * read_lab_file_test.cpp
 *
 *  Created on: 12.09.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "MRMCConfig.h"
#include "src/models/atomic_propositions_labeling.h"
#include "src/parser/read_lab_file.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"

TEST(ReadLabFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(mrmc::parser::read_lab_file(0,MRMC_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), mrmc::exceptions::file_IO_exception);
}

TEST(ReadLabFileTest, ParseTest) {
	//This test is based on a test case from the original MRMC.
	mrmc::models::AtomicPropositionsLabeling* labeling = NULL;

	//Parsing the file
	ASSERT_NO_THROW(labeling = mrmc::parser::read_lab_file(12, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	//Checking whether all propositions are in the labelling

	char phi[] = "phi", psi[] = "psi", smth[] = "smth";

	if (labeling != NULL) {
		ASSERT_TRUE(labeling->containsProposition(phi));
		ASSERT_TRUE(labeling->containsProposition(psi));
		ASSERT_TRUE(labeling->containsProposition(smth));

		//Testing whether all and only the correct nodes are labeled with "phi"
		ASSERT_TRUE(labeling->nodeHasProposition(phi,1));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,2));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,3));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,5));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,7));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,9));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,10));
		ASSERT_TRUE(labeling->nodeHasProposition(phi,11));

		ASSERT_FALSE(labeling->nodeHasProposition(phi,4));
		ASSERT_FALSE(labeling->nodeHasProposition(phi,6));

		//Testing whether all and only the correct nodes are labeled with "psi"
		ASSERT_TRUE(labeling->nodeHasProposition(psi,6));
		ASSERT_TRUE(labeling->nodeHasProposition(psi,7));
		ASSERT_TRUE(labeling->nodeHasProposition(psi,8));

		ASSERT_FALSE(labeling->nodeHasProposition(psi,1));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,2));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,3));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,4));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,5));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,9));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,10));
		ASSERT_FALSE(labeling->nodeHasProposition(psi,11));

		//Testing whether all and only the correct nodes are labeled with "smth"
		ASSERT_TRUE(labeling->nodeHasProposition(smth,4));
		ASSERT_TRUE(labeling->nodeHasProposition(smth,5));

		ASSERT_FALSE(labeling->nodeHasProposition(smth,1));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,2));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,3));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,6));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,7));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,8));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,9));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,10));
		ASSERT_FALSE(labeling->nodeHasProposition(smth,11));

		//Deleting the labeling
		delete labeling;
	} else {
		FAIL();
	}
}

TEST(ReadLabFileTest, WrongHeaderTest1) {
   ASSERT_THROW(mrmc::parser::read_lab_file(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header1.lab"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadLabFileTest, WrongHeaderTest2) {
   ASSERT_THROW(mrmc::parser::read_lab_file(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header2.lab"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadLabFileTest, WrongPropositionTest) {
   ASSERT_THROW(mrmc::parser::read_lab_file(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_proposition.lab"), mrmc::exceptions::wrong_file_format);
}

