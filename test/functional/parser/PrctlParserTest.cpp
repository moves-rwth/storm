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
#include "src/formula/actions/FormulaAction.h"
#include "src/formula/actions/InvertAction.h"
#include "src/formula/actions/SortAction.h"
#include "src/formula/actions/RangeAction.h"
#include "src/formula/actions/BoundAction.h"

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

TEST(PrctlParserTest, parsePathFormulaTest) {
	std::string input = "X( P<0.9 (a U b))";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Next<double>>(formula->getChild()).get(), nullptr);
	auto nextFormula = std::dynamic_pointer_cast<prctl::Next<double>>(formula->getChild());
	ASSERT_NE(std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(nextFormula->getChild()).get(), nullptr);
	auto probBoundFormula = std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(nextFormula->getChild());
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Until<double>>(probBoundFormula->getChild()).get(), nullptr);
	auto untilFormula = std::dynamic_pointer_cast<prctl::Until<double>>(probBoundFormula->getChild());
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getLeft()).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getRight()).get(), nullptr);
	ASSERT_EQ("a", std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getLeft())->getAp());
	ASSERT_EQ("b", std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getRight())->getAp());
	ASSERT_EQ(0.9, probBoundFormula->getBound());
	ASSERT_EQ(storm::property::LESS, probBoundFormula->getComparisonOperator());


	// The string representation is also correct.
	ASSERT_EQ("P = ? (X P < 0.900000 (a U b))", formula->toString());
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

TEST(PrctlParserTest, parsePrctlFilterTest) {
	std::string input = "filter[formula(b); invert; bound(<, 0.5); sort(value); range(0,3)](F a)";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_EQ(5, formula->getActionCount());
	ASSERT_NE(dynamic_cast<storm::property::action::FormulaAction<double>*>(formula->getAction(0)), nullptr);
	ASSERT_NE(dynamic_cast<storm::property::action::InvertAction<double>*>(formula->getAction(1)), nullptr);
	ASSERT_NE(dynamic_cast<storm::property::action::BoundAction<double>*>(formula->getAction(2)), nullptr);
	ASSERT_NE(dynamic_cast<storm::property::action::SortAction<double>*>(formula->getAction(3)), nullptr);
	ASSERT_NE(dynamic_cast<storm::property::action::RangeAction<double>*>(formula->getAction(4)), nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("filter[formula(b); invert; bound(<, 0.500000); sort(value, ascending); range(0, 3)](F a)", formula->toString());
}

TEST(PrctlParserTest, commentTest) {
	std::string input = "// This is a comment. And this is a commented out formula: R = ? [ F a ]";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser recognized the input as a comment.
	ASSERT_NE(nullptr, formula.get());

	// Test if the parser recognizes the comment at the end of a line.
	input = "R = ? [ F a ] // This is a comment.";
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);
	ASSERT_EQ("R = ? (F a)", formula->toString());
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
