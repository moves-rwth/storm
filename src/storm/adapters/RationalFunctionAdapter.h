#pragma once

#include "storm/adapters/RationalFunctionAdapter_Private.h"

namespace storm {

RationalFunctionVariable createRFVariable(std::string const& name);

}  // namespace storm

#ifdef STORM_CARL_SUPPORTS_FWD_DECL
// The version of carl that supported forward declarations
// includes a bugfix also required for instantiating these templates once
extern template class carl::MultivariatePolynomial<storm::RationalFunctionCoefficient>;
extern template class carl::FactorizedPolynomial<storm::RawPolynomial>;
extern template class carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>;
extern template class carl::RationalFunction<storm::Polynomial, true>;
#endif
