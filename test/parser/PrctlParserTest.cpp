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

TEST(PrctlParserTest, propositionalFormula) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/propositionalFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "!(a && b)");

	delete prctlParser;

}

TEST(PrctlParserTest, probabilisticFormula) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/probabilisticFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P > 0.500000 [F a]");

	delete prctlParser;

}

TEST(PrctlParserTest, rewardFormula) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/rewardFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);

	storm::formula::RewardBoundOperator<double>* op = static_cast<storm::formula::RewardBoundOperator<double>*>(prctlParser->getFormula());
	ASSERT_EQ(storm::formula::BoundOperator<double>::GREATER_EQUAL, op->getComparisonOperator());

	ASSERT_EQ(prctlParser->getFormula()->toString(), "R >= 15.000000 [(a U !b)]");

	delete prctlParser;

}
