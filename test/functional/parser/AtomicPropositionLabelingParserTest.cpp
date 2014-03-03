/*
 * AtomicPropositionLabelingParserTest.cpp
 *
 *  Created on: 03.03.2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/parser/AtomicPropositionLabelingParser.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/OutOfRangeException.h"

#include <memory>

TEST(AtomicPropositionLabelingParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(0,STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(AtomicPropositionLabelingParserTest, BasicParsing) {
	// This test is based on a test case from the original MRMC.
	
	// Parsing the file
	storm::models::AtomicPropositionsLabeling labeling = storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(12, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/pctl_general.lab");

	// Checking whether all propositions are in the labelling

	char phi[] = "phi", psi[] = "psi", smth[] = "smth";

	ASSERT_TRUE(labeling.containsAtomicProposition(phi));
	ASSERT_TRUE(labeling.containsAtomicProposition(psi));
	ASSERT_TRUE(labeling.containsAtomicProposition(smth));

	// Testing whether all and only the correct nodes are labeled with "phi"
	ASSERT_TRUE(labeling.getStateHasAtomicProposition(phi,0));
	ASSERT_TRUE(labeling.getStateHasAtomicProposition(phi,1));
	ASSERT_TRUE(labeling.getStateHasAtomicProposition(phi,2));

	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,3));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,4));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,5));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,6));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,7));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,8));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,9));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,10));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(phi,11));

	//Testing whether all and only the correct nodes are labeled with "psi"
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,0));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,1));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,2));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,3));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,4));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,5));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,6));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,7));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,8));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,9));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,10));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(psi,11));

	//Testing whether all and only the correct nodes are labeled with "smth"
	ASSERT_TRUE(labeling.getStateHasAtomicProposition(smth,2));

	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,0));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,1));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,3));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,4));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,5));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,6));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,7));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,8));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,9));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,10));
	ASSERT_FALSE(labeling.getStateHasAtomicProposition(smth,11));
}

TEST(AtomicPropositionLabelingParserTest, NoDeclarationTagHeader) {
	// No #DECLARATION tag in file
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/noDeclarationTag.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, NoEndTagHeader) {
	// No #END tag in file.
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/noEndTag.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, MisspelledDeclarationTagHeader) {
	// The #DECLARATION tag is misspelled.
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/declarationMisspell.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, MisspelledEndTagHeader) {
	// The #END tag is misspelled.
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/endMisspell.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, NoLabelDeclaredNoneGiven) {
	// No label between #DECLARATION and #END and no labels given.
	storm::models::AtomicPropositionsLabeling labeling = storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(13, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/noLabelsDecNoneGiven.lab");
	ASSERT_EQ(labeling.getNumberOfAtomicPropositions(), 0);
	for(uint_fast64_t i = 0; i < 13; i++) {
		ASSERT_TRUE(labeling.getPropositionsForState(i).empty());
	}
}

TEST(AtomicPropositionLabelingParserTest, UndeclaredLabelsGiven) {
	// Undeclared labels given.
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/undeclaredLabelsGiven.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, LabelForNonExistentState) {
	// The index of one of the state that are to be labeled is higher than the number of states in the model.
	ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/labelForNonexistentState.lab"), storm::exceptions::OutOfRangeException);
}

// Note: As implemented at the moment, each label given for a state in any line is set to true for that state (logical or over all lines for that state).
// This behavior might not be ideal as multiple lines for one state are not necessary and might indicate a corrupt or wrong file.
TEST(AtomicPropositionLabelingParserTest, DoubledLines) {
	// There are multiple lines attributing labels to the same state.
	storm::models::AtomicPropositionsLabeling labeling = storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(6, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/doubledLines.lab");
	ASSERT_EQ(labeling.getPropositionsForState(1).size(), 3);
}

TEST(AtomicPropositionLabelingParserTest, WrongProposition) {
   // Swapped the state index and the label at one entry.
   ASSERT_THROW(storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(3, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/swappedStateAndProposition.lab"), storm::exceptions::WrongFormatException);
}

TEST(AtomicPropositionLabelingParserTest, Whitespaces) {
	// Different configurations of allowed whitespaces are tested.

	// First parse the labeling file without added whitespaces and obtain the hash of its parsed representation.
	storm::models::AtomicPropositionsLabeling labeling = storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(13, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/withoutWhitespaces.lab");
	uint_fast64_t correctHash = labeling.getHash();

	// Now parse the labeling file with the added whitespaces and compare the hashes.
	labeling = storm::parser::AtomicPropositionLabelingParser::parseAtomicPropositionLabeling(13, STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/labParser/withWhitespaces.lab");
	ASSERT_EQ(correctHash, labeling.getHash());
}
