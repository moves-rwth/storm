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

namespace prctl = storm::property::prctl;

TEST(PrctlParserTest, parseApOnlyTest) {
	std::string input = "ap";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ(input, formula->toString());
}

TEST(PrctlParserTest, parsePropositionalFormulaTest) {
	std::string input =  "!(a & b) | a & ! c";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(!(a & b) | (a & !c))", formula->toString());
}

TEST(PrctlParserTest, parseProbabilisticFormulaTest) {
	std::string input =  "P > 0.5 [ F a ]";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);


	ASSERT_NE(std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(formula->getChild()).get(), nullptr);
	std::shared_ptr<prctl::ProbabilisticBoundOperator<double>> op = std::static_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(formula->getChild());

	ASSERT_EQ(storm::property::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("P > 0.500000 (F a)", formula->toString());
}

TEST(PrctlParserTest, parseRewardFormulaTest) {
	std::string input =  "R >= 15 [ I=5 ]";
	std::shared_ptr<prctl::PrctlFilter<double>>formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_NE(std::dynamic_pointer_cast<prctl::RewardBoundOperator<double>>(formula->getChild()).get(), nullptr);
	std::shared_ptr<prctl::RewardBoundOperator<double>> op = std::static_pointer_cast<prctl::RewardBoundOperator<double>>(formula->getChild());

	ASSERT_EQ(storm::property::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("R >= 15.000000 (I=5)", formula->toString());
}

TEST(PrctlParserTest, parseRewardNoBoundFormulaTest) {
	std::string input =  "R = ? [ F a ]";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("R = ? (F a)", formula->toString());
}

TEST(PrctlParserTest, parseProbabilisticNoBoundFormulaTest) {
	std::string input =  "P = ? [ a U <= 4 b & (!c) ]";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("P = ? (a U<=4 (b & !c))", formula->toString());
}

TEST(PrctlParserTest, parseComplexFormulaTest) {
	std::string input =  "R<=0.5 [ S ] & (R > 15 [ C<=0.5 ] | !P < 0.4 [ G P>0.9 [F<=7 a & b] ])";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(R <= 0.500000 (S) & (R > 15.000000 (C <= 0.500000) | !P < 0.400000 (G P > 0.900000 (F<=7 (a & b)))))", formula->toString());
}


TEST(PrctlParserTest, wrongProbabilisticFormulaTest) {
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("P > 0.5 [ a ]"),
			storm::exceptions::WrongFormatException
	);
}

TEST(PrctlParserTest, wrongFormulaTest) {
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("(a | b) & +"),
			storm::exceptions::WrongFormatException
	);
}

TEST(PrctlParserTest, wrongFormulaTest2) {
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula("P>0 [ F & a ]"),
			storm::exceptions::WrongFormatException
	);
}
