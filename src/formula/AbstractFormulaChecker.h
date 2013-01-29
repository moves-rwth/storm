#ifndef STORM_FORMULA_ABSTRACTFORMULACHECKER_H_
#define STORM_FORMULA_ABSTRACTFORMULACHECKER_H_

#include "src/formula/AbstractFormula.h"

namespace storm {
namespace formula {

template <class T>
class AbstractFormulaChecker {
	public:
		virtual bool conforms(const AbstractFormula<T>* formula) const = 0;
};

}
}

#endif