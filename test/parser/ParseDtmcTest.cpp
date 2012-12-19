/*
 * ParseDtmcTest.cpp
 *
 *  Created on: 03.12.2012
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "mrmc-config.h"
#include "src/parser/DtmcParser.h"
#include "src/utility/IoUtility.h"

TEST(ParseDtmcTest, parseAndOutput) {
	mrmc::parser::DtmcParser* dtmcParser;
	ASSERT_NO_THROW(dtmcParser = new mrmc::parser::DtmcParser(
			MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/pctl_general_input_01.tra",
			MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	ASSERT_NO_THROW(mrmc::utility::dtmcToDot(*(dtmcParser->getDtmc()), MRMC_CPP_TESTS_BASE_PATH "/parser/output.dot"));

	delete dtmcParser;
}


