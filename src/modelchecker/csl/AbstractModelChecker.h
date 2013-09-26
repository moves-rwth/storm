/*
 * AbstractModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_CSL_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_CSL_ABSTRACTMODELCHECKER_H_

#include "src/exceptions/InvalidPropertyException.h"
#include "src/formula/Csl.h"
#include "src/storage/BitVector.h"
#include "src/models/AbstractModel.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include <iostream>

extern log4cplus::Logger logger;

namespace storm {
namespace modelchecker {
namespace csl {

/*!
 * @brief
 * (Abstract) interface for all model checker classes.
 *
 * This class provides basic functions that are common to all model checkers (i.e. subclasses). It mainly declares
 * abstract methods that are implemented in the concrete subclasses, but also covers checking procedures that are common
 * to all model checkers for state-based models.
 */
template<class Type>
class AbstractModelChecker :
	// A list of interfaces the model checker supports. Typically, for each of the interfaces, a check method needs to
	// be implemented that performs the corresponding check.
	public virtual storm::property::csl::IApModelChecker<Type>,
	public virtual storm::property::csl::IAndModelChecker<Type>,
	public virtual storm::property::csl::IOrModelChecker<Type>,
	public virtual storm::property::csl::INotModelChecker<Type>,
	public virtual storm::property::csl::IUntilModelChecker<Type>,
	public virtual storm::property::csl::IEventuallyModelChecker<Type>,
	public virtual storm::property::csl::IGloballyModelChecker<Type>,
	public virtual storm::property::csl::INextModelChecker<Type>,
	public virtual storm::property::csl::ITimeBoundedUntilModelChecker<Type>,
	public virtual storm::property::csl::ITimeBoundedEventuallyModelChecker<Type>,
	public virtual storm::property::csl::INoBoundOperatorModelChecker<Type>,
	public virtual storm::property::csl::IProbabilisticBoundOperatorModelChecker<Type> {
	
public:
	/*!
	 * Constructs an AbstractModelChecker with the given model.
	 */
	explicit AbstractModelChecker(storm::models::AbstractModel<Type> const& model) : model(model) {
		// Intentionally left empty.
	}
	/*!
	 * Copy constructs an AbstractModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit AbstractModelChecker(AbstractModelChecker<Type> const& modelchecker) : model(modelchecker.model) {
		// Intentionally left empty.
	}
	
	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~AbstractModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * Returns a pointer to the model checker object that is of the requested type as given by the template parameters.
	 * @returns A pointer to the model checker object that is of the requested type as given by the template parameters.
	 * If the model checker is not of the requested type, type casting will fail and result in an exception.
	 */
	template <template <class T> class Target>
	const Target<Type>* as() const {
		try {
			const Target<Type>* target = dynamic_cast<const Target<Type>*>(this);
			return target;
		} catch (std::bad_cast& bc) {
			LOG4CPLUS_ERROR(logger, "Bad cast: tried to cast " << typeid(*this).name() << " to " << typeid(Target<Type>).name() << ".");
			throw bc;
		}
		return nullptr;
	}

	/*!
	 * Retrieves the model associated with this model checker as a constant reference to an object of the type given
	 * by the template parameter.
	 *
	 * @returns A constant reference of the specified type to the model associated with this model checker. If the model
	 * is not of the requested type, type casting will fail and result in an exception.
	 */
	template <class Model>
	Model const& getModel() const {
		try {
			Model const& target = dynamic_cast<Model const&>(this->model);
			return target;
		} catch (std::bad_cast& bc) {
			LOG4CPLUS_ERROR(logger, "Bad cast: tried to cast " << typeid(this->model).name() << " to " << typeid(Model).name() << ".");
			throw bc;
		}
	}

	/*!
	 * Checks the given abstract prctl formula on the model and prints the result (depending on the actual type of the formula)
	 * for all initial states, i.e. states that carry the atomic proposition "init".
	 *
	 * @param formula The formula to be checked.
	 */
	void check(storm::property::csl::AbstractCslFormula<Type> const& formula) const {
		if (dynamic_cast<storm::property::csl::AbstractStateFormula<Type> const*>(&formula) != nullptr) {
			this->check(static_cast<storm::property::csl::AbstractStateFormula<Type> const&>(formula));
		} else if (dynamic_cast<storm::property::csl::AbstractNoBoundOperator<Type> const*>(&formula) != nullptr) {
			this->check(static_cast<storm::property::csl::AbstractNoBoundOperator<Type> const&>(formula));
		}
	}

	/*!
	 * Checks the given state formula on the model and prints the result (true/false) for all initial states, i.e.
	 * states that carry the atomic proposition "init".
	 *
	 * @param stateFormula The formula to be checked.
	 */
	void check(storm::property::csl::AbstractStateFormula<Type> const& stateFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << stateFormula.toString());
		std::cout << "Model checking formula:\t" << stateFormula.toString() << std::endl;
		storm::storage::BitVector* result = nullptr;
		try {
			result = stateFormula.check(*this);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : model.getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result->get(initialState) ? "satisfied" : "not satisfied"));
				std::cout << "\t" << initialState << ": " << result->get(initialState) << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	/*!
	 * Checks the given formula (with no bound) on the model and prints the result (probability/rewards) for all
	 * initial states, i.e. states that carry the atomic proposition "init".
	 *
	 * @param noBoundFormula The formula to be checked.
	 */
	void check(storm::property::csl::AbstractNoBoundOperator<Type> const& noBoundFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << noBoundFormula.toString());
		std::cout << "Model checking formula:\t" << noBoundFormula.toString() << std::endl;
		std::vector<Type>* result = nullptr;
		try {
			result = this->checkNoBoundOperator(noBoundFormula);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : model.getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (*result)[initialState]);
				std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << " Skipping property." << std::endl;
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	/*!
	 * Checks the given formula consisting of a single atomic proposition.
	 *
	 * @param formula The formula to be checked.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkAp(storm::property::csl::Ap<Type> const& formula) const {
		if (formula.getAp() == "true") {
			return storm::storage::BitVector(model.getNumberOfStates(), true);
		} else if (formula.getAp() == "false") {
			return storm::storage::BitVector(model.getNumberOfStates());
		}

		if (!model.hasAtomicProposition(formula.getAp())) {
			LOG4CPLUS_ERROR(logger, "Atomic proposition '" << formula.getAp() << "' is invalid.");
			throw storm::exceptions::InvalidPropertyException() << "Atomic proposition '" << formula.getAp() << "' is invalid.";
		}

		return storm::storage::BitVector(model.getLabeledStates(formula.getAp()));
	}

	/*!
	 * Checks the given formula that is a logical "and" of two formulae.
	 *
	 * @param formula The formula to be checked.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkAnd(storm::property::csl::And<Type> const& formula) const {
		storm::storage::BitVector result = formula.getLeft().check(*this);
		storm::storage::BitVector right = formula.getRight().check(*this);
		result &= right;
		return result;
	}

	/*!
	 * Checks the given formula that is a logical "or" of two formulae.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual storm::storage::BitVector checkOr(storm::property::csl::Or<Type> const& formula) const {
		storm::storage::BitVector result = formula.getLeft().check(*this);
		storm::storage::BitVector right = formula.getRight().check(*this);
		result |= right;
		return result;
	}

	/*!
	 * Checks the given formula that is a logical "not" of a sub-formula.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkNot(const storm::property::csl::Not<Type>& formula) const {
		storm::storage::BitVector result = formula.getChild().check(*this);
		result.complement();
		return result;
	}


	/*!
	 * Checks the given formula that is a P operator over a path formula featuring a value bound.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkProbabilisticBoundOperator(storm::property::csl::ProbabilisticBoundOperator<Type> const& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type> quantitativeResult = formula.getPathFormula().check(*this, false);

		// Create resulting bit vector that will hold the yes/no-answer for every state.
		storm::storage::BitVector result(quantitativeResult.size());

		// Now, we can compute which states meet the bound specified in this operator and set the
		// corresponding bits to true in the resulting vector.
		for (uint_fast64_t i = 0; i < quantitativeResult.size(); ++i) {
			if (formula.meetsBound(quantitativeResult[i])) {
				result.set(i, true);
			}
		}

		return result;
	}

private:

	/*!
	 * A constant reference to the model associated with this model checker.
	 *
	 * @note that we do not own this object, but merely have a constant reference to  it. That means that using the
	 * model checker object is unsafe after the object has been destroyed.
	 */
	storm::models::AbstractModel<Type> const& model;
};

} // namespace csl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_CSL_DTMCPRCTLMODELCHECKER_H_ */
