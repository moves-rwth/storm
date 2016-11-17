/* 
 * File:   parametric.cpp
 * Author: Tim Quatmann
 * 
 * Created by Tim Quatmann on 08/03/16.
 */

#include <string>

#include "src/storm/utility/parametric.h"
#include "src/storm/utility/constants.h"
#include "src/storm/utility/macros.h"
#include "src/storm/settings/SettingsManager.h"
#include "src/storm/exceptions/IllegalArgumentException.h"
#include "src/storm/exceptions/NotImplementedException.h"

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
