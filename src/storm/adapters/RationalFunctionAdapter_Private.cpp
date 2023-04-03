#include "storm/adapters/RationalFunctionAdapter_Private.h"

#ifdef STORM_CARL_SUPPORTS_FWD_DECL
// See RationalFunctionAdapter.h
template class carl::MultivariatePolynomial<storm::RationalFunctionCoefficient>;
template class carl::FactorizedPolynomial<storm::RawPolynomial>;
template class carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>;
template class carl::RationalFunction<storm::Polynomial, true>;
#endif