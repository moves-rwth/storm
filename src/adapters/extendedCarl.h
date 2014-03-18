/** 
 * @file:   extendedCarl.h
 * @author: Sebastian Junges
 *
 * @since March 18, 2014
 */

#ifndef STORM_ADAPTERS_EXTENDEDCARL_H_
#define STORM_ADAPTERS_EXTENDEDCARL_H_

#include <carl/core/MultivariatePolynomial.h>

namespace carl
{
template<typename C, typename O, typename P>
inline size_t hash_value(carl::MultivariatePolynomial<C,O,P> const& p)
{
	std::hash<carl::MultivariatePolynomial<C,O,P>> h;
	return h(p);
}
}

#endif