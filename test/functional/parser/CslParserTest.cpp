/*
 * CslParserTest.cpp
 *
 *  Created on: 09.04.2013
 *      Author: Thomas Heinemann
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/CslParser.h"

TEST(CslParserTest, parseApOnlyTest) {
	std::string formula = "ap";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);
	ASSERT_EQ(cslFormula->toString(), formula);

	delete cslFormula;
}

TEST(CslParserTest, parsePropositionalFormulaTest) {
	std::string formula = "!(a & b) | a & ! c";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);
	ASSERT_EQ(cslFormula->toString(), "(!(a & b) | (a & !c))");

	delete cslFormula;
}

TEST(CslParserTest, parseProbabilisticFormulaTest) {
	std::string formula = "P > 0.5 [ F a ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);

	storm::property::csl::ProbabilisticBoundOperator<double>* op = static_cast<storm::property::csl::ProbabilisticBoundOperator<double>*>(cslFormula);
	ASSERT_EQ(storm::property::GREATER, op->getComparisonOperator());
	ASSERT_EQ(0.5, op->getBound());

	ASSERT_EQ(cslFormula->toString(), "P > 0.500000 [F a]");

	delete cslFormula;
}

TEST(CslParserTest, parseSteadyStateBoundFormulaTest) {
	std::string formula = "S >= 15 [ P < 0.2 [ a U<=3 b ] ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);

	storm::property::csl::SteadyStateBoundOperator<double>* op = static_cast<storm::property::csl::SteadyStateBoundOperator<double>*>(cslFormula);
	ASSERT_EQ(storm::property::GREATER_EQUAL, op->getComparisonOperator());
	ASSERT_EQ(15.0, op->getBound());

	ASSERT_EQ(cslFormula->toString(), "S >= 15.000000 [P < 0.200000 [a U[0.000000,3.000000] b]]");

	delete cslFormula;
}

TEST(CslParserTest, parseSteadyStateNoBoundFormulaTest) {
	std::string formula = "S = ? [ P <= 0.5 [ F<=3 a ] ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);
	ASSERT_EQ(cslFormula->toString(), "S = ? [P <= 0.500000 [F[0.000000,3.000000] a]]");

	delete cslFormula;
}

TEST(CslParserTest, parseProbabilisticNoBoundFormulaTest) {
	std::string formula = "P = ? [ a U [3,4] b & (!c) ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);
	ASSERT_EQ(cslFormula->toString(), "P = ? [a U[3.000000,4.000000] (b & !c)]");

	delete cslFormula;
}


TEST(CslParserTest, parseComplexFormulaTest) {
	std::string formula = "S<=0.5 [ P <= 0.5 [ a U c ] ] & (P > 0.5 [ G b] | !P < 0.4 [ G P>0.9 [F >=7 a & b] ])  //and a comment";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_NO_THROW(
		cslFormula = storm::parser::CslParser(formula);
	);

	ASSERT_NE(cslFormula, nullptr);
	ASSERT_EQ(cslFormula->toString(), "(S <= 0.500000 [P <= 0.500000 [a U c]] & (P > 0.500000 [G b] | !P < 0.400000 [G P > 0.900000 [F>=7.000000 (a & b)]]))");

	delete cslFormula;
}

TEST(CslParserTest, wrongProbabilisticFormulaTest) {
	std::string formula = "P > 0.5 [ a ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_THROW(
		cslFormula = storm::parser::CslParser(formula),	
		storm::exceptions::WrongFormatException
	);
}

TEST(CslParserTest, wrongFormulaTest) {
	std::string formula = "(a | b) & +";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_THROW(
		cslFormula = storm::parser::CslParser(formula),	
		storm::exceptions::WrongFormatException
	);
}

TEST(CslParserTest, wrongFormulaTest2) {
	std::string formula = "P>0 [ F & a ]";
	storm::property::csl::AbstractCslFormula<double>* cslFormula = nullptr;
	ASSERT_THROW(
		cslFormula = storm::parser::CslParser(formula),	
		storm::exceptions::WrongFormatException
	);
}
