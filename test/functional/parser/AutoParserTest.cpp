/*
 * AutoParserTest.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/AutoParser.h"
#include "src/exceptions/FileIoException.h"

TEST(AutoParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
	ASSERT_THROW(storm::parser::AutoParser::parseModel(STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not", STORM_CPP_TESTS_BASE_PATH "/nonExistingFile.not"), storm::exceptions::FileIoException);
}

TEST(AutoParserTest, BasicParsing) {

}

TEST(AutoParserTest, Whitespaces) {

}

TEST(AutoParserTest, Decision) {

}
