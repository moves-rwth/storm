/*
 * CslParserTest.cpp
 *
 *  Created on: 09.04.2013
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/CslParser.h"

TEST(CslParserTest, parseApOnlyTest) {
	std::string ap = "ap";
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser(ap);
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);


	ASSERT_EQ(ap, cslParser->getFormula()->toString());

	delete cslParser->getFormula();
	delete cslParser;

}

TEST(CslParserTest, parsePropositionalFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("!(a & b) | a & ! c")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);


	ASSERT_EQ(cslParser->getFormula()->toString(), "(!(a & b) | (a & !c))");

	delete cslParser->getFormula();
	delete cslParser;

}

TEST(CslParserTest, parseProbabilisticFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("P > 0.5 [ F a ]")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);

	storm::formula::ProbabilisticBoundOperator<double>* op = static_cast<storm::formula::ProbabilisticBoundOperator<double>*>(cslParser->getFormula());

	ASSERT_EQ(storm::formula::PathBoundOperator<double>::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	ASSERT_EQ(cslParser->getFormula()->toString(), "P > 0.500000 [F a]");

	delete cslParser->getFormula();
	delete cslParser;

}

TEST(CslParserTest, parseSteadyStateBoundFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("S >= 15 [ P < 0.2 [ a U<=3 b ] ]")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);

	storm::formula::SteadyStateBoundOperator<double>* op = static_cast<storm::formula::SteadyStateBoundOperator<double>*>(cslParser->getFormula());

	ASSERT_EQ(storm::formula::StateBoundOperator<double>::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	ASSERT_EQ("S >= 15.000000 [P < 0.200000 [a U[0.000000,3.000000] b]]", cslParser->getFormula()->toString());

	delete cslParser->getFormula();
	delete cslParser;
}

TEST(CslParserTest, parseSteadyStateNoBoundFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("S = ? [ P <= 0.5 [ F a ] ]")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);


	ASSERT_EQ(cslParser->getFormula()->toString(), "S = ? [P <= 0.500000 [F a]]");

	delete cslParser->getFormula();
	delete cslParser;

}

TEST(CslParserTest, parseProbabilisticNoBoundFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("P = ? [ a U [3,4] b & (!c) ]")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);


	ASSERT_EQ(cslParser->getFormula()->toString(), "P = ? [a U[3.000000,4.000000] (b & !c)]");

	delete cslParser->getFormula();
	delete cslParser;

}

TEST(CslParserTest, parseComplexFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_NO_THROW(
			cslParser = new storm::parser::CslParser("S<=0.5 [ P <= 0.5 [ a U c ] ] & (P > 0.5 [ G b] | !P < 0.4 [ G P>0.9 [F >=7 a & b] ])")
	);

	ASSERT_NE(cslParser->getFormula(), nullptr);

	//NOTE: This test case is dependent on the string output of the double value infinity.
	//		  In g++ and clang++ on Linux, it is "inf". If some compiler (or library) uses a different output, please
	//		  notify me and I will restructure this test case.
	ASSERT_EQ("(S <= 0.500000 [P <= 0.500000 [a U c]] & (P > 0.500000 [G b] | !P < 0.400000 [G P > 0.900000 [F[7.000000,inf] (a & b)]]))", cslParser->getFormula()->toString());
	delete cslParser->getFormula();
	delete cslParser;

}


TEST(CslParserTest, wrongProbabilisticFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_THROW(
			cslParser = new storm::parser::CslParser("P > 0.5 [ a ]"),
			storm::exceptions::WrongFormatException
	);

	delete cslParser;
}

TEST(CslParserTest, wrongFormulaTest) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_THROW(
			cslParser = new storm::parser::CslParser("(a | b) & +"),
			storm::exceptions::WrongFormatException
	);
	delete cslParser;
}

TEST(CslParserTest, wrongFormulaTest2) {
	storm::parser::CslParser* cslParser = nullptr;
	ASSERT_THROW(
			cslParser = new storm::parser::CslParser("P>0 [ F & a ]"),
			storm::exceptions::WrongFormatException
	);
	delete cslParser;
}
