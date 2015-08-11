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

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/UnexpectedException.h"


namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model) : 
                model(model),
                eliminationModelChecker(model){
            //intentionally left empty
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::canHandle(storm::logic::Formula const& formula) const {
             //for simplicity we only support state formulas with eventually (e.g. P<0.5 [ F "target" ])
            if(!formula.isStateFormula()){
                STORM_LOG_DEBUG("Region Model Checker could not handle formula " << formula << " Reason: expected a stateFormula");
                return false;
            }
            if(!formula.asStateFormula().isProbabilityOperatorFormula()){
                STORM_LOG_DEBUG("Region Model Checker could not handle formula " << formula << " Reason: expected a probabilityOperatorFormula");
                return false;
            }
            storm::logic::ProbabilityOperatorFormula const& probOpForm=formula.asStateFormula().asProbabilityOperatorFormula();
            if(!probOpForm.hasBound()){
                STORM_LOG_DEBUG("Region Model Checker could not handle formula " << formula << " Reason: The formula has no bound");
                return false;
            }
            if(!probOpForm.getSubformula().asPathFormula().isEventuallyFormula()){
                STORM_LOG_DEBUG("Region Model Checker could not handle formula " << formula << " Reason: expected an eventually subformula");
                return false;
            }
            if(model.getInitialStates().getNumberOfSetBits() != 1){
                STORM_LOG_DEBUG("Input model is required to have exactly one initial state.");
                return false;
            }
            return true;
        }
            
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::specifyFormula(storm::logic::Formula const& formula) {
            std::chrono::high_resolution_clock::time_point timePreprocessingStart = std::chrono::high_resolution_clock::now();
            STORM_LOG_THROW(this->canHandle(formula), storm::exceptions::IllegalArgumentException, "Tried to specify a formula that can not be handled.");
            
            this->hasOnlyLinearFunctions=false;
            this->isReachProbFunctionComputed=false;
            this->isResultConstant=false;
            this->smtSolver=nullptr;
            
            //Get subformula, target states
            //Note: canHandle already ensures that the formula has the right shape and that the model has a single initial state.
            this->probabilityOperatorFormula = std::unique_ptr<storm::logic::ProbabilityOperatorFormula>(new storm::logic::ProbabilityOperatorFormula(formula.asStateFormula().asProbabilityOperatorFormula()));
            storm::logic::EventuallyFormula const& eventuallyFormula = this->probabilityOperatorFormula->getSubformula().asPathFormula().asEventuallyFormula();
            std::unique_ptr<CheckResult> targetStatesResultPtr = this->eliminationModelChecker.check(eventuallyFormula.getSubformula());
            storm::storage::BitVector const& targetStates = targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector();
            
            // get a more simple model with a single target state, a single sink state and where states with constant outgoing transitions have been removed
            // Note: also checks for linear functions and a constant result
            computeSimplifiedModel(targetStates);
            //now create the models used for sampling and approximation
            this->samplingModel=std::make_shared<SamplingModel>(*this->simplifiedModel);
            this->approximationModel=std::make_shared<ApproximationModel>(*this->simplifiedModel);
           
            //some information for statistics...
            std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
            this->timePreprocessing= timePreprocessingEnd - timePreprocessingStart;
            this->numOfCheckedRegions=0;
            this->numOfRegionsSolvedThroughSampling=0;
            this->numOfRegionsSolvedThroughApproximation=0;
            this->numOfRegionsSolvedThroughSubsystemSmt=0;
            this->numOfRegionsSolvedThroughFullSmt=0;
            this->numOfRegionsExistsBoth=0;
            this->numOfRegionsAllSat=0;
            this->numOfRegionsAllViolated=0;
            this->timeCheckRegion=std::chrono::high_resolution_clock::duration::zero();
            this->timeSampling=std::chrono::high_resolution_clock::duration::zero();
            this->timeApproximation=std::chrono::high_resolution_clock::duration::zero();
            this->timeMDPBuild=std::chrono::high_resolution_clock::duration::zero();
            this->timeSubsystemSmt=std::chrono::high_resolution_clock::duration::zero();
            this->timeFullSmt=std::chrono::high_resolution_clock::duration::zero();
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::computeSimplifiedModel(storm::storage::BitVector const& targetStates){
                        
            //Compute the subset of states that have a probability of 0 or 1, respectively and reduce the considered states accordingly.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->model, storm::storage::BitVector(this->model.getNumberOfStates(),true), targetStates);
            storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
            storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
            if (this->model.getInitialStates().isDisjointFrom(maybeStates)) {
                STORM_LOG_WARN("The probability of the initial state is constant (0 or 1)");
                this->reachProbFunction = statesWithProbability0.get(*(this->model.getInitialStates().begin())) ? storm::utility::zero<ParametricType>() : storm::utility::one<ParametricType>();
                this->isReachProbFunctionComputed=true;
                this->isResultConstant=true;
                this->hasOnlyLinearFunctions=true;
            }
            // Determine the set of states that is reachable from the initial state without jumping over a target state.
            storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->model.getTransitionMatrix(), this->model.getInitialStates(), maybeStates, statesWithProbability1);
            // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
            maybeStates &= reachableStates;
            // Create a vector for the probabilities to go to a state with probability 1 in one step.
            std::vector<ParametricType> oneStepProbabilities = this->model.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
            // Determine the initial state of the sub-model.
            storm::storage::sparse::state_type initialState = *(this->model.getInitialStates() % maybeStates).begin();
            // We then build the submatrix that only has the transitions of the maybe states.
            storm::storage::SparseMatrix<ParametricType> submatrix = this->model.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
            
            // eliminate all states with only constant outgoing transitions
            //TODO: maybe also states with constant incoming tranistions. THEN the ordering of the eliminated states does matter.
            std::chrono::high_resolution_clock::time_point timeInitialStateEliminationStart = std::chrono::high_resolution_clock::now();
            // Convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            auto flexibleTransitions = this->eliminationModelChecker.getFlexibleSparseMatrix(submatrix);
            auto flexibleBackwardTransitions= this->eliminationModelChecker.getFlexibleSparseMatrix(submatrix.transpose(), true);
            // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
            storm::storage::BitVector subsystem = storm::storage::BitVector(submatrix.getRowCount(), true);
            //temporarily unselect the initial state to skip it.
            subsystem.set(initialState, false);
            this->hasOnlyLinearFunctions=true;
            bool allReachableFunctionsAreConstant=true;
            boost::optional<std::vector<ParametricType>> missingStateRewards;
            for (auto const& state : subsystem) {
                bool stateHasOnlyConstantOutgoingTransitions=true;
                for(auto const& entry : submatrix.getRow(state)){
                    if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                        allReachableFunctionsAreConstant=false;
                        stateHasOnlyConstantOutgoingTransitions=false;
                        this->hasOnlyLinearFunctions &= storm::utility::regions::functionIsLinear<ParametricType>(entry.getValue());
                    }
                }
                if(!this->parametricTypeComparator.isConstant(oneStepProbabilities[state])){
                    allReachableFunctionsAreConstant=false;
                    stateHasOnlyConstantOutgoingTransitions=false;
                    this->hasOnlyLinearFunctions &= storm::utility::regions::functionIsLinear<ParametricType>(oneStepProbabilities[state]);
                }
                if(stateHasOnlyConstantOutgoingTransitions){
                    this->eliminationModelChecker.eliminateState(flexibleTransitions, oneStepProbabilities, state, flexibleBackwardTransitions, missingStateRewards);
                    subsystem.set(state,false);
                }
            }
            subsystem.set(initialState, true);
            std::chrono::high_resolution_clock::time_point timeInitialStateEliminationEnd = std::chrono::high_resolution_clock::now();
            this->timeInitialStateElimination = timeInitialStateEliminationEnd-timeInitialStateEliminationStart;
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions." << std::endl);
            std::cout << "Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions." << std::endl;
            
            if(allReachableFunctionsAreConstant){
                //Check if this is also the case for the initial state
                for(auto const& entry : submatrix.getRow(initialState)){
                    allReachableFunctionsAreConstant&=this->parametricTypeComparator.isConstant(entry.getValue());
                }
                allReachableFunctionsAreConstant&=this->parametricTypeComparator.isConstant(oneStepProbabilities[initialState]);
                // Set the flag accordingly
                this->isResultConstant=allReachableFunctionsAreConstant;
                STORM_LOG_WARN_COND(!this->isResultConstant, "For the given property, the reachability probability is constant, i.e., independent of the region");
            }
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
                ParametricType valueToSinkState=storm::utility::regions::getNewFunction<ParametricType, CoefficientType>(storm::utility::one<CoefficientType>());
                //go through columns:
                for(auto const& entry: flexibleTransitions.getRow(oldStateIndex)){ 
                    STORM_LOG_THROW(newStateIndexMap[entry.getColumn()]!=flexibleTransitions.getNumberOfRows(), storm::exceptions::UnexpectedException, "There is a transition to a state that should have been eliminated.");
                    valueToSinkState-=entry.getValue();
                    matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex],newStateIndexMap[entry.getColumn()],entry.getValue());
                }
                //transition to target state
                if(!this->parametricTypeComparator.isZero(oneStepProbabilities[oldStateIndex])){ 
                    valueToSinkState-=oneStepProbabilities[oldStateIndex];
                    matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], targetState, oneStepProbabilities[oldStateIndex]);
                }
                //transition to sink state
                if(!this->parametricTypeComparator.isZero(valueToSinkState)){ 
                    matrixBuilder.addNextValue(newStateIndexMap[oldStateIndex], sinkState, valueToSinkState);
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
            boost::optional<std::vector<ParametricType>> noStateRewards;
            boost::optional<storm::storage::SparseMatrix<ParametricType>> noTransitionRewards;  
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;            
            // the final model
            this->simplifiedModel = std::make_shared<storm::models::sparse::Dtmc<ParametricType>>(matrixBuilder.build(), std::move(labeling), std::move(noStateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
        }
                    
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegion(ParameterRegion& region) {
            std::chrono::high_resolution_clock::time_point timeCheckRegionStart = std::chrono::high_resolution_clock::now();
            ++this->numOfCheckedRegions;
            
            STORM_LOG_THROW(this->probabilityOperatorFormula!=nullptr, storm::exceptions::InvalidStateException, "Tried to analyze a region although no property has been specified" );
            STORM_LOG_DEBUG("Analyzing the region " << region.toString());
            //std::cout << "Analyzing the region " << region.toString() << std::endl;
            
            //switches for the different steps.
            bool done=false;
            bool doApproximation=this->hasOnlyLinearFunctions; // this approach is only correct if the model has only linear functions
            bool doSampling=true;
            bool doSubsystemSmt=false; //this->hasOnlyLinearFunctions;  // this approach is only correct if the model has only linear functions
            bool doFullSmt=true;
            
            if(!done && this->isResultConstant){
                STORM_LOG_DEBUG("Checking a region although the result is constant, i.e., independent of the region. This makes sense none.");
                STORM_LOG_THROW(this->parametricTypeComparator.isConstant(getReachProbFunction()), storm::exceptions::UnexpectedException, "The result was assumed to be constant but it isn't.");
                if(valueIsInBoundOfFormula(storm::utility::regions::getConstantPart(getReachProbFunction()))){
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
                STORM_LOG_DEBUG("Checking approximative probabilities...");
                if (region.getCheckResult()==RegionCheckResult::UNKNOWN){
                    //Sample a single point to know whether we should try to prove ALLSAT or ALLVIOLATED
                    checkPoint(region,region.getLowerBounds(), false);
                }
                if(checkApproximativeProbabilities(region, lowerBounds, upperBounds)){
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
            
            std::chrono::high_resolution_clock::time_point timeSubsystemSmtStart = std::chrono::high_resolution_clock::now();
            if(!done && doSubsystemSmt){
                STORM_LOG_WARN("SubsystemSmt approach not yet implemented");
            }
            std::chrono::high_resolution_clock::time_point timeSubsystemSmtEnd = std::chrono::high_resolution_clock::now();
            
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
            this->timeSubsystemSmt += timeSubsystemSmtEnd - timeSubsystemSmtStart;
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
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkApproximativeProbabilities(ParameterRegion& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds) {
            STORM_LOG_THROW(this->hasOnlyLinearFunctions, storm::exceptions::UnexpectedException, "Tried to perform approximative method (only applicable if all functions are linear), but there are nonlinear functions.");
            std::chrono::high_resolution_clock::time_point timeMDPBuildStart = std::chrono::high_resolution_clock::now();
            this->approximationModel->instantiate(region);
            std::chrono::high_resolution_clock::time_point timeMDPBuildEnd = std::chrono::high_resolution_clock::now();
            this->timeMDPBuild += timeMDPBuildEnd-timeMDPBuildStart;
            
            // Decide whether we should minimize or maximize and whether to prove allsat or allviolated. (Hence, there are 4 cases)
            bool formulaHasUpperBound = this->probabilityOperatorFormula->getComparisonType()==storm::logic::ComparisonType::Less || this->probabilityOperatorFormula->getComparisonType()==storm::logic::ComparisonType::LessEqual;
            STORM_LOG_THROW((formulaHasUpperBound != (this->probabilityOperatorFormula->getComparisonType()==storm::logic::ComparisonType::Greater || this->probabilityOperatorFormula->getComparisonType()==storm::logic::ComparisonType::GreaterEqual)),
                    storm::exceptions::UnexpectedException, "Unexpected comparison Type of formula");
            switch (region.getCheckResult()){
                case RegionCheckResult::EXISTSSAT:
                case RegionCheckResult::UNKNOWN: 
                    //Try to prove ALLSAT
                    if(formulaHasUpperBound){ 
                        upperBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Maximize);
                        lowerBounds=std::vector<ConstantType>();
                        if(valueIsInBoundOfFormula(upperBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                            region.setCheckResult(RegionCheckResult::ALLSAT);
                            return true;
                        }
                    }
                    else{
                        lowerBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Minimize);
                        upperBounds=std::vector<ConstantType>();
                        if(valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                            region.setCheckResult(RegionCheckResult::ALLSAT);
                            return true;
                        }
                    }
                    break;
                case RegionCheckResult::EXISTSVIOLATED:
                    //Try to prove ALLVIOLATED
                    if(formulaHasUpperBound){ 
                        lowerBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Minimize);
                        upperBounds=std::vector<ConstantType>();
                        if(!valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                            region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                            return true;
                        }
                    }
                    else{
                        upperBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Maximize);
                        lowerBounds=std::vector<ConstantType>();
                        if(!valueIsInBoundOfFormula(upperBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                            region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                            return true;
                        }
                    }
                    break;
                default:
                     STORM_LOG_WARN("The checkresult of the current region should not be conclusive, i.e. it should be either EXISTSSAT or EXISTSVIOLATED or UNKNOWN in order to apply approximative probabilities");
            }
            
            if(region.getCheckResult()==RegionCheckResult::UNKNOWN){
                //In this case, it makes sense to try the other optimality Type. Again, 4 cases (Although the ALLSAT cases should not be reachable, since we already tried to prove this above).
                if(lowerBounds.empty()){
                    lowerBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Minimize);
                    if(!formulaHasUpperBound && valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                        return true;
                    }
                    if(formulaHasUpperBound && !valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                        return true;
                    }
                }
                if(upperBounds.empty()){
                    upperBounds = this->approximationModel->computeReachabilityProbabilities(storm::logic::OptimalityType::Maximize);
                    if(formulaHasUpperBound && valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                        return true;
                    }
                    if(!formulaHasUpperBound && !valueIsInBoundOfFormula(lowerBounds[*this->approximationModel->getModel()->getInitialStates().begin()])){
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                        return true;
                    }
                }
            }
            //if we reach this point than the result is still inconclusive.
            return false;            
        }
        
              template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkSamplePoints(ParameterRegion& region) {
            auto samplingPoints = region.getVerticesOfRegion(region.getVariables()); //test the 4 corner points
            for (auto const& point : samplingPoints){
                if(checkPoint(region, point, false)){
                    return true;
                }            
            }
            return false;
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkPoint(ParameterRegion& region, std::map<VariableType, CoefficientType>const& point, bool viaReachProbFunction) {
            bool valueInBoundOfFormula;
            if(viaReachProbFunction){
                valueInBoundOfFormula = this->valueIsInBoundOfFormula(storm::utility::regions::evaluateFunction(getReachProbFunction(), point));
            }
            else{
                this->samplingModel->instantiate(point);
                valueInBoundOfFormula=this->valueIsInBoundOfFormula(this->samplingModel->computeReachabilityProbabilities()[*this->samplingModel->getModel()->getInitialStates().begin()]);
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
        ParametricType SparseDtmcRegionModelChecker<ParametricType, ConstantType>::getReachProbFunction() {
            if(!this->isReachProbFunctionComputed){
                std::chrono::high_resolution_clock::time_point timeComputeReachProbFunctionStart = std::chrono::high_resolution_clock::now();
                //get the one step probabilities and the transition matrix of the simplified model without target/sink state
                //TODO check if this takes long... we could also store the oneSteps while creating the simplified model. Or(?) we keep the matrix the way it is and give the target state one step probability 1.
                storm::storage::SparseMatrix<ParametricType> backwardTransitions(this->simplifiedModel->getBackwardTransitions());
                std::vector<ParametricType> oneStepProbabilities(this->simplifiedModel->getNumberOfStates()-2, storm::utility::zero<ParametricType>());
                for(auto const& entry : backwardTransitions.getRow(*(this->simplifiedModel->getStates("target").begin()))){
                    if(entry.getColumn()<oneStepProbabilities.size()){
                        oneStepProbabilities[entry.getColumn()]=entry.getValue();
                    } //else case: only holds for the entry that corresponds to the selfloop on the target state..
                }
                storm::storage::BitVector maybeStates=~(this->simplifiedModel->getStates("target") | this->simplifiedModel->getStates("sink"));
                backwardTransitions=backwardTransitions.getSubmatrix(false,maybeStates,maybeStates);
                storm::storage::SparseMatrix<ParametricType> forwardTransitions=this->simplifiedModel->getTransitionMatrix().getSubmatrix(false,maybeStates,maybeStates);
                //now compute the functions using methods from elimination model checker
                storm::storage::BitVector newInitialStates = this->simplifiedModel->getInitialStates() % maybeStates;
                storm::storage::BitVector phiStates(this->simplifiedModel->getNumberOfStates(), true);
                boost::optional<std::vector<ParametricType>> noStateRewards;
                std::vector<std::size_t> statePriorities = this->eliminationModelChecker.getStatePriorities(forwardTransitions,backwardTransitions,newInitialStates,oneStepProbabilities);
                this->reachProbFunction=this->eliminationModelChecker.computeReachabilityValue(forwardTransitions, oneStepProbabilities, backwardTransitions, newInitialStates , phiStates, this->simplifiedModel->getStates("target"), noStateRewards, statePriorities);
                std::chrono::high_resolution_clock::time_point timeComputeReachProbFunctionEnd = std::chrono::high_resolution_clock::now();
            /*    std::string funcStr = " (/ " +
                                    this->reachProbFunction.nominator().toString(false, true) + " " +
                                    this->reachProbFunction.denominator().toString(false, true) +
                                " )";
                std::cout << std::endl <<"the resulting reach prob function is " << std::endl << funcStr << std::endl << std::endl;*/
                STORM_LOG_DEBUG("Computed reachProbFunction");
                this->isReachProbFunctionComputed=true;
                this->timeComputeReachProbFunction = timeComputeReachProbFunctionEnd-timeComputeReachProbFunctionStart;
            }
            return this->reachProbFunction;
        }

        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkFullSmt(ParameterRegion& region) {
            if (region.getCheckResult()==RegionCheckResult::UNKNOWN){
                //Sampling needs to be done (on a single point)
                checkPoint(region,region.getLowerBounds(), true);
            }
            
            if(this->smtSolver==nullptr){
                initializeSMTSolver(this->smtSolver, getReachProbFunction(),*this->probabilityOperatorFormula);
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
                        initializeSMTSolver(this->smtSolver,getReachProbFunction(), *this->probabilityOperatorFormula);
                    }
                    return false;
            }
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::initializeSMTSolver(std::shared_ptr<storm::solver::Smt2SmtSolver>& solver, const ParametricType& reachProbFunc, const storm::logic::ProbabilityOperatorFormula& formula) {
                    
            storm::expressions::ExpressionManager manager; //this manager will do nothing as we will use carl expressions..
            solver = std::shared_ptr<storm::solver::Smt2SmtSolver>(new storm::solver::Smt2SmtSolver(manager, true));
            
            ParametricType bound= storm::utility::regions::convertNumber<double, ParametricType>(this->probabilityOperatorFormula->getBound());
            
            // To prove that the property is satisfied in the initial state for all parameters,
            // we ask the solver whether the negation of the property is satisfiable and invert the answer.
            // In this case, assert that this variable is true:
            VariableType proveAllSatVar=storm::utility::regions::getNewVariable<VariableType>("storm_proveAllSat", storm::utility::regions::VariableSort::VS_BOOL);
            
            //Example:
            //Property:    P<=p [ F 'target' ] holds iff...
            // f(x)         <= p
            // Hence: If  f(x) > p is unsat, the property is satisfied for all parameters.
            
            storm::logic::ComparisonType proveAllSatRel; //the relation from the property needs to be inverted
            switch (this->probabilityOperatorFormula->getComparisonType()) {
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
            storm::utility::regions::addGuardedConstraintToSmtSolver(solver, proveAllSatVar, reachProbFunc, proveAllSatRel, bound);
            
            // To prove that the property is violated in the initial state for all parameters,
            // we ask the solver whether the the property is satisfiable and invert the answer.
            // In this case, assert that this variable is true:
            VariableType proveAllViolatedVar=storm::utility::regions::getNewVariable<VariableType>("storm_proveAllViolated", storm::utility::regions::VariableSort::VS_BOOL);        
            
            //Example:
            //Property:    P<=p [ F 'target' ] holds iff...
            // f(x)         <= p
            // Hence: If f(x)  <= p is unsat, the property is violated for all parameters. 
            storm::logic::ComparisonType proveAllViolatedRel = this->probabilityOperatorFormula->getComparisonType();
            storm::utility::regions::addGuardedConstraintToSmtSolver(solver, proveAllViolatedVar, reachProbFunc, proveAllViolatedRel, bound);          
        }

        template<typename ParametricType, typename ConstantType>
        template<typename ValueType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::valueIsInBoundOfFormula(ValueType value){
            STORM_LOG_THROW(this->probabilityOperatorFormula!=nullptr, storm::exceptions::InvalidStateException, "Tried to compare a value to the bound of a formula, but no formula specified.");
            double valueAsDouble = storm::utility::regions::convertNumber<ValueType, double>(value);
            switch (this->probabilityOperatorFormula->getComparisonType()) {
                case storm::logic::ComparisonType::Greater:
                    return (valueAsDouble > this->probabilityOperatorFormula->getBound());
                case storm::logic::ComparisonType::GreaterEqual:
                    return (valueAsDouble >= this->probabilityOperatorFormula->getBound());
                case storm::logic::ComparisonType::Less:
                    return (valueAsDouble < this->probabilityOperatorFormula->getBound());
                case storm::logic::ComparisonType::LessEqual:
                    return (valueAsDouble <= this->probabilityOperatorFormula->getBound());
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
            }
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::printStatisticsToStream(std::ostream& outstream) {
            
            if(this->probabilityOperatorFormula==nullptr){
                outstream << "Region Model Checker Statistics Error: No formula specified." << std::endl; 
                return;
            }
            
            std::chrono::milliseconds timePreprocessingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timePreprocessing);
            std::chrono::milliseconds timeInitialStateEliminationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitialStateElimination);
            std::chrono::milliseconds timeComputeReachProbFunctionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeComputeReachProbFunction);
            std::chrono::milliseconds timeCheckRegionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeCheckRegion);
            std::chrono::milliseconds timeSammplingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSampling);
            std::chrono::milliseconds timeApproximationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeApproximation);
            std::chrono::milliseconds timeMDPBuildInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeMDPBuild);
            std::chrono::milliseconds timeFullSmtInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeFullSmt);
            
            std::chrono::high_resolution_clock::duration timeOverall = timePreprocessing + timeCheckRegion; // + ...
            std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);
            
            uint_fast64_t numOfSolvedRegions= this->numOfRegionsExistsBoth + this->numOfRegionsAllSat + this->numOfRegionsAllViolated;
            
            outstream << std::endl << "Region Model Checker Statistics:" << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            outstream << "Model: " << this->model.getNumberOfStates() << " states, " << this->model.getNumberOfTransitions() << " transitions." << std::endl;
            outstream << "Simplified model: " << this->simplifiedModel->getNumberOfStates() << " states, " << this->simplifiedModel->getNumberOfTransitions() << " transitions" << std::endl;
            outstream << "Formula: " << *this->probabilityOperatorFormula << std::endl;
            outstream << (this->hasOnlyLinearFunctions ? "A" : "Not a") << "ll occuring functions in the model are linear" << std::endl;
            outstream << "Number of checked regions: " << this->numOfCheckedRegions << std::endl;
            outstream << "  Number of solved regions:  " <<  numOfSolvedRegions << "(" << numOfSolvedRegions*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    AllSat:      " <<  this->numOfRegionsAllSat << "(" << this->numOfRegionsAllSat*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    AllViolated: " <<  this->numOfRegionsAllViolated << "(" << this->numOfRegionsAllViolated*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    ExistsBoth:  " <<  this->numOfRegionsExistsBoth << "(" << this->numOfRegionsExistsBoth*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "    Unsolved:    " <<  this->numOfCheckedRegions - numOfSolvedRegions << "(" << (this->numOfCheckedRegions - numOfSolvedRegions)*100/this->numOfCheckedRegions << "%)" <<  std::endl;
            outstream << "  --  Note: %-numbers are relative to the NUMBER of regions, not the size of their area --" <<  std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughSampling << " regions solved through Sampling" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughApproximation << " regions solved through Approximation" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughSubsystemSmt << " regions solved through SubsystemSmt" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughFullSmt << " regions solved through FullSmt" << std::endl;
            outstream << std::endl;
            outstream << "Running times:" << std::endl;
            outstream << "  " << timeOverallInMilliseconds.count() << "ms overall (excluding model parsing)" << std::endl;
            outstream << "  " << timePreprocessingInMilliseconds.count() << "ms Preprocessing including... " << std::endl;
            outstream << "    " << timeInitialStateEliminationInMilliseconds.count() << "ms Initial state elimination of const transitions" << std::endl;
            outstream << "  " << timeComputeReachProbFunctionInMilliseconds.count() << "ms to compute the reachability probability function" << std::endl;
            outstream << "  " << timeCheckRegionInMilliseconds.count() << "ms Region Check including... " << std::endl;
            outstream << "    " << timeSammplingInMilliseconds.count() << "ms Sampling " << std::endl;
            outstream << "    " << timeApproximationInMilliseconds.count() << "ms Approximation including... " << std::endl;
            outstream << "      " << timeMDPBuildInMilliseconds.count() << "ms to build the MDP" << std::endl;
            outstream << "    " << timeFullSmtInMilliseconds.count() << "ms Full Smt solving" << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            
        }
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>;
#endif
        //note: for other template instantiations, add rules for the typedefs of VariableType and CoefficientType in utility/regions.h
        
    } // namespace modelchecker
} // namespace storm
