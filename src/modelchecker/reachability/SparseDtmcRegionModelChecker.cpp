#include "src/modelchecker/reachability/SparseDtmcRegionModelChecker.h"

#include <chrono>

#include "src/adapters/CarlAdapter.h"

//#include "src/storage/StronglyConnectedComponentDecomposition.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"

#include "modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "modelchecker/prctl/SparseMdpPrctlModelChecker.h"
//#include "modelchecker/reachability/SparseDtmcEliminationModelChecker.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/UnexpectedException.h"


namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::ParameterRegion(std::map<VariableType, BoundType> lowerBounds, std::map<VariableType, BoundType> upperBounds) : lowerBounds(lowerBounds), upperBounds(upperBounds), checkResult(RegionCheckResult::UNKNOWN) {
            //todo: check whether both mappings map the same variables
        }

        template<typename ParametricType, typename ConstantType>        
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setUnSatPoint(std::map<VariableType, BoundType> const& unSatPoint) {
            this->unSatPoint = unSatPoint;
        }
        
        template<typename ParametricType, typename ConstantType>
        std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getUnSatPoint() const {
            return unSatPoint;
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setSatPoint(std::map<VariableType, BoundType> const& satPoint) {
            this->satPoint = satPoint;
        }

        template<typename ParametricType, typename ConstantType>
        std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getSatPoint() const {
            return satPoint;
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setCheckResult(RegionCheckResult checkResult) {
            //a few sanity checks
            STORM_LOG_THROW((this->checkResult==RegionCheckResult::UNKNOWN || checkResult!=RegionCheckResult::UNKNOWN),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from something known to UNKNOWN ");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSSAT || checkResult!=RegionCheckResult::EXISTSUNSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSSAT to EXISTSUNSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSSAT || checkResult!=RegionCheckResult::ALLUNSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSSAT to ALLUNSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSUNSAT || checkResult!=RegionCheckResult::EXISTSSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSUNSAT to EXISTSSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSUNSAT || checkResult!=RegionCheckResult::ALLSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSUNSAT to ALLSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSBOTH || checkResult!=RegionCheckResult::ALLSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSBOTH to ALLSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::EXISTSBOTH || checkResult!=RegionCheckResult::ALLUNSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSBOTH to ALLUNSAT");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::ALLSAT || checkResult==RegionCheckResult::ALLSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from ALLSAT to something else");
            STORM_LOG_THROW((this->checkResult!=RegionCheckResult::ALLUNSAT || checkResult==RegionCheckResult::ALLUNSAT),storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from ALLUNSAT to something else");
            this->checkResult = checkResult;
        }

        template<typename ParametricType, typename ConstantType>        
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::RegionCheckResult SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getCheckResult() const {
            return checkResult;
        }

      
                
        template<typename ParametricType, typename ConstantType>
        std::set<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getVariables() const{
            std::set<VariableType> result;
            for(auto const& variableWithBound : lowerBounds) {
                result.insert(variableWithBound.first);
            }
            return result;
        }
        
        template<typename ParametricType, typename ConstantType>
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getLowerBound(VariableType const& variable) const{
            auto const& result = lowerBounds.find(variable);
            STORM_LOG_THROW(result!=lowerBounds.end(), storm::exceptions::IllegalArgumentException, "tried to find a lower bound of a variable that is not specified by this region");
            return (*result).second;
        }
        
        template<typename ParametricType, typename ConstantType>
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getUpperBound(VariableType const& variable) const{
            auto const& result = upperBounds.find(variable);
            STORM_LOG_THROW(result!=upperBounds.end(), storm::exceptions::IllegalArgumentException, "tried to find an upper bound of a variable that is not specified by this region");
            return (*result).second;
        }
        
        template<typename ParametricType, typename ConstantType>
        std::vector<std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType>> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const{
            std::size_t const numOfVariables=consideredVariables.size();
            std::size_t const numOfVertices=std::pow(2,numOfVariables);
            std::vector<std::map<VariableType, BoundType>> resultingVector(numOfVertices,std::map<VariableType, BoundType>());
            if(numOfVertices==1){
                //no variables are given, the returned vector should still contain an empty map
                return resultingVector;
            }
            
            for(uint_fast64_t vertexId=0; vertexId<numOfVertices; ++vertexId){
                //interprete vertexId as a bit sequence
                //the consideredVariables.size() least significant bits of vertex will always represent the next vertex
                //(00...0 = lower bounds for all variables, 11...1 = upper bounds for all variables)
                std::size_t variableIndex=0;
                for(auto const& variable : consideredVariables){
                    if( (vertexId>>variableIndex)%2==0  ){
                        resultingVector[vertexId].insert(std::pair<VariableType, BoundType>(variable, getLowerBound(variable)));
                    }
                    else{
                        resultingVector[vertexId].insert(std::pair<VariableType, BoundType>(variable, getUpperBound(variable)));
                    }
                    ++variableIndex;
                }
            }
            return resultingVector;
        }

        template<typename ParametricType, typename ConstantType>
        std::string SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::checkResultToString() const {
            switch (this->checkResult) {
                case RegionCheckResult::UNKNOWN:
                    return "unknown";
                case RegionCheckResult::EXISTSSAT:
                    return "ExistsSat";
                case RegionCheckResult::EXISTSUNSAT:
                    return "ExistsUnsat";
                case RegionCheckResult::EXISTSBOTH:
                    return "ExistsBoth";
                case RegionCheckResult::ALLSAT:
                    return "allSat";
                case RegionCheckResult::ALLUNSAT:
                    return "allUnsat";
            }
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not identify check result")
            return "ERROR";
        }
            
        template<typename ParametricType, typename ConstantType>
        std::string SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::toString() const {
            std::stringstream regionstringstream;
            for(auto var : this->getVariables()){
                regionstringstream << storm::utility::regions::convertNumber<SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType,double>(this->getLowerBound(var));
                regionstringstream << "<=";
                regionstringstream << storm::utility::regions::getVariableName(var);
                regionstringstream << "<=";
                regionstringstream << storm::utility::regions::convertNumber<SparseDtmcRegionModelChecker<ParametricType, ConstantType>::BoundType,double>(this->getUpperBound(var));
                regionstringstream << ",";
            }
            std::string regionstring = regionstringstream.str();
            //the last comma should actually be a semicolon
            regionstring = regionstring.substr(0,regionstring.length()-1) + ";";
            return regionstring;
        }
                
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model) : model(model), eliminationModelChecker(model), probabilityOperatorFormula(nullptr) {
            // Intentionally left empty.
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
            
            //Get subformula, initial state, target states
            //Note: canHandle already ensures that the formula has the right shape and that the model has a single initial state.
            this->probabilityOperatorFormula= std::unique_ptr<storm::logic::ProbabilityOperatorFormula>(new storm::logic::ProbabilityOperatorFormula(formula.asStateFormula().asProbabilityOperatorFormula()));
            storm::logic::EventuallyFormula const& eventuallyFormula = this->probabilityOperatorFormula->getSubformula().asPathFormula().asEventuallyFormula();
            std::unique_ptr<CheckResult> targetStatesResultPtr = this->eliminationModelChecker.check(eventuallyFormula.getSubformula());
            storm::storage::BitVector const& targetStates = targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector();

            // Then, compute the subset of states that has a probability of 0 or 1, respectively.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model, storm::storage::BitVector(model.getNumberOfStates(),true), targetStates);
            storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
            storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            
            // If the initial state is known to have either probability 0 or 1, we can directly set the reachProbFunction.
            if (model.getInitialStates().isDisjointFrom(maybeStates)) {
                STORM_LOG_DEBUG("The probability of the initial state is constant (0 or 1)");
                this->reachProbFunction = statesWithProbability0.get(*model.getInitialStates().begin()) ? storm::utility::zero<ParametricType>() : storm::utility::one<ParametricType>();
            }
            
            // Determine the set of states that is reachable from the initial state without jumping over a target state.
            storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(model.getTransitionMatrix(), model.getInitialStates(), maybeStates, statesWithProbability1);
            // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
            maybeStates &= reachableStates;
            // Create a vector for the probabilities to go to a state with probability 1 in one step.
            this->oneStepProbabilities = model.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
            // Determine the initial state of the sub-model.
            //storm::storage::BitVector newInitialStates = model.getInitialStates() % maybeStates;
            this->initialState = *(model.getInitialStates() % maybeStates).begin();
            // We then build the submatrix that only has the transitions of the maybe states.
            storm::storage::SparseMatrix<ParametricType> submatrix = model.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
            storm::storage::SparseMatrix<ParametricType> submatrixTransposed = submatrix.transpose();
            
            // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            this->flexibleTransitions = this->eliminationModelChecker.getFlexibleSparseMatrix(submatrix);
            this->flexibleBackwardTransitions = this->eliminationModelChecker.getFlexibleSparseMatrix(submatrixTransposed, true);
            
            // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
            this->subsystem = storm::storage::BitVector(submatrix.getRowCount(), true);
            
            std::chrono::high_resolution_clock::time_point timeInitialStateEliminationStart = std::chrono::high_resolution_clock::now();
            // eliminate all states with only constant outgoing transitions
            //TODO: maybe also states with constant incoming tranistions. THEN the ordering of the eliminated states does matter.
            eliminateStatesConstSucc(this->subsystem, this->flexibleTransitions, this->flexibleBackwardTransitions, this->oneStepProbabilities, this->hasOnlyLinearFunctions, this->initialState);
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions." << std::endl);
            std::cout << "Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << " states that had constant outgoing transitions." << std::endl;
            //eliminate the remaining states to get the reachability probability function
            this->sparseTransitions = flexibleTransitions.getSparseMatrix();
            this->sparseBackwardTransitions = this->sparseTransitions.transpose();
            std::chrono::high_resolution_clock::time_point timeComputeReachProbFunctionStart = std::chrono::high_resolution_clock::now();
            this->reachProbFunction = computeReachProbFunction(this->subsystem, this->flexibleTransitions, this->flexibleBackwardTransitions, this->sparseTransitions, this->sparseBackwardTransitions, this->oneStepProbabilities, this->initialState);
            std::chrono::high_resolution_clock::time_point timeComputeReachProbFunctionEnd = std::chrono::high_resolution_clock::now();
      //      std::cout << std::endl <<"the resulting reach prob function is " << std::endl << this->reachProbFunction << std::endl << std::endl;
            std::chrono::high_resolution_clock::time_point timeInitialStateEliminationEnd = std::chrono::high_resolution_clock::now();
            
            //some information for statistics...
            std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
            this->timePreprocessing= timePreprocessingEnd - timePreprocessingStart;
            this->timeInitialStateElimination = timeInitialStateEliminationEnd-timeInitialStateEliminationStart;
            this->timeComputeReachProbFunction = timeComputeReachProbFunctionEnd-timeComputeReachProbFunctionStart;
            this->numOfCheckedRegions=0;
            this->numOfRegionsSolvedThroughSampling=0;
            this->numOfRegionsSolvedThroughApproximation=0;
            this->numOfRegionsSolvedThroughSubsystemSmt=0;
            this->numOfRegionsSolvedThroughFullSmt=0;
            this->timeCheckRegion=std::chrono::high_resolution_clock::duration::zero();
            this->timeSampling=std::chrono::high_resolution_clock::duration::zero();
            this->timeApproximation=std::chrono::high_resolution_clock::duration::zero();
            this->timeMDPBuild=std::chrono::high_resolution_clock::duration::zero();
            this->timeSubsystemSmt=std::chrono::high_resolution_clock::duration::zero();
            this->timeFullSmt=std::chrono::high_resolution_clock::duration::zero();
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::eliminateStatesConstSucc(
                storm::storage::BitVector& subsys, 
                FlexibleMatrix& flexTransitions,
                FlexibleMatrix& flexBackwardTransitions,
                std::vector<ParametricType>& oneStepProbs,
                bool& allFunctionsAreLinear,
                storm::storage::sparse::state_type const& initialState) {
            
            //temporarily unselect the initial state to skip it.
            subsys.set(initialState, false);
            
            allFunctionsAreLinear=true;
            
            boost::optional<std::vector<ParametricType>> missingStateRewards;
            for (auto const& state : subsys) {
                bool onlyConstantOutgoingTransitions=true;
                for(auto& entry : flexTransitions.getRow(state)){
                    if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                        onlyConstantOutgoingTransitions=false;
                        allFunctionsAreLinear &= storm::utility::regions::functionIsLinear<ParametricType>(entry.getValue());
                    }
                }
                if(!this->parametricTypeComparator.isConstant(oneStepProbs[state])){
                    onlyConstantOutgoingTransitions=false;
                    allFunctionsAreLinear &= storm::utility::regions::functionIsLinear<ParametricType>(oneStepProbs[state]);
                }
                if(onlyConstantOutgoingTransitions){
                    this->eliminationModelChecker.eliminateState(flexTransitions, oneStepProbs, state, flexBackwardTransitions, missingStateRewards);
                    subsys.set(state,false);
                }
            }
            subsys.set(initialState, true);
        }
        
        
        template<typename ParametricType, typename ConstantType>
        ParametricType SparseDtmcRegionModelChecker<ParametricType, ConstantType>::computeReachProbFunction(
                storm::storage::BitVector const& subsys,
                FlexibleMatrix const& flexTransitions,
                FlexibleMatrix const& flexBackwardTransitions,
                storm::storage::SparseMatrix<ParametricType> const& spTransitions,
                storm::storage::SparseMatrix<ParametricType> const& spBackwardTransitions,
                std::vector<ParametricType> const& oneStepProbs,
                storm::storage::sparse::state_type const& initState){
            
            //Note: this function is very similar to eliminationModelChecker.computeReachabilityValue
            
            //get working copies of the flexible transitions and oneStepProbabilities with which we will actually work (eliminate states etc).
            FlexibleMatrix workingCopyFlexTrans(flexTransitions);
            FlexibleMatrix workingCopyFlexBackwTrans(flexBackwardTransitions);
            std::vector<ParametricType> workingCopyOneStepProbs(oneStepProbs);
            
            storm::storage::BitVector initialStates(subsys.size(),false);
            initialStates.set(initState, true);
            std::vector<std::size_t> statePriorities = this->eliminationModelChecker.getStatePriorities(spTransitions, spBackwardTransitions, initialStates, workingCopyOneStepProbs);
            boost::optional<std::vector<ParametricType>> missingStateRewards;
            
            // Remove the initial state from the states which we need to eliminate.
            storm::storage::BitVector statesToEliminate(subsys);
            statesToEliminate.set(initState, false);
            
            //pure state elimination or hybrid method?
            if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::State) {
                std::vector<storm::storage::sparse::state_type> states(statesToEliminate.begin(), statesToEliminate.end());
                std::sort(states.begin(), states.end(), [&statePriorities] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return statePriorities[a] < statePriorities[b]; });
                
                STORM_LOG_DEBUG("Eliminating " << states.size() << " states using the state elimination technique." << std::endl);
                for (auto const& state : states) {
                    this->eliminationModelChecker.eliminateState(workingCopyFlexTrans, workingCopyOneStepProbs, state, workingCopyFlexBackwTrans, missingStateRewards);
                }
            }
            else if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::Hybrid) {
                uint_fast64_t maximalDepth = 0;
                std::vector<storm::storage::sparse::state_type> entryStateQueue;
                STORM_LOG_DEBUG("Eliminating " << statesToEliminate.size() << " states using the hybrid elimination technique." << std::endl);
                maximalDepth = eliminationModelChecker.treatScc(workingCopyFlexTrans, workingCopyOneStepProbs, initialStates, statesToEliminate, spTransitions, workingCopyFlexBackwTrans, false, 0, storm::settings::sparseDtmcEliminationModelCheckerSettings().getMaximalSccSize(), entryStateQueue, missingStateRewards, statePriorities);
                
                // If the entry states were to be eliminated last, we need to do so now.
                STORM_LOG_DEBUG("Eliminating " << entryStateQueue.size() << " entry states as a last step.");
                if (storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet()) {
                    for (auto const& state : entryStateQueue) {
                        eliminationModelChecker.eliminateState(workingCopyFlexTrans, workingCopyOneStepProbs, state, workingCopyFlexBackwTrans, missingStateRewards);
                    }
                }
            }
            else {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The selected state elimination Method has not been implemented.");
            }
            //finally, eliminate the initial state
            this->eliminationModelChecker.eliminateState(workingCopyFlexTrans, workingCopyOneStepProbs, initState, workingCopyFlexBackwTrans, missingStateRewards);
            
            return workingCopyOneStepProbs[initState];
        }
        



        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegion(ParameterRegion& region) {
            std::chrono::high_resolution_clock::time_point timeCheckRegionStart = std::chrono::high_resolution_clock::now();
            ++this->numOfCheckedRegions;
            
            STORM_LOG_THROW(this->probabilityOperatorFormula!=nullptr, storm::exceptions::InvalidStateException, "Tried to analyze a region although no property has been specified" );
            STORM_LOG_DEBUG("Analyzing the region " << region.toString());
            std::cout << "Analyzing the region " << region.toString() << std::endl;
            
            //switches for the different steps.
            bool doApproximation=this->hasOnlyLinearFunctions; // this approach is only correct if the model has only linear functions
            bool doSampling=true;
            bool doSubsystemSmt=false; //this->hasOnlyLinearFunctions;  // this approach is only correct if the model has only linear functions
            bool doFullSmt=true;
            
            std::chrono::high_resolution_clock::time_point timeApproximationStart = std::chrono::high_resolution_clock::now();
            std::vector<ConstantType> lowerBounds;
            std::vector<ConstantType> upperBounds;
            if(doApproximation){
                STORM_LOG_DEBUG("Checking approximative probabilities...");
                std::cout << "  Checking approximative probabilities..." << std::endl;
                if(checkApproximativeProbabilities(region, lowerBounds, upperBounds)){
                    ++this->numOfRegionsSolvedThroughApproximation;
                    STORM_LOG_DEBUG("Result '" << region.checkResultToString() <<"' obtained through approximation.");
                    doSampling=false;
                    doSubsystemSmt=false;
                    doFullSmt=false;
                }
            }
            std::chrono::high_resolution_clock::time_point timeApproximationEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point timeSamplingStart = std::chrono::high_resolution_clock::now();
            if(doSampling){
                STORM_LOG_DEBUG("Checking sample points...");
                std::cout << "  Checking sample points..." << std::endl;
                if(checkSamplePoints(region)){
                    ++this->numOfRegionsSolvedThroughSampling;
                    STORM_LOG_DEBUG("Result '" << region.checkResultToString() <<"' obtained through sampling.");
                    doApproximation=false;
                    doSubsystemSmt=false;
                    doFullSmt=false;
                }
            }
            std::chrono::high_resolution_clock::time_point timeSamplingEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point timeSubsystemSmtStart = std::chrono::high_resolution_clock::now();
            if(doSubsystemSmt){
                STORM_LOG_WARN("SubsystemSmt approach not yet implemented");
            }
            std::chrono::high_resolution_clock::time_point timeSubsystemSmtEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point timeFullSmtStart = std::chrono::high_resolution_clock::now();
            if(doFullSmt){
                STORM_LOG_WARN("FullSmt approach not yet implemented");
            }
            std::chrono::high_resolution_clock::time_point timeFullSmtEnd = std::chrono::high_resolution_clock::now();
            
            
            
            //some information for statistics...
            std::chrono::high_resolution_clock::time_point timeCheckRegionEnd = std::chrono::high_resolution_clock::now();
            this->timeCheckRegion += timeCheckRegionEnd-timeCheckRegionStart;
            this->timeSampling += timeSamplingEnd - timeSamplingStart;
            this->timeApproximation += timeApproximationEnd - timeApproximationStart;
            this->timeSubsystemSmt += timeSubsystemSmtEnd - timeSubsystemSmtStart;
            this->timeFullSmt += timeFullSmtEnd - timeFullSmtStart;
            
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegions(std::vector<ParameterRegion>& regions) {
            for(auto& region : regions){
                this->checkRegion(region);
            }
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkSamplePoints(ParameterRegion& region) {

            auto samplingPoints = region.getVerticesOfRegion(region.getVariables()); //only test the 4 corner points (for now)
            
            for (auto const& point : samplingPoints){
                // check whether the property is satisfied or not at the given point
                if(this->valueIsInBoundOfFormula(storm::utility::regions::evaluateFunction<ParametricType, ConstantType>(this->reachProbFunction, point))){
                    if (region.getCheckResult()!=RegionCheckResult::EXISTSSAT){
                        region.setSatPoint(point);
                        if(region.getCheckResult()==RegionCheckResult::EXISTSUNSAT){
                            region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                            return true;
                        }
                        region.setCheckResult(RegionCheckResult::EXISTSSAT);
                    }
                }
                else{
                    if (region.getCheckResult()!=RegionCheckResult::EXISTSUNSAT){
                        region.setUnSatPoint(point);
                        if(region.getCheckResult()==RegionCheckResult::EXISTSSAT){
                            region.setCheckResult(RegionCheckResult::EXISTSBOTH);
                            return true;
                        }
                        region.setCheckResult(RegionCheckResult::EXISTSUNSAT);
                    }
                }
            }
            return false;
        }
        
        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkApproximativeProbabilities(ParameterRegion& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds) {
            STORM_LOG_THROW(this->hasOnlyLinearFunctions, storm::exceptions::UnexpectedException, "Tried to generate bounds on the probability (only applicable if all functions are linear), but there are nonlinear functions.");
            //build the mdp and a reachability formula and create a modelchecker
            std::chrono::high_resolution_clock::time_point timeMDPBuildStart = std::chrono::high_resolution_clock::now();
            storm::models::sparse::Mdp<ConstantType> mdp = buildMdpForApproximation(region);
            std::chrono::high_resolution_clock::time_point timeMDPBuildEnd = std::chrono::high_resolution_clock::now();
            this->timeMDPBuild += timeMDPBuildEnd-timeMDPBuildStart;
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
            storm::modelchecker::SparseMdpPrctlModelChecker<ConstantType> modelChecker(mdp);
            
            //Decide whether we should compute lower or upper bounds first.
            //This does not matter if the current result is unknown. However, let us assume that it is more likely that the final result will be ALLSAT. So we test this first.
            storm::logic::OptimalityType firstOpType;
            switch (this->probabilityOperatorFormula->getComparisonType()) {
                case storm::logic::ComparisonType::Greater:
                case storm::logic::ComparisonType::GreaterEqual:
                    switch (region.getCheckResult()){
                        case RegionCheckResult::EXISTSSAT:
                        case RegionCheckResult::UNKNOWN: 
                            firstOpType=storm::logic::OptimalityType::Minimize;
                            break;
                        case RegionCheckResult::EXISTSUNSAT:
                            firstOpType=storm::logic::OptimalityType::Maximize;
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The checkresult of the current region should not be conclusive, i.e. it should be either EXISTSSAT or EXISTSUNSAT or UNKNOWN");
                    }
                    break;
                case storm::logic::ComparisonType::Less:
                case storm::logic::ComparisonType::LessEqual:
                    switch (region.getCheckResult()){
                        case RegionCheckResult::EXISTSSAT:
                        case RegionCheckResult::UNKNOWN: 
                            firstOpType=storm::logic::OptimalityType::Maximize;
                            break;
                        case RegionCheckResult::EXISTSUNSAT:
                            firstOpType=storm::logic::OptimalityType::Minimize;
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The checkresult of the current region should not be conclusive, i.e. it should be either EXISTSSAT or EXISTSUNSAT or UNKNOWN");
                    }
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
            }
            
            //one iteration for each opType \in {Maximize, Minimize}
            storm::logic::OptimalityType opType=firstOpType;
            for(int i=1; i<=2; ++i){   
                //perform model checking on the mdp
                std::unique_ptr<storm::modelchecker::CheckResult> resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula,false,opType);

                //check if the approximation yielded a conclusive result
                if(opType==storm::logic::OptimalityType::Maximize){
                    upperBounds = resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                    if(valueIsInBoundOfFormula(upperBounds[*mdp.getInitialStates().begin()])){
                        if((this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::Less) || 
                                (this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::LessEqual)){
                            region.setCheckResult(RegionCheckResult::ALLSAT);
                            return true;
                        }
                    }
                    else{
                        if((this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::Greater) || 
                                (this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::GreaterEqual)){
                            region.setCheckResult(RegionCheckResult::ALLUNSAT);
                            return true;
                        }
                    }
                    //flip the optype for the next iteration
                    opType=storm::logic::OptimalityType::Minimize;
                    if(i==1) std::cout << "    Requiring a second model checker run (with Minimize)" << std::endl;
                }
                else if(opType==storm::logic::OptimalityType::Minimize){
                    lowerBounds = resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                    if(valueIsInBoundOfFormula(lowerBounds[*mdp.getInitialStates().begin()])){
                        if((this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::Greater) || 
                                (this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::GreaterEqual)){
                            region.setCheckResult(RegionCheckResult::ALLSAT);
                            return true;
                        }
                    }
                    else{
                        if((this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::Less) || 
                                (this->probabilityOperatorFormula->getComparisonType()== storm::logic::ComparisonType::LessEqual)){
                            region.setCheckResult(RegionCheckResult::ALLUNSAT);
                            return true;
                        }
                    }                
                    //flip the optype for the next iteration
                    opType=storm::logic::OptimalityType::Maximize;
                    if(i==1) std::cout << "    Requiring a second model checker run (with Maximize)" << std::endl;
                }
            }
            
            //if we reach this point than the result is still inconclusive.
            return false;            
        }
        
        template<typename ParametricType, typename ConstantType>
        storm::models::sparse::Mdp<ConstantType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::buildMdpForApproximation(const ParameterRegion& region) {
            //We are going to build a (non parametric) MDP which has an action for the lower bound and an action for the upper bound of every parameter 
            
            //The matrix (and the Choice labeling)
            
            //the matrix this->sparseTransitions might have empty rows where states have been eliminated.
            //The MDP matrix should not have such rows. We therefore leave them out, but we have to change the indices of the states accordingly.
            //These changes are computed in advance
            std::vector<storm::storage::sparse::state_type> newStateIndexMap(this->sparseTransitions.getRowCount(), this->sparseTransitions.getRowCount()); //initialize with some illegal index to easily check if a transition leads to an unselected state
            storm::storage::sparse::state_type newStateIndex=0;
            for(auto const& state : this->subsystem){
                newStateIndexMap[state]=newStateIndex;
                ++newStateIndex;
            }
            //We need to add a target state to which the oneStepProbabilities will lead as well as a sink state to which the "missing" probability will lead
            storm::storage::sparse::state_type numStates=newStateIndex+2;
            storm::storage::sparse::state_type targetState=numStates-2;
            storm::storage::sparse::state_type sinkState= numStates-1;
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(0, numStates, 0, true, true, numStates);
            //std::vector<boost::container::flat_set<uint_fast64_t>> choiceLabeling;
            
            //fill in the data row by row            
            storm::storage::sparse::state_type currentRow=0;
            for(storm::storage::sparse::state_type oldStateIndex : this->subsystem){
                //we first go through the row to find out a) which parameter occur in that row and b) at which point do we have to insert the selfloop
                storm::storage::sparse::state_type selfloopIndex=0;
                std::set<VariableType> occurringParameters;
                for(auto const& entry: this->sparseTransitions.getRow(oldStateIndex)){
                    storm::utility::regions::gatherOccurringVariables(entry.getValue(), occurringParameters);
                    if(entry.getColumn()<=oldStateIndex){
                        if(entry.getColumn()==oldStateIndex){
                            //there already is a selfloop so we do not have to add one.
                            selfloopIndex=numStates; // --> selfloop will never be inserted
                        }
                        else {
                            ++selfloopIndex;
                        }
                    }
                    STORM_LOG_THROW(newStateIndexMap[entry.getColumn()]!=this->sparseTransitions.getRowCount(), storm::exceptions::UnexpectedException, "There is a transition to a state that should have been eliminated.");
                }
                storm::utility::regions::gatherOccurringVariables(this->oneStepProbabilities[oldStateIndex], occurringParameters);
                
                //we now add a row for every combination of lower and upper bound of the variables
                auto const& substitutions = region.getVerticesOfRegion(occurringParameters);
                STORM_LOG_ASSERT(!substitutions.empty(), "there are no substitutions, i.e., no vertices of the current region. this should not be possible");
                matrixBuilder.newRowGroup(currentRow);
                for(size_t substitutionIndex=0; substitutionIndex< substitutions.size(); ++substitutionIndex){
                    ConstantType missingProbability = storm::utility::one<ConstantType>();
                    if(selfloopIndex==0){ //add the selfloop first.
                        matrixBuilder.addNextValue(currentRow, newStateIndexMap[oldStateIndex], storm::utility::zero<ConstantType>());
                        selfloopIndex=numStates; // --> selfloop will never be inserted again
                    }
                    for(auto const& entry : this->sparseTransitions.getRow(oldStateIndex)){
                        ConstantType value = storm::utility::regions::convertNumber<BoundType,ConstantType>(
                                             storm::utility::regions::evaluateFunction<ParametricType,ConstantType>(entry.getValue(),substitutions[substitutionIndex])
                                             );
                        missingProbability-=value;
                        storm::storage::sparse::state_type column = newStateIndexMap[entry.getColumn()];
                        matrixBuilder.addNextValue(currentRow, column, value);
                        --selfloopIndex;
                        if(selfloopIndex==0){ //add the selfloop now
                            matrixBuilder.addNextValue(currentRow, newStateIndexMap[oldStateIndex], storm::utility::zero<ConstantType>());
                            selfloopIndex=numStates; // --> selfloop will never be inserted again
                        }
                    }
                    
                    if(!this->parametricTypeComparator.isZero(this->oneStepProbabilities[oldStateIndex])){ //transition to target state
                        ConstantType value = storm::utility::regions::convertNumber<BoundType,ConstantType>(
                                             storm::utility::regions::evaluateFunction<ParametricType,ConstantType>(this->oneStepProbabilities[oldStateIndex],substitutions[substitutionIndex])
                                             );
                        missingProbability-=value;
                        matrixBuilder.addNextValue(currentRow, targetState, value);
                    }
                    if(!this->constantTypeComparator.isZero(missingProbability)){ //transition to sink state
                        STORM_LOG_THROW((missingProbability> -storm::utility::regions::convertNumber<double, ConstantType>(storm::settings::generalSettings().getPrecision())), storm::exceptions::UnexpectedException, "The missing probability is negative.");
                        matrixBuilder.addNextValue(currentRow, sinkState, missingProbability);
                    }
                    //boost::container::flat_set<uint_fast64_t> label;
                    //label.insert(substitutionIndex);
                    //choiceLabeling.emplace_back(label);
                    ++currentRow;
                }
            }
            //add self loops on the additional states (i.e., target and sink)
            //boost::container::flat_set<uint_fast64_t> labelAll;
            //labelAll.insert(substitutions.size());
            matrixBuilder.newRowGroup(currentRow);
            matrixBuilder.addNextValue(currentRow, targetState, storm::utility::one<ConstantType>());
            //    choiceLabeling.emplace_back(labelAll);
            ++currentRow;
            matrixBuilder.newRowGroup(currentRow);
            matrixBuilder.addNextValue(currentRow, sinkState, storm::utility::one<ConstantType>());
            //    choiceLabeling.emplace_back(labelAll);
            ++currentRow;
            
            //The labeling
            
            storm::models::sparse::StateLabeling stateLabeling(numStates);
            storm::storage::BitVector initLabel(numStates, false);
            initLabel.set(newStateIndexMap[this->initialState], true);
            stateLabeling.addLabel("init", std::move(initLabel));
            storm::storage::BitVector targetLabel(numStates, false);
            targetLabel.set(numStates-2, true);
            stateLabeling.addLabel("target", std::move(targetLabel));
            storm::storage::BitVector sinkLabel(numStates, false);
            sinkLabel.set(numStates-1, true);
            stateLabeling.addLabel("sink", std::move(sinkLabel));

            // The MDP
            boost::optional<std::vector<ConstantType>> noStateRewards;
            boost::optional<storm::storage::SparseMatrix<ConstantType>> noTransitionRewards;  
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;            
            
            return storm::models::sparse::Mdp<ConstantType>(matrixBuilder.build(), std::move(stateLabeling), std::move(noStateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
        }

        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        template<>
        std::pair<storm::storage::SparseMatrix<double>,std::vector<boost::container::flat_set<uint_fast64_t>>> SparseDtmcRegionModelChecker<storm::RationalFunction, double>::instantiateFlexibleMatrix(FlexibleMatrix const& matrix, std::vector<std::map<storm::Variable, storm::RationalFunction::CoeffType>> const& substitutions, storm::storage::BitVector const& filter, bool addSinkState, std::vector<storm::RationalFunction> const& oneStepProbabilities, bool addSelfLoops) const {
            
            //Check if the arguments are as expected
            STORM_LOG_THROW((std::is_same<storm::RationalFunction::CoeffType, cln::cl_RA>::value), storm::exceptions::IllegalArgumentException, "Unexpected Type of Coefficients");
            STORM_LOG_THROW(filter.size()==matrix.getNumberOfRows(), storm::exceptions::IllegalArgumentException, "Unexpected size of the filter");
            STORM_LOG_THROW(oneStepProbabilities.empty() || oneStepProbabilities.size()==matrix.getNumberOfRows(), storm::exceptions::IllegalArgumentException, "Unexpected size of the oneStepProbabilities");
            
            //get a mapping from old state indices to the new ones
            std::vector<storm::storage::sparse::state_type> newStateIndexMap(matrix.getNumberOfRows(), matrix.getNumberOfRows()); //initialize with some illegal index to easily check if a transition leads to an unselected state
            storm::storage::sparse::state_type newStateIndex=0;
            for(auto const& state : filter){
                newStateIndexMap[state]=newStateIndex;
                ++newStateIndex;
            }
            storm::storage::sparse::state_type numStates=filter.getNumberOfSetBits();
            STORM_LOG_ASSERT(newStateIndex==numStates, "unexpected number of new states");
            storm::storage::sparse::state_type targetState =0;
            storm::storage::sparse::state_type sinkState=0;
            if(!oneStepProbabilities.empty()){
                targetState=numStates;
                ++numStates;
            }
            if(addSinkState){
                sinkState=numStates;
                ++numStates;
            }
            //todo rows (i.e. the first parameter) should be numStates*substitutions.size ?
            storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, numStates, 0, true, true, numStates);
            std::vector<boost::container::flat_set<uint_fast64_t>> choiceLabeling;
            //fill in the data row by row            
            storm::storage::sparse::state_type currentRow=0;
            for(auto const& oldStateIndex : filter){
                matrixBuilder.newRowGroup(currentRow);
                for(size_t substitutionIndex=0; substitutionIndex< substitutions.size(); ++substitutionIndex){
                    double missingProbability = 1.0;
                    if(matrix.getRow(oldStateIndex).empty()){ //just add the selfloop if there is no transition
                        if(addSelfLoops){
                            matrixBuilder.addNextValue(currentRow, newStateIndexMap[oldStateIndex], storm::utility::zero<double>());
                        }
                    }
                    else{
                        FlexibleMatrix::const_iterator entry = matrix.getRow(oldStateIndex).begin();
                        for(; entry<matrix.getRow(oldStateIndex).end() && entry->getColumn()<oldStateIndex; ++entry){ //insert until we come to the diagonal entry
                            double value = cln::double_approx(entry->getValue().evaluate(substitutions[substitutionIndex]));
                            missingProbability-=value;
                            storm::storage::sparse::state_type column = newStateIndexMap[entry->getColumn()];
                            STORM_LOG_THROW(column<numStates, storm::exceptions::IllegalArgumentException, "Illegal filter: Selected a state that has a transition to an unselected state.");
                            matrixBuilder.addNextValue(currentRow, column, value);
                        }
                        if(addSelfLoops && entry->getColumn()!=oldStateIndex){ //maybe add a zero valued diagonal entry
                            matrixBuilder.addNextValue(currentRow, newStateIndexMap[oldStateIndex], storm::utility::zero<double>());
                        }
                        for(; entry < matrix.getRow(oldStateIndex).end(); ++entry){ //insert the rest
                            double value = cln::double_approx(entry->getValue().evaluate(substitutions[substitutionIndex]));
                            missingProbability-=value;
                            storm::storage::sparse::state_type column = newStateIndexMap[entry->getColumn()];
                            STORM_LOG_THROW(column<numStates, storm::exceptions::IllegalArgumentException, "Illegal filter: Selected a state that has a transition to an unselected state.");
                            matrixBuilder.addNextValue(currentRow, column, value);
                        }
                    }
                    if(!oneStepProbabilities.empty() && !oneStepProbabilities[oldStateIndex].isZero()){ //transition to target state
                        double value = cln::double_approx(oneStepProbabilities[oldStateIndex].evaluate(substitutions[substitutionIndex]));
                        missingProbability-=value;
                        matrixBuilder.addNextValue(currentRow, targetState, value);
                    }
                    storm::utility::ConstantsComparator<double> doubleComperator;
                    if(addSinkState && !doubleComperator.isZero(missingProbability)){ //transition to sink state
                        STORM_LOG_ASSERT(missingProbability> -storm::settings::generalSettings().getPrecision(), "The missing probability is negative.");
                        matrixBuilder.addNextValue(currentRow, sinkState, missingProbability);
                    }
                    boost::container::flat_set<uint_fast64_t> label;
                    label.insert(substitutionIndex);
                    choiceLabeling.emplace_back(label);
                    ++currentRow;
                }
            }
            //finally, add self loops on the additional states (i.e., target and sink)
            boost::container::flat_set<uint_fast64_t> labelAll;
            labelAll.insert(substitutions.size());
            if (!oneStepProbabilities.empty()){
                matrixBuilder.newRowGroup(currentRow);
                matrixBuilder.addNextValue(currentRow, targetState, storm::utility::one<double>());
                choiceLabeling.emplace_back(labelAll);
                ++currentRow;
            }
            
            if (addSinkState){
                matrixBuilder.newRowGroup(currentRow);
                matrixBuilder.addNextValue(currentRow, sinkState, storm::utility::one<double>());
                choiceLabeling.emplace_back(labelAll);
                ++currentRow;
            }
       
            return std::pair<storm::storage::SparseMatrix<double>, std::vector<boost::container::flat_set<uint_fast64_t>>>(matrixBuilder.build(), std::move(choiceLabeling));
        }
        
        template<typename ParametricType, typename ConstantType>
        std::pair<storm::storage::SparseMatrix<double>,std::vector<boost::container::flat_set<uint_fast64_t>>> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::instantiateFlexibleMatrix(FlexibleMatrix const& matrix, std::vector<std::map<storm::Variable, storm::RationalFunction::CoeffType>> const& substitutions, storm::storage::BitVector const& filter, bool addSinkState, std::vector<ParametricType> const& oneStepProbabilities, bool addSelfLoops) const{
            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Instantiation of flexible matrix is not supported for this type");
        }

#ifdef STORM_HAVE_CARL
                
        template<>
        void SparseDtmcRegionModelChecker<storm::RationalFunction, double>::eliminateStates(storm::storage::BitVector& subsystem, FlexibleMatrix& flexibleMatrix, std::vector<storm::RationalFunction>& oneStepProbabilities, FlexibleMatrix& flexibleBackwardTransitions, storm::storage::BitVector const& initialstates, storm::storage::SparseMatrix<storm::RationalFunction> const& forwardTransitions, boost::optional<std::vector<std::size_t>> const& statePriorities){
    
            if(true){ // eliminate all states with constant outgoing transitions
                storm::storage::BitVector statesToEliminate = ~initialstates;
                //todo: ordering of states important?
                boost::optional<std::vector<storm::RationalFunction>> missingStateRewards;
                for (auto const& state : statesToEliminate) {
                    bool onlyConstantOutgoingTransitions=true;
                    for(auto const& entry : flexibleMatrix.getRow(state)){
                        if(!entry.getValue().isConstant()){
                            onlyConstantOutgoingTransitions=false;
                            break;
                        }
                    }
                    if(onlyConstantOutgoingTransitions){
                        eliminationModelChecker.eliminateState(flexibleMatrix, oneStepProbabilities, state, flexibleBackwardTransitions, missingStateRewards);
                        subsystem.set(state,false);
                    }
                }

                //Note: we could also "eliminate" the initial state to get rid of its selfloop
            }
            else if(false){ //eliminate all states with standard state elimination
                boost::optional<std::vector<storm::RationalFunction>> missingStateRewards;
                
                storm::storage::BitVector statesToEliminate = ~initialstates;
                std::vector<storm::storage::sparse::state_type> states(statesToEliminate.begin(), statesToEliminate.end());
                
                if (statePriorities) {
                    std::sort(states.begin(), states.end(), [&statePriorities] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return statePriorities.get()[a] < statePriorities.get()[b]; });
                }
                
                STORM_LOG_DEBUG("Eliminating " << states.size() << " states using the state elimination technique." << std::endl);
                for (auto const& state : states) {
                    eliminationModelChecker.eliminateState(flexibleMatrix, oneStepProbabilities, state, flexibleBackwardTransitions, missingStateRewards);
                }
                subsystem=~statesToEliminate;
                
            }
            else if(false){ //hybrid method
                boost::optional<std::vector<storm::RationalFunction>> missingStateRewards;
                storm::storage::BitVector statesToEliminate = ~initialstates;
                uint_fast64_t maximalDepth =0;
                std::vector<storm::storage::sparse::state_type> entryStateQueue;
                STORM_LOG_DEBUG("Eliminating " << statesToEliminate.size() << " states using the hybrid elimination technique." << std::endl);
                maximalDepth = eliminationModelChecker.treatScc(flexibleMatrix, oneStepProbabilities, initialstates, statesToEliminate, forwardTransitions, flexibleBackwardTransitions, false, 0, storm::settings::sparseDtmcEliminationModelCheckerSettings().getMaximalSccSize(), entryStateQueue, missingStateRewards, statePriorities);
                
                // If the entry states were to be eliminated last, we need to do so now.
                STORM_LOG_DEBUG("Eliminating " << entryStateQueue.size() << " entry states as a last step.");
                if (storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet()) {
                    for (auto const& state : entryStateQueue) {
                        eliminationModelChecker.eliminateState(flexibleMatrix, oneStepProbabilities, state, flexibleBackwardTransitions, missingStateRewards);
                    }
                }
                subsystem=~statesToEliminate;     
            }
            std::cout << "Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " of " << subsystem.size() << "states." << std::endl;
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() - subsystem.getNumberOfSetBits() << " states." << std::endl);
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::eliminateStates(storm::storage::BitVector& subsystem, FlexibleMatrix& flexibleMatrix, std::vector<ParametricType>& oneStepProbabilities, FlexibleMatrix& flexibleBackwardTransitions, storm::storage::BitVector const& initialstates, storm::storage::SparseMatrix<ParametricType> const& forwardTransitions, boost::optional<std::vector<std::size_t>> const& statePriorities){
            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "elimination of states not suported for this type");
        }

        
        template<>
        void SparseDtmcRegionModelChecker<storm::RationalFunction, double>::formulateModelWithSMT(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType>& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities){
            carl::VariablePool& varPool = carl::VariablePool::getInstance();
            
            //first add a state variable for every state in the subsystem, providing that such a variable does not already exist.
            for (storm::storage::sparse::state_type state : subsystem){
                if(stateProbVars[state].isZero()){ //variable does not exist yet
                    storm::Variable stateVar = varPool.getFreshVariable("p_" + std::to_string(state));
                    std::shared_ptr<carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>> cache(new carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>());
                    storm::RationalFunction::PolyType stateVarAsPoly(storm::RationalFunction::PolyType::PolyType(stateVar), cache);

                    //each variable is in the interval [0,1]
                    solver.add(storm::RationalFunction(stateVarAsPoly), storm::CompareRelation::GEQ, storm::RationalFunction(0));
                    solver.add(storm::RationalFunction(stateVarAsPoly), storm::CompareRelation::LEQ, storm::RationalFunction(1));
                    stateProbVars[state] = stateVarAsPoly;
                }
            }
            
            //now lets add the actual transitions
            for (storm::storage::sparse::state_type state : subsystem){
                storm::RationalFunction reachProbability(oneStepProbabilities[state]);
                for(auto const& transition : flexibleMatrix.getRow(state)){
                    reachProbability += transition.getValue() * stateProbVars[transition.getColumn()];
                }
                //Todo: depending on the objective (i.e. the formlua) it suffices to use LEQ or GEQ here... maybe this is faster?
                solver.add(storm::RationalFunction(stateProbVars[state]), storm::CompareRelation::EQ, reachProbability);
            }
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::formulateModelWithSMT(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType>& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities){
            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "SMT formulation is not supported for this type");
        }
        
        template<>
        void SparseDtmcRegionModelChecker<storm::RationalFunction, double>::restrictProbabilityVariables(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType> const& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities, ParameterRegion const& region, storm::logic::ComparisonType const& compType){
            //We are going to build a new (non parametric) MDP which has an action for the lower bound and an action for the upper bound of every parameter 
                        
            //todo invent something better to obtain the substitutions.
            //this only works as long as there is only one parameter per state,
            // also: check whether the terms are linear/monotone(?)
            
            STORM_LOG_WARN("the probability restriction only works on linear terms which is not checked");
            storm::storage::sparse::state_type const numOfStates=subsystem.getNumberOfSetBits() + 2; //subsystem + target state + sink state
            storm::models::sparse::StateLabeling stateLabeling(numOfStates);
            stateLabeling.addLabel("init", storm::storage::BitVector(numOfStates, true));
            storm::storage::BitVector targetLabel(numOfStates, false);
            targetLabel.set(numOfStates-2, true);
            stateLabeling.addLabel("target", std::move(targetLabel));
            storm::storage::BitVector sinkLabel(numOfStates, false);
            sinkLabel.set(numOfStates-1, true);
            stateLabeling.addLabel("sink", std::move(sinkLabel));

            std::pair<storm::storage::SparseMatrix<double>,std::vector<boost::container::flat_set<uint_fast64_t>>> instantiation = instantiateFlexibleMatrix(flexibleMatrix, region.getVerticesOfRegion(region.getVariables()), subsystem, true, oneStepProbabilities, true);
            boost::optional<std::vector<double>> noStateRewards;
            boost::optional<storm::storage::SparseMatrix<double>> noTransitionRewards;            
            storm::models::sparse::Mdp<double> mdp(instantiation.first, std::move(stateLabeling),noStateRewards,noTransitionRewards,instantiation.second);
                        
            //we need the correct optimalityType for model checking as well as the correct relation for smt solving
            storm::logic::OptimalityType opType;
            storm::CompareRelation boundRelation;
            switch (compType){
                case storm::logic::ComparisonType::Greater:
                    opType=storm::logic::OptimalityType::Minimize;
                    boundRelation=storm::CompareRelation::GEQ;
                    break;
                case storm::logic::ComparisonType::GreaterEqual:
                    opType=storm::logic::OptimalityType::Minimize;
                    boundRelation=storm::CompareRelation::GEQ;
                    break;
                case storm::logic::ComparisonType::Less:
                    opType=storm::logic::OptimalityType::Maximize;
                    boundRelation=storm::CompareRelation::LEQ;
                    break;
                case storm::logic::ComparisonType::LessEqual:
                    opType=storm::logic::OptimalityType::Maximize;
                    boundRelation=storm::CompareRelation::LEQ;
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
            }
            
            //perform model checking on the mdp
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
            storm::modelchecker::SparseMdpPrctlModelChecker<double> modelChecker(mdp);
            std::unique_ptr<CheckResult> resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula,false,opType);
            std::vector<double> resultVector = resultPtr->asExplicitQuantitativeCheckResult<double>().getValueVector();            
            
            //formulate constraints for the solver
            uint_fast64_t boundDenominator = 1.0/storm::settings::generalSettings().getPrecision(); //we need to approx. the obtained bounds as rational numbers
            storm::storage::sparse::state_type subsystemState=0; //the subsystem uses other state indices
            for(storm::storage::sparse::state_type state : subsystem){
                uint_fast64_t boundNumerator = resultVector[subsystemState]*boundDenominator;
                storm::RationalFunction bound(boundNumerator);
                bound = bound/boundDenominator;
                //Todo: non-exact values might be problematic here...
                solver.add(storm::RationalFunction(stateProbVars[state]), boundRelation, bound);
                ++subsystemState;
            }
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::restrictProbabilityVariables(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType> const& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities, ParameterRegion const& region, storm::logic::ComparisonType const& compType){
            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "restricting Probability Variables is not supported for this type");
        }
        
        template<>
        bool SparseDtmcRegionModelChecker<storm::RationalFunction, double>::checkRegionOld(storm::logic::Formula const& formula, std::vector<ParameterRegion> parameterRegions){
            //Note: this is an 'experimental' implementation
            
            std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
                
            //Start with some preprocessing (inspired by computeUntilProbabilities...)
            //for simplicity we only support state formulas with eventually (e.g. P<0.5 [ F "target" ])
            //get the (sub)formulae and the vector of target states
            STORM_LOG_THROW(formula.isStateFormula(), storm::exceptions::IllegalArgumentException, "expected a stateFormula");
            STORM_LOG_THROW(formula.asStateFormula().isProbabilityOperatorFormula(), storm::exceptions::IllegalArgumentException, "expected a probabilityOperatorFormula");
            storm::logic::ProbabilityOperatorFormula const& probOpForm=formula.asStateFormula().asProbabilityOperatorFormula();
            STORM_LOG_THROW(probOpForm.hasBound(), storm::exceptions::IllegalArgumentException, "The formula has no bound");
            STORM_LOG_THROW(probOpForm.getSubformula().asPathFormula().isEventuallyFormula(), storm::exceptions::IllegalArgumentException, "expected an eventually subformula");
            storm::logic::EventuallyFormula const& eventuallyFormula = probOpForm.getSubformula().asPathFormula().asEventuallyFormula();
            std::unique_ptr<CheckResult> targetStatesResultPtr = eliminationModelChecker.check(eventuallyFormula.getSubformula());
            storm::storage::BitVector const& targetStates = targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector();
            // Do some sanity checks to establish some required properties.
            STORM_LOG_THROW(model.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
            storm::storage::sparse::state_type initialState = *model.getInitialStates().begin();
            // Then, compute the subset of states that has a probability of 0 or 1, respectively.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(model, storm::storage::BitVector(model.getNumberOfStates(),true), targetStates);
            storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
            storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
            // If the initial state is known to have either probability 0 or 1, we can directly return the result.
            if (model.getInitialStates().isDisjointFrom(maybeStates)) {
                STORM_LOG_DEBUG("The probability of all initial states was found in a preprocessing step.");
                double res= statesWithProbability0.get(*model.getInitialStates().begin()) ? 0.0 : 1.0;
                switch (probOpForm.getComparisonType()){
                    case storm::logic::ComparisonType::Greater:
                        return (res > probOpForm.getBound());
                    case storm::logic::ComparisonType::GreaterEqual:
                        return (res >= probOpForm.getBound());
                    case storm::logic::ComparisonType::Less:
                        return (res < probOpForm.getBound());
                    case storm::logic::ComparisonType::LessEqual:
                        return (res <= probOpForm.getBound());
                    default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
                }
            }
            // Determine the set of states that is reachable from the initial state without jumping over a target state.
            storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(model.getTransitionMatrix(), model.getInitialStates(), maybeStates, statesWithProbability1);
            // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
            maybeStates &= reachableStates;
            // Create a vector for the probabilities to go to a state with probability 1 in one step.
            std::vector<storm::RationalFunction> oneStepProbabilities = model.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
            // Determine the set of initial states of the sub-model.
            storm::storage::BitVector newInitialStates = model.getInitialStates() % maybeStates;
            // We then build the submatrix that only has the transitions of the maybe states.
            storm::storage::SparseMatrix<storm::RationalFunction> submatrix = model.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
            storm::storage::SparseMatrix<storm::RationalFunction> submatrixTransposed = submatrix.transpose();
            
            std::vector<std::size_t> statePriorities = eliminationModelChecker.getStatePriorities(submatrix, submatrixTransposed, newInitialStates, oneStepProbabilities);
            
            // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            FlexibleMatrix flexibleMatrix = eliminationModelChecker.getFlexibleSparseMatrix(submatrix);
            FlexibleMatrix flexibleBackwardTransitions = eliminationModelChecker.getFlexibleSparseMatrix(submatrixTransposed, true);
            
            std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
            
           // Create a bit vector that represents the current subsystem, i.e., states that we have not eliminated.
            storm::storage::BitVector subsystem = storm::storage::BitVector(submatrix.getRowCount(), true);
            eliminateStates(subsystem, flexibleMatrix, oneStepProbabilities, flexibleBackwardTransitions, newInitialStates, submatrix, statePriorities);
            
            std::chrono::high_resolution_clock::time_point timeStateElemEnd = std::chrono::high_resolution_clock::now();
            
            // SMT formulation of resulting pdtmc
            storm::expressions::ExpressionManager manager; //this manager will do nothing as we will use carl expressions
            storm::solver::Smt2SmtSolver solver(manager, true);
            // we will introduce a variable for every state which encodes the probability to reach a target state from this state.
            // we will store them as polynomials to easily use operations with rational functions
            std::vector<storm::RationalFunction::PolyType> stateProbVars(subsystem.size(), storm::RationalFunction::PolyType(0));
            // todo maybe introduce the parameters already at this point?
            formulateModelWithSMT(solver, stateProbVars, subsystem, flexibleMatrix, oneStepProbabilities);
            
            //the property should be satisfied in the initial state for all parameters.
            //this is equivalent to:
            //the negation of the property should not be satisfied for some parameter valuation.
            //Hence, we flip the comparison relation and later check whether all the constraints are unsat.
            storm::CompareRelation propertyCompRel;
            switch (probOpForm.getComparisonType()){
                case storm::logic::ComparisonType::Greater:
                    propertyCompRel=storm::CompareRelation::LEQ;
                    break;
                case storm::logic::ComparisonType::GreaterEqual:
                    propertyCompRel=storm::CompareRelation::LT;
                    break;
                case storm::logic::ComparisonType::Less:
                    propertyCompRel=storm::CompareRelation::GEQ;
                    break;
                case storm::logic::ComparisonType::LessEqual:
                    propertyCompRel=storm::CompareRelation::GT;
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
            }
            uint_fast64_t thresholdDenominator = 1.0/storm::settings::generalSettings().getPrecision();
            uint_fast64_t thresholdNumerator = probOpForm.getBound()*thresholdDenominator;
            storm::RationalFunction threshold(thresholdNumerator);
            threshold = threshold / thresholdDenominator;
            solver.add(storm::RationalFunction(stateProbVars[*newInitialStates.begin()]), propertyCompRel, threshold);
            
            //the bounds for the parameters
            solver.push();
            //STORM_LOG_THROW(parameterRegions.size()==1, storm::exceptions::NotImplementedException, "multiple regions not yet implemented");
            ParameterRegion region=parameterRegions[0];
            for(auto variable : region.getVariables()){
                storm::RawPolynomial lB(variable);
                lB -= region.getLowerBound(variable);
                solver.add(carl::Constraint<storm::RawPolynomial>(lB,storm::CompareRelation::GEQ));
                storm::RawPolynomial uB(variable);
                uB -= region.getUpperBound(variable);
                solver.add(carl::Constraint<storm::RawPolynomial>(uB,storm::CompareRelation::LEQ));
            }
            
            std::chrono::high_resolution_clock::time_point timeSmtFormulationEnd = std::chrono::high_resolution_clock::now();
            
            // find further restriction on probabilities
            restrictProbabilityVariables(solver,stateProbVars,subsystem,flexibleMatrix,oneStepProbabilities, parameterRegions[0], storm::logic::ComparisonType::Less); //probOpForm.getComparisonType());
            restrictProbabilityVariables(solver,stateProbVars,subsystem,flexibleMatrix,oneStepProbabilities, parameterRegions[0], storm::logic::ComparisonType::Greater);
            
            std::chrono::high_resolution_clock::time_point timeRestrictingEnd = std::chrono::high_resolution_clock::now();
            
            std::cout << "start solving ..." << std::endl;
            bool result;
                switch (solver.check()){
                case storm::solver::SmtSolver::CheckResult::Sat:
                    std::cout << "sat!" << std::endl;
                    result=false;
                    break;
                case storm::solver::SmtSolver::CheckResult::Unsat:
                    std::cout << "unsat!" << std::endl;
                    result=true;
                    break;
                case storm::solver::SmtSolver::CheckResult::Unknown:
                    std::cout << "unknown!" << std::endl;
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not solve the SMT-Problem (Check-result: Unknown)")
                    result=false;
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not solve the SMT-Problem (unexpected check-result)")
                    result=false;
            }
            
            std::chrono::high_resolution_clock::time_point timeSolvingEnd = std::chrono::high_resolution_clock::now();    
                
            std::chrono::high_resolution_clock::duration timePreprocessing = timePreprocessingEnd - timeStart;
            std::chrono::high_resolution_clock::duration timeStateElem = timeStateElemEnd - timePreprocessingEnd;
            std::chrono::high_resolution_clock::duration timeSmtFormulation = timeSmtFormulationEnd - timeStateElemEnd;
            std::chrono::high_resolution_clock::duration timeRestricting = timeRestrictingEnd - timeSmtFormulationEnd;
            std::chrono::high_resolution_clock::duration timeSolving = timeSolvingEnd- timeRestrictingEnd;
            std::chrono::high_resolution_clock::duration timeOverall = timeSolvingEnd - timeStart;
            std::chrono::milliseconds timePreprocessingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timePreprocessing);
            std::chrono::milliseconds timeStateElemInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeStateElem);
            std::chrono::milliseconds timeSmtFormulationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSmtFormulation);
            std::chrono::milliseconds timeRestrictingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeRestricting);
            std::chrono::milliseconds timeSolvingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSolving);
            std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);
            STORM_PRINT_AND_LOG(std::endl << "required time: " << timeOverallInMilliseconds.count() << "ms. Time Breakdown:" << std::endl);
            STORM_PRINT_AND_LOG("    * " << timePreprocessingInMilliseconds.count() << "ms for Preprocessing" << std::endl);
            STORM_PRINT_AND_LOG("    * " << timeStateElemInMilliseconds.count() << "ms for StateElemination" << std::endl);
            STORM_PRINT_AND_LOG("    * " << timeSmtFormulationInMilliseconds.count() << "ms for SmtFormulation" << std::endl);
            STORM_PRINT_AND_LOG("    * " << timeRestrictingInMilliseconds.count() << "ms for Restricting" << std::endl);
            STORM_PRINT_AND_LOG("    * " << timeSolvingInMilliseconds.count() << "ms for Solving" << std::endl);

            return result;
        }

        template<typename ParametricType, typename ConstantType>
        bool SparseDtmcRegionModelChecker<ParametricType, ConstantType>::checkRegionOld(storm::logic::Formula const& formula, std::vector<ParameterRegion> parameterRegions){
            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Region check is not supported for this type");
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
                outstream << "Statistic Region Model Checker Statistics Error: No formula specified." << std::endl; 
                return;
            }
            
            std::chrono::milliseconds timePreprocessingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timePreprocessing);
            std::chrono::milliseconds timeInitialStateEliminationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitialStateElimination);
            std::chrono::milliseconds timeComputeReachProbFunctionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeComputeReachProbFunction);
            std::chrono::milliseconds timeCheckRegionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeCheckRegion);
            std::chrono::milliseconds timeSammplingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSampling);
            std::chrono::milliseconds timeApproximationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeApproximation);
            std::chrono::milliseconds timeMDPBuildInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeMDPBuild);
            
            std::chrono::high_resolution_clock::duration timeOverall = timePreprocessing + timeCheckRegion; // + ...
            std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);
            
            std::size_t subsystemTransitions = this->sparseTransitions.getNonzeroEntryCount();
            for(auto const& transition : this->oneStepProbabilities){
                if(!this->parametricTypeComparator.isZero(transition)){
                    ++subsystemTransitions;
                }
            }
            
            outstream << std::endl << "Statistics Region Model Checker Statistics:" << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            outstream << "Model: " << this->model.getNumberOfStates() << " states, " << this->model.getNumberOfTransitions() << " transitions." << std::endl;
            outstream << "Reduced model: " << this->subsystem.getNumberOfSetBits() << " states, " << subsystemTransitions << "transitions" << std::endl;
            outstream << "Formula: " << *this->probabilityOperatorFormula << std::endl;
            outstream << (this->hasOnlyLinearFunctions ? "A" : "Not a") << "ll occuring functions in the model are linear" << std::endl;
            outstream << "Number of checked regions: " << this->numOfCheckedRegions << " with..." << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughSampling << " regions solved through sampling" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughApproximation << " regions solved through Approximation" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughSubsystemSmt << " regions solved through SubsystemSmt" << std::endl;
            outstream << "  " << this->numOfRegionsSolvedThroughFullSmt << " regions solved through FullSmt" << std::endl;
            outstream << "Running times:" << std::endl;
            outstream << "  " << timeOverallInMilliseconds.count() << "ms overall" << std::endl;
            outstream << "  " << timePreprocessingInMilliseconds.count() << "ms Preprocessing including... " << std::endl;
            outstream << "    " << timeInitialStateEliminationInMilliseconds.count() << "ms Initial state elimination including..." << std::endl;
            outstream << "      " << timeComputeReachProbFunctionInMilliseconds.count() << "ms to compute the reachability probability function" << std::endl;
            outstream << "  " << timeCheckRegionInMilliseconds.count() << "ms Region Check including... " << std::endl;
            outstream << "    " << timeSammplingInMilliseconds.count() << "ms Sampling " << std::endl;
            outstream << "    " << timeApproximationInMilliseconds.count() << "ms Approximation including... " << std::endl;
            outstream << "      " << timeMDPBuildInMilliseconds.count() << "ms to build the MDP" << std::endl;
            //outstream << "  " << timeInMilliseconds.count() << "ms " << std::endl;
            outstream << "-----------------------------------------------" << std::endl;
            
        }

        
#endif
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>;
#endif
        //note: for other template instantiations, add a rule for the typedefs of VariableType and BoundType
        
    } // namespace modelchecker
} // namespace storm
