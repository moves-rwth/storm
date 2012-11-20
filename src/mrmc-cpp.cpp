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

/* PlatformSTL Header Files */
#include <platformstl/performance/performance_counter.hpp>

#include <pantheios/pantheios.hpp>
#include <pantheios/backends/bec.file.h>
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = "mrmc-cpp";

#include "MRMCConfig.h"

#include "src/sparse/static_sparse_matrix.h"
#include "src/models/atomic_propositions_labeling.h"
#include "src/parser/read_lab_file.h"
#include "src/parser/read_tra_file.h"
#include "Eigen/Sparse"
 
int main(int argc, char* argv[]) {
	// Logging init
	pantheios_be_file_setFilePath("log.all");
	pantheios::log_INFORMATIONAL("MRMC-Cpp started.");

	if (argc < 2) {
		std::cout << "Required argument #1 inputTraFile.tra not found!" << std::endl;
		exit(-1);
	}

	mrmc::sparse::StaticSparseMatrix<double> *ssm;
	mrmc::sparse::StaticSparseMatrix<double> *ssmBack;

	// 1. Create an instance of the platformstl::performance_counter. (On
    // UNIX this will resolve to unixstl::performance_counter; on Win32 it
    // will resolve to winstl::performance_counter.)
    platformstl::performance_counter    counter;

	std::cout << "Building Matrix from File..." << std::endl;

	// 2. Begin the measurement
    counter.start();

	ssm = mrmc::parser::read_tra_file(argv[1]);
	//lab = mrmc::parser::read_lab_file(20302, "csl_unbounded_until_sim_06.lab");

	counter.stop();

	std::cout << "Loaded " << ssm->getNonZeroEntryCount() << " entries into (" << ssm->getRowCount() << " x " << ssm->getRowCount() << ")." << std::endl;
	std::cout << "Time needed was " << (unsigned long long)counter.get_microseconds() << std::endl;



	counter.start();
	auto esm = ssm->toEigenSparseMatrix();
	counter.stop();

	std::cout << "Loaded " << esm->nonZeros() << " entries into (" << esm->rows() << " x " << esm->cols() << ")." << std::endl;
	std::cout << "Time needed was " << (unsigned long long)counter.get_microseconds() << std::endl;

	ssmBack = new mrmc::sparse::StaticSparseMatrix<double>(esm->rows());

	counter.start();
	ssmBack->initialize(*esm);
	counter.stop();

	std::cout << "Loaded " << ssmBack->getNonZeroEntryCount() << " entries into (" << ssmBack->getRowCount() << " x " << ssmBack->getRowCount() << ")." << std::endl;
	std::cout << "Time needed was " << (unsigned long long)counter.get_microseconds() << std::endl;

	delete ssm;
	delete ssmBack;

	std::cout << std::endl;
	std::cout << ":: C-style vs. C++ Style Array init Showdown ::" << std::endl;

	uint_fast64_t *iArray = NULL;
	uint_fast32_t i = 0;
	uint_fast32_t j = 0;

	counter.start();
	for (i = 0; i < 10000; ++i) {
		iArray = (uint_fast64_t*)malloc(2097152 * sizeof(uint_fast64_t));
		memset(iArray, 0, 2097152 * sizeof(uint_fast64_t));
		free(iArray);
	}
	counter.stop();

	std::cout << "C-style:   " << (unsigned long long)counter.get_microseconds() << std::endl;

	counter.start();
	for (i = 0; i < 10000; ++i) {
		iArray = new uint_fast64_t[2097152]();
		delete[] iArray;
	}
	counter.stop();

	std::cout << "Cpp-style: " << (unsigned long long)counter.get_microseconds() << std::endl;

	return 0;
}

