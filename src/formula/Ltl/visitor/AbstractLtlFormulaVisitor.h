/*
 * AbstractLtlFormulaVisitor.h
 *
 *  Created on: 29.05.2013
 *      Author: thomas
 */

#ifndef STORM_PROPERTY_LTL_VISITOR_ABSTRACTLTLFORMULAVISITOR_H_
#define STORM_PROPERTY_LTL_VISITOR_ABSTRACTLTLFORMULAVISITOR_H_

// Forward declaration of visitor
namespace storm {
namespace property {
namespace ltl {
namespace visitor {

template <class T>
class AbstractLtlFormulaVisitor;

} /* namespace visitor */
}
}
}

#include "../AbstractLtlFormula.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include <typeinfo>

extern log4cplus::Logger logger;

namespace storm {
namespace property {
namespace ltl {
namespace visitor {

template <class T>
class AbstractLtlFormulaVisitor {
public:
	virtual ~AbstractLtlFormulaVisitor() {
		// TODO Auto-generated destructor stub
	}


	/*!
	 * Returns a pointer to the model checker object that is of the requested type as given by the template parameters.
	 * @returns A pointer to the model checker object that is of the requested type as given by the template parameters.
	 * If the model checker is not of the requested type, type casting will fail and result in an exception.
	 */
	template <template <class Type> class Target>
	Target<T>* as() {
		try {
			Target<T>* target = dynamic_cast<Target<T>*>(this);
			return target;
		} catch (std::bad_cast& bc) {
			LOG4CPLUS_ERROR(logger, "Bad cast: tried to cast " << typeid(*this).name() << " to " << typeid(Target<T>).name() << ".");
			throw bc;
		}
		return nullptr;
	}

	void visit(storm::property::ltl::AbstractLtlFormula<T> const& formula) {
		formula.visit(*this);
	}
};

} /* namespace visitor */
} /* namespace ltl*/
} /* namespace property */
} /* namespace storm */
#endif /* STORM_PROPERTY_LTL_VISITOR_ABSTRACTLTLFORMULAVISITOR_H_ */
