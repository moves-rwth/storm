/* 
 * File:   ApproximationModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:29 AM
 */

#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/region/ApproximationModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/vector.h"
#include "src/utility/regions.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::ApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, std::shared_ptr<storm::logic::Formula> formula) : formula(formula){
            if(this->formula->isEventuallyFormula()){
                this->computeRewards=false;
            } else if(this->formula->isReachabilityRewardFormula()){
                this->computeRewards=true;
                STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should be unique");
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << this->formula << ". Sampling model only supports eventually or reachability reward formulae.");
            }
            //Start with the probabilities
            storm::storage::SparseMatrix<ConstantType> probabilityMatrix;
            std::vector<std::size_t> rowSubstitutions;// the substitution used in every row (required if rewards are computed)
            std::vector<std::size_t> matrixEntryToEvalTableMapping;// This vector will get an entry for every probability-matrix entry
            //for the corresponding matrix entry, it stores the corresponding entry in the probability evaluation table (more precisely: the position in that table).
            //We can later transform this mapping into the desired mapping with iterators
            const std::size_t constantEntryIndex=std::numeric_limits<std::size_t>::max(); //this value is stored in the matrixEntrytoEvalTableMapping for every constant matrix entry. (also used for rewards later)
            initializeProbabilities(parametricModel, probabilityMatrix, rowSubstitutions, matrixEntryToEvalTableMapping, constantEntryIndex);
            
            //Now consider rewards
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ConstantType>> rewardModels;
            std::vector<std::size_t> rewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
            std::vector<std::size_t> transitionRewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
            if(this->computeRewards){
                boost::optional<std::vector<ConstantType>> stateActionRewards;
                initializeRewards(parametricModel, probabilityMatrix, rowSubstitutions, stateActionRewards, rewardEntryToEvalTableMapping, transitionRewardEntryToEvalTableMapping, constantEntryIndex);
                rewardModels.insert(std::pair<std::string, storm::models::sparse::StandardRewardModel<ConstantType>>("", storm::models::sparse::StandardRewardModel<ConstantType>(boost::optional<std::vector<ConstantType>>(), std::move(stateActionRewards))));
            }
            //Obtain other model ingredients and the approximation model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(rewardModels), std::move(noChoiceLabeling));
            
            //translate the matrixEntryToEvalTableMapping into the actual probability mapping
            typename storm::storage::SparseMatrix<ConstantType>::index_type matrixRow=0;
            auto tableIndex=matrixEntryToEvalTableMapping.begin();
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                for (; matrixRow<this->model->getTransitionMatrix().getRowGroupIndices()[row+1]; ++matrixRow){
                    auto approxModelEntry = this->model->getTransitionMatrix().getRow(matrixRow).begin();
                    for(auto const& parEntry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(*tableIndex == constantEntryIndex){
                            approxModelEntry->setValue(storm::utility::regions::convertNumber<ConstantType>(storm::utility::regions::getConstantPart(parEntry.getValue())));
                        } else {
                            this->probabilityMapping.emplace_back(std::make_pair(&(std::get<2>(this->probabilityEvaluationTable[*tableIndex])), approxModelEntry));
                        }
                        ++approxModelEntry;
                        ++tableIndex;
                    }
                }
            }
            if(this->computeRewards){
                //the same for rewards
                auto approxModelRewardEntry = this->model->getUniqueRewardModel()->second.getStateActionRewardVector().begin();
                for (std::size_t const& tableIndex : rewardEntryToEvalTableMapping){
                    if(tableIndex != constantEntryIndex){
                        this->rewardMapping.emplace_back(std::make_tuple(&(std::get<2>(this->rewardEvaluationTable[tableIndex])), &(std::get<3>(this->rewardEvaluationTable[tableIndex])), approxModelRewardEntry));
                    }
                    ++approxModelRewardEntry;
                }
                //Get the sets of reward parameters that map to "CHOSEOPTIMAL".
                this->choseOptimalRewardPars = std::vector<std::set<VariableType>>(this->rewardSubstitutions.size());
                for(std::size_t index = 0; index<this->rewardSubstitutions.size(); ++index){
                    for(auto const& sub : this->rewardSubstitutions[index]){
                        if(sub.second==TypeOfBound::CHOSEOPTIMAL){
                            this->choseOptimalRewardPars[index].insert(sub.first);
                        }
                    }
                }
            }
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::initializeProbabilities(storm::models::sparse::Dtmc<ParametricType>const& parametricModel,
                                                                                                                     storm::storage::SparseMatrix<ConstantType>& probabilityMatrix,
                                                                                                                     std::vector<std::size_t>& rowSubstitutions,
                                                                                                                     std::vector<std::size_t>& matrixEntryToEvalTableMapping,
                                                                                                                     std::size_t const& constantEntryIndex) {
            // Run through the rows of the original model and obtain the different substitutions, the probability evaluation table,
            // an MDP probability matrix with some dummy entries, and the mapping between the two
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(0, parametricModel.getNumberOfStates(), 0, true, true, parametricModel.getNumberOfStates());
            this->probabilitySubstitutions.emplace_back(std::map<VariableType, TypeOfBound>()); //we want that the empty substitution is always the first one
            std::size_t numOfNonConstEntries=0;
            typename storm::storage::SparseMatrix<ConstantType>::index_type matrixRow=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                matrixBuilder.newRowGroup(matrixRow);
                // Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
                std::set<VariableType> occurringVariables;
                for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                    storm::utility::regions::gatherOccurringVariables(entry.getValue(), occurringVariables);
                }
                std::size_t numOfSubstitutions=1ull<<occurringVariables.size(); //=2^(#variables). Note that there is still 1 substitution when #variables==0 (i.e.,the empty substitution)
                for(uint_fast64_t substitutionId=0; substitutionId<numOfSubstitutions; ++substitutionId){
                    //compute actual substitution from substitutionId by interpreting the Id as a bit sequence.
                    //the occurringVariables.size() least significant bits of substitutionId will always represent the substitution that we have to consider
                    //(00...0 = lower bounds for all parameters, 11...1 = upper bounds for all parameters)
                    std::map<VariableType, TypeOfBound> currSubstitution;
                    std::size_t parameterIndex=0;
                    for(auto const& parameter : occurringVariables){
                        if((substitutionId>>parameterIndex)%2==0){
                            currSubstitution.insert(std::make_pair(parameter, TypeOfBound::LOWER));
                        }
                        else{
                            currSubstitution.insert(std::make_pair(parameter, TypeOfBound::UPPER));
                        }
                        ++parameterIndex;
                    }
                    std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->probabilitySubstitutions, std::move(currSubstitution));
                    rowSubstitutions.push_back(substitutionIndex);
                    //For every substitution, run again through the row and add an entry in matrixEntryToEvalTableMapping as well as dummy entries in the matrix
                    //Note that this is still executed once, even if no parameters occur.
                    ConstantType dummyEntry=storm::utility::one<ConstantType>();
                    for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(storm::utility::isConstant(entry.getValue())){
                            matrixEntryToEvalTableMapping.emplace_back(constantEntryIndex);
                        } else {
                            ++numOfNonConstEntries;
                            std::size_t tableIndex=storm::utility::vector::findOrInsert(this->probabilityEvaluationTable, std::tuple<std::size_t, ParametricType, ConstantType>(substitutionIndex, entry.getValue(), storm::utility::zero<ConstantType>()));
                            matrixEntryToEvalTableMapping.emplace_back(tableIndex);
                        }
                        matrixBuilder.addNextValue(matrixRow, entry.getColumn(), dummyEntry);
                        dummyEntry=storm::utility::zero<ConstantType>(); 
                    }
                    ++matrixRow;
                }
            }
            this->probabilityMapping.reserve(numOfNonConstEntries);
            probabilityMatrix=matrixBuilder.build();
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::initializeRewards(storm::models::sparse::Dtmc<ParametricType> const& parametricModel,
                                                                                                               storm::storage::SparseMatrix<ConstantType> const& probabilityMatrix,
                                                                                                               std::vector<std::size_t> const& rowSubstitutions,
                                                                                                               boost::optional<std::vector<ConstantType>>& stateActionRewardVector,
                                                                                                               std::vector<std::size_t>& rewardEntryToEvalTableMapping,
                                                                                                               std::vector<std::size_t>& transitionRewardEntryToEvalTableMapping,
                                                                                                               std::size_t const& constantEntryIndex) {
            // run through the state reward vector of the parametric model.
            // Constant entries can be set directly.
            // For Parametric entries we set a dummy value and add one entry to the rewardEntryEvalTableMapping
            std::vector<ConstantType> stateActionRewardsAsVector;
            stateActionRewardsAsVector.reserve(probabilityMatrix.getRowCount());
            rewardEntryToEvalTableMapping.reserve(probabilityMatrix.getRowCount());
            std::size_t numOfNonConstRewEntries=0;
            this->rewardSubstitutions.emplace_back(std::map<VariableType, TypeOfBound>()); //we want that the empty substitution is always the first one
                
            for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                std::set<VariableType> occurringRewVariables;
                std::set<VariableType> occurringProbVariables;
                std::size_t evalTableIndex;
                if(storm::utility::isConstant(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state])){
                    ConstantType reward = storm::utility::regions::convertNumber<ConstantType>(storm::utility::regions::getConstantPart(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state]));
                    //Add one of these entries for every row in the row group of state
                    for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                        stateActionRewardsAsVector.emplace_back(reward);
                        rewardEntryToEvalTableMapping.emplace_back(constantEntryIndex);
                    }
                    evalTableIndex = constantEntryIndex;
                } else {
                    storm::utility::regions::gatherOccurringVariables(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], occurringRewVariables);
                    //For each row in the row group of state, we get the corresponding substitution, substitutionIndex and tableIndex
                    // We might find out that the reward is independent of the probability variables (and will thus be independent of nondeterministic choices)
                    // In that case, the reward function and the substitution will not change and thus we can use the same table index
                    bool rewardDependsOnProbVars=true;
                    std::size_t tableIndex;
                    for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                        if(rewardDependsOnProbVars){ //always executed in first iteration
                            rewardDependsOnProbVars=false;
                            std::map<VariableType, TypeOfBound> rewardSubstitution;
                            for(auto const& rewardVar : occurringRewVariables){
                                typename std::map<VariableType, TypeOfBound>::iterator const substitutionIt=this->probabilitySubstitutions[rowSubstitutions[matrixRow]].find(rewardVar);
                                if(substitutionIt==this->probabilitySubstitutions[rowSubstitutions[matrixRow]].end()){
                                    rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                                } else {
                                    rewardSubstitution.insert(*substitutionIt);
                                    rewardDependsOnProbVars=true;
                                }
                            }
                            std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->rewardSubstitutions, std::move(rewardSubstitution));
                            tableIndex=storm::utility::vector::findOrInsert(this->rewardEvaluationTable, std::tuple<std::size_t, ParametricType, ConstantType, ConstantType>(substitutionIndex, parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>()));
                        }
                        //insert table entries and dummy data
                        stateActionRewardsAsVector.emplace_back(storm::utility::zero<ConstantType>());
                        rewardEntryToEvalTableMapping.emplace_back(tableIndex);
                        ++numOfNonConstRewEntries;
                    }
                }
            }
            stateActionRewardVector=std::move(stateActionRewardsAsVector);
            this->rewardMapping.reserve(numOfNonConstRewEntries);
        }



        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::~ApproximationModel() {
            //Intentionally left empty
        }

        template<typename ParametricType, typename ConstantType>
        std::shared_ptr<storm::models::sparse::Mdp<ConstantType>> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::getModel() const {
            return this->model;
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::instantiate(const ParameterRegion& region) {
            //Instantiate the substitutions
            std::vector<std::map<VariableType, CoefficientType>> instantiatedSubs(this->probabilitySubstitutions.size());
            for(uint_fast64_t substitutionIndex=0; substitutionIndex<this->probabilitySubstitutions.size(); ++substitutionIndex){
                for(std::pair<VariableType, TypeOfBound> const& sub : this->probabilitySubstitutions[substitutionIndex]){
                    switch(sub.second){
                        case TypeOfBound::LOWER:
                            instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getLowerBound(sub.first)));
                            break;
                        case TypeOfBound::UPPER:
                            instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getUpperBound(sub.first)));
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of Bound");
                    }
                }
            }
            //write entries into evaluation table
            for(auto& tableEntry : this->probabilityEvaluationTable){
                std::get<2>(tableEntry)=storm::utility::regions::convertNumber<ConstantType>(
                        storm::utility::regions::evaluateFunction(
                                std::get<1>(tableEntry),
                                instantiatedSubs[std::get<0>(tableEntry)]
                            )
                        );
            }
            //write the instantiated values to the matrix according to the mapping
            for(auto& mappingPair : this->probabilityMapping){
                mappingPair.second->setValue(*(mappingPair.first));
            }
            
            if(this->computeRewards){
                //Instantiate the substitutions
                std::vector<std::map<VariableType, CoefficientType>> instantiatedRewardSubs(this->rewardSubstitutions.size());
                for(uint_fast64_t substitutionIndex=0; substitutionIndex<this->rewardSubstitutions.size(); ++substitutionIndex){
                    for(std::pair<VariableType, TypeOfBound> const& sub : this->rewardSubstitutions[substitutionIndex]){
                        switch(sub.second){
                            case TypeOfBound::LOWER:
                                instantiatedRewardSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getLowerBound(sub.first)));
                                break;
                            case TypeOfBound::UPPER:
                                instantiatedRewardSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getUpperBound(sub.first)));
                                break;
                            case TypeOfBound::CHOSEOPTIMAL:
                                //Insert some dummy value
                                instantiatedRewardSubs[substitutionIndex].insert(std::make_pair(sub.first, storm::utility::zero<ConstantType>()));
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of Bound when instantiating a reward substitution. Index: " << substitutionIndex << " TypeOfBound: "<< ((int)sub.second));
                        }
                    }
                }
                //write entries into evaluation table
                for(auto& tableEntry : this->rewardEvaluationTable){
                    ConstantType minValue=storm::utility::infinity<ConstantType>();
                    ConstantType maxValue=storm::utility::zero<ConstantType>();
                    //Iterate over the different combinations of lower and upper bounds and update the min/max values
                    auto const& vertices=region.getVerticesOfRegion(this->choseOptimalRewardPars[std::get<0>(tableEntry)]);
                    for(auto const& vertex : vertices){
                        //extend the substitution
                        for(auto const& sub : vertex){
                            instantiatedRewardSubs[std::get<0>(tableEntry)][sub.first]=sub.second;
                        }
                        ConstantType currValue = storm::utility::regions::convertNumber<ConstantType>(
                                storm::utility::regions::evaluateFunction(
                                    std::get<1>(tableEntry),
                                    instantiatedRewardSubs[std::get<0>(tableEntry)]
                                )
                            );
                        minValue=std::min(minValue, currValue);
                        maxValue=std::max(maxValue, currValue);
                    }
                    std::get<2>(tableEntry)= minValue;
                    std::get<3>(tableEntry)= maxValue;
                }
                //Note: the rewards are written to the model as soon as the optimality type is known (i.e. in computeValues)
            }
        }

        template<typename ParametricType, typename ConstantType>
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::computeValues(storm::solver::OptimizationDirection const& optDir) {
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ConstantType>> modelChecker(*this->model);
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr;
            
            if(this->computeRewards){
                //write the reward values into the model
                switch(optDir){
                    case storm::solver::OptimizationDirection::Minimize:
                        for(auto& mappingTriple : this->rewardMapping){
                            *std::get<2>(mappingTriple)=*std::get<0>(mappingTriple);
                        }
                        break;
                    case storm::solver::OptimizationDirection::Maximize:
                        for(auto& mappingTriple : this->rewardMapping){
                            *std::get<2>(mappingTriple)=*std::get<1>(mappingTriple);
                        }
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given optimality Type is not supported.");
                }
                //perform model checking on the mdp
                boost::optional<std::string> noRewardModelName; //it should be uniquely given
                resultPtr = modelChecker.computeReachabilityRewards(this->formula->asReachabilityRewardFormula(), noRewardModelName, false, optDir);
            }
            else {
                //perform model checking on the mdp
                resultPtr = modelChecker.computeEventuallyProbabilities(this->formula->asEventuallyFormula(), false, optDir);
            }
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }


#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::ApproximationModel;
#endif
    }
}