#ifndef STORM_MODELCHECKER_PRCTL_CREATEPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_PRCTL_CREATEPRCTLMODELCHECKER_H_

#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"
/*!
 * Creates a model checker for the given DTMC that complies with the given options.
 *
 * @param dtmc A reference to the DTMC for which the model checker is to be created.
 * @return A pointer to the resulting model checker.
 */
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Dtmc<double> const & dtmc) {
    // Create the appropriate model checker.
	storm::settings::Settings* s = storm::settings::Settings::getInstance();
	std::string const& linsolver = s->getOptionByLongName("linsolver").getArgument(0).getValueAsString();
	if (linsolver == "gmm++") {
		return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc, new storm::solver::GmmxxLinearEquationSolver<double>());
	} else if (linsolver == "native") {
		return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc, new storm::solver::NativeLinearEquationSolver<double>());
    }
    
	// The control flow should never reach this point, as there is a default setting for matrixlib.
	std::string message = "No matrix library suitable for DTMC model checking has been set.";
	throw storm::exceptions::InvalidSettingsException() << message;
	return nullptr;
}

/*!
 * Creates a model checker for the given MDP that complies with the given options.
 *
 * @param mdp The Dtmc that the model checker will check
 * @return
 */
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Mdp<double> const & mdp) {
    // Create the appropriate model checker.
    return new storm::modelchecker::prctl::SparseMdpPrctlModelChecker<double>(mdp);
}

#endif