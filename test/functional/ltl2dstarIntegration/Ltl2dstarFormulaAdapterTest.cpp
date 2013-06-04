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

TEST(Ltl2dstarFormulaAdapterTest, ConvertAndFormulaTest) {
	storm::parser::LtlParser parser ("a & b");

	storm::models::AtomicPropositionsLabeling labeling(1, 2);
	labeling.addAtomicProposition("a");
	labeling.addAtomicProposition("b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) && (b)");

	delete parser.getFormula();
}

TEST(Ltl2dstarFormulaAdapterTest, ConvertUntilFormulaTest) {
	storm::parser::LtlParser parser ("a U b");

	storm::models::AtomicPropositionsLabeling labeling(1, 2);
	labeling.addAtomicProposition("a");
	labeling.addAtomicProposition("b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) U (b)");
	delete parser.getFormula();
}

TEST(Ltl2dstarFormulaAdapterTest, ConvertNextEventuallyFormulaTest) {
	storm::parser::LtlParser parser ("(F a) & X b");

	storm::models::AtomicPropositionsLabeling labeling(1, 2);
	labeling.addAtomicProposition("a");
	labeling.addAtomicProposition("b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	ASSERT_EQ(ltlFormula.toStringInfix(), "(<> (a)) && (X (b))");
	delete parser.getFormula();
}

TEST(Ltl2dstarFormulaAdapterTest, ConvertBoundedEventuallyFormulaTest) {
	storm::parser::LtlParser parser ("F <= 3 a");

	storm::models::AtomicPropositionsLabeling labeling(1, 2);
	labeling.addAtomicProposition("a");
	labeling.addAtomicProposition("b");

	LTLFormula ltlFormula = storm::adapters::Ltl2dstarFormulaAdapter<double>::convert(*parser.getFormula(), labeling);

	//Bounded eventually is "unrolled", as Ltl2dstar does not support bounded operators
	ASSERT_EQ(ltlFormula.toStringInfix(), "(a) || (X ((a) || (X ((a) || (X (a))))))");
	delete parser.getFormula();
}
