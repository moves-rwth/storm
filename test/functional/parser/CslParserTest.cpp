/*
 * CslParserTest.cpp
 *
 *  Created on: 09.04.2013
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/CslParser.h"
#include "src/exceptions/WrongFormatException.h"

TEST(CslParserTest, parseApOnlyTest) {
	std::string input = "ap";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ(input, formula->toString());

	delete formula;
}

TEST(CslParserTest, parsePropositionalFormulaTest) {
	std::string input = "!(a & b) | a & ! c";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(!(a & b) | (a & !c))", formula->toString());

	delete formula;
}

TEST(CslParserTest, parseProbabilisticFormulaTest) {
	std::string input = "P > 0.5 [ F a ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_NE(dynamic_cast<storm::property::csl::ProbabilisticBoundOperator<double>*>(formula->getChild()), nullptr);
	storm::property::csl::ProbabilisticBoundOperator<double>* op = static_cast<storm::property::csl::ProbabilisticBoundOperator<double>*>(formula->getChild());
	ASSERT_EQ(storm::property::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("P > 0.500000 (F a)", formula->toString());

	delete formula;
}

TEST(CslParserTest, parseSteadyStateBoundFormulaTest) {
	std::string input = "S >= 15 [ P < 0.2 [ a U<=3 b ] ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	storm::property::csl::SteadyStateBoundOperator<double>* op = static_cast<storm::property::csl::SteadyStateBoundOperator<double>*>(formula->getChild());
	ASSERT_EQ(storm::property::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("S >= 15.000000 (P < 0.200000 (a U[0.000000,3.000000] b))", formula->toString());

	delete formula;
}

TEST(CslParserTest, parseSteadyStateNoBoundFormulaTest) {
	std::string input = "S = ? [ P <= 0.5 [ F<=3 a ] ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("S = ? (P <= 0.500000 (F[0.000000,3.000000] a))", formula->toString());

	delete formula;
}

TEST(CslParserTest, parseProbabilisticNoBoundFormulaTest) {
	std::string input = "P = ? [ a U [3,4] b & (!c) ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("P = ? (a U[3.000000,4.000000] (b & !c))", formula->toString());

	delete formula;
}


TEST(CslParserTest, parseComplexFormulaTest) {
	std::string input = "S<=0.5 [ P <= 0.5 [ a U c ] ] & (P > 0.5 [ G b] | !P < 0.4 [ G P>0.9 [F >=7 a & b] ])  //and a comment";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("(S <= 0.500000 (P <= 0.500000 (a U c)) & (P > 0.500000 (G b) | !P < 0.400000 (G P > 0.900000 (F>=7.000000 (a & b)))))", formula->toString());

	delete formula;
}

TEST(CslParserTest, wrongProbabilisticFormulaTest) {
	std::string input = "P > 0.5 [ a ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
	delete formula;
}

TEST(CslParserTest, wrongFormulaTest) {
	std::string input = "(a | b) & +";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
	delete formula;
}

TEST(CslParserTest, wrongFormulaTest2) {
	std::string input = "P>0 [ F & a ]";
	storm::property::csl::CslFilter<double>* formula = nullptr;
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
	delete formula;
}
