/*
 * AbstractModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_LTL_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_LTL_ABSTRACTMODELCHECKER_H_

#include "src/exceptions/InvalidPropertyException.h"
#include "src/formula/Ltl.h"
#include "src/storage/BitVector.h"
#include "src/models/AbstractModel.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include <iostream>

extern log4cplus::Logger logger;

namespace storm {
namespace modelchecker {
namespace ltl {

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
	public virtual storm::property::ltl::IApModelChecker<Type>,
	public virtual storm::property::ltl::IAndModelChecker<Type>,
	public virtual storm::property::ltl::IOrModelChecker<Type>,
	public virtual storm::property::ltl::INotModelChecker<Type>,
	public virtual storm::property::ltl::IUntilModelChecker<Type>,
	public virtual storm::property::ltl::IEventuallyModelChecker<Type>,
	public virtual storm::property::ltl::IGloballyModelChecker<Type>,
	public virtual storm::property::ltl::INextModelChecker<Type>,
	public virtual storm::property::ltl::IBoundedUntilModelChecker<Type>,
	public virtual storm::property::ltl::IBoundedEventuallyModelChecker<Type> {
	
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
	 * Checks the given state formula on the model and prints the result (true/false) for all initial states, i.e.
	 * states that carry the atomic proposition "init".
	 *
	 * @param stateFormula The formula to be checked.
	 */
	void check(storm::property::ltl::AbstractLtlFormula<Type> const& ltlFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << ltlFormula.toString());
		std::cout << "Model checking formula:\t" << ltlFormula.toString() << std::endl;
		storm::storage::BitVector* result = nullptr;
		try {
			result = ltlFormula.check(*this);
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
	 * Checks the given formula consisting of a single atomic proposition.
	 *
	 * @param formula The formula to be checked.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual std::vector<Type>* checkAp(storm::property::ltl::Ap<Type> const& formula) const {
		return nullptr;
	}

	/*!
	 * Checks the given formula that is a logical "and" of two formulae.
	 *
	 * @param formula The formula to be checked.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual std::vector<Type>* checkAnd(storm::property::ltl::And<Type> const& formula) const {
		return nullptr;
	}

	/*!
	 * Checks the given formula that is a logical "or" of two formulae.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual std::vector<Type>* checkOr(storm::property::ltl::Or<Type> const& formula) const {
		return nullptr;
	}

	/*!
	 * Checks the given formula that is a logical "not" of a sub-formula.
	 *
	 * @param formula The formula to check.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual std::vector<Type>* checkNot(const storm::property::ltl::Not<Type>& formula) const {
		return nullptr;
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

} // namespace ltl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_LTL_ABSTRACTMODELCHECKER_H_ */
