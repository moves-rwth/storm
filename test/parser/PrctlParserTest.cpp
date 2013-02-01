/*
 * PrctlParserTest.cpp
 *
 *  Created on: 01.02.2013
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/parser/PrctlParser.h"
#include <iostream>

TEST(PrctlParserTest, parseAndOutput) {
	storm::parser::PrctlParser* prctlParser = nullptr;
	ASSERT_NO_THROW(
			prctlParser = new storm::parser::PrctlParser(STORM_CPP_TESTS_BASE_PATH "/parser/prctl_files/testFormula.prctl")
	);

	ASSERT_NE(prctlParser->getFormula(), nullptr);


	std::cout << prctlParser->getFormula()->toString();

	delete prctlParser;

}
