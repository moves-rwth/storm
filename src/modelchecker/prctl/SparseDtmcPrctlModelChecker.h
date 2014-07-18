/*
 * SparseDtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_PRCTL_SPARSEDTMCPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_PRCTL_SPARSEDTMCPRCTLMODELCHECKER_H_

#include "src/modelchecker/prctl/AbstractModelChecker.h"
#include "src/models/Dtmc.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/utility/vector.h"
#include "src/utility/graph.h"

#include <vector>

namespace storm {
namespace modelchecker {
namespace prctl {

/*!
 * @brief
 * Interface for all model checkers that can verify PRCTL formulae over DTMCs represented as a sparse matrix.
 */
template<class Type>
class SparseDtmcPrctlModelChecker : public AbstractModelChecker<Type> {

public:
    /*!
	 * Constructs a SparseDtmcPrctlModelChecker with the given model.
	 *
	 * @param model The DTMC to be checked.
	 */
	explicit SparseDtmcPrctlModelChecker(storm::models::Dtmc<Type> const& model, storm::solver::LinearEquationSolver<Type>* linearEquationSolver) : AbstractModelChecker<Type>(model), linearEquationSolver(linearEquationSolver) {
		// Intentionally left empty.
	}
    
	/*!
	 * Copy constructs a SparseDtmcPrctlModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit SparseDtmcPrctlModelChecker(storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<Type> const& modelChecker) : AbstractModelChecker<Type>(modelChecker), linearEquationSolver(modelChecker.linearEquationSolver->clone()) {
		// Intentionally left empty.
	}

	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~SparseDtmcPrctlModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * Returns a constant reference to the DTMC associated with this model checker.
	 * @returns A constant reference to the DTMC associated with this model checker.
	 */
	storm::models::Dtmc<Type> const& getModel() const {
		return AbstractModelChecker<Type>::template getModel<storm::models::Dtmc<Type>>();
	}

	/*!
	 * Checks the given formula that is a bounded-until formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkBoundedUntil(storm::property::prctl::BoundedUntil<Type> const& formula, bool qualitative) const {
		return this->checkBoundedUntil(formula.getLeft()->check(*this), formula.getRight()->check(*this), formula.getBound(), qualitative);
	}

	/*!
	 * Computes the probability to satisfy phi until psi inside a given bound for each state in the model.
	 *
	 * @param phiStates A bit vector indicating which states satisfy phi.
     * @param psiStates A bit vector indicating which states satisfy psi.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkBoundedUntil(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, uint_fast64_t stepBound, bool qualitative) const {
        std::vector<Type> result(this->getModel().getNumberOfStates());
        
        // If we identify the states that have probability 0 of reaching the target states, we can exclude them in the
        // further analysis.
        storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(this->getModel(), this->getModel().getBackwardTransitions(), phiStates, psiStates, true, stepBound);
        LOG4CPLUS_INFO(logger, "Found " << statesWithProbabilityGreater0.getNumberOfSetBits() << " 'maybe' states.");
        
        // Check if we already know the result (i.e. probability 0) for all initial states and
        // don't compute anything in this case.
        if (this->getModel().getInitialStates().isDisjointFrom(statesWithProbabilityGreater0)) {
            LOG4CPLUS_INFO(logger, "The probabilities for the initial states were determined in a preprocessing step."
                           << " No exact probabilities were computed.");
            // Set the values for all maybe-states to 0.5 to indicate that their probability values are not 0 (and
            // not necessarily 1).
            storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, Type(0.5));
        } else {
            // In this case we have have to compute the probabilities.

            // We can eliminate the rows and columns from the original transition probability matrix that have probability 0.
            storm::storage::SparseMatrix<Type> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, statesWithProbabilityGreater0, statesWithProbabilityGreater0, true);
            
            // Compute the new set of target states in the reduced system.
            storm::storage::BitVector rightStatesInReducedSystem = psiStates % statesWithProbabilityGreater0;
            
            // Make all rows absorbing that satisfy the second sub-formula.
            submatrix.makeRowsAbsorbing(rightStatesInReducedSystem);
            
            // Create the vector with which to multiply.
            std::vector<Type> subresult(statesWithProbabilityGreater0.getNumberOfSetBits());
            storm::utility::vector::setVectorValues(subresult, rightStatesInReducedSystem, storm::utility::constantOne<Type>());
            
            // Perform the matrix vector multiplication as often as required by the formula bound.
            if (linearEquationSolver != nullptr) {
                this->linearEquationSolver->performMatrixVectorMultiplication(submatrix, subresult, nullptr, stepBound);
            } else {
                throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
            }
            
            // Set the values of the resulting vector accordingly.
            storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);
            storm::utility::vector::setVectorValues<Type>(result, ~statesWithProbabilityGreater0, storm::utility::constantZero<Type>());
        }
        
		return result;
	}

	/*!
	 * Checks the given formula that is a next formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkNext(storm::property::prctl::Next<Type> const& formula, bool qualitative) const {
		// First, we need to compute the states that satisfy the child formula of the next-formula.
		storm::storage::BitVector nextStates = formula.getChild()->check(*this);

		// Create the vector with which to multiply and initialize it correctly.
		std::vector<Type> result(this->getModel().getNumberOfStates());
		storm::utility::vector::setVectorValues(result, nextStates, storm::utility::constantOne<Type>());

		// Perform one single matrix-vector multiplication.
        if (linearEquationSolver != nullptr) {
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result);
        } else {
            throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
        }
        
		return result;
	}

	/*!
	 * Checks the given formula that is a bounded-eventually formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkBoundedEventually(storm::property::prctl::BoundedEventually<Type> const& formula, bool qualitative) const {
        return this->checkBoundedUntil(storm::storage::BitVector(this->getModel().getNumberOfStates(), true), formula.getChild()->check(*this), formula.getBound(), qualitative);
	}

	/*!
	 * Checks the given formula that is an eventually formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkEventually(storm::property::prctl::Eventually<Type> const& formula, bool qualitative) const {
		// Create equivalent temporary until formula and check it.
		storm::property::prctl::Until<Type> temporaryUntilFormula(std::shared_ptr<storm::property::prctl::Ap<Type>>(new storm::property::prctl::Ap<Type>("true")), formula.getChild());
		return this->checkUntil(temporaryUntilFormula, qualitative);
	}

	/*!
	 * Checks the given formula that is a globally formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkGlobally(storm::property::prctl::Globally<Type> const& formula, bool qualitative) const {
		// Create "equivalent" (equivalent up to negation) temporary eventually formula and check it.
		storm::property::prctl::Eventually<Type> temporaryEventuallyFormula(std::shared_ptr<storm::property::prctl::Not<Type>>(new storm::property::prctl::Not<Type>(formula.getChild())));
		std::vector<Type> result = this->checkEventually(temporaryEventuallyFormula, qualitative);

		// Now subtract the resulting vector from the constant one vector to obtain final result.
		storm::utility::vector::subtractFromConstantOneVector(result);
		return result;
	}


	/*!
	 * Check the given formula that is an until formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkUntil(storm::property::prctl::Until<Type> const& formula, bool qualitative) const {
		return this->checkUntil(formula.getLeft()->check(*this), formula.getRight()->check(*this), qualitative);
	}

	/*!
	 * Computes the  probability to satisfy phi until psi for each state in the model.
	 *
	 * @param phiStates A bit vector indicating which states satisfy phi.
     * @param psiStates A bit vector indicating which states satisfy psi.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bounds 0 and 1. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bounds 0 and 1.
	 * @returns The probabilities for the given formula to hold on every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact probabilities might not be computed.
	 */
	virtual std::vector<Type> checkUntil(storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative) const {

		// We need to identify the states which have to be taken out of the matrix, i.e.
		// all states that have probability 0 and 1 of satisfying the until-formula.
        std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->getModel(), phiStates, psiStates);
		storm::storage::BitVector statesWithProbability0 = std::move(statesWithProbability01.first);
		storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);

		// Perform some logging.
        storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability0.getNumberOfSetBits() << " 'no' states.");
		LOG4CPLUS_INFO(logger, "Found " << statesWithProbability1.getNumberOfSetBits() << " 'yes' states.");
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type> result(this->getModel().getNumberOfStates());

        // Check whether we need to compute exact probabilities for some states.
        if (this->getModel().getInitialStates().isDisjointFrom(maybeStates) || qualitative) {
            if (qualitative) {
                LOG4CPLUS_INFO(logger, "The formula was checked qualitatively. No exact probabilities were computed.");
            } else {
                LOG4CPLUS_INFO(logger, "The probabilities for the initial states were determined in a preprocessing step."
                               << " No exact probabilities were computed.");
            }
            // Set the values for all maybe-states to 0.5 to indicate that their probability values
            // are neither 0 nor 1.
            storm::utility::vector::setVectorValues<Type>(result, maybeStates, Type(0.5));
        } else {
            // In this case we have have to compute the probabilities.
            
            // We can eliminate the rows and columns from the original transition probability matrix.
            storm::storage::SparseMatrix<Type> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, true);
            // Converting the matrix from the fixpoint notation to the form needed for the equation
            // system. That is, we go from x = A*x + b to (I-A)x = b.
            submatrix.convertToEquationSystem();
            
            // Initialize the x vector with 0.5 for each element. This is the initial guess for
            // the iterative solvers. It should be safe as for all 'maybe' states we know that the
            // probability is strictly larger than 0.
            std::vector<Type> x(maybeStates.getNumberOfSetBits(), Type(0.5));
            
            // Prepare the right-hand side of the equation system. For entry i this corresponds to
            // the accumulated probability of going from state i to some 'yes' state.
            std::vector<Type> b = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
            
            // Now solve the created system of linear equations.
            if (linearEquationSolver != nullptr) {
                this->linearEquationSolver->solveEquationSystem(submatrix, x, b);
            } else {
                throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
            }
            
            // Set values of resulting vector according to result.
            storm::utility::vector::setVectorValues<Type>(result, maybeStates, x);
        }

		// Set values of resulting vector that are known exactly.
		storm::utility::vector::setVectorValues<Type>(result, statesWithProbability0, storm::utility::constantZero<Type>());
		storm::utility::vector::setVectorValues<Type>(result, statesWithProbability1, storm::utility::constantOne<Type>());

		return result;
	}

	/*!
	 * Checks the given formula that is an instantaneous reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type> checkInstantaneousReward(storm::property::prctl::InstantaneousReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has a state-based reward model.
		if (!this->getModel().hasStateRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing (state-based) reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing (state-based) reward model for formula.";
		}

		// Initialize result to state rewards of the model.
		std::vector<Type> result(this->getModel().getStateRewardVector());

		// Perform the actual matrix-vector multiplication as long as the bound of the formula is met.
        if (linearEquationSolver != nullptr) {
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result, nullptr, formula.getBound());
        } else {
            throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
        }

		return result;
	}

	/*!
	 * Check the given formula that is a cumulative reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type> checkCumulativeReward(storm::property::prctl::CumulativeReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula.");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Compute the reward vector to add in each step based on the available reward models.
		std::vector<Type> totalRewardVector;
		if (this->getModel().hasTransitionRewards()) {
			totalRewardVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
			if (this->getModel().hasStateRewards()) {
                storm::utility::vector::addVectorsInPlace(totalRewardVector, this->getModel().getStateRewardVector());
			}
		} else {
			totalRewardVector = std::vector<Type>(this->getModel().getStateRewardVector());
		}

		// Initialize result to either the state rewards of the model or the null vector.
		std::vector<Type> result;
		if (this->getModel().hasStateRewards()) {
			result = std::vector<Type>(this->getModel().getStateRewardVector());
		} else {
			result.resize(this->getModel().getNumberOfStates());
		}

		// Perform the actual matrix-vector multiplication as long as the bound of the formula is met.
        if (linearEquationSolver != nullptr) {
            this->linearEquationSolver->performMatrixVectorMultiplication(this->getModel().getTransitionMatrix(), result, &totalRewardVector, formula.getBound());
        } else {
            throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
        }

		return result;
	}

	/*!
	 * Checks the given formula that is a reachability reward formula.
	 *
	 * @param formula The formula to check.
	 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
	 * results are only compared against the bound 0. If set to true, this will most likely results that are only
	 * qualitatively correct, i.e. do not represent the correct value, but only the correct relation with respect to the
	 * bound 0.
	 * @returns The reward values for the given formula for every state of the model associated with this model
	 * checker. If the qualitative flag is set, exact values might not be computed.
	 */
	virtual std::vector<Type> checkReachabilityReward(storm::property::prctl::ReachabilityReward<Type> const& formula, bool qualitative) const {
		// Only compute the result if the model has at least one reward model.
		if (!this->getModel().hasStateRewards() && !this->getModel().hasTransitionRewards()) {
			LOG4CPLUS_ERROR(logger, "Missing reward model for formula. Skipping formula");
			throw storm::exceptions::InvalidPropertyException() << "Missing reward model for formula.";
		}

		// Determine the states for which the target predicate holds.
		storm::storage::BitVector targetStates = formula.getChild()->check(*this);

		// Determine which states have a reward of infinity by definition.
		storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
		storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(this->getModel(), this->getModel().getBackwardTransitions(), trueStates, targetStates);
		infinityStates.complement();
		storm::storage::BitVector maybeStates = ~targetStates & ~infinityStates;
		LOG4CPLUS_INFO(logger, "Found " << infinityStates.getNumberOfSetBits() << " 'infinity' states.");
		LOG4CPLUS_INFO(logger, "Found " << targetStates.getNumberOfSetBits() << " 'target' states.");
		LOG4CPLUS_INFO(logger, "Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states.");

		// Create resulting vector.
		std::vector<Type> result(this->getModel().getNumberOfStates());

        // Check whether we need to compute exact rewards for some states.
        if (this->getModel().getInitialStates().isDisjointFrom(maybeStates)) {
            LOG4CPLUS_INFO(logger, "The rewards for the initial states were determined in a preprocessing step."
                            << " No exact rewards were computed.");
            // Set the values for all maybe-states to 1 to indicate that their reward values
            // are neither 0 nor infinity.
            storm::utility::vector::setVectorValues<Type>(result, maybeStates, storm::utility::constantOne<Type>());
        } else {
            // In this case we have to compute the reward values for the remaining states.
            // We can eliminate the rows and columns from the original transition probability matrix.
            storm::storage::SparseMatrix<Type> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates, true);
            // Converting the matrix from the fixpoint notation to the form needed for the equation
            // system. That is, we go from x = A*x + b to (I-A)x = b.
            submatrix.convertToEquationSystem();
            
            // Initialize the x vector with 1 for each element. This is the initial guess for
            // the iterative solvers.
            std::vector<Type> x(submatrix.getColumnCount(), storm::utility::constantOne<Type>());
            
            // Prepare the right-hand side of the equation system.
            std::vector<Type> b(submatrix.getRowCount());
            if (this->getModel().hasTransitionRewards()) {
                // If a transition-based reward model is available, we initialize the right-hand
                // side to the vector resulting from summing the rows of the pointwise product
                // of the transition probability matrix and the transition reward matrix.
                std::vector<Type> pointwiseProductRowSumVector = this->getModel().getTransitionMatrix().getPointwiseProductRowSumVector(this->getModel().getTransitionRewardMatrix());
                storm::utility::vector::selectVectorValues(b, maybeStates, pointwiseProductRowSumVector);
                
                if (this->getModel().hasStateRewards()) {
                    // If a state-based reward model is also available, we need to add this vector
                    // as well. As the state reward vector contains entries not just for the states
                    // that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    std::vector<Type> subStateRewards(b.size());
                    storm::utility::vector::selectVectorValues(subStateRewards, maybeStates, this->getModel().getStateRewardVector());
                    storm::utility::vector::addVectorsInPlace(b, subStateRewards);
                }
            } else {
                // If only a state-based reward model is  available, we take this vector as the
                // right-hand side. As the state reward vector contains entries not just for the
                // states that we still consider (i.e. maybeStates), we need to extract these values
                // first.
                storm::utility::vector::selectVectorValues(b, maybeStates, this->getModel().getStateRewardVector());
            }
            
            // Now solve the resulting equation system.
            if (linearEquationSolver != nullptr) {
                this->linearEquationSolver->solveEquationSystem(submatrix, x, b);
            } else {
                throw storm::exceptions::InvalidStateException() << "No valid linear equation solver available.";
            }
            
            // Set values of resulting vector according to result.
            storm::utility::vector::setVectorValues<Type>(result, maybeStates, x);
        }

		// Set values of resulting vector that are known exactly.
		storm::utility::vector::setVectorValues(result, targetStates, storm::utility::constantZero<Type>());
		storm::utility::vector::setVectorValues(result, infinityStates, storm::utility::constantInfinity<Type>());

		return result;
	}

	/*!
	 * Checks the given formula.
	 * @note This methods overrides the method of the base class to give an additional warning that declaring that minimal or maximal probabilities
	 *       should be computed for the formula makes no sense in the context of a deterministic model.
	 *
	 * @param formula The formula to check.
	 * @param optOperator True iff minimum probabilities/rewards are to be computed.
	 * @returns The probabilities to satisfy the formula or the rewards accumulated by it, represented by a vector.
	 */
	virtual std::vector<Type> checkOptimizingOperator(storm::property::prctl::AbstractPathFormula<Type> const & formula, bool optOperator) const override {

		LOG4CPLUS_WARN(logger, "Formula contains min/max operator, which is not meaningful over deterministic models.");

		std::vector<Type> result = formula.check(*this, false);

		return result;
	}

	/*!
	 * Checks the given formula and determines whether minimum or maximum probabilities or rewards are to be computed for the formula.
	 * @note This methods overrides the method of the base class to give an additional warning that declaring that minimal or maximal probabilities
	 *       should be computed for the formula makes no sense in the context of a deterministic model.
	 *
	 * @param formula The formula to check.
	 * @param optOperator True iff minimum probabilities/rewards are to be computed.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual storm::storage::BitVector checkOptimizingOperator(storm::property::prctl::AbstractStateFormula<Type> const & formula, bool optOperator) const override {

		LOG4CPLUS_WARN(logger, "Formula contains min/max operator, which is not meaningful over deterministic models.");

		storm::storage::BitVector result = formula.check(*this);

		return result;
	}

private:
    // An object that is used for solving linear equations and performing matrix-vector multiplication.
    std::unique_ptr<storm::solver::LinearEquationSolver<Type>> linearEquationSolver;
};

} // namespace prctl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_PRCTL_SPARSEDTMCPRCTLMODELCHECKER_H_ */
