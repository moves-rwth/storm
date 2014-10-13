//#ifndef STORM_STORAGE_PARAMETERS_H_
//#define STORM_STORAGE_PARAMETERS_H_

#pragma once

#include "storm-config.h"
#ifdef STORM_HAVE_CARL

#include "../adapters/extendedCarl.h"

namespace storm
{
//	typedef carl::MultivariatePolynomial<cln::cl_RA> Polynomial;
	typedef carl::Variable  Variable;
    typedef carl::MultivariatePolynomial<cln::cl_RA> RawPolynomial;
    typedef carl::FactorizedPolynomial<RawPolynomial> Polynomial;
//    typedef RawPolynomial Polynomial;
	
	typedef carl::CompareRelation CompareRelation;
	typedef carl::RationalFunction<Polynomial> RationalFunction;
}
#endif

//#endif

