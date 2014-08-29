/*
 * FilterTest.cpp
 *
 *  Created on: Aug 6, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/models/Dtmc.h"
#include "src/parser/DeterministicModelParser.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/properties/prctl/PrctlFilter.h"
#include "src/properties/csl/CslFilter.h"
#include "src/properties/ltl/LtlFilter.h"
#include "src/parser/PrctlParser.h"
#include "src/parser/CslParser.h"
#include "src/parser/MarkovAutomatonParser.h"
#include "src/properties/actions/InvertAction.h"

#include <memory>

typedef typename storm::properties::action::AbstractAction<double>::Result Result;

TEST(PrctlFilterTest, generalFunctionality) {
	// Test filter queries of increasing complexity.

	// Setup model and modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Find the best valued state to finally reach a 'b' state.
	std::string input = "filter[sort(value, descending); range(0,0)](F b)";
	std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	// Here we test if the check method gives the correct result.
	// To capture the output, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	formula->check(mc);

	// Reset cout to the original buffer.
	std::cout.rdbuf(sbuf);

	std::string output = buffer.str();
	ASSERT_NE(std::string::npos, output.find("\t6: 1"));

	// The remaining queries use evaluate directly as its easier to work with in a test environment.
	// Get the probability to reach a b state for all states but those that cannot reach a 'b' state or are 'b' states themselves.
	// Sorted by value; highest first.
	input = "filter[formula(P<=0(F b) | b); invert; sort(value, desc)](F b)";
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	Result result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(5, result.selection.getNumberOfSetBits());
	ASSERT_TRUE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_TRUE(result.selection[2]);
	ASSERT_TRUE(result.selection[3]);
	ASSERT_TRUE(result.selection[4]);
	ASSERT_FALSE(result.selection[5]);
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(7, result.stateMap[1]);
	ASSERT_EQ(3, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(0, result.stateMap[5]);
	ASSERT_EQ(2, result.stateMap[6]);
	ASSERT_EQ(5, result.stateMap[7]);

	// Get the probability for reaching a 'd' state only for those states that have a probability to do so of at most 0.5.
	// Sorted by value; lowest first.
	input = "filter[bound(<, 0.5); sort(value, ascending)](F d)";
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(4, result.selection.getNumberOfSetBits());
	ASSERT_FALSE(result.selection[0]);
	ASSERT_FALSE(result.selection[1]);
	ASSERT_FALSE(result.selection[2]);
	ASSERT_TRUE(result.selection[3]);
	ASSERT_FALSE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_TRUE(result.selection[6]);
	ASSERT_TRUE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(5, result.stateMap[0]);
	ASSERT_EQ(6, result.stateMap[1]);
	ASSERT_EQ(7, result.stateMap[2]);
	ASSERT_EQ(3, result.stateMap[3]);
	ASSERT_EQ(4, result.stateMap[4]);
	ASSERT_EQ(1, result.stateMap[5]);
	ASSERT_EQ(0, result.stateMap[6]);
	ASSERT_EQ(2, result.stateMap[7]);

	// Get the three highest indexed states reaching an 'a' state with probability at most 0.3.
	input = "filter[sort(value); range(5,7); sort(index, descending)](P<=0.3(F a))";
	ASSERT_NO_THROW(
				formula = storm::parser::PrctlParser::parsePrctlFormula(input)
	);

	result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(3, result.selection.getNumberOfSetBits());
	ASSERT_FALSE(result.selection[0]);
	ASSERT_FALSE(result.selection[1]);
	ASSERT_FALSE(result.selection[2]);
	ASSERT_TRUE(result.selection[3]);
	ASSERT_FALSE(result.selection[4]);
	ASSERT_FALSE(result.selection[5]);
	ASSERT_TRUE(result.selection[6]);
	ASSERT_TRUE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(7, result.stateMap[0]);
	ASSERT_EQ(6, result.stateMap[1]);
	ASSERT_EQ(5, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(3, result.stateMap[4]);
	ASSERT_EQ(2, result.stateMap[5]);
	ASSERT_EQ(1, result.stateMap[6]);
	ASSERT_EQ(0, result.stateMap[7]);

}

TEST(PrctlFilterTest, Safety) {
	// Setup model and modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Make a stub formula as child.
	auto apFormula = std::make_shared<storm::properties::prctl::Ap<double>>("a");

	// Test the filter for nullptr action handling.
	auto formula = std::make_shared<storm::properties::prctl::PrctlFilter<double>>(apFormula, nullptr);

	ASSERT_EQ(0, formula->getActionCount());
	ASSERT_NO_THROW(formula->evaluate(mc));

	// Repeat with vector containing only one action but three nullptr.
	std::vector<std::shared_ptr<storm::properties::action::AbstractAction<double>>> actions(4, nullptr);
	actions[1] = std::make_shared<storm::properties::action::InvertAction<double>>();

	formula = std::make_shared<storm::properties::prctl::PrctlFilter<double>>(apFormula, actions);

	ASSERT_EQ(1, formula->getActionCount());
	ASSERT_NO_THROW(formula->evaluate(mc));

	// Test the filter for nullptr child formula handling.
	// It sholud not write anything to the standard out and return an empty result.
	formula = std::make_shared<storm::properties::prctl::PrctlFilter<double>>();
	Result result;

	// To capture the output, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	ASSERT_NO_THROW(result = formula->evaluate(mc));

	// Reset cout to the original buffer.
	std::cout.rdbuf(sbuf);

	ASSERT_EQ(0, buffer.str().length());

	ASSERT_EQ(0, result.pathResult.size());
	ASSERT_EQ(0, result.stateResult.size());
	ASSERT_EQ(0, result.selection.size());
	ASSERT_EQ(0, result.stateMap.size());
}

TEST(CslFilterTest, generalFunctionality) {
	// Test filter queries of increasing complexity.

	// Setup model and modelchecker.
	storm::models::MarkovAutomaton<double> model = storm::parser::MarkovAutomatonParser::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_cslFilterTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_cslFilterTest.lab");
	storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> mc(model, std::make_shared<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>());

	// Find the best valued state to finally reach a 'r1' state.
	std::string input = "filter[max; sort(value, descending); range(0,0)](F r1)";
	std::shared_ptr<storm::properties::csl::CslFilter<double>> formula(nullptr);
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);


	// Here we test if the check method gives the correct result.
	// To capture the output, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	formula->check(mc);

	// Reset cout to the original buffer.
	std::cout.rdbuf(sbuf);

	std::string output = buffer.str();
	ASSERT_NE(std::string::npos, output.find("\t6: 1"));

	// The remaining queries use evaluate directly as its easier to work with in a test environment.
	// Get the maximum probability to reach an 'r1' state for all states but those that cannot reach an 'r1' state or are 'r1' states themselves.
	// Sorted by value; highest first.
	input = "filter[max; formula(P<=0(F r1) | r1); invert; sort(value, desc)](F r1)";
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	Result result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(6, result.selection.getNumberOfSetBits());
	ASSERT_TRUE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_TRUE(result.selection[2]);
	ASSERT_TRUE(result.selection[3]);
	ASSERT_TRUE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(5, result.stateMap[1]);
	ASSERT_EQ(2, result.stateMap[2]);
	ASSERT_EQ(3, result.stateMap[3]);
	ASSERT_EQ(0, result.stateMap[4]);
	ASSERT_EQ(4, result.stateMap[5]);
	ASSERT_EQ(1, result.stateMap[6]);
	ASSERT_EQ(7, result.stateMap[7]);

	// Get the minimum probability for reaching an 'err' state only for those states that have a probability to do so of at most 0.2.
	// Sorted by value; lowest first.
	input = "filter[min; bound(<, 0.2); sort(value, ascending)](F err)";
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(5, result.selection.getNumberOfSetBits());
	ASSERT_FALSE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_FALSE(result.selection[2]);
	ASSERT_FALSE(result.selection[3]);
	ASSERT_TRUE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_TRUE(result.selection[6]);
	ASSERT_TRUE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(7, result.stateMap[1]);
	ASSERT_EQ(5, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(0, result.stateMap[5]);
	ASSERT_EQ(2, result.stateMap[6]);
	ASSERT_EQ(3, result.stateMap[7]);

	// Get the three highest indexed states reaching an 'r2' state with probability at most 0.3.
	input = "filter[sort(value); range(5,7); sort(index, descending)](P<=0.3(F r2))";
	ASSERT_NO_THROW(
				formula = storm::parser::CslParser::parseCslFormula(input)
	);

	result = formula->evaluate(mc);

	// Test the selection.
	ASSERT_EQ(3, result.selection.getNumberOfSetBits());
	ASSERT_FALSE(result.selection[0]);
	ASSERT_FALSE(result.selection[1]);
	ASSERT_FALSE(result.selection[2]);
	ASSERT_TRUE(result.selection[3]);
	ASSERT_FALSE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_TRUE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Test the sorting.
	ASSERT_EQ(7, result.stateMap[0]);
	ASSERT_EQ(6, result.stateMap[1]);
	ASSERT_EQ(5, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(3, result.stateMap[4]);
	ASSERT_EQ(2, result.stateMap[5]);
	ASSERT_EQ(1, result.stateMap[6]);
	ASSERT_EQ(0, result.stateMap[7]);
}

TEST(CslFilterTest, Safety) {
	// Setup model and modelchecker.
	storm::models::MarkovAutomaton<double> model = storm::parser::MarkovAutomatonParser::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_cslFilterTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_cslFilterTest.lab");
	storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> mc(model, std::make_shared<storm::solver::GmmxxNondeterministicLinearEquationSolver<double>>());

	// Make a stub formula as child.
	auto apFormula = std::make_shared<storm::properties::csl::Ap<double>>("r1");

	// Test the filter for nullptr action handling.
	auto formula = std::make_shared<storm::properties::csl::CslFilter<double>>(apFormula, nullptr, storm::properties::MAXIMIZE);

	ASSERT_NO_THROW(formula->evaluate(mc));
	ASSERT_EQ(0, formula->getActionCount());

	// Repeat with vector containing only one action but three nullptr.
	std::vector<std::shared_ptr<storm::properties::action::AbstractAction<double>>> actions(4, nullptr);
	actions[1] = std::make_shared<storm::properties::action::InvertAction<double>>();

	formula = std::make_shared<storm::properties::csl::CslFilter<double>>(apFormula, actions, storm::properties::MAXIMIZE);

	ASSERT_EQ(1, formula->getActionCount());
	ASSERT_NO_THROW(formula->evaluate(mc));

	// Test the filter for nullptr child formula handling.
	// It sholud not write anything to the standard out and return an empty result.
	formula = std::make_shared<storm::properties::csl::CslFilter<double>>();
	Result result;

	// To capture the output, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	ASSERT_NO_THROW(result = formula->evaluate(mc));

	// Reset cout to the original buffer.
	std::cout.rdbuf(sbuf);

	ASSERT_EQ(0, buffer.str().length());

	ASSERT_EQ(0, result.pathResult.size());
	ASSERT_EQ(0, result.stateResult.size());
	ASSERT_EQ(0, result.selection.size());
	ASSERT_EQ(0, result.stateMap.size());
}

// TODO Set up the LtlFilterTest once an Ltl modelchecker has been implemented.
