/*
 * PrctlParserTest.cpp
 *
 *  Created on: 01.02.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrctlParser.h"
#include "src/exceptions/WrongFormatException.h"

TEST(PrctlParserTest, parseApOnlyTest) {
	std::string input = "ap";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ(input, formula->toString());

	delete formula;

}

TEST(PrctlParserTest, parsePropositionalFormulaTest) {
	std::string input =  "!(a & b) | a & ! c";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(!(a & b) | (a & !c))", formula->toString());

	delete formula;

}

TEST(PrctlParserTest, parseProbabilisticFormulaTest) {
	std::string input =  "P > 0.5 [ F a ]";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);


	ASSERT_NE(dynamic_cast<storm::property::prctl::ProbabilisticBoundOperator<double>*>(formula->getChild()), nullptr);
	storm::property::prctl::ProbabilisticBoundOperator<double>* op = static_cast<storm::property::prctl::ProbabilisticBoundOperator<double>*>(formula->getChild());

	ASSERT_EQ(storm::property::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("P > 0.500000 [F a]", formula->toString());

	delete formula;

}

TEST(PrctlParserTest, parseRewardFormulaTest) {
	std::string input =  "R >= 15 [ I=5 ]";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_NE(dynamic_cast<storm::property::prctl::RewardBoundOperator<double>*>(formula->getChild()), nullptr);
	storm::property::prctl::RewardBoundOperator<double>* op = static_cast<storm::property::prctl::RewardBoundOperator<double>*>(formula->getChild());

	ASSERT_EQ(storm::property::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("R >= 15.000000 [I=5]", formula->toString());

	delete formula;
}

TEST(PrctlParserTest, parseRewardNoBoundFormulaTest) {
	std::string input =  "R = ? [ F a ]";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("R = ? [F a]", formula->toString());

	delete formula;
}

TEST(PrctlParserTest, parseProbabilisticNoBoundFormulaTest) {
	std::string input =  "P = ? [ a U <= 4 b & (!c) ]";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("P = ? [a U<=4 (b & !c)]", formula->toString());

	delete formula;
}

TEST(PrctlParserTest, parseComplexFormulaTest) {
	std::string input =  "R<=0.5 [ S ] & (R > 15 [ C<=0.5 ] | !P < 0.4 [ G P>0.9 [F<=7 a & b] ])";
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(R <= 0.500000 [S] & (R > 15.000000 [C <= 0.500000] | !P < 0.400000 [G P > 0.900000 [F<=7 (a & b)]]))", formula->toString());

	delete formula;
}


TEST(PrctlParserTest, wrongProbabilisticFormulaTest) {
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("P > 0.5 [ a ]"),
			storm::exceptions::WrongFormatException
	);

	delete formula;
}

TEST(PrctlParserTest, wrongFormulaTest) {
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("(a | b) & +"),
			storm::exceptions::WrongFormatException
	);
	delete formula;
}

TEST(PrctlParserTest, wrongFormulaTest2) {
	storm::property::prctl::PrctlFilter<double>* formula = nullptr;
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("P>0 [ F & a ]"),
			storm::exceptions::WrongFormatException
	);
	delete formula;
}
