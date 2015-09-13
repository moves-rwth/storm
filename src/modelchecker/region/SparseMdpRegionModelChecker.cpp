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
                
                //lets count the number of states without nondet choices and with constant outgoing transitions
                //TODO: Remove this
                storm::storage::SparseMatrix<ParametricType>const& matrix=this->getModel().getTransitionMatrix();
                uint_fast64_t stateCounter=0;
                for (uint_fast64_t state=0; state<this->getModel().getNumberOfStates();++state){
                    if(matrix.getRowGroupSize(state)==1){
                        bool hasConstTransitions=true;
                        for(auto const& entry : matrix.getRowGroup(state)){
                            if(!storm::utility::isConstant(entry.getValue())){
                                hasConstTransitions = false;
                            }
                        }
                        if(hasConstTransitions){
                            ++stateCounter;
                        }
                    }
                }
                std::cout << "Found that " << stateCounter << " of " << this->getModel().getNumberOfStates() << " states could be eliminated" << std::endl;
                
                
                
                //TODO: Actually eliminate the states...
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
