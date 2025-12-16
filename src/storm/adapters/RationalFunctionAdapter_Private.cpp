#include "storm/adapters/RationalFunctionAdapter_Private.h"

// See RationalFunctionAdapter.h
template class carl::MultivariatePolynomial<storm::RationalFunctionCoefficient>;
template class carl::FactorizedPolynomial<storm::RawPolynomial>;
template class carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>;
template class carl::RationalFunction<storm::Polynomial, true>;