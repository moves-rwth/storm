/*
 * MappedFileTest.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include <string>
#include "src/parser/MappedFile.h"
#include "src/exceptions/FileIoException.h"

TEST(MappedFileTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::MappedFile(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(MappedFileTest, BasicFunctionality) {

	// Open a file and test if the content is loaded as expected.
	storm::parser::MappedFile file(STORM_CPP_TESTS_BASE_PATH "/functional/parser/testStringFile.txt");
	std::string testString = "This is a test string.\n";
	ASSERT_EQ(file.getDataEnd() - file.getData(), testString.length());
	char const * testStringPtr = testString.c_str();
	for(char* dataPtr = file.getData(); dataPtr < file.getDataEnd(); dataPtr++) {
		ASSERT_EQ(*dataPtr, *testStringPtr);
		testStringPtr++;
	}
}

TEST(MappedFileTest, ExistsAndReadble) {

	// Test the fileExistsAndIsReadable() method under various circumstances.

	// File exists and is readable.
	ASSERT_TRUE(storm::parser::MappedFile::fileExistsAndIsReadable(STORM_CPP_TESTS_BASE_PATH "/functional/parser/testStringFile.txt"));

	// File does not exist.
	ASSERT_FALSE(storm::parser::MappedFile::fileExistsAndIsReadable(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"));

	// File exists but is not readable.
	// TODO: Find portable solution to providing a situation in which a file exists but is not readable.
	//ASSERT_FALSE(storm::parser::MappedFile::fileExistsAndIsReadable(STORM_CPP_TESTS_BASE_PATH "/functional/parser/unreadableFile.txt"));
}
