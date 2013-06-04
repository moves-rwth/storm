/*
 * Ltl2dstarFormulaAdapterTest.cpp
 *
 *  Created on: 01.06.2013
 *      Author: thomas
 */



#include "gtest/gtest.h"

#include "src/formula/Ltl.h"
#include "src/adapters/Ltl2dstarFormulaAdapter.h"
#include "src/parser/LtlParser.h"

class Ltl2dstarFormulaAdapterTest : public testing::Test {
protected:
	virtual void SetUp() {
		labeling.addAtomicProposition("a");
		labeling.addAtomicProposition("b");
	}

	//virtual void TearDown()

	storm::models::AtomicPropositionsLabeling labeling = storm::models::AtomicPropositionsLabeling(1,2);
};

TEST_F(Ltl2dstarFormulaAdapterTest, ConvertAndFormulaTest) {
	storm::parser::LtlParser parser ("a & b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) && (b)");

	delete parser.getFormula();
}

TEST_F(Ltl2dstarFormulaAdapterTest, ConvertUntilFormulaTest) {
	storm::parser::LtlParser parser ("a U b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) U (b)");
	delete parser.getFormula();
}

TEST_F(Ltl2dstarFormulaAdapterTest, ConvertNextEventuallyFormulaTest) {
	storm::parser::LtlParser parser ("(F a) & X b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(<> (a)) && (X (b))");
	delete parser.getFormula();
}

TEST_F(Ltl2dstarFormulaAdapterTest, ConvertBoundedEventuallyFormulaTest) {
	storm::parser::LtlParser parser ("F <= 3 a");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	//Bounded eventually is "unrolled", as Ltl2dstar does not support bounded operators
	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) || (X ((a) || (X ((a) || (X (a))))))");
	delete parser.getFormula();
}

TEST_F(Ltl2dstarFormulaAdapterTest, ConvertBoundedUntilFormulaTest) {
	storm::parser::LtlParser parser ("a U<=3 b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	//Bounded until is "unrolled", as Ltl2dstar does not support bounded operators
	ASSERT_EQ(ltlFormula.toStringInfix(), "(b) || ((a) && (X ((b) || ((a) && (X ((b) || ((a) && (X (b)))))))))");
	delete parser.getFormula();
}
