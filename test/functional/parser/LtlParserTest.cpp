/*
 * LtlParserTest.cpp
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/LtlParser.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/properties/actions/InvertAction.h"
#include "src/properties/actions/SortAction.h"
#include "src/properties/actions/RangeAction.h"
#include "src/properties/actions/BoundAction.h"

namespace ltl = storm::properties::ltl;

TEST(LtlParserTest, parseApOnlyTest) {
	std::string input = "ap";
	std::shared_ptr<storm::properties::ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_TRUE(formula->getChild()->isPropositional());

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ(input, formula->toString());
}

TEST(LtlParserTest, parsePropositionalFormulaTest) {
	std::string input = "!(a & b) | a & ! c";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_TRUE(formula->getChild()->isPropositional());

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ("(!(a & b) | (a & !c))", formula->toString());
}

/*!
 * The following test checks whether in the formula "F & b", F is correctly interpreted as proposition instead of the
 * "Eventually" operator.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest) {
	std::string input = "F & b";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_TRUE(formula->getChild()->isPropositional());

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ("(F & b)", formula->toString());
}

/*!
 * The following test checks whether in the formula "F F", F is interpreted as "eventually" operator or atomic proposition,
 * depending where it occurs.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest2) {
	std::string input = "F F";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_FALSE(formula->getChild()->isPropositional());

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ("F F", formula->toString());
}

TEST(LtlParserTest, parseBoundedEventuallyFormulaTest) {
	std::string input = "F<=5 a";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());

	std::shared_ptr<storm::properties::ltl::BoundedEventually<double>> op = std::dynamic_pointer_cast<storm::properties::ltl::BoundedEventually<double>>(formula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(static_cast<uint_fast64_t>(5), op->getBound());


	ASSERT_EQ("F<=5 a", formula->toString());
}

TEST(LtlParserTest, parseBoundedUntilFormulaTest) {
	std::string input = "a U<=3 b";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_NE(formula.get(), nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());

	std::shared_ptr<storm::properties::ltl::BoundedUntil<double>> op = std::dynamic_pointer_cast<storm::properties::ltl::BoundedUntil<double>>(formula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(static_cast<uint_fast64_t>(3), op->getBound());


	ASSERT_EQ("(a U<=3 b)", formula->toString());
}

TEST(LtlParserTest, parseLtlFilterTest) {
	std::string input = "filter[max; invert; bound(<, 0.5); sort(value); range(0,3)](X a)";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::LtlParser::parseLtlFormula(input)
	);

	// The parser did not falsely recognize the input as a comment.
	ASSERT_NE(formula, nullptr);

	ASSERT_FALSE(formula->getChild()->isPropositional());

	ASSERT_EQ(storm::properties::MAXIMIZE, formula->getOptimizingOperator());

	ASSERT_EQ(4, formula->getActionCount());
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::InvertAction<double>>(formula->getAction(0)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::BoundAction<double>>(formula->getAction(1)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::SortAction<double>>(formula->getAction(2)).get(), nullptr);
	ASSERT_NE(std::dynamic_pointer_cast<storm::properties::action::RangeAction<double>>(formula->getAction(3)).get(), nullptr);

	// The input was parsed correctly.
	ASSERT_EQ("filter[max; invert; bound(<, 0.500000); sort(value, ascending); range(0, 3)](X a)", formula->toString());
}

TEST(LtlParserTest, commentTest) {
	std::string input = "// This is a comment. And this is a commented out formula: F X a";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::LtlParser::parseLtlFormula(input)
	);

	// The parser recognized the input as a comment.
	ASSERT_NE(nullptr, formula.get());
	ASSERT_EQ(nullptr, formula->getChild().get());

	// Test if the parser recognizes the comment at the end of a line.
	input = "F X a // This is a comment.";
	ASSERT_NO_THROW(
				formula = storm::parser::LtlParser::parseLtlFormula(input)
	);

	ASSERT_FALSE(formula->getChild()->isPropositional());

	ASSERT_EQ("F X a", formula->toString());
}

TEST(LtlParserTest, parseComplexUntilTest) {
	std::string input = "a U b U<=3 c";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ("((a U b) U<=3 c)", formula->toString());
}

TEST(LtlParserTest, parseComplexFormulaTest) {
	std::string input = "a U F b | G a & F<=3 a U<=7 b // and a comment";
	std::shared_ptr<ltl::LtlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
		formula = storm::parser::LtlParser::parseLtlFormula(input);
	);

	ASSERT_NE(formula.get(), nullptr);
	ASSERT_EQ("(a U F (b | G (a & F<=3 (a U<=7 b))))", formula->toString());
}

TEST(LtlParserTest, wrongFormulaTest) {
	std::string input = "(a | c) & +";
	ASSERT_THROW(
		storm::parser::LtlParser::parseLtlFormula(input),
		storm::exceptions::WrongFormatException
	);
}
