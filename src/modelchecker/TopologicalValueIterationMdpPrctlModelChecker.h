/*
 * GmmxxDtmcPrctlModelChecker.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELCHECKER_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_

#include <cmath>

#include "src/models/Mdp.h"
#include "src/modelchecker/MdpPrctlModelChecker.h"
#include "src/utility/GraphAnalyzer.h"
#include "src/utility/Vector.h"
#include "src/utility/ConstTemplates.h"
#include "src/utility/Settings.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/storage/JacobiDecomposition.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {

namespace modelChecker {

/*
 * A model checking engine that makes use of the gmm++ backend.
 */
template <class Type>
class TopologicalValueIterationMdpPrctlModelChecker : public MdpPrctlModelChecker<Type> {

public:
	explicit TopologicalValueIterationMdpPrctlModelChecker(storm::models::Mdp<Type>& mdp) : MdpPrctlModelChecker<Type>(mdp) { }

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
		storm::settings::Settings* s = storm::settings::instance();

		// Get relevant user-defined settings for solving the equations.
		double precision = s->get<double>("precision");
		unsigned maxIterations = s->get<unsigned>("maxiter");
		bool relative = s->get<bool>("relative");

		std::vector<std::vector<uint_fast64_t>> stronglyConnectedComponents;
		storm::models::GraphTransitions<Type> stronglyConnectedComponentsDependencyGraph;
		storm::utility::GraphAnalyzer::performSccDecomposition(matrix, nondeterministicChoiceIndices, stronglyConnectedComponents, stronglyConnectedComponentsDependencyGraph);

		// Set up the environment for the power method.
		std::vector<Type> multiplyResult(matrix.getRowCount());
		std::vector<Type>* currentX = &x;
		std::vector<Type>* newX = new std::vector<Type>(x.size());
		std::vector<Type>* swap = nullptr;
		uint_fast64_t iterations = 0;
		bool converged = false;

		/*
		for (auto sccIt = stronglyConnectedComponents.rbegin(), sccIte = stronglyConnectedComponents.rend(); sccIt != sccIte; ++sccIt) {
			converged = false;

			std::cout << "for scc! " << *sccIt << std::endl;
			while (!converged && iterations < maxIterations) {
				std::cout << "first line " << std::endl;
				// Compute x' = A*x + b.
				matrix.multiplyWithVector(**sccIt, nondeterministicChoiceIndices, *currentX, multiplyResult);
				std::cout << "after mult" << std::endl;
				storm::utility::addVectors(**sccIt, nondeterministicChoiceIndices, multiplyResult, b);
				std::cout << "after add " << std::endl;

				// Reduce the vector x' by applying min/max for all non-deterministic choices.
				if (this->minimumOperatorStack.top()) {
					storm::utility::reduceVectorMin(multiplyResult, newX, nondeterministicChoiceIndices);
				} else {
					storm::utility::reduceVectorMax(multiplyResult, newX, nondeterministicChoiceIndices);
				}

				std::cout << "comp" << std::endl;
				// Determine whether the method converged.
				converged = storm::utility::equalModuloPrecision(*currentX, *newX, precision, relative);

				// Update environment variables.
				swap = currentX;
				currentX = newX;
				newX = swap;
				++iterations;
			}
		}

		if (iterations % 2 == 1) {
			std::swap(x, *currentX);
			delete currentX;
		} else {
			delete newX;
		}

		// Check if the solver converged and issue a warning otherwise.
		if (converged) {
			LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
		} else {
			LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
		}

		*/
	}
};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_ */
