/*
 *	MRMC - C++ Rebuild
 *	
 *	MRMC is a model checker for discrete-time and continuous-time Markov
 *	reward models. It supports reward extensions of PCTL and CSL (PRCTL
 *	and CSRL), and allows for the automated verification of properties
 *	concerning long-run and instantaneous rewards as well as cumulative
 *	rewards.
 *	
 *  Authors: Philipp Berger
 *
 *  Description: Central part of the application containing the main() Method
 */

#include <iostream>

#include "MRMCConfig.h"
 
int main(int argc, char* argv[]) {
	
	std::cout << "Hello, World." << std::endl;
	std::cout << "This is MRMC-Cpp Version " << MRMC_CPP_VERSION_MAJOR << "." << MRMC_CPP_VERSION_MINOR << std::endl;
	
	return 0;
}

