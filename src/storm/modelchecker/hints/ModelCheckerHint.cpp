#include "storm/modelchecker/hints/ModelCheckerHint.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"

namespace storm {
namespace modelchecker {

bool ModelCheckerHint::isEmpty() const {
    return true;
}

bool ModelCheckerHint::isExplicitModelCheckerHint() const {
    return false;
}

template<typename ValueType>
ExplicitModelCheckerHint<ValueType> const& ModelCheckerHint::asExplicitModelCheckerHint() const {
    return dynamic_cast<ExplicitModelCheckerHint<ValueType> const&>(*this);
}

template<typename ValueType>
ExplicitModelCheckerHint<ValueType>& ModelCheckerHint::asExplicitModelCheckerHint() {
    return dynamic_cast<ExplicitModelCheckerHint<ValueType>&>(*this);
}

template ExplicitModelCheckerHint<double> const& ModelCheckerHint::asExplicitModelCheckerHint() const;
template ExplicitModelCheckerHint<double>& ModelCheckerHint::asExplicitModelCheckerHint();
template ExplicitModelCheckerHint<storm::RationalNumber> const& ModelCheckerHint::asExplicitModelCheckerHint() const;
template ExplicitModelCheckerHint<storm::RationalNumber>& ModelCheckerHint::asExplicitModelCheckerHint();
template ExplicitModelCheckerHint<storm::RationalFunction> const& ModelCheckerHint::asExplicitModelCheckerHint() const;
template ExplicitModelCheckerHint<storm::RationalFunction>& ModelCheckerHint::asExplicitModelCheckerHint();

}  // namespace modelchecker
}  // namespace storm
