#pragma once
#include "storm-config.h"

#include "storm/adapters/RationalNumberForward.h"
#ifdef STORM_CARL_SUPPORTS_FWD_DECL
#include "carl/core/MultivariatePolynomialForward.h"
#else
// This must be included first
// (for old version. Once STORM_CARL_SUPPORTS_FWD_DECL is true, this is no longer relevant).
#include <carl/numbers/numbers.h>
// These must be included later...
#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/core/Relation.h>
#include <carl/core/VariablePool.h>
#include "storm/adapters/RationalNumberAdapter.h"
#endif

namespace carl {

template<typename Number>
class Interval;

template<typename P>
class FactorizedPolynomial;

template<typename P>
class Cache;

template<typename P, bool as>
class RationalFunction;

}  // namespace carl

namespace storm {

typedef carl::Variable RationalFunctionVariable;

#if defined(STORM_HAVE_CLN) && defined(STORM_USE_CLN_RF)
typedef ClnRationalNumber RationalFunctionCoefficient;
#elif defined(STORM_HAVE_GMP) && !defined(STORM_USE_CLN_RF)
typedef GmpRationalNumber RationalFunctionCoefficient;
#elif defined(STORM_USE_CLN_RF)
#error CLN is to be used, but is not available.
#else
#error GMP is to be used, but is not available.
#endif

typedef carl::MultivariatePolynomial<RationalFunctionCoefficient> RawPolynomial;
typedef carl::FactorizedPolynomial<RawPolynomial> Polynomial;

typedef carl::RationalFunction<Polynomial, true> RationalFunction;
typedef carl::Interval<double> Interval;

}  // namespace storm
