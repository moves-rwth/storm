/*
 * parse_dtmc_test.cpp
 *
 *  Created on: 03.12.2012
 *      Author: Thomas Heinemann
 */


#include "gtest/gtest.h"
#include "mrmc-config.h"
#include "src/utility/utility.h"

TEST(ParseDtmcTest, parseAndOutput) {
	mrmc::models::Dtmc<double>* myDtmc;
	ASSERT_NO_THROW(myDtmc = mrmc::utility::parseDTMC(
			MRMC_CPP_TESTS_BASE_PATH "/parser/tra_files/pctl_general_input_01.tra",
			MRMC_CPP_TESTS_BASE_PATH "/parser/lab_files/pctl_general_input_01.lab"));

	ASSERT_NO_THROW(mrmc::utility::dtmcToDot(myDtmc, MRMC_CPP_TESTS_BASE_PATH "/parser/output.dot"));

	delete myDtmc;
}


