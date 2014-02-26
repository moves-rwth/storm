/*
 * NondeterministicModelParserTest.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/NondeterministicModelParser.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"
#include "src/exceptions/FileIoException.h"

TEST(NondeterministicModelParserTest, NonExistingFile) {
	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
}

TEST(NondeterministicModelParserTest, BasicMdpParsing) {

}


TEST(NondeterministicModelParserTest, BasicCtmdpParsing) {

}

TEST(NondeterministicModelParserTest, UnmatchedFiles) {

}
