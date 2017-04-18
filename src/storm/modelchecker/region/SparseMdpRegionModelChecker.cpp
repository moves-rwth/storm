#include "storm/modelchecker/region/SparseMdpRegionModelChecker.h"

#include <chrono>
#include <memory>
#include <boost/optional.hpp>

#include "storm/adapters/CarlAdapter.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/region/RegionCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/RegionSettings.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/stateelimination/PrioritizedStateEliminator.h"
#include "storm/solver/stateelimination/StaticStatePriorityQueue.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/logic/FragmentSpecification.h"

namespace storm {
    namespace modelchecker {
        namespace region {

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::SparseMdpRegionModelChecker(std::shared_ptr<ParametricSparseModelType> model, SparseRegionModelCheckerSettings const& settings) :
                    SparseRegionModelChecker<ParametricSparseModelType, ConstantType>(model, settings){
                STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidArgumentException, "Tried to create an mdp region model checker for a model that is not an mdp");
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
                    if (eventuallyFormula.getSubformula().isInFragment(storm::logic::propositional())) {
                        return true;
                    }
              //  } else if (formula.isReachabilityRewardFormula()) {
              //      storm::logic::ReachabilityRewardFormula reachabilityRewardFormula = formula.asReachabilityRewardFormula();
             //       if (reachabilityRewardFormula.getSubformula().isPropositionalFormula()) {
             //           return true;
             //      }
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
                STORM_LOG_THROW(this->getModel()->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Input model is required to have exactly one initial state.");
                storm::storage::BitVector maybeStates, targetStates;
                preprocessForProbabilities(maybeStates, targetStates, isApproximationApplicable, constantResult);
                if(constantResult && constantResult.get()>=storm::utility::zero<ConstantType>()){
                    //The result is already known. Nothing else to do here
                    return;
                }
                STORM_LOG_DEBUG("Elimination of deterministic states with constant outgoing transitions is happening now.");
                // Determine the set of states that is reachable from the initial state without jumping over a target state.
                storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->getModel()->getTransitionMatrix(), this->getModel()->getInitialStates(), maybeStates, targetStates);
                // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                maybeStates &= reachableStates;
                // Create a vector for the probabilities to go to a target state in one step.
                std::vector<ParametricType> oneStepProbabilities = this->getModel()->getTransitionMatrix().getConstrainedRowGroupSumVector(maybeStates, targetStates);
                // Determine the initial state of the sub-model.
                storm::storage::sparse::state_type initialState = *(this->getModel()->getInitialStates() % maybeStates).begin();
                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ParametricType> submatrix = this->getModel()->getTransitionMatrix().getSubmatrix(true, maybeStates, maybeStates);
                boost::optional<std::vector<ParametricType>> noStateRewards;
                // Eliminate all deterministic states with only constant outgoing transitions
                // Convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                storm::storage::FlexibleSparseMatrix<ParametricType> flexibleTransitions(submatrix);
                storm::storage::FlexibleSparseMatrix<ParametricType> flexibleBackwardTransitions(submatrix.transpose(), true);
                // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
                storm::storage::BitVector subsystem(submatrix.getRowGroupCount(), true);
                //The states that we consider to eliminate
                storm::storage::BitVector considerToEliminate(submatrix.getRowGroupCount(), true);
                considerToEliminate.set(initialState, false);


                std::vector<uint64_t> statesToEliminate;
                for (auto const& state : considerToEliminate) {
                    bool eliminateThisState=false;
                    if(submatrix.getRowGroupSize(state) == 1) {
                        eliminateThisState=true;
                         //state is deterministic. Check if outgoing transitions are constant
                        for(auto const& entry : submatrix.getRowGroup(state)){
                            if(!storm::utility::isConstant(entry.getValue())){
                                eliminateThisState=false;
                                break;
                            }
                        }
                        if(!storm::utility::isConstant(oneStepProbabilities[submatrix.getRowGroupIndices()[state]])){
                            eliminateThisState=false;
                        }
                    }
                    if(eliminateThisState) {
                        subsystem.set(state, false);
                        statesToEliminate.push_back(state);
                    }
                }
                storm::solver::stateelimination::PrioritizedStateEliminator<ParametricType> eliminator(flexibleTransitions, flexibleBackwardTransitions, statesToEliminate, oneStepProbabilities);
                eliminator.eliminateAll();
                STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions.");

                //Build the simple model
                STORM_LOG_DEBUG("Building the resulting simplified model.");
                //The matrix. The flexibleTransitions matrix might have empty rows where states have been eliminated.
                //The new matrix should not have such rows. We therefore leave them out, but we have to change the indices of the states accordingly.
                std::vector<storm::storage::sparse::state_type> newStateIndexMap(flexibleTransitions.getRowCount(), flexibleTransitions.getRowCount()); //initialize with some illegal index
                storm::storage::sparse::state_type newStateIndex=0;
                for(auto const& state : subsystem){
                    newStateIndexMap[state]=newStateIndex;
                    ++newStateIndex;
                }
                //We need to add a target state to which the oneStepProbabilities will lead as well as a sink state to which the "missing" probability will lead
                storm::storage::sparse::state_type numStates=newStateIndex+2;
                storm::storage::sparse::state_type targetState=numStates-2;
                storm::storage::sparse::state_type sinkState= numStates-1;
                //We can now fill in the data.
                storm::storage::SparseMatrixBuilder<ParametricType> matrixBuilder(0, numStates, 0, true, true, numStates);
                std::size_t curRow = 0;
                for(auto oldRowGroup : subsystem){
                    matrixBuilder.newRowGroup(curRow);
                    for (auto oldRow = submatrix.getRowGroupIndices()[oldRowGroup]; oldRow < submatrix.getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        ParametricType missingProbability=storm::utility::region::getNewFunction<ParametricType, CoefficientType>(storm::utility::one<CoefficientType>());
                        //go through columns:
                        for(auto& entry: flexibleTransitions.getRow(oldRow)){ 
                            STORM_LOG_THROW(newStateIndexMap[entry.getColumn()]!=flexibleTransitions.getRowCount(), storm::exceptions::UnexpectedException, "There is a transition to a state that should have been eliminated.");
                            missingProbability-=entry.getValue();
                            matrixBuilder.addNextValue(curRow,newStateIndexMap[entry.getColumn()], storm::utility::simplify(entry.getValue()));
                        }
                        //transition to target state
                        if(!storm::utility::isZero(oneStepProbabilities[oldRow])){
                            missingProbability-=oneStepProbabilities[oldRow];
                            matrixBuilder.addNextValue(curRow, targetState, storm::utility::simplify(oneStepProbabilities[oldRow]));
                        }
                        //transition to sink state
                        if(!storm::utility::isZero(storm::utility::simplify(missingProbability))){ 
                            matrixBuilder.addNextValue(curRow, sinkState, missingProbability);
                        }
                        ++curRow;
                    }
                }
                //add self loops on the additional states (i.e., target and sink)
                matrixBuilder.newRowGroup(curRow);
                matrixBuilder.addNextValue(curRow, targetState, storm::utility::one<ParametricType>());
                ++curRow;
                matrixBuilder.newRowGroup(curRow);
                matrixBuilder.addNextValue(curRow, sinkState, storm::utility::one<ParametricType>());
                
                //Get a new labeling
                storm::models::sparse::StateLabeling labeling(numStates);
                storm::storage::BitVector initLabel(numStates, false);
                initLabel.set(newStateIndexMap[initialState], true);
                labeling.addLabel("init", std::move(initLabel));
                storm::storage::BitVector targetLabel(numStates, false);
                targetLabel.set(targetState, true);
                labeling.addLabel("target", std::move(targetLabel));
                storm::storage::BitVector sinkLabel(numStates, false);
                sinkLabel.set(sinkState, true);
                labeling.addLabel("sink", std::move(sinkLabel));
                
                //Other ingredients
                std::unordered_map<std::string, ParametricRewardModelType> noRewardModels;
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
                simpleModel = std::make_shared<storm::models::sparse::Mdp<ParametricType>>(matrixBuilder.build(), std::move(labeling), std::move(noRewardModels), std::move(noChoiceLabeling));
                
                //Get the simplified formula
                std::shared_ptr<storm::logic::AtomicLabelFormula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
                std::shared_ptr<storm::logic::EventuallyFormula> eventuallyFormula(new storm::logic::EventuallyFormula(targetFormulaPtr));
                simpleFormula = std::shared_ptr<storm::logic::OperatorFormula>(new storm::logic::ProbabilityOperatorFormula(eventuallyFormula, storm::logic::OperatorInformation(boost::none, this->getSpecifiedFormula()->getBound())));
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocessForProbabilities(storm::storage::BitVector& maybeStates,
                                                                                                                  storm::storage::BitVector& targetStates,
                                                                                                                  bool& isApproximationApplicable,
                                                                                                                  boost::optional<ConstantType>& constantResult) {
                STORM_LOG_DEBUG("Preprocessing for Mdps and reachability probabilities invoked.");
                //Get Target States
                storm::modelchecker::SparsePropositionalModelChecker<ParametricSparseModelType> modelChecker(*(this->getModel()));
                std::unique_ptr<CheckResult> targetStatesResultPtr = modelChecker.check(
                            this->getSpecifiedFormula()->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula()
                        );
                targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                
                //maybeStates: Compute the subset of states that have a probability of 0 or 1, respectively and reduce the considered states accordingly.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
                if (this->specifiedFormulaHasLowerBound()){
                    statesWithProbability01 = storm::utility::graph::performProb01Min(this->getModel()->getTransitionMatrix(), this->getModel()->getTransitionMatrix().getRowGroupIndices(), this->getModel()->getBackwardTransitions(), storm::storage::BitVector(this->getModel()->getNumberOfStates(),true), targetStates);
                } else {
                    statesWithProbability01 = storm::utility::graph::performProb01Max(this->getModel()->getTransitionMatrix(), this->getModel()->getTransitionMatrix().getRowGroupIndices(), this->getModel()->getBackwardTransitions(), storm::storage::BitVector(this->getModel()->getNumberOfStates(),true), targetStates);
                }
                maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
                STORM_LOG_DEBUG("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states. Total number of states is " << maybeStates.size() << ".");
                // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
                storm::storage::sparse::state_type initialState = *(this->getModel()->getInitialStates().begin());
                if (!maybeStates.get(initialState)) {
                    STORM_LOG_WARN("The probability of the initial state is constant (zero or one)");
                    constantResult = statesWithProbability01.first.get(initialState) ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>();
                    isApproximationApplicable = true;
                    return; //nothing else to do...
                }
                //extend target states
                targetStates=statesWithProbability01.second;
                //check if approximation is applicable and whether the result is constant
                isApproximationApplicable=true;
                bool isResultConstant=true;
                for (auto state=maybeStates.begin(); (state!=maybeStates.end()) && isApproximationApplicable; ++state) {
                    for(auto const& entry : this->getModel()->getTransitionMatrix().getRowGroup(*state)){
                        if(!storm::utility::isConstant(entry.getValue())){
                            isResultConstant=false;
                            if(!storm::utility::region::functionIsLinear(entry.getValue())){
                                isApproximationApplicable=false;
                                //break;
                            }
                        }
                    }
                }
                if(isResultConstant){
                    STORM_LOG_WARN("For the given property, the reachability Value is constant, i.e., independent of the region");
                    constantResult = storm::utility::convertNumber<ConstantType>(-1.0); //-1 denotes that the result is constant but not yet computed
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::checkPoint(ParameterRegion<ParametricType>& region, std::map<VariableType, CoefficientType>const& point, bool /*favorViaFunction*/) {
                            if(this->checkFormulaOnSamplingPoint(point)){
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
            bool SparseMdpRegionModelChecker<ParametricSparseModelType, ConstantType>::checkSmt(ParameterRegion<ParametricType>& /*region*/) {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "checkSmt invoked but smt solving has not been implemented for MDPs.");
            }

#ifdef STORM_HAVE_CARL
            template class SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
#endif
        } // namespace region 
    } // namespace modelchecker
} // namespace storm
