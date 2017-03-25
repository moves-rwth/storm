#include "storm/modelchecker/region/SparseDtmcRegionModelChecker.h"

#include <chrono>
#include <memory>
#include <boost/optional.hpp>

#include "storm/adapters/CarlAdapter.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/region/RegionCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/RegionSettings.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/stateelimination/MultiValueStateEliminator.h"
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
            SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SparseDtmcRegionModelChecker(std::shared_ptr<ParametricSparseModelType> model, SparseRegionModelCheckerSettings const& settings) :
                    SparseRegionModelChecker<ParametricSparseModelType, ConstantType>(model, settings){
                //intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::~SparseDtmcRegionModelChecker(){
                //intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::canHandle(storm::logic::Formula const& formula) const {
                 //for simplicity we only support state formulas with eventually (e.g. P<0.5 [ F "target" ]) and reachability reward formulas
                if (formula.isProbabilityOperatorFormula()) {
                    storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                    return probabilityOperatorFormula.hasBound() &&  this->canHandle(probabilityOperatorFormula.getSubformula());
                } else if (formula.isRewardOperatorFormula()) {
                    storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                    return rewardOperatorFormula.hasBound() && this->canHandle(rewardOperatorFormula.getSubformula());
                } else if (formula.isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& eventuallyFormula = formula.asEventuallyFormula();
                    if (eventuallyFormula.getSubformula().isInFragment(storm::logic::propositional())) {
                        return true;
                    }
                } else if (formula.isReachabilityRewardFormula()) {
                    storm::logic::EventuallyFormula reachabilityRewardFormula = formula.asReachabilityRewardFormula();
                    if (reachabilityRewardFormula.getSubformula().isInFragment(storm::logic::propositional())) {
                        return true;
                    }
                }
                 STORM_LOG_DEBUG("Region Model Checker could not handle (sub)formula " << formula);
                return false;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocess(std::shared_ptr<ParametricSparseModelType>& simpleModel,
                                                                                                   std::shared_ptr<storm::logic::OperatorFormula>& simpleFormula,
                                                                                                   bool& isApproximationApplicable,
                                                                                                   boost::optional<ConstantType>& constantResult){
                STORM_LOG_DEBUG("Preprocessing for DTMC started.");
                STORM_LOG_THROW(this->getModel()->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Input model is required to have exactly one initial state.");
                //Reset some data
                this->smtSolver=nullptr;
                this->reachabilityFunction=nullptr;

                //Preprocessing depending on the type of the considered formula
                storm::storage::BitVector maybeStates, targetStates;
                boost::optional<std::vector<ParametricType>> stateRewards;
                if (this->isComputeRewards()) {
                    std::vector<ParametricType> stateRewardsAsVector;
                    preprocessForRewards(maybeStates, targetStates, stateRewardsAsVector, isApproximationApplicable, constantResult);
                    stateRewards=std::move(stateRewardsAsVector);
                } else {
                    preprocessForProbabilities(maybeStates, targetStates, isApproximationApplicable, constantResult);
                }
                if(constantResult && constantResult.get()>=storm::utility::zero<ConstantType>()){
                    //The result is already known. Nothing else to do here
                    return;
                }
                STORM_LOG_DEBUG("Elimination of states with constant outgoing transitions is happening now.");
                // Determine the set of states that is reachable from the initial state without jumping over a target state.
                storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->getModel()->getTransitionMatrix(), this->getModel()->getInitialStates(), maybeStates, targetStates);
                // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                maybeStates &= reachableStates;
                // Create a vector for the probabilities to go to a target state in one step.
                std::vector<ParametricType> oneStepProbabilities = this->getModel()->getTransitionMatrix().getConstrainedRowSumVector(maybeStates, targetStates);
                // Determine the initial state of the sub-model.
                storm::storage::sparse::state_type initialState = *(this->getModel()->getInitialStates() % maybeStates).begin();
                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ParametricType> submatrix = this->getModel()->getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                // Eliminate all states with only constant outgoing transitions
                // Convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                storm::storage::FlexibleSparseMatrix<ParametricType> flexibleTransitions(submatrix);
                storm::storage::FlexibleSparseMatrix<ParametricType> flexibleBackwardTransitions(submatrix.transpose(), true);
                // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
                storm::storage::BitVector subsystem(submatrix.getRowCount(), true);
                //The states that we consider to eliminate
                storm::storage::BitVector considerToEliminate(submatrix.getRowCount(), true);
                considerToEliminate.set(initialState, false);

                std::vector<storm::storage::sparse::state_type> statesToEliminate;
                for (auto const& state : considerToEliminate) {
                    bool eliminateThisState=true;
                    for(auto const& entry : flexibleTransitions.getRow(state)){
                        if(!storm::utility::isConstant(entry.getValue())){
                            eliminateThisState=false;
                            break;
                        }
                    }
                    if(!storm::utility::isConstant(oneStepProbabilities[state])){
                        eliminateThisState=false;
                    }
                    if(this->isComputeRewards() && eliminateThisState && !storm::utility::isConstant(stateRewards.get()[state])){
                        //Note: The state reward does not need to be constant but we need to make sure that
                        //no parameter of this reward function occurs as a parameter in the probability functions of the predecessors
                        // (otherwise, more complex functions might occur in our simple model)
                        std::set<VariableType> probVars;
                        for(auto const& predecessor : flexibleBackwardTransitions.getRow(state)){
                            for(auto const& predecessorTransition : flexibleTransitions.getRow(predecessor.getColumn())){
                                storm::utility::region::gatherOccurringVariables(predecessorTransition.getValue(), probVars);
                            }
                        }
                        std::set<VariableType> rewardVars;
                        storm::utility::region::gatherOccurringVariables(stateRewards.get()[state], rewardVars);
                        for(auto const& rewardVar : rewardVars){
                            if(probVars.find(rewardVar)!=probVars.end()){
                                eliminateThisState=false;
                                break;
                            }
                        }
                    }
                    if(eliminateThisState){
                        subsystem.set(state,false);
                        statesToEliminate.push_back(state);
                    }

                }
                if(stateRewards) {
                    storm::solver::stateelimination::MultiValueStateEliminator<ParametricType> eliminator(flexibleTransitions, flexibleBackwardTransitions, statesToEliminate, oneStepProbabilities, stateRewards.get());
                    eliminator.eliminateAll();
                } else {
                    storm::solver::stateelimination::PrioritizedStateEliminator<ParametricType> eliminator(flexibleTransitions, flexibleBackwardTransitions, statesToEliminate, oneStepProbabilities);
                    eliminator.eliminateAll();
                }
                STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions.");

                //Build the simple model
                STORM_LOG_DEBUG("Building the resulting simplified model.");
                //The matrix. The flexibleTransitions matrix might have empty rows where states have been eliminated.
                //The new matrix should not have such rows. We therefore leave them out, but we have to change the indices of the states accordingly.
                std::vector<storm::storage::sparse::state_type> newStateIndexMap(flexibleTransitions.getRowCount(), flexibleTransitions.getRowCount()); //initialize with some illegal index to easily check if a transition leads to an unselected state
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
                storm::storage::SparseMatrixBuilder<ParametricType> matrixBuilder(numStates, numStates);
                for(storm::storage::sparse::state_type oldStateIndex : subsystem){ 
                    ParametricType missingProbability=storm::utility::region::getNewFunction<ParametricType, CoefficientType>(storm::utility::one<CoefficientType>());
                    //go through columns:
                    for(auto& entry: flexibleTransitions.getRow(oldStateIndex)){ 
                        STORM_LOG_THROW(newStateIndexMap[entry.getColumn()]!=flexibleTransitions.getRowCount(), storm::exceptions::UnexpectedException, "There is a transition to a state that should have been eliminated.");
                        missingProbability-=entry.getValue();
                        matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex],newStateIndexMap[entry.getColumn()], storm::utility::simplify(entry.getValue()));
                    }
                    if(this->isComputeRewards()){
                        // the missing probability always leads to target
                        if(!storm::utility::isZero(missingProbability)){
                            matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], targetState, storm::utility::simplify(missingProbability));
                        }
                    } else{
                        //transition to target state
                        if(!storm::utility::isZero(oneStepProbabilities[oldStateIndex])){
                            missingProbability-=oneStepProbabilities[oldStateIndex];
                            matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], targetState, storm::utility::simplify(oneStepProbabilities[oldStateIndex]));
                        }
                        //transition to sink state
                        if(!storm::utility::isZero(storm::utility::simplify(missingProbability))){ 
                            matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], sinkState, missingProbability);
                        }
                    }
                }
                //add self loops on the additional states (i.e., target and sink)
                matrixBuilder.addNextValue(targetState, targetState, storm::utility::one<ParametricType>());
                matrixBuilder.addNextValue(sinkState, sinkState, storm::utility::one<ParametricType>());
                //The labeling
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
                // other ingredients
                std::unordered_map<std::string, ParametricRewardModelType> rewardModels;
                if(this->isComputeRewards()){
                    std::size_t newState = 0;
                    for (auto oldstate : subsystem) {
                        if(oldstate!=newState){
                            stateRewards.get()[newState++] = std::move(storm::utility::simplify(stateRewards.get()[oldstate]));
                        } else {
                            ++newState;
                        }
                    }
                    stateRewards.get()[newState++] = storm::utility::zero<ParametricType>(); //target state
                    stateRewards.get()[newState++] = storm::utility::zero<ParametricType>(); //sink state
                    stateRewards.get().resize(newState);
                    rewardModels.insert(std::pair<std::string, ParametricRewardModelType>("", ParametricRewardModelType(std::move(stateRewards))));
                }
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;            
                // the final model
                simpleModel = std::make_shared<storm::models::sparse::Dtmc<ParametricType>>(matrixBuilder.build(), std::move(labeling), std::move(rewardModels), std::move(noChoiceLabeling));
                // the corresponding formula
                STORM_LOG_DEBUG("Building the resulting simplified formula.");
                std::shared_ptr<storm::logic::AtomicLabelFormula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
                if(this->isComputeRewards()){
                    std::shared_ptr<storm::logic::EventuallyFormula> eventuallyFormula(new storm::logic::EventuallyFormula(targetFormulaPtr, storm::logic::FormulaContext::Reward));
                    simpleFormula = std::shared_ptr<storm::logic::OperatorFormula>(new storm::logic::RewardOperatorFormula(eventuallyFormula, boost::none, storm::logic::OperatorInformation(boost::none, this->getSpecifiedFormula()->getBound())));
                } else {
                    std::shared_ptr<storm::logic::EventuallyFormula> eventuallyFormula(new storm::logic::EventuallyFormula(targetFormulaPtr));
                    simpleFormula = std::shared_ptr<storm::logic::OperatorFormula>(new storm::logic::ProbabilityOperatorFormula(eventuallyFormula, storm::logic::OperatorInformation(boost::none, this->getSpecifiedFormula()->getBound())));
                }
                //Check if the reachability function needs to be computed
                if((this->getSettings().getSmtMode()==storm::settings::modules::RegionSettings::SmtMode::FUNCTION) ||
                        (this->getSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::EVALUATE)){
                    this->computeReachabilityFunction(*(this->getSimpleModel())->template as<storm::models::sparse::Dtmc<ParametricType>>());
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocessForProbabilities(storm::storage::BitVector& maybeStates,
                                                                                                                   storm::storage::BitVector& targetStates,
                                                                                                                   bool& isApproximationApplicable,
                                                                                                                   boost::optional<ConstantType>& constantResult) {
                STORM_LOG_DEBUG("Preprocessing for Dtmcs and reachability probabilities invoked.");
                //Get Target States
                storm::modelchecker::SparsePropositionalModelChecker<ParametricSparseModelType> modelChecker(*(this->getModel()));
                std::unique_ptr<CheckResult> targetStatesResultPtr = modelChecker.check(
                            this->getSpecifiedFormula()->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula()
                        );
                targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                //maybeStates: Compute the subset of states that have a probability of 0 or 1, respectively and reduce the considered states accordingly.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->getModel()->getBackwardTransitions(), storm::storage::BitVector(this->getModel()->getNumberOfStates(),true), targetStates);
                maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
                STORM_LOG_DEBUG("Found " << maybeStates.getNumberOfSetBits() << " 'maybe' states. Total number of states is " << maybeStates.size() << ".");
                // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
                storm::storage::sparse::state_type initialState = *(this->getModel()->getInitialStates().begin());
                if (!maybeStates.get(initialState)) {
                    STORM_LOG_WARN("The probability of the initial state is constant (zero or one)");
                    this->reachabilityFunction = std::make_shared<ParametricType>(statesWithProbability01.first.get(initialState) ? storm::utility::zero<ParametricType>() : storm::utility::one<ParametricType>());
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
                    for(auto const& entry : this->getModel()->getTransitionMatrix().getRow(*state)){
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
                    constantResult = storm::utility::convertNumber<ConstantType>(-1.0);
                }
            }


            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::preprocessForRewards(storm::storage::BitVector& maybeStates,
                                                                                                             storm::storage::BitVector& targetStates,
                                                                                                             std::vector<ParametricType>& stateRewards,
                                                                                                             bool& isApproximationApplicable,
                                                                                                             boost::optional<ConstantType>& constantResult) {
                STORM_LOG_DEBUG("Preprocessing for Dtmcs and reachability rewards invoked.");
                //get the correct reward model
                ParametricRewardModelType const* rewardModel;
                if(this->getSpecifiedFormula()->asRewardOperatorFormula().hasRewardModelName()){
                    std::string const& rewardModelName = this->getSpecifiedFormula()->asRewardOperatorFormula().getRewardModelName();
                    STORM_LOG_THROW(this->getModel()->hasRewardModel(rewardModelName), storm::exceptions::InvalidPropertyException, "The Property specifies refers to the reward model '" << rewardModelName << "which is not defined by the given model");
                    rewardModel=&(this->getModel()->getRewardModel(rewardModelName));
                } else {
                    STORM_LOG_THROW(this->getModel()->hasRewardModel(), storm::exceptions::InvalidArgumentException, "No reward model specified");
                    STORM_LOG_THROW(this->getModel()->hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "Ambiguous reward model. Specify it in the formula!");
                    rewardModel=&(this->getModel()->getUniqueRewardModel());
                }
                //Get target states
                storm::modelchecker::SparsePropositionalModelChecker<ParametricSparseModelType> modelChecker(*(this->getModel()));
                std::unique_ptr<CheckResult> targetStatesResultPtr = modelChecker.check(
                            this->getSpecifiedFormula()->asRewardOperatorFormula().getSubformula().asReachabilityRewardFormula().getSubformula()
                        );
                targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                //maybeStates: Compute the subset of states that has a reachability reward less than infinity.
                storm::storage::BitVector statesWithProbability1 = storm::utility::graph::performProb1(this->getModel()->getBackwardTransitions(), storm::storage::BitVector(this->getModel()->getNumberOfStates(), true), targetStates);
                maybeStates = ~targetStates & statesWithProbability1;
                //Compute the new state reward vector
                stateRewards=rewardModel->getTotalRewardVector(maybeStates.getNumberOfSetBits(), this->getModel()->getTransitionMatrix(), maybeStates);
                // If the initial state is known to have 0 reward or an infinite reachability reward value, we can directly set the reachRewardFunction.
                storm::storage::sparse::state_type initialState = *this->getModel()->getInitialStates().begin();
                if (!maybeStates.get(initialState)) {
                    STORM_LOG_WARN("The expected reward of the initial state is constant (infinity or zero)");
                    // Note: storm::utility::infinity<storm::RationalFunction> does not work at this moment.
                    // In that case, we are going to throw in exception if the function is accessed (i.e. in getReachabilityFunction);
                    this->reachabilityFunction = statesWithProbability1.get(initialState) ? std::make_shared<ParametricType>(storm::utility::zero<ParametricType>()) : nullptr;
                    constantResult = statesWithProbability1.get(initialState) ? storm::utility::zero<ConstantType>() : storm::utility::infinity<ConstantType>();
                    isApproximationApplicable = true;
                    return; //nothing else to do...
                }
                 //check if approximation is applicable and whether the result is constant
                isApproximationApplicable=true;
                bool isResultConstant=true;
                std::set<VariableType> rewardPars; //the set of parameters that occur on a reward function
                std::set<VariableType> probPars;   //the set of parameters that occur on a probability function
                for (auto state=maybeStates.begin(); state!=maybeStates.end() && isApproximationApplicable; ++state) {
                    //Constant/Linear probability functions
                    for(auto const& entry : this->getModel()->getTransitionMatrix().getRow(*state)){
                        if(!storm::utility::isConstant(entry.getValue())){
                            isResultConstant=false;
                            if(!storm::utility::region::functionIsLinear(entry.getValue())){
                                isApproximationApplicable=false;
                                break;
                            }
                            storm::utility::region::gatherOccurringVariables(entry.getValue(), probPars);
                        }
                    }
                    //Constant/Linear state rewards
                    if(rewardModel->hasStateRewards() && !storm::utility::isConstant(rewardModel->getStateRewardVector()[*state])){
                        isResultConstant=false;
                        if(!storm::utility::region::functionIsLinear(rewardModel->getStateRewardVector()[*state])){
                            isApproximationApplicable=false;
                            break;
                        }
                        storm::utility::region::gatherOccurringVariables(rewardModel->getStateRewardVector()[*state], rewardPars);
                    }
                    //Constant/Linear transition rewards
                    if(rewardModel->hasTransitionRewards()){
                        for(auto const& entry : rewardModel->getTransitionRewardMatrix().getRow(*state)) {
                            if(!storm::utility::isConstant(entry.getValue())){
                                isResultConstant=false;
                                if(!storm::utility::region::functionIsLinear(entry.getValue())){
                                    isApproximationApplicable=false;
                                    break;
                                }
                                storm::utility::region::gatherOccurringVariables(entry.getValue(), rewardPars);
                            }
                        }
                    }
                }
                //Finally, we need to check whether rewardPars and probPars are disjoint
                //Note: It would also work to simply rename the parameters that occur in both sets.
                //This is to avoid getting functions with local maxima like p * (1-p)
                for(auto const& rewardVar : rewardPars){
                    if(probPars.find(rewardVar)!=probPars.end()){
                        isApproximationApplicable=false;
                        break;
                    }
                }
                if(isResultConstant){
                    STORM_LOG_WARN("For the given property, the reachability Value is constant, i.e., independent of the region");
                    constantResult = storm::utility::convertNumber<ConstantType>(-1.0);
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::computeReachabilityFunction(storm::models::sparse::Dtmc<ParametricType> const& simpleModel){
                std::chrono::high_resolution_clock::time_point timeComputeReachabilityFunctionStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Computing the Reachability function...");
                //get the one step probabilities and the transition matrix of the simple model without target/sink state
                storm::storage::SparseMatrix<ParametricType> backwardTransitions(simpleModel.getBackwardTransitions());
                std::vector<ParametricType> oneStepProbabilities(simpleModel.getNumberOfStates()-2, storm::utility::zero<ParametricType>());
                for(auto const& entry : backwardTransitions.getRow(*(simpleModel.getStates("target").begin()))){
                    if(entry.getColumn()<oneStepProbabilities.size()){
                        oneStepProbabilities[entry.getColumn()]=entry.getValue();
                    } //else case: only holds for the entry that corresponds to the selfloop on the target state..
                }
                storm::storage::BitVector maybeStates=~(simpleModel.getStates("target") | simpleModel.getStates("sink"));
                backwardTransitions=backwardTransitions.getSubmatrix(false,maybeStates,maybeStates);
                storm::storage::SparseMatrix<ParametricType> forwardTransitions=simpleModel.getTransitionMatrix().getSubmatrix(false,maybeStates,maybeStates);
                //now compute the functions using methods from elimination model checker
                storm::storage::BitVector newInitialStates = simpleModel.getInitialStates() % maybeStates;
                storm::storage::BitVector phiStates(simpleModel.getNumberOfStates(), true);
                std::vector<ParametricType> values;
                if(this->isComputeRewards()){
                    values = simpleModel.getUniqueRewardModel().getTotalRewardVector(maybeStates.getNumberOfSetBits(), simpleModel.getTransitionMatrix(), maybeStates);
                } else {
                    values = oneStepProbabilities;
                }
//                storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ParametricType>> eliminationModelChecker(simpleModel);
//                std::vector<std::size_t> statePriorities = eliminationModelChecker.getStatePriorities(forwardTransitions,backwardTransitions,newInitialStates,oneStepProbabilities);
//                this->reachabilityFunction=std::make_shared<ParametricType>(eliminationModelChecker.computeReachabilityValue(forwardTransitions, oneStepProbabilities, backwardTransitions, newInitialStates , true, phiStates, simpleModel.getStates("target"), stateRewards, statePriorities));
	          std::vector<ParametricType> reachFuncVector = storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ParametricType>>::computeReachabilityValues(
	                    forwardTransitions, values, backwardTransitions, newInitialStates, true, oneStepProbabilities);
		  this->reachabilityFunction=std::make_shared<ParametricType>(std::move(reachFuncVector[*simpleModel.getInitialStates().begin()]));
                   /* std::string funcStr = " (/ " +
                                    this->reachabilityFunction->nominator().toString(false, true) + " " +
                                    this->reachabilityFunction->denominator().toString(false, true) +
                                " )";
                    std::cout << std::endl <<"the resulting reach prob function is " << std::endl << funcStr << std::endl << std::endl;*/
                STORM_LOG_DEBUG("Done with computing the reachabilityFunction");
                std::chrono::high_resolution_clock::time_point timeComputeReachabilityFunctionEnd = std::chrono::high_resolution_clock::now();
                this->timeComputeReachabilityFunction = timeComputeReachabilityFunctionEnd-timeComputeReachabilityFunctionStart;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::checkPoint(ParameterRegion<ParametricType>& region, std::map<VariableType, CoefficientType>const& point, bool favorViaFunction) {
                bool valueInBoundOfFormula;
                if((this->getSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::EVALUATE) ||
                        (!this->getSettings().doSample() && favorViaFunction)){
                    //evaluate the reachability function
                    valueInBoundOfFormula = this->valueIsInBoundOfFormula(this->evaluateReachabilityFunction(point));
                }
                else{
                    //instantiate the sampling model
                    valueInBoundOfFormula = this->checkFormulaOnSamplingPoint(point);
                }

                if(valueInBoundOfFormula){
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
            std::shared_ptr<typename SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::ParametricType> const& SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::getReachabilityFunction() {
                if(this->reachabilityFunction==nullptr){
                    //Todo: remove workaround (infinity<storm::RationalNumber>() does not work)
                    std::map<VariableType, CoefficientType> emptySubstitution;
                    if(this->isResultConstant() && this->getReachabilityValue(emptySubstitution)==storm::utility::infinity<ConstantType>()){
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Requested the reachability function but it can not be represented (The value is infinity)");
                        return this->reachabilityFunction;
                    }
                    STORM_LOG_WARN("Reachability Function requested but it has not been computed when specifying the formula. Will compute it now.");
                    computeReachabilityFunction(*(this->getSimpleModel())->template as<storm::models::sparse::Dtmc<ParametricType>>());
                }
                STORM_LOG_THROW((!this->isResultConstant() || storm::utility::isConstant(*this->reachabilityFunction)), storm::exceptions::UnexpectedException, "The result was assumed to be constant but it isn't.");
                return this->reachabilityFunction;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            typename SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::CoefficientType SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::evaluateReachabilityFunction(std::map<VariableType, CoefficientType> const& point) {
                return storm::utility::region::evaluateFunction(*getReachabilityFunction(), point);
            }


            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::checkSmt(ParameterRegion<ParametricType>& region) {
                STORM_LOG_THROW((this->getSettings().getSmtMode()==storm::settings::modules::RegionSettings::SmtMode::FUNCTION), storm::exceptions::NotImplementedException, "Selected SMT mode has not been implemented.");
                if (region.getCheckResult()==RegionCheckResult::UNKNOWN){
                    //Sampling needs to be done (on a single point)
                    checkPoint(region,region.getSomePoint(), true);
                }

                if(this->smtSolver==nullptr){
                    initializeSMTSolver();
                }

                this->smtSolver->push();

                //add constraints for the region
                for(auto const& variable : region.getVariables()) {
                    storm::utility::region::addParameterBoundsToSmtSolver(this->smtSolver, variable, storm::logic::ComparisonType::GreaterEqual, region.getLowerBoundary(variable));
                    storm::utility::region::addParameterBoundsToSmtSolver(this->smtSolver, variable, storm::logic::ComparisonType::LessEqual, region.getUpperBoundary(variable));
                }

                //add constraint that states what we want to prove            
                VariableType proveAllSatVar=storm::utility::region::getVariableFromString<VariableType>("storm_proveAllSat");     
                VariableType proveAllViolatedVar=storm::utility::region::getVariableFromString<VariableType>("storm_proveAllViolated");     
                switch(region.getCheckResult()){
                    case RegionCheckResult::EXISTSBOTH:
                        STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::EXISTSBOTH), "checkSmt invoked although the result is already clear (EXISTSBOTH). Will validate this now...");
                    case RegionCheckResult::ALLSAT:
                        STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::ALLSAT), "checkSmt invoked although the result is already clear (ALLSAT). Will validate this now...");
                    case RegionCheckResult::EXISTSSAT:
                        storm::utility::region::addBoolVariableToSmtSolver(this->smtSolver, proveAllSatVar, true);
                        storm::utility::region::addBoolVariableToSmtSolver(this->smtSolver, proveAllViolatedVar, false);
                        break;
                    case RegionCheckResult::ALLVIOLATED:
                        STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::ALLVIOLATED), "checkSmt invoked although the result is already clear (ALLVIOLATED). Will validate this now...");
                    case RegionCheckResult::EXISTSVIOLATED:
                        storm::utility::region::addBoolVariableToSmtSolver(this->smtSolver, proveAllSatVar, false);
                        storm::utility::region::addBoolVariableToSmtSolver(this->smtSolver, proveAllViolatedVar, true);
                        break;
                    default:
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not handle the current region CheckResult: " << region.getCheckResult());
                }

                storm::solver::SmtSolver::CheckResult solverResult= this->smtSolver->check();
                this->smtSolver->pop();

                switch(solverResult){
                    case storm::solver::SmtSolver::CheckResult::Sat:
                        switch(region.getCheckResult()){
                            case RegionCheckResult::EXISTSSAT:
                                region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                                //There is also a violated point
                                STORM_LOG_WARN("Extracting a violated point from the smt solver is not yet implemented!");
                                break;
                            case RegionCheckResult::EXISTSVIOLATED:
                                region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                                //There is also a sat point
                                STORM_LOG_WARN("Extracting a sat point from the smt solver is not yet implemented!");
                                break;
                            case RegionCheckResult::EXISTSBOTH:
                                //That was expected
                                STORM_LOG_WARN("result EXISTSBOTH Validated!");
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The solver gave an unexpected result (sat)");
                        }
                        return true;
                    case storm::solver::SmtSolver::CheckResult::Unsat:
                        switch(region.getCheckResult()){
                            case RegionCheckResult::EXISTSSAT:
                                region.setCheckResult(RegionCheckResult::ALLSAT);
                                break;
                            case RegionCheckResult::EXISTSVIOLATED:
                                region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                                break;
                            case RegionCheckResult::ALLSAT:
                                //That was expected...
                                STORM_LOG_WARN("result ALLSAT Validated!");
                                break;
                            case RegionCheckResult::ALLVIOLATED:
                                //That was expected...
                                STORM_LOG_WARN("result ALLVIOLATED Validated!");
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The solver gave an unexpected result (unsat)");
                        }
                        return true;
                    case storm::solver::SmtSolver::CheckResult::Unknown:
                    default:
                        STORM_LOG_WARN("The SMT solver was not able to compute a result for this region. (Timeout? Memout?)");
                        if(this->smtSolver->isNeedsRestart()){
                            initializeSMTSolver();
                        }
                        return false;
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::initializeSMTSolver() {
                STORM_LOG_DEBUG("Initializing the Smt Solver");

                storm::expressions::ExpressionManager manager; //this manager will do nothing as we will use carl expressions..
                this->smtSolver = std::shared_ptr<storm::solver::SmtlibSmtSolver>(new storm::solver::SmtlibSmtSolver(manager, true));

                ParametricType bound= storm::utility::convertNumber<ParametricType>(this->getSpecifiedFormulaBound());

                // To prove that the property is satisfied in the initial state for all parameters,
                // we ask the solver whether the negation of the property is satisfiable and invert the answer.
                // In this case, assert that this variable is true:
                VariableType proveAllSatVar=storm::utility::region::getNewVariable<VariableType>("storm_proveAllSat", storm::utility::region::VariableSort::VS_BOOL);

                //Example:
                //Property:    P<=p [ F 'target' ] holds iff...
                // f(x)         <= p
                // Hence: If  f(x) > p is unsat, the property is satisfied for all parameters.

                storm::logic::ComparisonType proveAllSatRel; //the relation from the property needs to be inverted
                switch (this->getSpecifiedFormula()->getComparisonType()) {
                    case storm::logic::ComparisonType::Greater:
                        proveAllSatRel=storm::logic::ComparisonType::LessEqual;
                        break;
                    case storm::logic::ComparisonType::GreaterEqual:
                        proveAllSatRel=storm::logic::ComparisonType::Less;
                        break;
                    case storm::logic::ComparisonType::Less:
                        proveAllSatRel=storm::logic::ComparisonType::GreaterEqual;
                        break;
                    case storm::logic::ComparisonType::LessEqual:
                        proveAllSatRel=storm::logic::ComparisonType::Greater;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
                }
                storm::utility::region::addGuardedConstraintToSmtSolver(this->smtSolver, proveAllSatVar, *getReachabilityFunction(), proveAllSatRel, bound);

                // To prove that the property is violated in the initial state for all parameters,
                // we ask the solver whether the the property is satisfiable and invert the answer.
                // In this case, assert that this variable is true:
                VariableType proveAllViolatedVar=storm::utility::region::getNewVariable<VariableType>("storm_proveAllViolated", storm::utility::region::VariableSort::VS_BOOL);        

                //Example:
                //Property:    P<=p [ F 'target' ] holds iff...
                // f(x)         <= p
                // Hence: If f(x)  <= p is unsat, the property is violated for all parameters. 
                storm::logic::ComparisonType proveAllViolatedRel = this->getSpecifiedFormula()->getComparisonType();
                storm::utility::region::addGuardedConstraintToSmtSolver(this->smtSolver, proveAllViolatedVar, *getReachabilityFunction(), proveAllViolatedRel, bound);          
            }



#ifdef STORM_HAVE_CARL
            template class SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
#endif
        } // namespace region 
    } // namespace modelchecker
} // namespace storm
