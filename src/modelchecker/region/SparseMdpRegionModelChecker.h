#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPREGIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPREGIONMODELCHECKER_H_

#include "src/modelchecker/region/SparseRegionModelChecker.h"

#include "src/models/sparse/StandardRewardModel.h"
#include "src/models/sparse/Mdp.h"
#include "src/utility/region.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class SparseMdpRegionModelChecker : public SparseRegionModelChecker<ParametricSparseModelType, ConstantType> {
            public:

                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename ParametricSparseModelType::RewardModelType ParametricRewardModelType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                explicit SparseMdpRegionModelChecker(std::shared_ptr<ParametricSparseModelType> model);

                virtual ~SparseMdpRegionModelChecker();

                /*!
                 * Checks if the given formula can be handled by This region model checker
                 * @param formula the formula to be checked
                 */
                virtual bool canHandle(storm::logic::Formula const& formula) const;

            protected:
                
                /*!
                 * Checks whether the approximation technique is applicable and whether the model checking result is independent of parameters (i.e., constant).
                 * In the latter case, the given parameter is set either to the correct result or -1
                 * Computes a model with a single target and at most one sink state.
                 * Eliminates all states for which the outgoing transitions are constant.
                 * If rewards are relevant, transition rewards are transformed to state rewards
                 * 
                 * @note this->specifiedFormula and this->computeRewards has to be set accordingly before calling this function
                 */
                virtual void preprocess(std::shared_ptr<ParametricSparseModelType>& simpleModel, std::shared_ptr<storm::logic::OperatorFormula>& simpleFormula, bool& isApproximationApplicable, boost::optional<ConstantType>& constantResult);

                private:
                /*!
                 * Does some sanity checks and preprocessing steps on the currently specified model and 
                 * reachability probability formula, i.e., 
                 * * Computes maybeStates and targetStates
                 * * Sets whether approximation is applicable
                 * * If the result is constant, it is already computed or set to -1
                 * 
                 * @note The returned set of target states also includes states where an 'actual' target state is reached with probability 1
                 * 
                 */
                void preprocessForProbabilities(storm::storage::BitVector& maybeStates, storm::storage::BitVector& targetStates, bool& isApproximationApplicable, boost::optional<ConstantType>& constantResult);

                /*!
                 * Checks the value of the function at the given sampling point.
                 * May set the satPoint and violatedPoint of the regions if thy are not yet specified and such point is given.
                 * Also changes the regioncheckresult of the region to EXISTSSAT, EXISTSVIOLATED, or EXISTSBOTH
                 * 
                 * @param favorViaFunction if not stated otherwise (e.g. in the settings), the sampling will be done via the
                 *                          reachabilityFunction if this flag is true. If the flag is false, sampling will be 
                 *                          done via instantiation of the samplingmodel. Note that this argument is ignored,
                 *                          unless sampling has been turned of in the settings
                 * 
                 * @return true if an violated point as well as a sat point has been found, i.e., the check result is changed to EXISTSOTH
                 */
                virtual bool checkPoint(ParameterRegion<ParametricType>& region, std::map<VariableType, CoefficientType>const& point, bool favorViaFunction=false);
                
                /*!
                 * Starts the SMTSolver to get the result.
                 * The current regioncheckresult of the region should be EXISTSSAT or EXISTVIOLATED.
                 * Otherwise, a sampingPoint will be computed.
                 * True is returned iff the solver was successful (i.e., it returned sat or unsat)
                 * A Sat- or Violated point is set, if the solver has found one (not yet implemented!).
                 * The region checkResult of the given region is changed accordingly.
                 */
                virtual bool checkSmt(ParameterRegion<ParametricType>& region); 

            };
        } //namespace region
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPREGIONMODELCHECKER_H_ */
