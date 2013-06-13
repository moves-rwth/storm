/*
 * LtlParserTest.cpp
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/LtlParser.h"

TEST(LtlParserTest, parseApOnlyTest) {
	std::string formula = "ap";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), formula);

	delete ltlFormula;
}

TEST(LtlParserTest, parsePropositionalFormulaTest) {
	std::string formula = "!(a & b) | a & ! c";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(!(a & b) | (a & !c))");

	delete ltlFormula;
}

/*!
 * The following test checks whether in the formula "F & b", F is correctly interpreted as proposition instead of the
 * "Eventually" operator.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest) {
	std::string formula = "F & b";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(F & b)");

	delete ltlFormula;
}

/*!
 * The following test checks whether in the formula "F F", F is interpreted as "eventually" operator or atomic proposition,
 * depending where it occurs.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest2) {
	std::string formula = "F F";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), "F F");

	delete ltlFormula;
}

TEST(LtlParserTest, parseBoundedEventuallyFormulaTest) {
	std::string formula = "F<=5 a";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);

	storm::property::ltl::BoundedEventually<double>* op = static_cast<storm::property::ltl::BoundedEventually<double>*>(ltlFormula);
	ASSERT_EQ(static_cast<uint_fast64_t>(5), op->getBound());


	ASSERT_EQ(ltlFormula->toString(), "F<=5 a");

	delete ltlFormula;
}

TEST(LtlParserTest, parseBoundedUntilFormulaTest) {
	std::string formula = "a U<=3 b";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);

	storm::property::ltl::BoundedUntil<double>* op = static_cast<storm::property::ltl::BoundedUntil<double>*>(ltlFormula);
	ASSERT_EQ(static_cast<uint_fast64_t>(3), op->getBound());


	ASSERT_EQ(ltlFormula->toString(), "(a U<=3 b)");

	delete ltlFormula;
}

TEST(LtlParserTest, parseComplexUntilTest) {
	std::string formula = "a U b U<=3 c";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), "((a U b) U<=3 c)");

	delete ltlFormula;
}

TEST(LtlParserTest, parseComplexFormulaTest) {
	std::string formula = "a U F b | G a & F<=3 a U<=7 b // and a comment";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_NO_THROW(
		ltlFormula = storm::parser::LtlParser(formula);
	);

	ASSERT_NE(ltlFormula, nullptr);
	ASSERT_EQ(ltlFormula->toString(), "(a U F (b | G (a & F<=3 (a U<=7 b))))");

	delete ltlFormula;
}

TEST(LtlParserTest, wrongFormulaTest) {
	std::string formula = "(a | c) & +";
	storm::property::ltl::AbstractLtlFormula<double>* ltlFormula = nullptr;
	ASSERT_THROW(
		ltlFormula = storm::parser::LtlParser(formula),
		storm::exceptions::WrongFormatException
	);
}
