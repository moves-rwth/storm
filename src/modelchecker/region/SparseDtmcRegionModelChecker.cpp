#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"

#include <chrono>

#include "src/adapters/CarlAdapter.h"

#include "src/modelchecker/region/ParameterRegion.h"
#include "src/modelchecker/region/ApproximationModel.h"
#include "src/modelchecker/region/SamplingModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/RegionSettings.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/UnexpectedException.h"


namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model) : 
                model(model),
                eliminationModelChecker(model),
                specifiedFormula(nullptr){
            STORM_LOG_THROW(model.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Model is required to have exactly one initial state.");
        }
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::~SparseDtmcRegionModelChecker(){
            //intentionally left empty
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::canHandle(storm::logic::Formula const& formula) const {
             //for simplicity we only support state formulas with eventually (e.g. P<0.5 [ F "target" ]) and reachability reward formulas
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                return probabilityOperatorFormula.hasBound() &&  this->canHandle(probabilityOperatorFormula.getSubformula());
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                return rewardOperatorFormula.hasBound() && this->canHandle(rewardOperatorFormula.getSubformula());
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
            } else if (formula.isConditionalPathFormula()) {
                storm::logic::ConditionalPathFormula conditionalPathFormula = formula.asConditionalPathFormula();
                if (conditionalPathFormula.getLeftSubformula().isEventuallyFormula() && conditionalPathFormula.getRightSubformula().isEventuallyFormula()) {
                    return this->canHandle(conditionalPathFormula.getLeftSubformula()) && this->canHandle(conditionalPathFormula.getRightSubformula());
                }
            }
             STORM_LOG_DEBUG("Region Model Checker could not handle (sub)formula " << formula);
            return false;
        }
            
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::specifyFormula(std::shared_ptr<storm::logic::Formula> formula) {
            std::chrono::high_resolution_clock::time_point timeSpecifyFormulaStart = std::chrono::high_resolution_clock::now();
            STORM_LOG_THROW(this->canHandle(*formula), storm::exceptions::InvalidArgumentException, "Tried to specify a formula that can not be handled.");
            
            this->isResultConstant=false;
            this->isApproximationApplicable=false;
            this->smtSolver=nullptr;
            this->approximationModel=nullptr;
            this->samplingModel=nullptr;
            this->reachabilityFunction=nullptr;
            
            //Note: canHandle already ensures that the formula has the right shape.
            this->specifiedFormula = formula;
            // set some information regarding the formula and model. Also computes a more simple version of the model
            preprocess();
            if(!this->isResultConstant){
                //now create the model used for Approximation
                if(storm::settings::regionSettings().doApprox()){
                    initializeApproximationModel(*this->simplifiedModel);
                }
                //now create the model used for Sampling
                if(storm::settings::regionSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE ||
                        (!storm::settings::regionSettings().doSample() && storm::settings::regionSettings().getApproxMode()==storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST)){
                    initializeSamplingModel(*this->simplifiedModel);
                }
                //Check if the reachability function needs to be computed
                if((storm::settings::regionSettings().getSmtMode()==storm::settings::modules::RegionSettings::SmtMode::FUNCTION) || 
                        (storm::settings::regionSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::EVALUATE)){
                    computeReachabilityFunction(*this->simplifiedModel);
                }
            }
            //some information for statistics...
            std::chrono::high_resolution_clock::time_point timeSpecifyFormulaEnd = std::chrono::high_resolution_clock::now();
            this->timeSpecifyFormula= timeSpecifyFormulaEnd - timeSpecifyFormulaStart;
            this->numOfCheckedRegions=0;
            this->numOfRegionsSolvedThroughSampling=0;
            this->numOfRegionsSolvedThroughApproximation=0;
            this->numOfRegionsSolvedThroughFullSmt=0;
            this->numOfRegionsExistsBoth=0;
            this->numOfRegionsAllSat=0;
            this->numOfRegionsAllViolated=0;
            this->timeCheckRegion=std::chrono::high_resolution_clock::duration::zero();
            this->timeSampling=std::chrono::high_resolution_clock::duration::zero();
            this->timeApproximation=std::chrono::high_resolution_clock::duration::zero();
            this->timeMDPBuild=std::chrono::high_resolution_clock::duration::zero();
            this->timeFullSmt=std::chrono::high_resolution_clock::duration::zero();
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::preprocess(){
            std::chrono::high_resolution_clock::time_point timePreprocessingStart = std::chrono::high_resolution_clock::now();
            STORM_LOG_THROW(this->model.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Input model is required to have exactly one initial state.");
            
            //first some preprocessing depending on the type of the considered formula
            storm::storage::BitVector maybeStates, targetStates;
            boost::optional<std::vector<ParametricType>> stateRewards;
            if (this->specifiedFormula->isProbabilityOperatorFormula()) {
                preprocessForProbabilities(maybeStates, targetStates);
            }
            else if (this->specifiedFormula->isRewardOperatorFormula()) {
                std::vector<ParametricType> stateRewardsAsVector;
                preprocessForRewards(maybeStates, targetStates, stateRewardsAsVector);
                stateRewards=std::move(stateRewardsAsVector);
            }
            else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The specified property " << this->specifiedFormula << "is not supported");
            }
            // Determine the set of states that is reachable from the initial state without jumping over a target state.
            storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->model.getTransitionMatrix(), this->model.getInitialStates(), maybeStates, targetStates);
            // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
            maybeStates &= reachableStates;
            // Create a vector for the probabilities to go to a target state in one step.
            std::vector<ParametricType> oneStepProbabilities = this->model.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, targetStates);
            // Determine the initial state of the sub-model.
            storm::storage::sparse::state_type initialState = *(this->model.getInitialStates() % maybeStates).begin();
            // We then build the submatrix that only has the transitions of the maybe states.
            storm::storage::SparseMatrix<ParametricType> submatrix = this->model.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
            
            // eliminate all states with only constant outgoing transitions (and possibly rewards)
            // Convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            auto flexibleTransitions = this->eliminationModelChecker.getFlexibleSparseMatrix(submatrix);
            auto flexibleBackwardTransitions= this->eliminationModelChecker.getFlexibleSparseMatrix(submatrix.transpose(), true);
            // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
            storm::storage::BitVector subsystem(submatrix.getRowCount(), true);
            //The states that we consider to eliminate
            storm::storage::BitVector considerToEliminate(submatrix.getRowCount(), true);
            considerToEliminate.set(initialState, false);
            this->isApproximationApplicable=true;
            this->isResultConstant=true;
            for (auto const& state : considerToEliminate) {
                bool eliminateThisState=true;
                for(auto const& entry : flexibleTransitions.getRow(state)){
                    if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                        this->isResultConstant=false;
                        eliminateThisState=false;
                        this->isApproximationApplicable &= storm::utility::regions::functionIsLinear<ParametricType>(entry.getValue());
                    }
                }
                if(!this->parametricTypeComparator.isConstant(oneStepProbabilities[state])){
                    this->isResultConstant=false;
                    eliminateThisState=false;
                    this->isApproximationApplicable &= storm::utility::regions::functionIsLinear<ParametricType>(oneStepProbabilities[state]);
                }
                if(this->computeRewards && eliminateThisState && !this->parametricTypeComparator.isConstant(stateRewards.get()[state])){
                    //Note: The state reward does not need to be constant but we need to make sure that
                    //no parameter of this reward function occurs as a parameter in the probability functions of the predecessors
                    // (otherwise, more complex functions might occur in our simplified model)
                    // TODO: test if we should really remove these states (the resulting reward functions are less simple this way)
                    std::set<VariableType> probVars;
                    for(auto const& predecessor : flexibleBackwardTransitions.getRow(state)){
                        for(auto const& predecessorTransition : flexibleTransitions.getRow(predecessor.getColumn())){
                            storm::utility::regions::gatherOccurringVariables(predecessorTransition.getValue(), probVars);
                        }
                    }
                    std::set<VariableType> rewardVars;
                    storm::utility::regions::gatherOccurringVariables(stateRewards.get()[state], rewardVars);
                    for(auto const& rewardVar : rewardVars){
                        if(probVars.find(rewardVar)!=probVars.end()){
                            eliminateThisState=false;
                            break;
                        }
                    }
                }
                if(eliminateThisState){
                    this->eliminationModelChecker.eliminateState(flexibleTransitions, oneStepProbabilities, state, flexibleBackwardTransitions, stateRewards);
                    subsystem.set(state,false);
                }
            }
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions.");
            if(this->isResultConstant){
                //Check if this is also the case for the initial state
                for(auto const& entry : flexibleTransitions.getRow(initialState)){
                    this->isResultConstant&=this->parametricTypeComparator.isConstant(entry.getValue());
                }
                this->isResultConstant&=this->parametricTypeComparator.isConstant(oneStepProbabilities[initialState]);
            }
            if(this->computeRewards && (this->isApproximationApplicable || this->isResultConstant)){
                //We will need to check whether this is also the case for the reward functions.
                for(auto const& state : maybeStates){
                    std::set<VariableType> rewardVars;
                    if(this->model.hasStateRewards() && !this->parametricTypeComparator.isConstant(this->model.getStateRewardVector()[state])){
                        this->isResultConstant=false;
                        this->isApproximationApplicable &= storm::utility::regions::functionIsLinear<ParametricType>(this->model.getStateRewardVector()[state]);
                        storm::utility::regions::gatherOccurringVariables(stateRewards.get()[state], rewardVars);
                    }
                    if(this->model.hasTransitionRewards()){
                        for(auto const& entry : this->model.getTransitionRewardMatrix().getRow(state)) {
                            if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                                this->isResultConstant=false;
                                this->isApproximationApplicable &= storm::utility::regions::functionIsLinear<ParametricType>(entry.getValue());
                                storm::utility::regions::gatherOccurringVariables(entry.getValue(), rewardVars);
                            }
                        }
                    }
                    if(!rewardVars.empty()){
                        std::set<VariableType> probVars;
                        for(auto const& entry: this->model.getTransitionMatrix().getRow(state)){
                            storm::utility::regions::gatherOccurringVariables(entry.getValue(), probVars);
                        }
                        for(auto const& rewardVar : rewardVars){
                            if(probVars.find(rewardVar)!=probVars.end()){
                                this->isApproximationApplicable=false;
                                break;
                            }
                        }
                    }
                }
            }
            STORM_LOG_WARN_COND(!this->isResultConstant, "For the given property, the reachability Value is constant, i.e., independent of the region");
            
            //Build the simplified model
            //The matrix. The flexibleTransitions matrix might have empty rows where states have been eliminated.
            //The new matrix should not have such rows. We therefore leave them out, but we have to change the indices of the states accordingly.
            std::vector<storm::storage::sparse::state_type> newStateIndexMap(flexibleTransitions.getNumberOfRows(), flexibleTransitions.getNumberOfRows()); //initialize with some illegal index to easily check if a transition leads to an unselected state
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
                ParametricType missingProbability=storm::utility::regions::getNewFunction<ParametricType, CoefficientType>(storm::utility::one<CoefficientType>());
                //go through columns:
                for(auto const& entry: flexibleTransitions.getRow(oldStateIndex)){ 
                    STORM_LOG_THROW(newStateIndexMap[entry.getColumn()]!=flexibleTransitions.getNumberOfRows(), storm::exceptions::UnexpectedException, "There is a transition to a state that should have been eliminated.");
                    missingProbability-=entry.getValue();
                    matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex],newStateIndexMap[entry.getColumn()],entry.getValue());
                }
                if(this->computeRewards){
                    // the missing probability always leads to target
                    if(!this->parametricTypeComparator.isZero(missingProbability)){
                        matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], targetState,missingProbability);
                    }
                } else{
                    //transition to target state
                    if(!this->parametricTypeComparator.isZero(oneStepProbabilities[oldStateIndex])){
                        missingProbability-=oneStepProbabilities[oldStateIndex];
                        matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], targetState, oneStepProbabilities[oldStateIndex]);
                    }
                    //transition to sink state
                    if(!this->parametricTypeComparator.isZero(missingProbability)){ 
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
            if(this->computeRewards){
                storm::utility::vector::selectVectorValues(stateRewards.get(), subsystem);
                stateRewards->push_back(storm::utility::zero<ParametricType>()); //target state
                stateRewards->push_back(storm::utility::zero<ParametricType>()); //sink state
            }
            boost::optional<storm::storage::SparseMatrix<ParametricType>> noTransitionRewards;  
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;            
            // the final model
            this->simplifiedModel = std::make_shared<storm::models::sparse::Dtmc<ParametricType>>(matrixBuilder.build(), std::move(labeling), std::move(stateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
            std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
            this->timePreprocessing = timePreprocessingEnd - timePreprocessingStart;  
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::preprocessForProbabilities(storm::storage::BitVector& maybeStates, storm::storage::BitVector& targetStates) {
            this->computeRewards=false;
            //Get bounds, comparison type, target states
            storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = this->specifiedFormula->asProbabilityOperatorFormula();
            this->specifiedFormulaCompType=probabilityOperatorFormula.getComparisonType();
            this->specifiedFormulaBound=probabilityOperatorFormula.getBound();
            std::unique_ptr<CheckResult> targetStatesResultPtr=this->eliminationModelChecker.check(probabilityOperatorFormula.getSubformula().asEventuallyFormula().getSubformula());
            targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                
            //maybeStates: Compute the subset of states that have a probability of 0 or 1, respectively and reduce the considered states accordingly.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->model, storm::storage::BitVector(this->model.getNumberOfStates(),true), targetStates);
            maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
            // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
            storm::storage::sparse::state_type initialState = *this->model.getInitialStates().begin();
            if (!maybeStates.get(initialState)) {
                STORM_LOG_WARN("The probability of the initial state is constant (zero or one)");
                this->reachabilityFunction = std::make_shared<ParametricType>(statesWithProbability01.first.get(initialState) ? storm::utility::zero<ParametricType>() : storm::utility::one<ParametricType>());
                this->isResultConstant=true;
                return; //nothing else to do...
            }
            //extend target states
            targetStates=statesWithProbability01.second;
        }
        
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::preprocessForRewards(storm::storage::BitVector& maybeStates, storm::storage::BitVector& targetStates, std::vector<ParametricType>& stateRewards) {
            this->computeRewards=true;
            STORM_LOG_THROW(this->model.hasStateRewards() || this->model.hasTransitionRewards(), storm::exceptions::InvalidArgumentException, "Input model does not have a reward model.");
            //Get bounds, comparison type, target states
            storm::logic::RewardOperatorFormula const& rewardOperatorFormula = this->specifiedFormula->asRewardOperatorFormula();
            this->specifiedFormulaCompType=rewardOperatorFormula.getComparisonType();
            this->specifiedFormulaBound=rewardOperatorFormula.getBound();
            std::unique_ptr<CheckResult> targetStatesResultPtr=this->eliminationModelChecker.check(rewardOperatorFormula.getSubformula().asReachabilityRewardFormula().getSubformula());
            targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                
            //maybeStates: Compute the subset of states that has a reachability reward less than infinity.
            storm::storage::BitVector statesWithProbability1 = storm::utility::graph::performProb1(this->model.getBackwardTransitions(), storm::storage::BitVector(this->model.getNumberOfStates(), true), targetStates);
            maybeStates = ~targetStates & statesWithProbability1;
            // If the initial state is known to have 0 reward or an infinite reward value, we can directly set the reachRewardFunction.
            storm::storage::sparse::state_type initialState = *this->model.getInitialStates().begin();
            if (!maybeStates.get(initialState)) {
                STORM_LOG_WARN("The expected reward of the initial state is constant (infinity or zero)");
                this->reachabilityFunction = std::make_shared<ParametricType>(statesWithProbability1.get(initialState) ? storm::utility::zero<ParametricType>() : storm::utility::infinity<ParametricType>());
                this->isResultConstant=true;
                return; //nothing else to do...
            }
            
            stateRewards=std::vector<ParametricType>(maybeStates.getNumberOfSetBits());
            if (this->model.hasTransitionRewards()) {
                // If a transition-based reward model is available, we initialize the state reward vector to the row sums of the matrix
                // that results from taking the pointwise product of the transition probability matrix and the transition reward matrix.
                std::vector<ParametricType> pointwiseProductRowSumVector = this->model.getTransitionMatrix().getPointwiseProductRowSumVector(this->model.getTransitionRewardMatrix());
                storm::utility::vector::selectVectorValues(stateRewards, maybeStates, pointwiseProductRowSumVector);
                
                if (this->model.hasStateRewards()) {
                    // If a state-based reward model is also available, we need to add this vector
                    // as well. As the state reward vector contains entries not just for the states
                    // that we still consider (i.e. maybeStates), we need to extract these values
                    // first.
                    std::vector<ParametricType> subStateRewards(stateRewards.size());
                    storm::utility::vector::selectVectorValues(subStateRewards, maybeStates, this->model.getStateRewardVector());
                    storm::utility::vector::addVectors(stateRewards, subStateRewards, stateRewards);
                }
            } else {
                // If only a state-based reward model is  available, we take this vector as the
                // right-hand side. As the state reward vector contains entries not just for the
                // states that we still consider (i.e. maybeStates), we need to extract these values
                // first.
                storm::utility::vector::selectVectorValues(stateRewards, maybeStates, this->model.getStateRewardVector());
            }
            for(auto& stateReward: stateRewards){
                storm::utility::simplify(stateReward);
            }
        }



        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::initializeApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& simpleModel) {
            std::chrono::high_resolution_clock::time_point timeInitApproxModelStart = std::chrono::high_resolution_clock::now();
            this->approximationModel=std::make_shared<ApproximationModel>(simpleModel, this->computeRewards);
            std::chrono::high_resolution_clock::time_point timeInitApproxModelEnd = std::chrono::high_resolution_clock::now();
            this->timeInitApproxModel=timeInitApproxModelEnd - timeInitApproxModelStart;
            STORM_LOG_DEBUG("Initialized Approximation Model");
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::initializeSamplingModel(storm::models::sparse::Dtmc<ParametricType> const& simpleModel) {
            std::chrono::high_resolution_clock::time_point timeInitSamplingModelStart = std::chrono::high_resolution_clock::now();
            this->samplingModel=std::make_shared<SamplingModel>(simpleModel, this->computeRewards);
            std::chrono::high_resolution_clock::time_point timeInitSamplingModelEnd = std::chrono::high_resolution_clock::now();
            this->timeInitSamplingModel = timeInitSamplingModelEnd - timeInitSamplingModelStart;
            STORM_LOG_DEBUG("Initialized Sampling Model");
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::computeReachabilityFunction(storm::models::sparse::Dtmc<ParametricType> const& simpleModel){
            std::chrono::high_resolution_clock::time_point timeComputeReachabilityFunctionStart = std::chrono::high_resolution_clock::now();
            //get the one step probabilities and the transition matrix of the simplified model without target/sink state
            //TODO check if this takes long... we could also store the oneSteps while creating the simplified model. Or(?) we keep the matrix the way it is and give the target state one step probability 1.
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
            boost::optional<std::vector<ParametricType>> stateRewards;
            if(this->computeRewards){
                std::vector<ParametricType> stateRewardsAsVector(maybeStates.getNumberOfSetBits());
                storm::utility::vector::selectVectorValues(stateRewardsAsVector, maybeStates, simpleModel.getStateRewardVector());
                stateRewards=std::move(stateRewardsAsVector);
            }
            std::vector<std::size_t> statePriorities = this->eliminationModelChecker.getStatePriorities(forwardTransitions,backwardTransitions,newInitialStates,oneStepProbabilities);
            this->reachabilityFunction=std::make_shared<ParametricType>(this->eliminationModelChecker.computeReachabilityValue(forwardTransitions, oneStepProbabilities, backwardTransitions, newInitialStates , phiStates, simpleModel.getStates("target"), stateRewards, statePriorities));
               /* std::string funcStr = " (/ " +
                                this->reachabilityFunction->nominator().toString(false, true) + " " +
                                this->reachabilityFunction->denominator().toString(false, true) +
                            " )";
                std::cout << std::endl <<"the resulting reach prob function is " << std::endl << funcStr << std::endl << std::endl;*/
            STORM_LOG_DEBUG("Computed reachabilityFunction");
            std::chrono::high_resolution_clock::time_point timeComputeReachabilityFunctionEnd = std::chrono::high_resolution_clock::now();
            this->timeComputeReachabilityFunction = timeComputeReachabilityFunctionEnd-timeComputeReachabilityFunctionStart;
        }
                    
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegion(ParameterRegion& region) {
            std::chrono::high_resolution_clock::time_point timeCheckRegionStart = std::chrono::high_resolution_clock::now();
            ++this->numOfCheckedRegions;
            
            STORM_LOG_THROW(this->specifiedFormula!=nullptr, storm::exceptions::InvalidStateException, "Tried to analyze a region although no property has been specified" );
            STORM_LOG_DEBUG("Analyzing the region " << region.toString());
            //std::cout << "Analyzing the region " << region.toString() << std::endl;
            
            //switches for the different steps.
            bool done=false;
            STORM_LOG_WARN_COND( (!storm::settings::regionSettings().doApprox() || this->isApproximationApplicable), "the approximation is only correct if the model has only linear functions. As this is not the case, approximation is deactivated");
            bool doApproximation=storm::settings::regionSettings().doApprox() && this->isApproximationApplicable;
            bool doSampling=storm::settings::regionSettings().doSample();
            bool doFullSmt=storm::settings::regionSettings().doSmt();
            
            if(!done && this->isResultConstant){
                STORM_LOG_DEBUG("Checking a region although the result is constant, i.e., independent of the region. This makes sense none.");
                STORM_LOG_THROW(this->parametricTypeComparator.isConstant(*getReachabilityFunction()), storm::exceptions::UnexpectedException, "The result was assumed to be constant but it isn't.");
                if(valueIsInBoundOfFormula(storm::utility::regions::getConstantPart(*getReachabilityFunction()))){
                    region.setCheckResult(RegionCheckResult::ALLSAT);
                }
                else{
                    region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                }
                done=true;
            }
            
            std::chrono::high_resolution_clock::time_point timeApproximationStart = std::chrono::high_resolution_clock::now();
            std::vector<ConstantType> lowerBounds;
            std::vector<ConstantType> upperBounds;
            if(!done && doApproximation){
                STORM_LOG_DEBUG("Checking approximative values...");
                if(checkApproximativeValues(region, lowerBounds, upperBounds)){
                    ++this->numOfRegionsSolvedThroughApproximation;
                    STORM_LOG_DEBUG("Result '" << region.checkResultToString() <<"' obtained through approximation.");
                    done=true;
                }
            }
            std::chrono::high_resolution_clock::time_point timeApproximationEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point timeSamplingStart = std::chrono::high_resolution_clock::now();
            if(!done && doSampling){
                STORM_LOG_DEBUG("Checking sample points...");
                if(checkSamplePoints(region)){
                    ++this->numOfRegionsSolvedThroughSampling;
                    STORM_LOG_DEBUG("Result '" << region.checkResultToString() <<"' obtained through sampling.");
                    done=true;
                }
            }
            std::chrono::high_resolution_clock::time_point timeSamplingEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point timeFullSmtStart = std::chrono::high_resolution_clock::now();
            if(!done && doFullSmt){
                STORM_LOG_DEBUG("Checking with Smt Solving...");
                if(checkFullSmt(region)){
                    ++this->numOfRegionsSolvedThroughFullSmt;
                    STORM_LOG_DEBUG("Result '" << region.checkResultToString() <<"' obtained through Smt Solving.");
                    done=true;
                }
            }
            std::chrono::high_resolution_clock::time_point timeFullSmtEnd = std::chrono::high_resolution_clock::now();
            
            //some information for statistics...
            std::chrono::high_resolution_clock::time_point timeCheckRegionEnd = std::chrono::high_resolution_clock::now();
            this->timeCheckRegion += timeCheckRegionEnd-timeCheckRegionStart;
            this->timeSampling += timeSamplingEnd - timeSamplingStart;
            this->timeApproximation += timeApproximationEnd - timeApproximationStart;
            this->timeFullSmt += timeFullSmtEnd - timeFullSmtStart;
            switch(region.getCheckResult()){
                case RegionCheckResult::EXISTSBOTH:
                    ++this->numOfRegionsExistsBoth;
                    break;
                case RegionCheckResult::ALLSAT:
                    ++this->numOfRegionsAllSat;
                    break;
                case RegionCheckResult::ALLVIOLATED:
                    ++this->numOfRegionsAllViolated;
                    break;
                default:
                    break;
            }
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegions(std::vector<ParameterRegion>& regions) {
            STORM_LOG_DEBUG("Checking " << regions.size() << "regions.");
            std::cout << "Checking " << regions.size() << " regions. Progress: ";
            std::cout.flush();
                    
            uint_fast64_t progress=0;
            uint_fast64_t checkedRegions=0;
            for(auto& region : regions){
                this->checkRegion(region);
                if((checkedRegions++)*10/regions.size()==progress){
                    std::cout << progress++;
                    std::cout.flush();
                }
            }
            std::cout << " done!" << std::endl;
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkApproximativeValues(ParameterRegion& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds) {
            STORM_LOG_THROW(this->isApproximationApplicable, storm::exceptions::UnexpectedException, "Tried to perform approximative method (only applicable if all functions are linear), but there are nonlinear functions.");
            std::chrono::high_resolution_clock::time_point timeMDPBuildStart = std::chrono::high_resolution_clock::now();
            getApproximationModel()->instantiate(region);
            std::chrono::high_resolution_clock::time_point timeMDPBuildEnd = std::chrono::high_resolution_clock::now();
            this->timeMDPBuild += timeMDPBuildEnd-timeMDPBuildStart;
            
            // Decide whether the formula has an upper or a lower bond ({<, <=} or {>, >=}) and whether to prove allsat or allviolated. (Hence, there are 4 cases)
            bool formulaHasUpperBound = this->specifiedFormulaCompType==storm::logic::ComparisonType::Less || this->specifiedFormulaCompType==storm::logic::ComparisonType::LessEqual;
            STORM_LOG_THROW((formulaHasUpperBound != (this->specifiedFormulaCompType==storm::logic::ComparisonType::Greater || this->specifiedFormulaCompType==storm::logic::ComparisonType::GreaterEqual)),
                    storm::exceptions::UnexpectedException, "Unexpected comparison Type of formula");
            bool proveAllSat;
            switch (region.getCheckResult()){
                case RegionCheckResult::UNKNOWN: 
                    switch(storm::settings::regionSettings().getApproxMode()){
                        case storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST:
                            //Sample a single point to know whether we should try to prove ALLSAT or ALLVIOLATED
                            checkPoint(region,region.getLowerBounds(), false);
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
            if((formulaHasUpperBound && proveAllSat) || (!formulaHasUpperBound && !proveAllSat)){
                //these are the cases in which we need to compute upper bounds
                upperBounds = getApproximationModel()->computeValues(storm::logic::OptimalityType::Maximize);
                lowerBounds = std::vector<ConstantType>();
                formulaSatisfied = valueIsInBoundOfFormula(upperBounds[*getApproximationModel()->getModel()->getInitialStates().begin()]);
            }
            else{
                //for the remaining cases we compute lower bounds
                lowerBounds = getApproximationModel()->computeValues(storm::logic::OptimalityType::Minimize);
                upperBounds = std::vector<ConstantType>();
                formulaSatisfied = valueIsInBoundOfFormula(lowerBounds[*getApproximationModel()->getModel()->getInitialStates().begin()]);
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
                    lowerBounds = getApproximationModel()->computeValues(storm::logic::OptimalityType::Minimize);
                    formulaSatisfied=valueIsInBoundOfFormula(lowerBounds[*getApproximationModel()->getModel()->getInitialStates().begin()]);
                }
                else{
                    upperBounds = getApproximationModel()->computeValues(storm::logic::OptimalityType::Maximize);
                    formulaSatisfied=valueIsInBoundOfFormula(upperBounds[*getApproximationModel()->getModel()->getInitialStates().begin()]);
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
        
        template<typename ParametricType, typename ConstantType>
        std::shared_ptr<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::getApproximationModel() {
            if(this->approximationModel==nullptr){
                STORM_LOG_WARN("Approximation model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                initializeApproximationModel(*this->simplifiedModel);
            }
            return this->approximationModel;
        }
        
              template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkSamplePoints(ParameterRegion& region) {
            auto samplingPoints = region.getVerticesOfRegion(region.getVariables()); //test the 4 corner points
            for (auto const& point : samplingPoints){
                if(checkPoint(region, point)){
                    return true;
                }            
            }
            return false;
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkPoint(ParameterRegion& region, std::map<VariableType, CoefficientType>const& point, bool favorViaFunction) {
            bool valueInBoundOfFormula;
            if((storm::settings::regionSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::EVALUATE) ||
                    (!storm::settings::regionSettings().doSample() && favorViaFunction)){
                valueInBoundOfFormula = this->valueIsInBoundOfFormula(storm::utility::regions::evaluateFunction(*getReachabilityFunction(), point));
            }
            else{
                getSamplingModel()->instantiate(point);
                valueInBoundOfFormula=this->valueIsInBoundOfFormula(getSamplingModel()->computeValues()[*getSamplingModel()->getModel()->getInitialStates().begin()]);
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
         
        template<typename ParametricType, typename ConstantType>
        std::shared_ptr<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::getSamplingModel() {
            if(this->samplingModel==nullptr){
                STORM_LOG_WARN("Sampling model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                initializeSamplingModel(*this->simplifiedModel);
            }
            return this->samplingModel;
        }
        
        template<typename ParametricType, typename ConstantType>
        std::shared_ptr<ParametricType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::getReachabilityFunction() {
            if(this->reachabilityFunction==nullptr){
                STORM_LOG_WARN("Reachability Function requested but it has not been computed when specifying the formula. Will compute it now.");
                computeReachabilityFunction(*this->simplifiedModel);
            }
            return this->reachabilityFunction;
        }

        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkFullSmt(ParameterRegion& region) {
            STORM_LOG_THROW((storm::settings::regionSettings().getSmtMode()==storm::settings::modules::RegionSettings::SmtMode::FUNCTION), storm::exceptions::NotImplementedException, "Selected SMT mode has not been implemented.");
            if (region.getCheckResult()==RegionCheckResult::UNKNOWN){
                //Sampling needs to be done (on a single point)
                checkPoint(region,region.getLowerBounds(), true);
            }
            
            if(this->smtSolver==nullptr){
                initializeSMTSolver();
            }
            
            this->smtSolver->push();
            
            //add constraints for the region
            for(auto const& variable : region.getVariables()) {
                storm::utility::regions::addParameterBoundsToSmtSolver(this->smtSolver, variable, storm::logic::ComparisonType::GreaterEqual, region.getLowerBound(variable));
                storm::utility::regions::addParameterBoundsToSmtSolver(this->smtSolver, variable, storm::logic::ComparisonType::LessEqual, region.getUpperBound(variable));
            }
            
            //add constraint that states what we want to prove            
            VariableType proveAllSatVar=storm::utility::regions::getVariableFromString<VariableType>("storm_proveAllSat");     
            VariableType proveAllViolatedVar=storm::utility::regions::getVariableFromString<VariableType>("storm_proveAllViolated");     
            switch(region.getCheckResult()){
                case RegionCheckResult::EXISTSBOTH:
                    STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::EXISTSBOTH), "checkFullSmt invoked although the result is already clear (EXISTSBOTH). Will validate this now...");
                case RegionCheckResult::ALLSAT:
                    STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::ALLSAT), "checkFullSmt invoked although the result is already clear (ALLSAT). Will validate this now...");
                case RegionCheckResult::EXISTSSAT:
                    storm::utility::regions::addBoolVariableToSmtSolver(this->smtSolver, proveAllSatVar, true);
                    storm::utility::regions::addBoolVariableToSmtSolver(this->smtSolver, proveAllViolatedVar, false);
                    break;
                case RegionCheckResult::ALLVIOLATED:
                    STORM_LOG_WARN_COND((region.getCheckResult()!=RegionCheckResult::ALLVIOLATED), "checkFullSmt invoked although the result is already clear (ALLVIOLATED). Will validate this now...");
                case RegionCheckResult::EXISTSVIOLATED:
                    storm::utility::regions::addBoolVariableToSmtSolver(this->smtSolver, proveAllSatVar, false);
                    storm::utility::regions::addBoolVariableToSmtSolver(this->smtSolver, proveAllViolatedVar, true);
                    break;
                default:
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not handle the current region CheckResult: " << region.checkResultToString());
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
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::initializeSMTSolver() {
                    
            storm::expressions::ExpressionManager manager; //this manager will do nothing as we will use carl expressions..
            this->smtSolver = std::shared_ptr<storm::solver::Smt2SmtSolver>(new storm::solver::Smt2SmtSolver(manager, true));
            
            ParametricType bound= storm::utility::regions::convertNumber<double, ParametricType>(this->specifiedFormulaBound);
            
            // To prove that the property is satisfied in the initial state for all parameters,
            // we ask the solver whether the negation of the property is satisfiable and invert the answer.
            // In this case, assert that this variable is true:
            VariableType proveAllSatVar=storm::utility::regions::getNewVariable<VariableType>("storm_proveAllSat", storm::utility::regions::VariableSort::VS_BOOL);
            
            //Example:
            //Property:    P<=p [ F 'target' ] holds iff...
            // f(x)         <= p
            // Hence: If  f(x) > p is unsat, the property is satisfied for all parameters.
            
            storm::logic::ComparisonType proveAllSatRel; //the relation from the property needs to be inverted
            switch (this->specifiedFormulaCompType) {
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
            storm::utility::regions::addGuardedConstraintToSmtSolver(this->smtSolver, proveAllSatVar, *getReachabilityFunction(), proveAllSatRel, bound);
            
            // To prove that the property is violated in the initial state for all parameters,
            // we ask the solver whether the the property is satisfiable and invert the answer.
            // In this case, assert that this variable is true:
            VariableType proveAllViolatedVar=storm::utility::regions::getNewVariable<VariableType>("storm_proveAllViolated", storm::utility::regions::VariableSort::VS_BOOL);        
            
            //Example:
            //Property:    P<=p [ F 'target' ] holds iff...
            // f(x)         <= p
            // Hence: If f(x)  <= p is unsat, the property is violated for all parameters. 
            storm::logic::ComparisonType proveAllViolatedRel = this->specifiedFormulaCompType;
            storm::utility::regions::addGuardedConstraintToSmtSolver(this->smtSolver, proveAllViolatedVar, *getReachabilityFunction(), proveAllViolatedRel, bound);          
        }

        template<typename ParametricType, typename ConstantType>
        template<typename ValueType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::valueIsInBoundOfFormula(ValueType value){
            STORM_LOG_THROW(this->specifiedFormula!=nullptr, storm::exceptions::InvalidStateException, "Tried to compare a value to the bound of a formula, but no formula specified.");
            double valueAsDouble = storm::utility::regions::convertNumber<ValueType, double>(value);
            switch (this->specifiedFormulaCompType) {
                case storm::logic::ComparisonType::Greater:
                    return (valueAsDouble > this->specifiedFormulaBound);
                case storm::logic::ComparisonType::GreaterEqual:
                    return (valueAsDouble >= this->specifiedFormulaBound);
                case storm::logic::ComparisonType::Less:
                    return (valueAsDouble < this->specifiedFormulaBound);
                case storm::logic::ComparisonType::LessEqual:
                    return (valueAsDouble <= this->specifiedFormulaBound);
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
            }
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::printStatisticsToStream(std::ostream& outstream) {
            
            if(this->specifiedFormula==nullptr){
                outstream << "Region Model Checker Statistics Error: No formula specified." << std::endl; 
                return;
            }
            
            std::chrono::milliseconds timeSpecifyFormulaInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSpecifyFormula);
            std::chrono::milliseconds timePreprocessingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timePreprocessing);
            std::chrono::milliseconds timeInitSamplingModelInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitSamplingModel);
            std::chrono::milliseconds timeInitApproxModelInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitApproxModel);
            std::chrono::milliseconds timeComputeReachabilityFunctionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeComputeReachabilityFunction);
            std::chrono::milliseconds timeCheckRegionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeCheckRegion);
            std::chrono::milliseconds timeSammplingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSampling);
            std::chrono::milliseconds timeApproximationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeApproximation);
            std::chrono::milliseconds timeMDPBuildInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeMDPBuild);
            std::chrono::milliseconds timeFullSmtInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeFullSmt);
            
            std::chrono::high_resolution_clock::duration timeOverall = timeSpecifyFormula + timeCheckRegion; // + ...
            std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);
            
            uint_fast64_t numOfSolvedRegions= this->numOfRegionsExistsBoth + this->numOfRegionsAllSat + this->numOfRegionsAllViolated;
            
            outstream << std::endl << "Region Model Checker Statistics:" << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            outstream << "Model: " << this->model.getNumberOfStates() << " states, " << this->model.getNumberOfTransitions() << " transitions." << std::endl;
            outstream << "Formula: " << *this->specifiedFormula << std::endl;
            if(this->isResultConstant){
                outstream << "The requested value is constant (i.e. independent of any parameters)" << std::endl;
            }
            else{
                outstream << "Simplified model: " << this->simplifiedModel->getNumberOfStates() << " states, " << this->simplifiedModel->getNumberOfTransitions() << " transitions" << std::endl;
            }
            outstream << "Approximation is " << (this->isApproximationApplicable ? "" : "not ") << "applicable" << std::endl;
            outstream << "Number of checked regions: " << this->numOfCheckedRegions << std::endl;
            outstream << "  Number of solved regions:  " <<  numOfSolvedRegions << "(" << numOfSolvedRegions*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    AllSat:      " <<  this->numOfRegionsAllSat << "(" << this->numOfRegionsAllSat*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    AllViolated: " <<  this->numOfRegionsAllViolated << "(" << this->numOfRegionsAllViolated*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    ExistsBoth:  " <<  this->numOfRegionsExistsBoth << "(" << this->numOfRegionsExistsBoth*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    Unsolved:    " <<  this->numOfCheckedRegions - numOfSolvedRegions << "(" << (this->numOfCheckedRegions - numOfSolvedRegions)*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "  --  Note: %-numbers are relative to the NUMBER of regions, not the size of their area --" <<  std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughApproximation << " regions solved through Approximation" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughSampling << " regions solved through Sampling" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughFullSmt << " regions solved through FullSmt" << std::endl;
            outstream << std::endl;
            outstream << "Running times:" << std::endl;
            outstream << "  " << timeOverallInMilliseconds.count() << "ms overall (excluding model parsing, bisimulation (if applied))" << std::endl;
            outstream << "  " << timeSpecifyFormulaInMilliseconds.count() << "ms Initialization for the specified formula, including... " << std::endl;
            outstream << "    " << timePreprocessingInMilliseconds.count() << "ms for Preprocessing (mainly to obtain a simplified model, i.e., state elimination of const transitions)" << std::endl;
            outstream << "    " << timeInitApproxModelInMilliseconds.count() << "ms to initialize the Approximation Model" << std::endl;
            outstream << "    " << timeInitSamplingModelInMilliseconds.count() << "ms to initialize the Sampling Model" << std::endl;
            outstream << "    " << timeComputeReachabilityFunctionInMilliseconds.count() << "ms to compute the reachability function" << std::endl;
            outstream << "  " << timeCheckRegionInMilliseconds.count() << "ms Region Check including... " << std::endl;
            outstream << "    " << timeApproximationInMilliseconds.count() << "ms Approximation including... " << std::endl;
            outstream << "      " << timeMDPBuildInMilliseconds.count() << "ms to build the MDP" << std::endl;
            outstream << "    " << timeSammplingInMilliseconds.count() << "ms Sampling " << std::endl;
            outstream << "    " << timeFullSmtInMilliseconds.count() << "ms Smt solving" << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            
        }
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>;
#endif
        //note: for other template instantiations, add rules for the typedefs of VariableType and CoefficientType in utility/regions.h
        
    } // namespace modelchecker
} // namespace storm
