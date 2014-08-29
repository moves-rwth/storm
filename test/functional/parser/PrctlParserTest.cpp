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
#include "src/properties/actions/FormulaAction.h"
#include "src/properties/actions/InvertAction.h"
#include "src/properties/actions/SortAction.h"
#include "src/properties/actions/RangeAction.h"
#include "src/properties/actions/BoundAction.h"

namespace prctl = storm::properties::prctl;

TEST(PrctlParserTest, parseApOnlyTest) {
	std::string input = "ap";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
			formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_TRUE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_TRUE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	// The input was parsed correctly.
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Next<double>>(formula->getChild()).get(), nullptr);
	auto nextFormula = std::dynamic_pointer_cast<prctl::Next<double>>(formula->getChild());
	ASSERT_FALSE(nextFormula->isPropositional());
	ASSERT_FALSE(nextFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(nextFormula->getChild()).get(), nullptr);
	auto probBoundFormula = std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(nextFormula->getChild());
	ASSERT_EQ(0.9, probBoundFormula->getBound());
	ASSERT_EQ(storm::properties::LESS, probBoundFormula->getComparisonOperator());
	ASSERT_FALSE(probBoundFormula->isPropositional());
	ASSERT_TRUE(probBoundFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<prctl::Until<double>>(probBoundFormula->getChild()).get(), nullptr);
	auto untilFormula = std::dynamic_pointer_cast<prctl::Until<double>>(probBoundFormula->getChild());
	ASSERT_FALSE(untilFormula->isPropositional());
	ASSERT_FALSE(untilFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getLeft()).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getRight()).get(), nullptr);
	ASSERT_EQ("a", std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getLeft())->getAp());
	ASSERT_EQ("b", std::dynamic_pointer_cast<prctl::Ap<double>>(untilFormula->getRight())->getAp());

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
	ASSERT_NE(formula.get(), nullptr);


	ASSERT_NE(std::dynamic_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(formula->getChild()).get(), nullptr);
	std::shared_ptr<prctl::ProbabilisticBoundOperator<double>> op = std::static_pointer_cast<prctl::ProbabilisticBoundOperator<double>>(formula->getChild());

	ASSERT_EQ(storm::properties::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());
	ASSERT_FALSE(op->isPropositional());
	ASSERT_TRUE(op->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_NE(std::dynamic_pointer_cast<prctl::RewardBoundOperator<double>>(formula->getChild()).get(), nullptr);
	std::shared_ptr<prctl::RewardBoundOperator<double>> op = std::static_pointer_cast<prctl::RewardBoundOperator<double>>(formula->getChild());

	ASSERT_EQ(storm::properties::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());
	ASSERT_FALSE(op->isPropositional());
	ASSERT_FALSE(op->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

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
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("(R <= 0.500000 (S) & (R > 15.000000 (C <= 0.500000) | !P < 0.400000 (G P > 0.900000 (F<=7 (a & b)))))", formula->toString());
}

TEST(PrctlParserTest, parsePrctlFilterTest) {
	std::string input = "filter[max; formula(b); invert; bound(<, 0.5); sort(value); range(0,3)](F a)";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_EQ(storm::properties::MAXIMIZE, formula->getOptimizingOperator());

	ASSERT_EQ(5, formula->getActionCount());
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::FormulaAction<double>>(formula->getAction(0)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::InvertAction<double>>(formula->getAction(1)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::BoundAction<double>>(formula->getAction(2)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::SortAction<double>>(formula->getAction(3)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::RangeAction<double>>(formula->getAction(4)).get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("filter[max; formula(b); invert; bound(<, 0.500000); sort(value, ascending); range(0, 3)](F a)", formula->toString());
}

TEST(PrctlParserTest, commentTest) {
	std::string input = "// This is a comment. And this is a commented out formula: P<=0.5 [ X a ]";
	std::shared_ptr<prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// The parser recognized the input as a comment.
	ASSERT_NE(nullptr, formula.get());

	// Test if the parser recognizes the comment at the end of a line.
	input = "P<=0.5 [ X a ] // This is a comment.";
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	ASSERT_EQ("P <= 0.500000 (X a)", formula->toString());
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
