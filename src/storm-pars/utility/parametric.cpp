#include <string>

#include "storm-pars/utility/parametric.h"
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
            typename CoefficientType<storm::RationalFunction>::type evaluate<storm::RationalFunction>(storm::RationalFunction const& function, Valuation<storm::RationalFunction> const& valuation){
                return function.evaluate(valuation);
            }

            template<>
            typename storm::RationalFunction substitute<storm::RationalFunction>(storm::RationalFunction const& function, Valuation<storm::RationalFunction> const& valuation){
                return function.substitute(valuation);
            }


            template<>
            void gatherOccurringVariables<storm::RationalFunction>(storm::RationalFunction const& function, std::set<typename VariableType<storm::RationalFunction>::type>& variableSet){
                function.gatherVariables(variableSet);
            }
            
            template<>
            bool isLinear<storm::RationalFunction>(storm::RationalFunction const& function) {
                return storm::utility::isConstant(function.denominator()) && function.nominator().isLinear();
            }
            
            template<>
            bool isMultiLinearPolynomial<storm::RationalFunction>(storm::RationalFunction const& function) {
                if (!storm::utility::isConstant(function.denominator())) {
                    return false;
                }
                auto varInfos = function.nominator().getVarInfo<false>();
                for (auto const& varInfo : varInfos) {
                    if (varInfo.second.maxDegree() > 1) {
                        return false;
                    }
                }
                return true;
            }
#endif
        }
    }
}
