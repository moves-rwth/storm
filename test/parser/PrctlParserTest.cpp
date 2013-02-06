/*
 * PrctlParserTest.cpp
 *
 *  Created on: 01.02.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrctlParser.h"
#include <iostream>

TEST(PrctlParserTest, apOnly) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/apOnly.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P");

	delete prctlParser;

}

TEST(PrctlParserTest, formulaTest1) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/testFormula1.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "!(a && b)");

	delete prctlParser;

}

TEST(PrctlParserTest, formulaTest2) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/testFormula2.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P<0.500000 [F a]");

	delete prctlParser;

}
