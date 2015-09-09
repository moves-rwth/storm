#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_

#include "src/modelchecker/region/AbstractSparseRegionModelChecker.h"

#include "src/models/sparse/StandardRewardModel.h"
#include "src/models/sparse/Dtmc.h"
#include "src/utility/region.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/solver/Smt2SmtSolver.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class SparseDtmcRegionModelChecker : public AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType> {
            public:

                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename ParametricSparseModelType::RewardModelType ParametricRewardModelType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                explicit SparseDtmcRegionModelChecker(ParametricSparseModelType const& model);

                virtual ~SparseDtmcRegionModelChecker();

                /*!
                 * Checks if the given formula can be handled by This region model checker
                 * @param formula the formula to be checked
                 */
                virtual bool canHandle(storm::logic::Formula const& formula) const;

                /*!
                 * Returns the reachability  function. 
                 * If it is not yet available, it is computed.
                 */
                std::shared_ptr<ParametricType> const& getReachabilityFunction();

                /*!
                 * Returns the reachability Value at the specified point. 
                 * The given flag decides whether to initialize a sampling model or to evaluate a reachability function.
                 * Might invoke sampling model initialization or the computation of the reachability function (if these are not available yet)
                 * 
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 * @param evaluateFunction If set, the reachability function is evaluated. Otherwise, the sampling model is instantiated.
                 */
                virtual ConstantType getReachabilityValue(std::map<VariableType, CoefficientType>const& point, bool evaluateFunction=false);
                
                /*!
                 * Evaluates the reachability function with the given substitution.
                 * Makes some checks for the case that the result should be constant.
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 */
                CoefficientType evaluateReachabilityFunction(std::map<VariableType, CoefficientType>const& point);

            protected:
                
                /*!
                 * Checks whether the approximation technique is applicable and whether the model checking result is independent of parameters (i.e., constant)
                 * 
                 * Computes a model with a single target and at most one sink state.
                 * Eliminates all states for which the outgoing transitions are constant.
                 * If rewards are relevant, transition rewards are transformed to state rewards
                 * 
                 * @note this->specifiedFormula and this->computeRewards has to be set accordingly before calling this function
                 */
                virtual void preprocess(std::shared_ptr<ParametricSparseModelType>& simpleModel, std::shared_ptr<storm::logic::Formula>& simpleFormula, bool& isApproximationApplicable, bool& isResultConstant);

                private:
                /*!
                 * Does some sanity checks and preprocessing steps on the currently specified model and 
                 * reachability probability formula, i.e., 
                 * * Sets some formula data and that we do not compute rewards
                 * * Computes maybeStates and targetStates
                 * * Sets the flags that state whether the result is constant and approximation is applicable
                 * 
                 * @note The returned set of target states also includes states where an 'actual' target state is reached with probability 1
                 * 
                 */
                void preprocessForProbabilities(storm::storage::BitVector& maybeStates, storm::storage::BitVector& targetStates, bool& isApproximationApplicable, bool& isResultConstant);

                /*!
                 * Does some sanity checks and preprocessing steps on the currently specified model and 
                 * reachability reward formula, i.e.
                 * * Sets some formula data and that we do compute rewards
                 * * Computes maybeStates, targetStates
                 * * Computes a new stateReward vector that considers state+transition rewards of the original model. (in a sense that we can abstract away from transition rewards)
                 * * Sets the flags that state whether the result is constant and approximation is applicable
                 * 
                 * @note stateRewards.size will equal to maybeStates.numberOfSetBits
                 * 
                 */
                void preprocessForRewards(storm::storage::BitVector& maybeStates, storm::storage::BitVector& targetStates, std::vector<ParametricType>& stateRewards, bool& isApproximationApplicable, bool& isResultConstant);

                /*!
                 * Computes the reachability function via state elimination
                 * @note computeFlagsAndSimplifiedModel should be called before calling this
                 */
                void computeReachabilityFunction(ParametricSparseModelType const& simpleModel);

                /*!
                 * Instantiates the approximation model to compute bounds on the maximal/minimal reachability probability (or reachability reward).
                 * If the current region result is EXISTSSAT (or EXISTSVIOLATED), then this function tries to prove ALLSAT (or ALLVIOLATED).
                 * If this succeeded, then the region check result is changed accordingly.
                 * If the current region result is UNKNOWN, then this function first tries to prove ALLSAT and if that failed, it tries to prove ALLVIOLATED.
                 * In any case, the computed bounds are written to the given lowerBounds/upperBounds.
                 * However, if only the lowerBounds (or upperBounds) have been computed, the other vector is set to a vector of size 0.
                 * True is returned iff either ALLSAT or ALLVIOLATED could be proved.
                 */
                virtual bool checkApproximativeValues(ParameterRegion<ParametricType>& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds); 

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
                bool checkSmt(ParameterRegion<ParametricType>& region); 

                //initializes this->smtSolver which can later be used to give an exact result regarding the whole model.
                void initializeSMTSolver();

                // The  function for the reachability probability (or: reachability reward) in the initial state 
                std::shared_ptr<ParametricType> reachabilityFunction;
                // workaround to represent that the result is infinity (utility::infinity<storm::RationalFunction>() does not work at this moment)
                bool isResultInfinity;
                
                // Instance of an elimination model checker to access its functions
                storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ParametricType>> eliminationModelChecker;
                
                // the smt solver that is used to prove properties with the help of the reachabilityFunction
                std::shared_ptr<storm::solver::Smt2SmtSolver> smtSolver;

            };
        } //namespace region
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_ */
