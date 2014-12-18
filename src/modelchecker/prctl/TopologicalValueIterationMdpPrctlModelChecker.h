/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/TopologicalValueIterationNondeterministicLinearEquationSolver.h"
#include "src/exceptions/InvalidPropertyException.h"
#include <cmath>

namespace storm {
namespace modelchecker {
namespace prctl {

/*
 * An implementation of the SparseMdpPrctlModelChecker interface that uses topoligical value iteration for solving
 * equation systems.
 */
template <class Type>
class TopologicalValueIterationMdpPrctlModelChecker : public SparseMdpPrctlModelChecker<Type> {

public:
	/*!
	 * Constructs a SparseMdpPrctlModelChecker with the given model.
	 *
	 * @param model The MDP to be checked.
	 */
	explicit TopologicalValueIterationMdpPrctlModelChecker(storm::models::Mdp<Type> const& model) : SparseMdpPrctlModelChecker<Type>(model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<Type>>(new storm::solver::TopologicalValueIterationNondeterministicLinearEquationSolver<Type>())) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructs a SparseMdpPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit TopologicalValueIterationMdpPrctlModelChecker(storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<Type> const& modelchecker)
    : SparseMdpPrctlModelChecker<Type>(modelchecker) {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~TopologicalValueIterationMdpPrctlModelChecker() { }
};

} // namespace prctl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_ */
