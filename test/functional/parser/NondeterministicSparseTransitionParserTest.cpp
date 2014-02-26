/*
 * NondeterministicSparseTransitionParserTest.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/NondeterministicSparseTransitionParser.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/InternalOptionMemento.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/WrongFormatException.h"

TEST(NondeterministicSparseTransitionParserTest, NonExistingFile) {

	// No matter what happens, please do NOT create a file with the name "nonExistingFile.not"!
}


TEST(NondeterministicSparseTransitionParserTest, BasicTransitionsParsing) {

}

TEST(NondeterministicSparseTransitionParserTest, BasicTransitionsRewardsParsing) {

}

TEST(NondeterministicSparseTransitionParserTest, Whitespaces) {

}

TEST(NondeterministicSparseTransitionParserTest, MixedTransitionOrder) {

}

TEST(NondeterministicSparseTransitionParserTest, FixDeadlocks) {

}

TEST(NondeterministicSparseTransitionParserTest, DontFixDeadlocks) {

}

TEST(NondeterministicSparseTransitionParserTest, DoubledLines) {

}
