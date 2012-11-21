/*
 * read_lab_file_test.cpp
 *
 *  Created on: 12.09.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "MRMCConfig.h"
#include "src/models/atomic_propositions_labeling.h"
#include "src/parser/readLabFile.h"
#include "src/exceptions/file_IO_exception.h"
#include "src/exceptions/wrong_file_format.h"

TEST(ReadLabFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(mrmc::parser::readLabFile(0,MRMC_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), mrmc::exceptions::file_IO_exception);
}

TEST(ReadLabFileTest, ParseTest) {
	//This test is based on a test case from the original MRMC.
	mrmc::models::AtomicPropositionsLabeling* labeling = NULL;

	//Parsing the file
	ASSERT_NO_THROW(labeling = mrmc::parser::readLabFile(12, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	//Checking whether all propositions are in the labelling

	char phi[] = "phi", psi[] = "psi", smth[] = "smth";

	if (labeling != NULL) {
		ASSERT_TRUE(labeling->containsAtomicProposition(phi));
		ASSERT_TRUE(labeling->containsAtomicProposition(psi));
		ASSERT_TRUE(labeling->containsAtomicProposition(smth));

		//Testing whether all and only the correct nodes are labeled with "phi"
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,1));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,2));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,3));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,5));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,7));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,9));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,10));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(phi,11));

		ASSERT_FALSE(labeling->stateHasAtomicProposition(phi,4));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(phi,6));

		//Testing whether all and only the correct nodes are labeled with "psi"
		ASSERT_TRUE(labeling->stateHasAtomicProposition(psi,6));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(psi,7));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(psi,8));

		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,1));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,2));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,3));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,4));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,5));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,9));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,10));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(psi,11));

		//Testing whether all and only the correct nodes are labeled with "smth"
		ASSERT_TRUE(labeling->stateHasAtomicProposition(smth,4));
		ASSERT_TRUE(labeling->stateHasAtomicProposition(smth,5));

		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,1));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,2));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,3));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,6));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,7));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,8));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,9));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,10));
		ASSERT_FALSE(labeling->stateHasAtomicProposition(smth,11));

		//Deleting the labeling
		delete labeling;
	} else {
		FAIL();
	}
}

TEST(ReadLabFileTest, WrongHeaderTest1) {
   ASSERT_THROW(mrmc::parser::readLabFile(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header1.lab"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadLabFileTest, WrongHeaderTest2) {
   ASSERT_THROW(mrmc::parser::readLabFile(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header2.lab"), mrmc::exceptions::wrong_file_format);
}

TEST(ReadLabFileTest, WrongPropositionTest) {
   ASSERT_THROW(mrmc::parser::readLabFile(3, MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_proposition.lab"), mrmc::exceptions::wrong_file_format);
}

