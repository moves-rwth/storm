/*
 * PrctlParserTest.cpp
 *
 *  Created on: 01.02.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrctlParser.h"

TEST(PrctlParserTest, parseApOnlyTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/apOnly.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parsePropositionalFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/propositionalFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "!(a && b)");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseProbabilisticFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/probabilisticFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);

	storm::formula::ProbabilisticBoundOperator<double>* op = static_cast<storm::formula::ProbabilisticBoundOperator<double>*>(prctlParser->getFormula());

	ASSERT_EQ(storm::formula::BoundOperator<double>::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	ASSERT_EQ(prctlParser->getFormula()->toString(), "P > 0.500000 [F a]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseRewardFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/rewardFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);

	storm::formula::RewardBoundOperator<double>* op = static_cast<storm::formula::RewardBoundOperator<double>*>(prctlParser->getFormula());

	ASSERT_EQ(storm::formula::BoundOperator<double>::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	ASSERT_EQ(prctlParser->getFormula()->toString(), "R >= 15.000000 [(a U !b)]");

	delete prctlParser->getFormula();
	delete prctlParser;
}

TEST(PrctlParserTest, parseRewardNoBoundFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/rewardNoBoundFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "R = ? [(a U<=4 (b && !c))]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseProbabilisticNoBoundFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/probabilisticNoBoundFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P = ? [F<=42 !a]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseComplexFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/complexFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "(P <= 0.500000 [F a] && (R > 15.000000 [G !b] || P > 0.900000 [F<=7 (a && b)]))");
	delete prctlParser->getFormula();
	delete prctlParser;

}
