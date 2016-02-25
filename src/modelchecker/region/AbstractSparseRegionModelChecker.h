/* 
 * File:   AbstractSparseRegionModelChecker.h
 * Author: tim
 *
 * Created on September 9, 2015, 12:34 PM
 */

#ifndef STORM_MODELCHECKER_REGION_ABSTRACTSPARSEREGIONMODELCHECKER_H
#define	STORM_MODELCHECKER_REGION_ABSTRACTSPARSEREGIONMODELCHECKER_H

#include <ostream>
#include <boost/optional.hpp>

#include "src/utility/region.h"
#include "src/modelchecker/region/ParameterRegion.h"
#include "src/logic/Formulas.h"

namespace storm {
    namespace modelchecker{
        namespace region{
            template<typename ParametricType, typename ConstantType>
            class AbstractSparseRegionModelChecker {
            public:

                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                
                /*!
                 * Checks if the given formula can be handled by This region model checker
                 * @param formula the formula to be checked
                 */
                virtual bool canHandle(storm::logic::Formula const& formula) const = 0;
                
                /*!
                 * Specifies the considered formula.
                 * A few preprocessing steps are performed.
                 * If another formula has been specified before, all 'context' regarding the old formula is lost.
                 * 
                 * @param formula the formula to be considered.
                 */
                virtual void specifyFormula(std::shared_ptr<storm::logic::Formula> formula) = 0;
                
                /*!
                 * Checks for every given region whether the specified formula holds for all parameters that lie in that region.
                 * Sets the region checkresult accordingly.
                 * TODO: set region.satpoint and violated point correctly.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param region The considered region
                 */
                virtual void checkRegions(std::vector<ParameterRegion<ParametricType>>& regions) = 0;
                
                /*!
                 * Refines a given region and checks whether the specified formula holds for all parameters in the subregion.
                 * The procedure stops as soon as the fraction of the area of regions where the result is neither "ALLSAT" nor "ALLVIOLATED" is less then the given threshold.
                 * 
                 * It is required that the given vector of regions contains exactly one region (the parameter space). All the analyzed regions are appended to that vector.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param regions The considered region
                 * @param refinementThreshold The considered threshold.
                 */
                virtual void refineAndCheckRegion(std::vector<ParameterRegion<ParametricType>>& regions, double const& refinementThreshold) = 0;
                
                /*!
                 * Checks whether the given formula holds for all parameters that lie in the given region.
                 * Sets the region checkresult accordingly.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param region The considered region
                 * 
                 */
                virtual void checkRegion(ParameterRegion<ParametricType>& region) = 0;
                
                /*!
                 * Returns the reachability Value at the specified point by instantiating and checking the sampling model. 
                 * 
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 */
                virtual ConstantType getReachabilityValue(std::map<VariableType, CoefficientType>const& point) = 0;
                
                /*!
                 * Computes the reachability Value at the specified point by instantiating and checking the sampling model. 
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 * @return true iff the specified formula is satisfied
                 */
                virtual bool checkFormulaOnSamplingPoint(std::map<VariableType, CoefficientType>const& point) = 0;
                
                /*!
                 * Computes the approximative Value for the given region by instantiating and checking the approximation model. 
                 * returns true iff the provided formula is satisfied w.r.t. the approximative value
                 * 
                 * @param region The region for which to compute the approximative value
                 * @param proveAllSat if set to true, it is checked whether the property is satisfied for all parameters in the given region. Otherwise, it is checked
                          whether the property is violated for all parameters.
                 * @return true iff the objective (given by the proveAllSat flag) was accomplished.
                 */
                virtual bool checkRegionWithApproximation(ParameterRegion<ParametricType> const& region, bool proveAllSat) = 0;
                
                /*!
                 * Returns true iff the given value satisfies the bound given by the specified property
                 */
                virtual bool valueIsInBoundOfFormula(ConstantType const& value) = 0;
                
                /*!
                 * Returns true iff the given value satisfies the bound given by the specified property
                 */
                virtual bool valueIsInBoundOfFormula(CoefficientType const& value) = 0;
                
                /*!
                 * Prints statistical information to the given stream.
                 */
                virtual void printStatisticsToStream(std::ostream& outstream) = 0;
            };
            
        } //namespace region
    } //namespace modelchecker
} //namespace storm

#endif	/* STORM_MODELCHECKER_REGION_ABSTRACTSPARSEREGIONMODELCHECKER_H */

