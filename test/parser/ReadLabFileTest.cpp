/*
 * ReadLabFileTest.cpp
 *
 *  Created on: 12.09.2012
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFileFormatException.h"

#include <memory>

TEST(ReadLabFileTest, NonExistingFileTest) {
   //No matter what happens, please don't create a file with the name "nonExistingFile.not"! :-)
   ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser(0,STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(ReadLabFileTest, ParseTest) {
	//This test is based on a test case from the original MRMC.
	
	
	storm::parser::AtomicPropositionLabelingParser* parser = nullptr;
	//Parsing the file
	ASSERT_NO_THROW(parser = new storm::parser::AtomicPropositionLabelingParser(12, STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> labeling(parser->getLabeling());

	//Checking whether all propositions are in the labelling

	char phi[] = "phi", psi[] = "psi", smth[] = "smth";

	if (labeling != nullptr) {
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
		delete parser;
	} else {
		FAIL();
	}
}

TEST(ReadLabFileTest, WrongHeaderTest1) {
   ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser(3, STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header1.lab"), storm::exceptions::WrongFileFormatException);
}

TEST(ReadLabFileTest, WrongHeaderTest2) {
   ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser(3, STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_header2.lab"), storm::exceptions::WrongFileFormatException);
}

TEST(ReadLabFileTest, WrongPropositionTest) {
   ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser(3, STORM_CPP_TESTS_BASE_PATH "/parser/lab_files/wrong_format_proposition.lab"), storm::exceptions::WrongFileFormatException);
}

