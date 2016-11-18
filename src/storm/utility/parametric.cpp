/* 
 * File:   parametric.cpp
 * Author: Tim Quatmann
 * 
 * Created by Tim Quatmann on 08/03/16.
 */

#include <string>

#include "storm/utility/parametric.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/settings/SettingsManager.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"

#ifdef STORM_HAVE_CARL
#include<carl/numbers/numbers.h>
#include<carl/core/VariablePool.h>
#endif

namespace storm {
    namespace utility{
        namespace parametric {
            
#ifdef STORM_HAVE_CARL
            template<>
            typename CoefficientType<storm::RationalFunction>::type evaluate<storm::RationalFunction>(storm::RationalFunction const& function, std::map<typename VariableType<storm::RationalFunction>::type, typename CoefficientType<storm::RationalFunction>::type> const& valuation){
                return function.evaluate(valuation);
            }
            
            template<>
            typename CoefficientType<storm::RationalFunction>::type getConstantPart<storm::RationalFunction>(storm::RationalFunction const& function){
                return function.constantPart();
            }
#endif
        }
    }
}
