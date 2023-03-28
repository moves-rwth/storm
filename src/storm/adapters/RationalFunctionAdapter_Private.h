#pragma once
#include "storm-config.h"

#include "RationalFunctionForward.h"

#include "storm/adapters/RationalNumberAdapter.h"

#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/core/Relation.h>
#include <carl/core/VariablePool.h>

namespace carl {
// Define hash values for all polynomials and rational function.
// Needed for boost::hash_combine() and other functions
template<typename C, typename O, typename P>
inline size_t hash_value(carl::MultivariatePolynomial<C, O, P> const& p) {
    std::hash<carl::MultivariatePolynomial<C, O, P>> h;
    return h(p);
}

template<typename Pol>
inline size_t hash_value(carl::FactorizedPolynomial<Pol> const& p) {
    std::hash<carl::FactorizedPolynomial<Pol>> h;
    return h(p);
}

template<typename Pol, bool AutoSimplify>
inline size_t hash_value(carl::RationalFunction<Pol, AutoSimplify> const& f) {
    std::hash<carl::RationalFunction<Pol, AutoSimplify>> h;
    return h(f);
}

template<typename Number>
inline size_t hash_value(carl::Interval<Number> const& i) {
    std::hash<carl::Interval<Number>> h;
    return h(i);
}

}  // namespace carl

namespace storm {
typedef carl::Cache<carl::PolynomialFactorizationPair<RawPolynomial>> RawPolynomialCache;
typedef carl::Relation CompareRelation;

}  // namespace storm