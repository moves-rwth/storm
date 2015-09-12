#include "src/modelchecker/region/SparseMdpRegionModelChecker.h"

#include <chrono>
#include <memory>
#include <boost/optional.hpp>

#include "src/adapters/CarlAdapter.h"
#include "src/logic/Formulas.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/region/RegionCheckResult.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/RegionSettings.h"
#include "src/solver/OptimizationDirection.h"
#include "src/storage/sparse/StateType.h"
#include "src/utility/constants.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/NotSupportedException.h"


namespace storm {
    namespace modelchecker {
        namespace region {

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::SparseMdpRegionModelChecker(ParametricSparseModelType const& model) : 
                    AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>(model){
                //intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::~SparseMdpRegionModelChecker(){
                //intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::canHandle(storm::logic::Formula const& formula) const {
                 //for simplicity we only support state formulas with eventually (e.g. P<0.5 [ F "target" ])
                if (formula.isProbabilityOperatorFormula()) {
                    storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                    return probabilityOperatorFormula.hasBound() &&  this->canHandle(probabilityOperatorFormula.getSubformula());
                //} else if (formula.isRewardOperatorFormula()) {
                //    storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                //    return rewardOperatorFormula.hasBound() && this->canHandle(rewardOperatorFormula.getSubformula());
                } else if (formula.isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& eventuallyFormula = formula.asEventuallyFormula();
                    if (eventuallyFormula.getSubformula().isPropositionalFormula()) {
                        return true;
                    }
                } else if (formula.isReachabilityRewardFormula()) {
                    storm::logic::ReachabilityRewardFormula reachabilityRewardFormula = formula.asReachabilityRewardFormula();
                    if (reachabilityRewardFormula.getSubformula().isPropositionalFormula()) {
                        return true;
                    }
                }
                 STORM_LOG_DEBUG("Region Model Checker could not handle (sub)formula " << formula);
                return false;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocess(std::shared_ptr<ParametricSparseModelType>& simpleModel,
                                                                                                  std::shared_ptr<storm::logic::OperatorFormula>& simpleFormula,
                                                                                                  bool& isApproximationApplicable,
                                                                                                  boost::optional<ConstantType>& constantResult){
                STORM_LOG_DEBUG("Preprocessing for MDPs started.");
                STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Input model is required to have exactly one initial state.");
                storm::storage::BitVector maybeStates, targetStates;
                preprocessForProbabilities(maybeStates, targetStates, isApproximationApplicable, constantResult);
                //TODO: Actually get a more simple model. This makes a deep copy of the model...
                STORM_LOG_WARN("No simplification of the original model (like elimination of constant transitions) is happening. Will just use a copy of the original model");
                simpleModel = std::make_shared<ParametricSparseModelType>(this->getModel()); //Note: an actual copy is technically not necessary.. but we will do it here..
                simpleFormula = this->getSpecifiedFormula();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocessForProbabilities(storm::storage::BitVector& maybeStates,
                                                                                                                  storm::storage::BitVector& targetStates,
                                                                                                                  bool& isApproximationApplicable,
                                                                                                                  boost::optional<ConstantType>& constantResult) {
                STORM_LOG_DEBUG("Preprocessing for Mdps and reachability probabilities invoked.");
                //Get Target States
                storm::logic::AtomicLabelFormula const& labelFormula = this->getSpecifiedFormula()->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula().asAtomicLabelFormula();
                storm::modelchecker::SparsePropositionalModelChecker<ParametricSparseModelType> modelChecker(this->getModel());
                std::unique_ptr<CheckResult> targetStatesResultPtr = modelChecker.checkAtomicLabelFormula(labelFormula);
                targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                
                //maybeStates: Compute the subset of states that have a probability of 0 or 1, respectively and reduce the considered states accordingly.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
                if (this->specifiedFormulaHasUpperBound()){
                    statesWithProbability01 = storm::utility::graph::performProb01Max(this->getModel(), storm::storage::BitVector(this->getModel().getNumberOfStates(),true), targetStates);
                } else {
                    statesWithProbability01 = storm::utility::graph::performProb01Min(this->getModel(), storm::storage::BitVector(this->getModel().getNumberOfStates(),true), targetStates);
                }
                maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
                // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
                storm::storage::sparse::state_type initialState = *this->getModel().getInitialStates().begin();
                if (!maybeStates.get(initialState)) {
                    STORM_LOG_WARN("The probability of the initial state is constant (zero or one)");
                    constantResult = statesWithProbability01.first.get(initialState) ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>();
                    return; //nothing else to do...
                }
                //extend target states
                targetStates=statesWithProbability01.second;
                //check if approximation is applicable and whether the result is constant
                isApproximationApplicable=true;
                bool isResultConstant=true;
                for (auto state=maybeStates.begin(); (state!=maybeStates.end()) && isApproximationApplicable; ++state) {
                    for(auto const& entry : this->getModel().getTransitionMatrix().getRowGroup(*state)){
                        if(!storm::utility::isConstant(entry.getValue())){
                            isResultConstant=false;
                            if(!storm::utility::region::functionIsLinear(entry.getValue())){
                                isApproximationApplicable=false;
                                break;
                            }
                        }
                    }
                }
                if(isResultConstant){
                    STORM_LOG_WARN("For the given property, the reachability Value is constant, i.e., independent of the region");
                    constantResult = storm::utility::region::convertNumber<ConstantType>(-1.0); //-1 denotes that the result is constant but not yet computed
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::checkApproximativeValues(ParameterRegion<ParametricType>& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds) {
                std::chrono::high_resolution_clock::time_point timeMDPBuildStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The approximation model for mdps has not been implemented yet");
                this->getApproximationModel()->instantiate(region);
                std::chrono::high_resolution_clock::time_point timeMDPBuildEnd = std::chrono::high_resolution_clock::now();
                this->timeApproxModelInstantiation += timeMDPBuildEnd-timeMDPBuildStart;

                // Decide whether to prove allsat or allviolated. 
                bool proveAllSat;
                switch (region.getCheckResult()){
                    case RegionCheckResult::UNKNOWN: 
                        switch(storm::settings::regionSettings().getApproxMode()){
                            case storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST:
                                //Sample a single point to know whether we should try to prove ALLSAT or ALLVIOLATED
                                checkPoint(region,region.getSomePoint(), false);
                                proveAllSat= (region.getCheckResult()==RegionCheckResult::EXISTSSAT);
                                break;
                            case storm::settings::modules::RegionSettings::ApproxMode::GUESSALLSAT:
                                proveAllSat=true;
                                break;
                            case storm::settings::modules::RegionSettings::ApproxMode::GUESSALLVIOLATED:
                                proveAllSat=false;
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The specified approxmode is not supported");
                        }
                        break;
                    case RegionCheckResult::ALLSAT:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive (ALLSAT)");
                         //Intentionally no break;
                    case RegionCheckResult::EXISTSSAT:
                        proveAllSat=true;
                        break;
                    case RegionCheckResult::ALLVIOLATED:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive (ALLViolated)");
                         //Intentionally no break;
                    case RegionCheckResult::EXISTSVIOLATED:
                        proveAllSat=false;
                        break;
                    default:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive, i.e. it should be either EXISTSSAT or EXISTSVIOLATED or UNKNOWN in order to apply approximative values");
                         proveAllSat=true;
                }

                bool formulaSatisfied;
                if((this->specifiedFormulaHasUpperBound() && proveAllSat) || (!this->specifiedFormulaHasUpperBound() && !proveAllSat)){
                    //these are the cases in which we need to compute upper bounds
                    upperBounds = this->getApproximationModel()->computeValues(storm::solver::OptimizationDirection::Maximize);
                    lowerBounds = std::vector<ConstantType>();
                    formulaSatisfied = this->valueIsInBoundOfFormula(upperBounds[*this->getApproximationModel()->getModel()->getInitialStates().begin()]);
                }
                else{
                    //for the remaining cases we compute lower bounds
                    lowerBounds = this->getApproximationModel()->computeValues(storm::solver::OptimizationDirection::Minimize);
                    upperBounds = std::vector<ConstantType>();
                    formulaSatisfied = this->valueIsInBoundOfFormula(lowerBounds[*this->getApproximationModel()->getModel()->getInitialStates().begin()]);
                }

                //check if approximation was conclusive
                if(proveAllSat && formulaSatisfied){
                    region.setCheckResult(RegionCheckResult::ALLSAT);
                    return true;
                }
                if(!proveAllSat && !formulaSatisfied){
                    region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                    return true;
                }

                if(region.getCheckResult()==RegionCheckResult::UNKNOWN){
                    //In this case, it makes sense to try to prove the contrary statement
                    proveAllSat=!proveAllSat;

                    if(lowerBounds.empty()){
                        lowerBounds = this->getApproximationModel()->computeValues(storm::solver::OptimizationDirection::Minimize);
                        formulaSatisfied=this->valueIsInBoundOfFormula(lowerBounds[*this->getApproximationModel()->getModel()->getInitialStates().begin()]);
                    }
                    else{
                        upperBounds = this->getApproximationModel()->computeValues(storm::solver::OptimizationDirection::Maximize);
                        formulaSatisfied=this->valueIsInBoundOfFormula(upperBounds[*this->getApproximationModel()->getModel()->getInitialStates().begin()]);
                    }

                    //check if approximation was conclusive
                    if(proveAllSat && formulaSatisfied){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                        return true;
                    }
                    if(!proveAllSat && !formulaSatisfied){
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                        return true;
                    }
                }
                //if we reach this point than the result is still inconclusive.
                return false;            
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::checkPoint(ParameterRegion<ParametricType>& region, std::map<VariableType, CoefficientType>const& point, bool favorViaFunction) {
                if(this->valueIsInBoundOfFormula(this->getReachabilityValue(point))){
                    if (region.getCheckResult()!=RegionCheckResult::EXISTSSAT){
                        region.setSatPoint(point);
                        if(region.getCheckResult()==RegionCheckResult::EXISTSVIOLATED){
                            region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                            return true;
                        }
                        region.setCheckResult(RegionCheckResult::EXISTSSAT);
                    }
                }
                else{
                    if (region.getCheckResult()!=RegionCheckResult::EXISTSVIOLATED){
                        region.setViolatedPoint(point);
                        if(region.getCheckResult()==RegionCheckResult::EXISTSSAT){
                            region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                            return true;
                        }
                        region.setCheckResult(RegionCheckResult::EXISTSVIOLATED);
                    }
                }
                return false;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::checkSmt(ParameterRegion<ParametricType>& region) {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "checkSmt invoked but smt solving has not been implemented for MDPs.");
            }

#ifdef STORM_HAVE_CARL
            template class SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
#endif
        } // namespace region 
    } // namespace modelchecker
} // namespace storm
