//
//  parametric.h
//
//  Created by Tim Quatmann on 08/03/16.
//
//

#ifndef STORM_UTILITY_PARAMETRIC_H
#define STORM_UTILITY_PARAMETRIC_H

#include "storm/adapters/CarlAdapter.h"

#include <map>

namespace storm {
    namespace utility {
        namespace parametric {
            
            /*!
             * Access the type of variables from a given function type
             */
            template<typename FunctionType>
            struct VariableType { typedef void type; };
            /*!
             * Acess the type of coefficients from a given function type
             */
            template<typename FunctionType>
            struct CoefficientType { typedef void type; };
            
#ifdef STORM_HAVE_CARL
            template<>
            struct VariableType<storm::RationalFunction> { typedef storm::RationalFunctionVariable type; };
            template<>
            struct CoefficientType<storm::RationalFunction> { typedef storm::RationalFunctionCoefficient type; };
#endif
            
            /*!
             * Evaluates the given function wrt. the given valuation
             */
            template<typename FunctionType>
            typename CoefficientType<FunctionType>::type evaluate(FunctionType const& function, std::map<typename VariableType<FunctionType>::type, typename CoefficientType<FunctionType>::type> const& valuation);
            
            /*!
             * Retrieves the constant part of the given function.
             */
            template<typename FunctionType>
            typename CoefficientType<FunctionType>::type getConstantPart(FunctionType const& function);

        }
        
    }
}


#endif /* STORM_UTILITY_PARAMETRIC_H */
