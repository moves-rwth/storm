/*
 * ActionTest.cpp
 *
 *  Created on: Jun 27, 2014
 *      Author: Manuel Sascha Weiand
 */

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/formula/actions/BoundAction.h"
#include "src/formula/actions/FormulaAction.h"
#include "src/formula/actions/InvertAction.h"
#include "src/formula/actions/RangeAction.h"
#include "src/formula/actions/SortAction.h"

#include "src/parser/MarkovAutomatonParser.h"
#include "src/parser/DeterministicModelParser.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/exceptions/InvalidArgumentException.h"

typedef typename storm::property::action::AbstractAction<double>::Result Result;

TEST(ActionTest, BoundActionFunctionality) {

	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());

	// Test the action.
	// First test that the boundAction build by the empty constructor does not change the selection.
	storm::property::action::BoundAction<double> action;
	Result result = action.evaluate(input, mc);

	for(auto value : result.selection) {
		ASSERT_TRUE(input.selection[value]);
	}

	// Test that using a strict bound can give different results than using a non-strict bound.
	action = storm::property::action::BoundAction<double>(storm::property::GREATER, 0);
	result = action.evaluate(input, mc);

	for(uint_fast64_t i = 0; i < result.selection.size()-2; i++) {
		ASSERT_TRUE(result.selection[i]);
	}
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Test whether the filter actually uses the selection given by the input.
	action = storm::property::action::BoundAction<double>(storm::property::LESS, 0.5);
	result = action.evaluate(result, mc);

	ASSERT_FALSE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Check whether the state order has any effect on the selected states, which it should not.
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = pathResult.size() - i - 1;
	}

	action = storm::property::action::BoundAction<double>(storm::property::GREATER, 0);
	result = action.evaluate(input, mc);

	for(uint_fast64_t i = 0; i < result.selection.size()-2; i++) {
		ASSERT_TRUE(result.selection[i]);
	}
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Test the functionality for state formulas instead.
	input.pathResult = std::vector<double>();
	input.stateResult = mc.checkAp(storm::property::prctl::Ap<double>("a"));
	action = storm::property::action::BoundAction<double>(storm::property::GREATER, 0.5);
	result = action.evaluate(input, mc);

	for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
		if(i == 5) {
			ASSERT_TRUE(result.selection[i]);
		} else {
			ASSERT_FALSE(result.selection[i]);
		}
	}

	// Make sure that the modelchecker has no influence on the result.
	storm::models::MarkovAutomaton<double> ma = storm::parser::MarkovAutomatonParser::parseMarkovAutomaton(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/ma_general.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/ma_general.lab");
	storm::modelchecker::csl::SparseMarkovAutomatonCslModelChecker<double> cslMc(ma);
	result = action.evaluate(input, cslMc);

	for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
		if(i == 5) {
			ASSERT_TRUE(result.selection[i]);
		} else {
			ASSERT_FALSE(result.selection[i]);
		}
	}
}

TEST(ActionTest, BoundActionSafety) {

	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("a"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());

	// First, test unusual bounds.
	storm::property::action::BoundAction<double> action(storm::property::LESS, -2044);
	Result result;

	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(0, result.selection.getNumberOfSetBits());

	action = storm::property::action::BoundAction<double>(storm::property::GREATER_EQUAL, 5879);

	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(0, result.selection.getNumberOfSetBits());

	action = storm::property::action::BoundAction<double>(storm::property::LESS_EQUAL, 5879);

	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(result.selection.size(), result.selection.getNumberOfSetBits());

	// Now, check the behavior under a undefined comparison type.
	action = storm::property::action::BoundAction<double>(static_cast<storm::property::ComparisonType>(10), 5879);
	ASSERT_THROW(action.toString(), storm::exceptions::InvalidArgumentException);
	ASSERT_THROW(action.evaluate(input, mc), storm::exceptions::InvalidArgumentException);

	// Test for a result input with both results filled.
	// It should put out a warning and use the pathResult.
	action = storm::property::action::BoundAction<double>(storm::property::GREATER_EQUAL, 0.5);
	input.stateResult = stateResult;

	// To capture the warning, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf *sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	std::cout.rdbuf(sbuf);

	ASSERT_FALSE(buffer.str().empty());
	ASSERT_TRUE(result.selection[0]);
	ASSERT_FALSE(result.selection[1]);
	ASSERT_TRUE(result.selection[2]);
	ASSERT_TRUE(result.selection[5]);

	// Check for empty input.
	ASSERT_NO_THROW(result = action.evaluate(Result(), mc));
}

TEST(ActionTest, FormulaActionFunctionality) {

	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Test the action.
	// First test that the empty action does no change to the input.
	storm::property::action::FormulaAction<double> action;
	input.selection.set(0,false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		if(i != 0) {
			ASSERT_TRUE(result.selection[i]);
		} else {
			ASSERT_FALSE(result.selection[i]);
		}
		ASSERT_EQ(i, result.stateMap[i]);
		ASSERT_EQ(input.pathResult[i], result.pathResult[i]);
	}
	ASSERT_TRUE(result.stateResult.size() == 0);
	input.selection.set(0,true);

	// Now test the general functionality.
	action = storm::property::action::FormulaAction<double>(std::make_shared<storm::property::prctl::ProbabilisticBoundOperator<double>>(storm::property::LESS, 0.5, std::make_shared<storm::property::prctl::Eventually<double>>(std::make_shared<storm::property::prctl::Ap<double>>("b"))));
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_TRUE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_TRUE(result.selection[2]);
	ASSERT_FALSE(result.selection[3]);
	ASSERT_FALSE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	// Check that the actual modelchecking results are not touched.
	ASSERT_EQ(input.stateResult.size(), result.stateResult.size());
	ASSERT_EQ(input.pathResult.size(), result.pathResult.size());
	for(uint_fast64_t i = 0; i < input.pathResult.size(); i++) {
		ASSERT_EQ(input.pathResult[i], result.pathResult[i]);
	}

	// Do the same but this time using a state result instead of a path result.
	input.pathResult = std::vector<double>();
	input.stateResult = stateResult;
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_TRUE(result.selection[0]);
	ASSERT_TRUE(result.selection[1]);
	ASSERT_TRUE(result.selection[2]);
	ASSERT_FALSE(result.selection[3]);
	ASSERT_FALSE(result.selection[4]);
	ASSERT_TRUE(result.selection[5]);
	ASSERT_FALSE(result.selection[6]);
	ASSERT_FALSE(result.selection[7]);

	ASSERT_EQ(input.stateResult.size(), result.stateResult.size());
	ASSERT_EQ(input.pathResult.size(), result.pathResult.size());
	for(uint_fast64_t i = 0; i < input.stateResult.size(); i++) {
		ASSERT_EQ(input.stateResult[i], result.stateResult[i]);
	}
}

TEST(ActionTest, FormulaActionSafety){
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Check that constructing the action using a nullptr and using an empty constructor leads to the same behavior.
	storm::property::action::FormulaAction<double> action(std::shared_ptr<storm::property::csl::AbstractStateFormula<double>>(nullptr));
	input.selection.set(0,false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		if(i != 0) {
			ASSERT_TRUE(result.selection[i]);
		} else {
			ASSERT_FALSE(result.selection[i]);
		}
		ASSERT_EQ(i, result.stateMap[i]);
		ASSERT_EQ(input.pathResult[i], result.pathResult[i]);
	}
	ASSERT_TRUE(result.stateResult.size() == 0);
	input.selection.set(0,true);
	ASSERT_NO_THROW(action.toString());
}

TEST(ActionTest, InvertActionFunctionality){
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = pathResult.size()-i-1;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Check whether the selection becomes inverted while the rest stays the same.
	storm::property::action::InvertAction<double> action;
	input.selection.set(0,false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		if(i != 0) {
			ASSERT_FALSE(result.selection[i]);
		} else {
			ASSERT_TRUE(result.selection[i]);
		}
		ASSERT_EQ(pathResult.size()-i-1, result.stateMap[i]);
		ASSERT_EQ(input.pathResult[i], result.pathResult[i]);
	}
	ASSERT_TRUE(result.stateResult.size() == 0);
	input.selection.set(0,true);
	ASSERT_NO_THROW(action.toString());
}

TEST(ActionTest, RangeActionFunctionality){
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Test if the action selects the first 3 states in relation to the order given by the stateMap.
	// First in index order.
	storm::property::action::RangeAction<double> action(0,2);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
		ASSERT_EQ(input.stateMap[i], result.stateMap[i]);
	}
	for(uint_fast64_t i = 0; i < 3; i++) {
		ASSERT_TRUE(result.selection[i]);
	}
	for(uint_fast64_t i = 3; i < result.selection.size(); i++) {
		ASSERT_FALSE(result.selection[i]);
	}
	input.selection.clear();
	input.selection.complement();

	// Now against index order.
	for(uint_fast64_t i = 0; i < input.pathResult.size(); i++) {
		input.stateMap[i] = input.pathResult.size()-i-1;
	}
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
		ASSERT_EQ(input.stateMap[i], result.stateMap[i]);
	}
	for(uint_fast64_t i = 0; i < 3; i++) {
		ASSERT_TRUE(result.selection[result.selection.size()-i-1]);
	}
	for(uint_fast64_t i = 3; i < result.selection.size(); i++) {
		ASSERT_FALSE(result.selection[result.selection.size()-i-1]);
	}
	input.selection.clear();
	input.selection.complement();

	// Finally test a random order.
	std::srand(time(nullptr));
	uint_fast64_t pos1, pos2, temp;
	for(uint_fast64_t i = 0; i < 100; i++) {
		// Randomly select two positions.
		pos1 = rand() % result.selection.size();
		pos2 = rand() % result.selection.size();

		// Swap the values there.
		temp = input.stateMap[pos1];
		input.stateMap[pos1] = input.stateMap[pos2];
		input.stateMap[pos2] = temp;
	}
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < 8; i++) {
		ASSERT_EQ(input.stateMap[i], result.stateMap[i]);
	}
	for(uint_fast64_t i = 0; i < 3; i++) {
		ASSERT_TRUE(result.selection[result.stateMap[i]]);
	}
	for(uint_fast64_t i = 3; i < result.selection.size(); i++) {
		ASSERT_FALSE(result.selection[result.stateMap[i]]);
	}

	// Test that specifying and interval of (i,i) selects only state i.
	for(uint_fast64_t i = 0; i < input.selection.size(); i++) {
		action = storm::property::action::RangeAction<double>(i,i);
		ASSERT_NO_THROW(result = action.evaluate(input, mc));
		ASSERT_EQ(1, result.selection.getNumberOfSetBits());
		ASSERT_TRUE(result.selection[result.stateMap[i]]);
	}
}

TEST(ActionTest, RangeActionSafety){
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = i;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Test invalid ranges.

	// To capture the warning, redirect cout and test the written buffer content.
	std::stringstream buffer;
	std::streambuf * sbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	storm::property::action::RangeAction<double> action(0,42);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_TRUE(result.selection.full());

	ASSERT_FALSE(buffer.str().empty());
	buffer.str("");

	action = storm::property::action::RangeAction<double>(42,98);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_TRUE(result.selection.empty());

	ASSERT_FALSE(buffer.str().empty());
	std::cout.rdbuf(sbuf);

	ASSERT_THROW(storm::property::action::RangeAction<double>(3,1), storm::exceptions::IllegalArgumentValueException);
}

TEST(ActionTest, SortActionFunctionality){
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = pathResult.size()-i-1;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, storm::storage::BitVector());
	Result result;

	// Test that sorting preserves everything except the state map.
	storm::property::action::SortAction<double> action;
	ASSERT_NO_THROW(action.toString());
	input.selection.set(0,false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		if(i != 0) {
			ASSERT_TRUE(result.selection[i]);
		} else {
			ASSERT_FALSE(result.selection[i]);
		}
		ASSERT_EQ(i, result.stateMap[i]);
		ASSERT_EQ(input.pathResult[i], result.pathResult[i]);
	}
	ASSERT_TRUE(result.stateResult.size() == 0);
	input.selection.set(0,true);

	// Test sorting cases. Note that the input selection should be irrelevant for the resulting state order.
	// 1) index, ascending -> see above
	// 2) index descending
	input.selection.set(3,false);
	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::INDEX, false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		ASSERT_EQ(pathResult.size()-i-1, result.stateMap[i]);
	}

	// 3) value, ascending
	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::VALUE);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(7, result.stateMap[1]);
	ASSERT_EQ(3, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(0, result.stateMap[5]);
	ASSERT_EQ(2, result.stateMap[6]);
	ASSERT_EQ(5, result.stateMap[7]);

	// 3) value, decending
	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::VALUE, false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(5, result.stateMap[0]);
	ASSERT_EQ(2, result.stateMap[1]);
	ASSERT_EQ(0, result.stateMap[2]);
	ASSERT_EQ(1, result.stateMap[3]);
	ASSERT_EQ(4, result.stateMap[4]);
	ASSERT_EQ(3, result.stateMap[5]);
	ASSERT_EQ(6, result.stateMap[6]);
	ASSERT_EQ(7, result.stateMap[7]);

	// Check that this also works for state results instead.
	input.pathResult = std::vector<double>();
	input.stateResult = stateResult;

	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::VALUE);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(5, result.stateMap[0]);
	ASSERT_EQ(6, result.stateMap[1]);
	ASSERT_EQ(7, result.stateMap[2]);
	ASSERT_EQ(0, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(2, result.stateMap[5]);
	ASSERT_EQ(3, result.stateMap[6]);
	ASSERT_EQ(4, result.stateMap[7]);

	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::VALUE, false);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(0, result.stateMap[0]);
	ASSERT_EQ(1, result.stateMap[1]);
	ASSERT_EQ(2, result.stateMap[2]);
	ASSERT_EQ(3, result.stateMap[3]);
	ASSERT_EQ(4, result.stateMap[4]);
	ASSERT_EQ(5, result.stateMap[5]);
	ASSERT_EQ(6, result.stateMap[6]);
	ASSERT_EQ(7, result.stateMap[7]);

	// Test if the resulting order does not depend on the input order.
	input.stateResult = storm::storage::BitVector();
	input.pathResult = pathResult;

	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::INDEX);
	ASSERT_NO_THROW(input = action.evaluate(input, mc));
	action = storm::property::action::SortAction<double>(storm::property::action::SortAction<double>::VALUE);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(7, result.stateMap[1]);
	ASSERT_EQ(3, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(0, result.stateMap[5]);
	ASSERT_EQ(2, result.stateMap[6]);
	ASSERT_EQ(5, result.stateMap[7]);

}

TEST(ActionTest, SortActionSafety){
	// Check that the path result has priority over the state result if for some erronous reason both are given.
	// Setup the modelchecker.
	storm::models::Dtmc<double> model = storm::parser::DeterministicModelParser::parseDtmc(STORM_CPP_TESTS_BASE_PATH "/functional/parser/tra_files/dtmc_actionTest.tra", STORM_CPP_TESTS_BASE_PATH "/functional/parser/lab_files/dtmc_actionTest.lab");
	storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double> mc(model, new storm::solver::GmmxxLinearEquationSolver<double>());

	// Build the filter input.
	// Basically the modelchecking result of "F a" on the used DTMC.
	std::vector<double> pathResult = mc.checkEventually(storm::property::prctl::Eventually<double>(std::make_shared<storm::property::prctl::Ap<double>>("a")), false);
	storm::storage::BitVector stateResult = mc.checkAp(storm::property::prctl::Ap<double>("c"));
	std::vector<uint_fast64_t> stateMap(pathResult.size());
	for(uint_fast64_t i = 0; i < pathResult.size(); i++) {
		stateMap[i] = pathResult.size()-i-1;
	}
	Result input(storm::storage::BitVector(pathResult.size(), true), stateMap, pathResult, stateResult);
	Result result;

	storm::property::action::SortAction<double> action(storm::property::action::SortAction<double>::VALUE);
	ASSERT_NO_THROW(result = action.evaluate(input, mc));
	ASSERT_EQ(6, result.stateMap[0]);
	ASSERT_EQ(7, result.stateMap[1]);
	ASSERT_EQ(3, result.stateMap[2]);
	ASSERT_EQ(4, result.stateMap[3]);
	ASSERT_EQ(1, result.stateMap[4]);
	ASSERT_EQ(0, result.stateMap[5]);
	ASSERT_EQ(2, result.stateMap[6]);
	ASSERT_EQ(5, result.stateMap[7]);
}
