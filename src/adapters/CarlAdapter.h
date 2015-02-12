/**
 * @file:   extendedCarl.h
 * @author: Sebastian Junges
 *
 * @since March 18, 2014
 */

#ifndef STORM_ADAPTERS_EXTENDEDCARL_H_
#define STORM_ADAPTERS_EXTENDEDCARL_H_

#include <cln/cln.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/core/VariablePool.h>
#include <carl/core/Constraint.h>
#include <carl/core/FactorizedPolynomial.h>

namespace carl
{
    template<typename C, typename O, typename P>
    inline size_t hash_value(carl::MultivariatePolynomial<C,O,P> const& p)
    {
        std::hash<carl::MultivariatePolynomial<C,O,P>> h;
        return h(p);
    }
    
    template<typename Pol>
    inline size_t hash_value(carl::RationalFunction<Pol> const& f)
    {
        std::hash<Pol> h;
        return h(f.nominator()) ^ h(f.denominator());
    }
    
    template<typename Pol>
    inline size_t hash_value(carl::FactorizedPolynomial<Pol> const& p)
    {
        std::hash<FactorizedPolynomial<Pol>> h;
        return h(p);
    }
}


#endif