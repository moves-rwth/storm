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
#include "src/formula/actions/FormulaAction.h"
#include "src/formula/actions/InvertAction.h"
#include "src/formula/actions/SortAction.h"
#include "src/formula/actions/RangeAction.h"
#include "src/formula/actions/BoundAction.h"

namespace csl = storm::property::csl;

TEST(CslParserTest, parseApOnlyTest) {
	std::string input = "ap";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_TRUE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ(input, formula->toString());
}

TEST(CslParserTest, parsePropositionalFormulaTest) {
	std::string input = "!(a & b) | a & ! c";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_TRUE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("(!(a & b) | (a & !c))", formula->toString());
}

TEST(CslParserTest, parsePathFormulaTest) {
	std::string input = "X( P<0.9 (a U b))";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	// The input was parsed correctly.
	ASSERT_NE(std::dynamic_pointer_cast<csl::Next<double>>(formula->getChild()).get(), nullptr);
	auto nextFormula = std::dynamic_pointer_cast<csl::Next<double>>(formula->getChild());
	ASSERT_FALSE(nextFormula->isPropositional());
	ASSERT_FALSE(nextFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<csl::ProbabilisticBoundOperator<double>>(nextFormula->getChild()).get(), nullptr);
	auto probBoundFormula = std::dynamic_pointer_cast<csl::ProbabilisticBoundOperator<double>>(nextFormula->getChild());
	ASSERT_EQ(0.9, probBoundFormula->getBound());
	ASSERT_EQ(storm::property::LESS, probBoundFormula->getComparisonOperator());
	ASSERT_FALSE(probBoundFormula->isPropositional());
	ASSERT_TRUE(probBoundFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<csl::Until<double>>(probBoundFormula->getChild()).get(), nullptr);
	auto untilFormula = std::dynamic_pointer_cast<csl::Until<double>>(probBoundFormula->getChild());
	ASSERT_FALSE(untilFormula->isPropositional());
	ASSERT_FALSE(untilFormula->isProbEventuallyAP());

	ASSERT_NE(std::dynamic_pointer_cast<csl::Ap<double>>(untilFormula->getLeft()).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<csl::Ap<double>>(untilFormula->getRight()).get(), nullptr);
	ASSERT_EQ("a", std::dynamic_pointer_cast<csl::Ap<double>>(untilFormula->getLeft())->getAp());
	ASSERT_EQ("b", std::dynamic_pointer_cast<csl::Ap<double>>(untilFormula->getRight())->getAp());


	// The string representation is also correct.
	ASSERT_EQ("P = ? (X P < 0.900000 (a U b))", formula->toString());
}

TEST(CslParserTest, parseProbabilisticFormulaTest) {
	std::string input = "P > 0.5 [ F a ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	auto op = std::dynamic_pointer_cast<storm::property::csl::ProbabilisticBoundOperator<double>>(formula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(storm::property::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());
	ASSERT_FALSE(op->isPropositional());
	ASSERT_TRUE(op->isProbEventuallyAP());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("P > 0.500000 (F a)", formula->toString());
}

TEST(CslParserTest, parseSteadyStateBoundFormulaTest) {
	std::string input = "S >= 15 [ P < 0.2 [ a U<=3 b ] ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	auto op = std::dynamic_pointer_cast<storm::property::csl::SteadyStateBoundOperator<double>>(formula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(storm::property::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());
	ASSERT_FALSE(op->isPropositional());
	ASSERT_FALSE(op->isProbEventuallyAP());

	// Test the string representation for the parsed formula.
	ASSERT_EQ("S >= 15.000000 (P < 0.200000 (a U[0.000000,3.000000] b))", formula->toString());
}

TEST(CslParserTest, parseSteadyStateNoBoundFormulaTest) {
	std::string input = "S = ? [ P <= 0.5 [ F<=3 a ] ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("S = ? (P <= 0.500000 (F[0.000000,3.000000] a))", formula->toString());
}

TEST(CslParserTest, parseProbabilisticNoBoundFormulaTest) {
	std::string input = "P = ? [ a U [3,4] b & (!c) ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("P = ? (a U[3.000000,4.000000] (b & !c))", formula->toString());
}


TEST(CslParserTest, parseComplexFormulaTest) {
	std::string input = "S<=0.5 [ P <= 0.5 [ a U c ] ] & (P > 0.5 [ G b] | !P < 0.4 [ G P>0.9 [F >=7 a & b] ])  //and a comment";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input);
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("S <= 0.500000 ((P <= 0.500000 (a U c) & (P > 0.500000 (G b) | !P < 0.400000 (G P > 0.900000 (F>=7.000000 (a & b))))))", formula->toString());
}

TEST(CslParserTest, parseCslFilterTest) {
	std::string input = "filter[max; formula(b); invert; bound(<, 0.5); sort(value); range(0,3)](F a)";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_EQ(storm::property::MAXIMIZE, formula->getOptimizingOperator());

	ASSERT_EQ(5, formula->getActionCount());
	ASSERT_NE(std::dynamic_pointer_cast<storm::property::action::FormulaAction<double>>(formula->getAction(0)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::property::action::InvertAction<double>>(formula->getAction(1)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::property::action::BoundAction<double>>(formula->getAction(2)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::property::action::SortAction<double>>(formula->getAction(3)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::property::action::RangeAction<double>>(formula->getAction(4)).get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	// The input was parsed correctly.
	ASSERT_EQ("filter[max; formula(b); invert; bound(<, 0.500000); sort(value, ascending); range(0, 3)](F a)", formula->toString());
}

TEST(CslParserTest, commentTest) {
	std::string input = "// This is a comment. And this is a commented out formula: P<=0.5 [ X a ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	// The parser recognized the input as a comment.
	ASSERT_NE(nullptr, formula.get());

	// Test if the parser recognizes the comment at the end of a line.
	input = "P<=0.5 [ X a ] // This is a comment.";
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	ASSERT_FALSE(formula->getChild()->isPropositional());
	ASSERT_FALSE(formula->getChild()->isProbEventuallyAP());

	ASSERT_EQ("P <= 0.500000 (X a)", formula->toString());
}

TEST(CslParserTest, wrongProbabilisticFormulaTest) {
	std::string input = "P > 0.5 [ a ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
}

TEST(CslParserTest, wrongFormulaTest) {
	std::string input = "(a | b) & +";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
}

TEST(CslParserTest, wrongFormulaTest2) {
	std::string input = "P>0 [ F & a ]";
	std::shared_ptr<csl::CslFilter<double>> formula(nullptr);
	ASSERT_THROW(
		formula = storm::parser::CslParser::parseCslFormula(input),
		storm::exceptions::WrongFormatException
	);
}
