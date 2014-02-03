/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
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
	explicit TopologicalValueIterationMdpPrctlModelChecker(storm::models::Mdp<Type> const& model) : SparseMdpPrctlModelChecker<Type>(model) {
		// Intentionally left empty.
	}

	/*!
	 * Copy constructs a SparseMdpPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit TopologicalValueIterationMdpPrctlModelChecker(storm::modelchecker::prctl::TopologicalValueIterationMdpPrctlModelChecker<Type> const& modelchecker)
		: SparseMdpPrctlModelChecker<Type>(modelchecker),  minimumOperatorStack() {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~TopologicalValueIterationMdpPrctlModelChecker() { }

private:
	/*!
	 * Solves the given equation system under the given parameters using the power method.
	 *
	 * @param A The matrix A specifying the coefficients of the equations.
	 * @param x The vector x for which to solve the equations. The initial value of the elements of
	 * this vector are used as the initial guess and might thus influence performance and convergence.
	 * @param b The vector b specifying the values on the right-hand-sides of the equations.
	 * @return The solution of the system of linear equations in form of the elements of the vector
	 * x.
	 */
	void solveEquationSystem(storm::storage::SparseMatrix<Type> const& matrix, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) const {
		// Get the settings object to customize solving.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();

		// Get relevant user-defined settings for solving the equations.
		double precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
		uint_fast64_t maxIterations = s->getOptionByLongName("maxIterations").getArgument(0).getValueAsUnsignedInteger();
		bool relative = s->getOptionByLongName("relative").getArgument(0).getValueAsBoolean();

		// Now, we need to determine the SCCs of the MDP and a topological sort.
        std::vector<std::vector<uint_fast64_t>> stronglyConnectedComponents = storm::utility::graph::performSccDecomposition(this->getModel(), stronglyConnectedComponents, stronglyConnectedComponentsDependencyGraph);
        storm::storage::SparseMatrix<T> stronglyConnectedComponentsDependencyGraph = this->getModel().extractSccDependencyGraph(stronglyConnectedComponents);
		std::vector<uint_fast64_t> topologicalSort = storm::utility::graph::getTopologicalSort(stronglyConnectedComponentsDependencyGraph);

		// Set up the environment for the power method.
		std::vector<Type> multiplyResult(matrix.getRowCount());
		std::vector<Type>* currentX = &x;
		std::vector<Type>* newX = new std::vector<Type>(x.size());
		std::vector<Type>* swap = nullptr;
		uint_fast64_t currentMaxLocalIterations = 0;
		uint_fast64_t localIterations = 0;
		uint_fast64_t globalIterations = 0;
		bool converged = true;

		// Iterate over all SCCs of the MDP as specified by the topological sort. This guarantees that an SCC is only
		// solved after all SCCs it depends on have been solved.
		for (auto sccIndexIt = topologicalSort.begin(); sccIndexIt != topologicalSort.end() && converged; ++sccIndexIt) {
			std::vector<uint_fast64_t> const& scc = stronglyConnectedComponents[*sccIndexIt];

			// For the current SCC, we need to perform value iteration until convergence.
			localIterations = 0;
			converged = false;
			while (!converged && localIterations < maxIterations) {
				// Compute x' = A*x + b.
				matrix.multiplyWithVector(scc, nondeterministicChoiceIndices, *currentX, multiplyResult);
				storm::utility::addVectors(scc, nondeterministicChoiceIndices, multiplyResult, b);

				// Reduce the vector x' by applying min/max for all non-deterministic choices.
				if (this->minimumOperatorStack.top()) {
					storm::utility::reduceVectorMin(multiplyResult, newX, scc, nondeterministicChoiceIndices);
				} else {
					storm::utility::reduceVectorMax(multiplyResult, newX, scc, nondeterministicChoiceIndices);
				}

				// Determine whether the method converged.
				// TODO: It seems that the equalModuloPrecision call that compares all values should have a higher
				// running time. In fact, it is faster. This has to be investigated.
				// converged = storm::utility::equalModuloPrecision(*currentX, *newX, scc, precision, relative);
				converged = storm::utility::equalModuloPrecision(*currentX, *newX, precision, relative);

				// Update environment variables.
				swap = currentX;
				currentX = newX;
				newX = swap;
				++localIterations;
				++globalIterations;
			}

			// As the "number of iterations" of the full method is the maximum of the local iterations, we need to keep
			// track of the maximum.
			if (localIterations > currentMaxLocalIterations) {
				currentMaxLocalIterations = localIterations;
			}
		}

		// If we performed an odd number of global iterations, we need to swap the x and currentX, because the newest
		// result is currently stored in currentX, but x is the output vector.
		// TODO: Check whether this is correct or should be put into the for-loop over SCCs.
		if (globalIterations % 2 == 1) {
			std::swap(x, *currentX);
			delete currentX;
		} else {
			delete newX;
		}

		// Check if the solver converged and issue a warning otherwise.
		if (converged) {
			LOG4CPLUS_INFO(logger, "Iterative solver converged after " << currentMaxLocalIterations << " iterations.");
		} else {
			LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
		}
	}
};

} // namespace prctl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_ */
