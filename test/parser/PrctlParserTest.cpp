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
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("!(a & b) | a & ! c")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "(!(a & b) | (a & !c))");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseProbabilisticFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("P > 0.5 [ F a ]")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);

	storm::formula::prctl::ProbabilisticBoundOperator<double>* op = static_cast<storm::formula::prctl::ProbabilisticBoundOperator<double>*>(prctlParser->getFormula());

	ASSERT_EQ(storm::formula::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	ASSERT_EQ(prctlParser->getFormula()->toString(), "P > 0.500000 [F a]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseRewardFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("R >= 15 [ I=5 ]")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);

	storm::formula::prctl::RewardBoundOperator<double>* op = static_cast<storm::formula::prctl::RewardBoundOperator<double>*>(prctlParser->getFormula());

	ASSERT_EQ(storm::formula::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	ASSERT_EQ("R >= 15.000000 [I=5]", prctlParser->getFormula()->toString());

	delete prctlParser->getFormula();
	delete prctlParser;
}

TEST(PrctlParserTest, parseRewardNoBoundFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("R = ? [ F a ]")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "R = ? [F a]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseProbabilisticNoBoundFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("P = ? [ a U <= 4 b & (!c) ]")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ(prctlParser->getFormula()->toString(), "P = ? [a U<=4 (b & !c)]");

	delete prctlParser->getFormula();
	delete prctlParser;

}

TEST(PrctlParserTest, parseComplexFormulaTest) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser("R<=0.5 [ S ] & (R > 15 [ C<=0.5 ] | !P < 0.4 [ G P>0.9 [F<=7 a & b] ])")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	ASSERT_EQ("(R <= 0.500000 [S] & (R > 15.000000 [C <= 0.500000] | !P < 0.400000 [G P > 0.900000 [F<=7 (a & b)]]))", prctlParser->getFormula()->toString());
	delete prctlParser->getFormula();
	delete prctlParser;

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
			prctlParser = new storm::parser::PrctlParser("(a | b) & +"),
			storm::exceptions::WrongFormatException
	);
	delete prctlParser;
}

TEST(PrctlParserTest, wrongFormulaTest2) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_THROW(
			prctlParser = new storm::parser::PrctlParser("P>0 [ F & a ]"),
			storm::exceptions::WrongFormatException
	);
	delete prctlParser;
}
