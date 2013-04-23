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
	std::string ap = "ap";
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser(ap);
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);


	ASSERT_EQ(ltlParser->getFormula()->toString(), ap);

	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, parsePropositionalFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("!(a & b) | a & ! c")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);


	ASSERT_EQ(ltlParser->getFormula()->toString(), "(!(a & b) | (a & !c))");

	delete ltlParser->getFormula();
	delete ltlParser;
}

/*!
 * The following test checks whether in the formula "F & b", F is correctly interpreted as proposition instead of the
 * "Eventually" operator.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("F & b")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);


	ASSERT_EQ(ltlParser->getFormula()->toString(), "(F & b)");

	delete ltlParser->getFormula();
	delete ltlParser;
}

/*!
 * The following test checks whether in the formula "F F", F is interpreted as "eventually" operator or atomic proposition,
 * depending where it occurs.
 */
TEST(LtlParserTest, parseAmbiguousFormulaTest2) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("F F")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);


	ASSERT_EQ(ltlParser->getFormula()->toString(), "F F");

	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, parseBoundedEventuallyFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("F<=5 a")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);

	storm::formula::ltl::BoundedEventually<double>* op = static_cast<storm::formula::ltl::BoundedEventually<double>*>(ltlParser->getFormula());

	ASSERT_EQ(static_cast<uint_fast64_t>(5), op->getBound());

	ASSERT_EQ(ltlParser->getFormula()->toString(), "F<=5 a");

	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, parseBoundedUntilFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("a U<=3 b")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);

	storm::formula::ltl::BoundedUntil<double>* op = static_cast<storm::formula::ltl::BoundedUntil<double>*>(ltlParser->getFormula());

	ASSERT_EQ(static_cast<uint_fast64_t>(3), op->getBound());

	ASSERT_EQ("(a U<=3 b)", ltlParser->getFormula()->toString());

	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, parseComplexUntilTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("a U b U<=3 c")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);


	ASSERT_EQ(ltlParser->getFormula()->toString(), "((a U b) U<=3 c)");

	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, parseComplexFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_NO_THROW(
			ltlParser = new storm::parser::LtlParser("a U F b | G a & F<=3 a U<=7 b")
	);

	ASSERT_NE(ltlParser->getFormula(), nullptr);

	ASSERT_EQ("(a U F (b | G (a & F<=3 (a U<=7 b))))", ltlParser->getFormula()->toString());
	delete ltlParser->getFormula();
	delete ltlParser;
}

TEST(LtlParserTest, wrongFormulaTest) {
	storm::parser::LtlParser* ltlParser = nullptr;
	ASSERT_THROW(
			ltlParser = new storm::parser::LtlParser("(a | b) & +"),
			storm::exceptions::WrongFormatException
	);
	delete ltlParser;
}
