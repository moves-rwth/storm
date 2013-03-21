/*
 * PrctlParserTest.cpp
 *
 *  Created on: 01.02.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrctlParser.h"
#include "src/parser/PrctlFileParser.h"

TEST(PrctlParserTest, parseApOnlyTest) {
	std::string ap = "ap";
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(ap);
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(ap, prctlParser->getFormula()->toString());

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parsePropositionalFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/propositionalFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);


	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "(!(a & b) | (a & !c))");

	delete prctlFileParser->getFormula();
	delete prctlFileParser;

}

TEST(PrctlParserTest, parseProbabilisticFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/probabilisticFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);

	storm::formula::ProbabilisticBoundOperator<double>* op = static_cast<storm::formula::ProbabilisticBoundOperator<double>*>(prctlFileParser->getFormula());

	ASSERT_EQ(storm::formula::PathBoundOperator<double>::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "P > 0.500000 [F a]");

	delete prctlFileParser->getFormula();
	delete prctlFileParser;

}

TEST(PrctlParserTest, parseRewardFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/rewardFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);

	storm::formula::RewardBoundOperator<double>* op = static_cast<storm::formula::RewardBoundOperator<double>*>(prctlFileParser->getFormula());

	ASSERT_EQ(storm::formula::PathBoundOperator<double>::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "R >= 15.000000 [a U !b]");

	delete prctlFileParser->getFormula();
	delete prctlFileParser;
}

TEST(PrctlParserTest, parseRewardNoBoundFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/rewardNoBoundFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);


	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "R = ? [a U<=4 (b & !c)]");

	delete prctlFileParser->getFormula();
	delete prctlFileParser;

}

TEST(PrctlParserTest, parseProbabilisticNoBoundFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/probabilisticNoBoundFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);


	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "P = ? [F<=42 !a]");

	delete prctlFileParser->getFormula();
	delete prctlFileParser;

}

TEST(PrctlParserTest, parseComplexFormulaTest) {
	storm::parser::PrctlFileParser* prctlFileParser = nullptr;
	ASSERT_NO_THROW(
			prctlFileParser = new storm::parser::PrctlFileParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/complexFormula.prctl")
	);

	ASSERT_NE(prctlFileParser->getFormula(), nullptr);


	ASSERT_EQ(prctlFileParser->getFormula()->toString(), "(P <= 0.500000 [F a] & (R > 15.000000 [G P > 0.900000 [F<=7 (a & b)]] | !P < 0.400000 [G !b]))");
	delete prctlFileParser->getFormula();
	delete prctlFileParser;

}


TEST(PrctlParserTest, wrongProbabilisticFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_THROW(
			prctlParser = new storm::parser::PrctlParser("P > 0.5 [ a ]"),
			storm::exceptions::WrongFormatException
	);

	delete prctlParser;
}

TEST(PrctlParserTest, wrongFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_THROW(
			prctlParser = new storm::parser::PrctlFileParser("(a | b) & ü"),
			storm::exceptions::WrongFormatException
	);
	delete prctlParser;
}

TEST(PrctlParserTest, wrongFormulaTest2) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_THROW(
			prctlParser = new storm::parser::PrctlFileParser("P>0 [ F & a ]"),
			storm::exceptions::WrongFormatException
	);
	delete prctlParser;
}
