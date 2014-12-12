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
storm::modelchecker::prctl::AbstractModelChecker<double>* createPrctlModelChecker(storm::models::Dtmc<double> const& dtmc) {
    return new storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<double>(dtmc);
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