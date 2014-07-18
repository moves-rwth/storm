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

namespace ltl = storm::property::ltl;

TEST(LtlParserTest, parseApOnlyTest) {
	std::string formula = "ap";
	std::shared_ptr<storm::property::ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), formula);
}

TEST(LtlParserTest, parsePropositionalFormulaTest) {
	std::string formula = "!(a & b) | a & ! c";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(!(a & b) | (a & !c))");
}

/*!
 * The following test checks whether in the formula "F & b", F is correctly interpreted as proposition instead of the
 * "Eventually" operator.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest) {
	std::string formula = "F & b";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(F & b)");
}

/*!
 * The following test checks whether in the formula "F F", F is interpreted as "eventually" operator or atomic proposition,
 * depending where it occurs.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest2) {
	std::string formula = "F F";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), "F F");
}

TEST(LtlParserTest, parseBoundedEventuallyFormulaTest) {
	std::string formula = "F<=5 a";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);

	std::shared_ptr<storm::property::ltl::BoundedEventually<double>> op = std::dynamic_pointer_cast<storm::property::ltl::BoundedEventually<double>>(ltlFormula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(static_cast<uint_fast64_t>(5), op->getBound());


	ASSERT_EQ(ltlFormula->toString(), "F<=5 a");
}

TEST(LtlParserTest, parseBoundedUntilFormulaTest) {
	std::string formula = "a U<=3 b";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);

	std::shared_ptr<storm::property::ltl::BoundedUntil<double>> op = std::dynamic_pointer_cast<storm::property::ltl::BoundedUntil<double>>(ltlFormula->getChild());
	ASSERT_NE(op.get(), nullptr);
	ASSERT_EQ(static_cast<uint_fast64_t>(3), op->getBound());


	ASSERT_EQ(ltlFormula->toString(), "(a U<=3 b)");
}

TEST(LtlParserTest, parseComplexUntilTest) {
	std::string formula = "a U b U<=3 c";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), "((a U b) U<=3 c)");
}

TEST(LtlParserTest, parseComplexFormulaTest) {
	std::string formula = "a U F b | G a & F<=3 a U<=7 b // and a comment";
	std::shared_ptr<ltl::LtlFilter<double>> ltlFormula(nullptr);
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser::parseLtlFormula(formula);
	);

	ASSERT_NE(ltlFormula.get(), nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(a U F (b | G (a & F<=3 (a U<=7 b))))");
}

TEST(LtlParserTest, wrongFormulaTest) {
	std::string formula = "(a | c) & +";
	ASSERT_THROW(
		storm::parser::LtlParser::parseLtlFormula(formula),
		storm::exceptions::WrongFormatException
	);
}
