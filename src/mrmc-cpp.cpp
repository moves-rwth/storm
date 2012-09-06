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
#include <cstdio>

#include <pantheios/pantheios.hpp>
#include <pantheios/backends/bec.file.h>
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = "mrmc-cpp";

#include "MRMCConfig.h"

#include "src/sparse/static_sparse_matrix.h"
#include "src/dtmc/atomic_proposition.h"
 
int main(int argc, char* argv[]) {
	// Logging init
	pantheios_be_file_setFilePath("log.all");
	pantheios::log_INFORMATIONAL("MRMC-Cpp started.");

	std::cout << "Hello, World." << std::endl;
	std::cout << "This is MRMC-Cpp Version " << MRMC_CPP_VERSION_MAJOR << "." << MRMC_CPP_VERSION_MINOR << std::endl;

	mrmc::sparse::StaticSparseMatrix<int> *ssm = new mrmc::sparse::StaticSparseMatrix<int>(10, 10);

	ssm->initialize();
	ssm->addNextValue(2, 3, 1);
	ssm->addNextValue(2, 4, 2);
	ssm->addNextValue(2, 5, 3);
	ssm->addNextValue(2, 6, 4);
	ssm->addNextValue(2, 7, 5);

	ssm->addNextValue(3, 4, 6);
	ssm->addNextValue(3, 5, 7);
	ssm->addNextValue(3, 6, 8);
	ssm->addNextValue(3, 7, 9);
	ssm->addNextValue(3, 8, 10);

	ssm->finalize();

	int target;

	for (int row = 1; row <= 10; ++row) {
		for (int col = 1; col <= 10; ++col) {
			if (ssm->getValue(row, col, &target)) {
				printf(" T%i", target);
			} else {
				printf(" _%i", target);
			}
		}
		printf("\n");
	}

	return 0;
}

