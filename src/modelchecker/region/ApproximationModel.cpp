/* 
 * File:   ApproximationModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:29 AM
 */

#include "src/modelchecker/region/ApproximationModel.h"
#include "src/modelchecker/region/ParameterRegion.h"
#include "modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::ApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, bool computeRewards) : computeRewards(computeRewards){
            // Run through the rows of the original model and obtain
            // (1) the different substitutions (this->substitutions) and the substitution used for every row
            std::vector<std::size_t> rowSubstitutions;
            this->substitutions.emplace_back(std::map<VariableType, TypeOfBound>()); //we want that the empty substitution is always the first one
            // (2) the set of distinct pairs <Function, Substitution> 
            std::set<std::pair<ParametricType, std::size_t>> distinctFuncSubs;
            // (3) the MDP Probability matrix with some dummy entries
            storm::storage::SparseMatrixBuilder<ConstantType> probabilityMatrixBuilder(0, parametricModel.getNumberOfStates(), 0, true, true, parametricModel.getNumberOfStates());
            typename storm::storage::SparseMatrix<ConstantType>::index_type probMatrixRow=0;
            std::size_t numOfNonConstProbEntries=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                probabilityMatrixBuilder.newRowGroup(probMatrixRow);
                // For (1): Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
                std::set<VariableType> occurringProbVariables;
                for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                    storm::utility::regions::gatherOccurringVariables(entry.getValue(), occurringProbVariables);
                }
                std::size_t numOfSubstitutions=1ull<<occurringProbVariables.size(); //=2^(#variables). Note that there is still 1 substitution when #variables==0 (i.e.,the empty substitution)
                for(uint_fast64_t substitutionId=0; substitutionId<numOfSubstitutions; ++substitutionId){
                    //compute actual substitution from substitutionId by interpreting the Id as a bit sequence.
                    //the occurringProbVariables.size() least significant bits of substitutionId will always represent the substitution that we have to consider
                    //(00...0 = lower bounds for all parameters, 11...1 = upper bounds for all parameters)
                    std::map<VariableType, TypeOfBound> currSubstitution;
                    std::size_t parameterIndex=0;
                    for(auto const& parameter : occurringProbVariables){
                        if((substitutionId>>parameterIndex)%2==0){
                            currSubstitution.insert(std::make_pair(parameter, TypeOfBound::LOWER));
                        }
                        else{
                            currSubstitution.insert(std::make_pair(parameter, TypeOfBound::UPPER));
                        }
                        ++parameterIndex;
                    }
                    //Find the current substitution in this->substitutions (add it if we see this substitution the first time)
                    std::size_t substitutionIndex = std::find(this->substitutions.begin(),this->substitutions.end(), currSubstitution) - this->substitutions.begin();
                    if(substitutionIndex==this->substitutions.size()){
                        this->substitutions.emplace_back(std::move(currSubstitution));
                    }
                    rowSubstitutions.push_back(substitutionIndex);
                    //Run again through the row and...
                    //For (2): add pair <function, substitution> for the occuring functions and the current substitution
                    //For (3): add a row with dummy entries for the current substitution
                    //Note that this is still executed even if no variables occur. However, we will not put any <function, substitution> pair into distninctFuncSubs if substitution is empty.
                    ConstantType dummyEntry=storm::utility::one<ConstantType>();
                    for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                            distinctFuncSubs.insert(std::make_pair(entry.getValue(), substitutionIndex));
                            ++numOfNonConstProbEntries;
                        }
                        probabilityMatrixBuilder.addNextValue(probMatrixRow, entry.getColumn(), dummyEntry);
                        dummyEntry=storm::utility::zero<ConstantType>(); 
                    }
                    ++probMatrixRow;
                }
            }
            storm::storage::SparseMatrix<ConstantType> probabilityMatrix(probabilityMatrixBuilder.build());
            this->probabilityMapping.reserve(numOfNonConstProbEntries);
            
            // Now obtain transition and staterewards (if required)
            boost::optional<std::vector<ConstantType>> stateRewards;
            boost::optional<storm::storage::SparseMatrix<ConstantType>> transitionRewards;
            std::vector<std::size_t> stateRewardSubstituions;
            std::vector<std::size_t> transitionRewardRowSubstitutions;
            std::set<std::pair<ParametricType, std::size_t>> distinctRewardFuncSubs;
            if(this->computeRewards){
                this->rewardSubstitutions.emplace_back(std::map<VariableType, TypeOfBound>()); //we want that the empty substitution is always the first one
                //stateRewards
                std::vector<ConstantType> stateRewardsAsVector(parametricModel.getNumberOfStates());
                stateRewardSubstituions = std::vector<std::size_t>(parametricModel.getNumberOfStates(),0); //init with empty substitution (=0)
                std::size_t numOfNonConstStateRewEntries=0;
                //TransitionRewards
                storm::storage::SparseMatrixBuilder<ConstantType> rewardMatrixBuilder(probabilityMatrix.getRowCount(), probabilityMatrix.getColumnCount(), 0, true, true, probabilityMatrix.getRowGroupCount());
                transitionRewardRowSubstitutions = std::vector<std::size_t>(rowSubstitutions.size(),0); //init with empty substitution (=0)
                std::size_t numOfNonConstTransitonRewEntries=0;
                for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                    rewardMatrixBuilder.newRowGroup(probabilityMatrix.getRowGroupIndices()[state]);
                    std::set<VariableType> occurringRewVariables;
                    std::set<VariableType> occurringProbVariables;
                    bool makeStateReward=true; //we make state reward whenever no probability parameter occurs in the state reward function
                    if(this->parametricTypeComparator.isConstant(parametricModel.getStateRewardVector()[state])){
                        //constant reward for this state
                        stateRewardsAsVector[state]=storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parametricModel.getStateRewardVector()[state]));
                    } else {
                        //reward depends on parameters. Lets find out if also probability parameters occur here.
                        //If this is the case, the reward depends on the nondeterministic choices and should be given as transition rewards.
                        stateRewardsAsVector[state]=storm::utility::zero<ConstantType>();
                        storm::utility::regions::gatherOccurringVariables(parametricModel.getStateRewardVector()[state], occurringRewVariables);
                        for(auto const& entry : parametricModel.getTransitionMatrix().getRow(state)){
                            storm::utility::regions::gatherOccurringVariables(entry.getValue(), occurringProbVariables);
                        }
                        for( auto const& rewVar : occurringRewVariables){
                            if(occurringProbVariables.find(rewVar)!=occurringProbVariables.end()){
                                makeStateReward=false;
                                break;
                            }
                        }
                        if(makeStateReward){
                            std::map<VariableType, TypeOfBound> rewardSubstitution;
                            for(auto const& rewardVar : occurringRewVariables){
                                rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                            }
                            //Find the substitution in this->rewardSubstitutions (add it if we see this substitution the first time)
                            std::size_t substitutionIndex = std::find(this->rewardSubstitutions.begin(),this->rewardSubstitutions.end(), rewardSubstitution) - this->rewardSubstitutions.begin();
                            if(substitutionIndex==this->rewardSubstitutions.size()){
                                this->rewardSubstitutions.emplace_back(std::move(rewardSubstitution));
                            }
                            distinctRewardFuncSubs.insert(std::make_pair(parametricModel.getStateRewardVector()[state], substitutionIndex));
                            ++numOfNonConstStateRewEntries;
                            stateRewardSubstituions[state]=substitutionIndex;
                        } else {
                            for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                                //Reserve space for transition rewards
                                for(auto const& matrixEntry : probabilityMatrix.getRow(matrixRow)){
                                    rewardMatrixBuilder.addNextValue(matrixRow,matrixEntry.getColumn(),storm::utility::zero<ConstantType>());
                                }
                                //Get the corresponding substitution
                                std::map<VariableType, TypeOfBound> rewardSubstitution;
                                for(auto const& rewardVar : occurringRewVariables){
                                    typename std::map<VariableType, TypeOfBound>::iterator const substitutionIt=this->substitutions[rowSubstitutions[matrixRow]].find(rewardVar);
                                    if(substitutionIt==this->substitutions[rowSubstitutions[matrixRow]].end()){
                                        rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                                    } else {
                                        rewardSubstitution.insert(*substitutionIt);
                                    }
                                }
                                //Find the substitution in this->rewardSubstitutions (add it if we see this substitution the first time)
                                std::size_t substitutionIndex = std::find(this->rewardSubstitutions.begin(),this->rewardSubstitutions.end(), rewardSubstitution) - this->rewardSubstitutions.begin();
                                if(substitutionIndex==this->rewardSubstitutions.size()){
                                    this->rewardSubstitutions.emplace_back(std::move(rewardSubstitution));
                                }
                                distinctRewardFuncSubs.insert(std::make_pair(parametricModel.getStateRewardVector()[state], substitutionIndex));
                                ++numOfNonConstTransitonRewEntries;
                                transitionRewardRowSubstitutions[matrixRow]=substitutionIndex;
                            }
                        }
                    }
                }
                stateRewards=std::move(stateRewardsAsVector);
                this->stateRewardMapping.reserve(numOfNonConstStateRewEntries);
                transitionRewards=rewardMatrixBuilder.build();
                this->transitionRewardMapping.reserve(numOfNonConstTransitonRewEntries);
            }
            //Obtain other model ingredients and the approximation model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(stateRewards), std::move(transitionRewards), std::move(noChoiceLabeling));
            
            //Get the (probability) evaluation table. Note that it remains sorted due to the fact that distinctFuncSubs is sorted
            this->evaluationTable.reserve(distinctFuncSubs.size());
            for(auto const& funcSub : distinctFuncSubs){
                this->evaluationTable.emplace_back(funcSub.first, funcSub.second, storm::utility::zero<ConstantType>());
            }
            //Fill in the entries for the probability mapping
            probMatrixRow=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                for (; probMatrixRow<this->model->getTransitionMatrix().getRowGroupIndices()[row+1]; ++probMatrixRow){
                    auto appEntry = this->model->getTransitionMatrix().getRow(probMatrixRow).begin();
                    for(auto const& parEntry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(this->parametricTypeComparator.isConstant(parEntry.getValue())){
                            appEntry->setValue(storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parEntry.getValue())));
                        } else {
                            std::tuple<ParametricType, std::size_t, ConstantType> searchedTuple(parEntry.getValue(), rowSubstitutions[probMatrixRow], storm::utility::zero<ConstantType>());
                            auto const tableIt= std::find(this->evaluationTable.begin(), this->evaluationTable.end(), searchedTuple);
                            STORM_LOG_THROW((*tableIt==searchedTuple),storm::exceptions::UnexpectedException, "Could not find the current tuple in the evaluationTable. Either the table is missing that tuple or it is not sorted properly");
                            this->probabilityMapping.emplace_back(std::make_pair(&(std::get<2>(*tableIt)), appEntry));
                        }
                        ++appEntry;
                    }
                }
            }
            if(this->computeRewards){
                //Get the (reward) evaluation table. Note that it remains sorted due to the fact that distinctRewFuncSubs is sorted
                this->rewardEvaluationTable.reserve(distinctRewardFuncSubs.size());
                for(auto const& funcSub : distinctRewardFuncSubs){
                    this->rewardEvaluationTable.emplace_back(funcSub.first, funcSub.second, storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>());
                }
                
                for (std::size_t state=0; state< parametricModel.getNumberOfStates(); ++state){
                    //check if the state reward is not constant
                    if(stateRewardSubstituions[state]!=0){
                        std::tuple<ParametricType, std::size_t, ConstantType, ConstantType> searchedTuple(parametricModel.getStateRewardVector()[state], stateRewardSubstituions[state], storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>());
                        auto const tableIt= std::find(this->rewardEvaluationTable.begin(), this->rewardEvaluationTable.end(), searchedTuple);
                        STORM_LOG_THROW((*tableIt==searchedTuple),storm::exceptions::UnexpectedException, "Could not find the current tuple in the rewardEvaluationTable. Either the table is missing that tuple or it is not sorted properly (stateRewards)");
                        this->stateRewardMapping.emplace_back(std::make_tuple(&(std::get<2>(*tableIt)), &(std::get<3>(*tableIt)), &(this->model->getStateRewardVector()[state])));
                    }
                    //check if transitionrewards are not constant
                    for(auto matrixRow=this->model->getTransitionRewardMatrix().getRowGroupIndices()[state]; matrixRow<this->model->getTransitionRewardMatrix().getRowGroupIndices()[state+1]; ++matrixRow){
                        if(transitionRewardRowSubstitutions[matrixRow]!=0){
                            //Note that all transitions of the same row will have the same reward value
                            std::tuple<ParametricType, std::size_t, ConstantType, ConstantType> searchedTuple(parametricModel.getStateRewardVector()[state], transitionRewardRowSubstitutions[matrixRow], storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>());
                            auto const tableIt= std::find(this->rewardEvaluationTable.begin(), this->rewardEvaluationTable.end(), searchedTuple);
                            STORM_LOG_THROW((*tableIt==searchedTuple),storm::exceptions::UnexpectedException, "Could not find the current tuple in the rewardEvaluationTable. Either the table is missing that tuple or it is not sorted properly (transitionRewards)");
                            for(auto transitionMatrixEntry=this->model->getTransitionRewardMatrix().getRow(matrixRow).begin(); transitionMatrixEntry<this->model->getTransitionRewardMatrix().getRow(matrixRow).end(); ++transitionMatrixEntry){
                                this->transitionRewardMapping.emplace_back(std::make_tuple(&(std::get<2>(*tableIt)), &(std::get<3>(*tableIt)), transitionMatrixEntry));
                            }
                        }
                    }
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
            std::vector<std::map<VariableType, CoefficientType>> instantiatedSubs(this->substitutions.size());
            for(uint_fast64_t substitutionIndex=0; substitutionIndex<this->substitutions.size(); ++substitutionIndex){
                for(std::pair<VariableType, TypeOfBound> const& sub : this->substitutions[substitutionIndex]){
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
            for(auto& tableEntry : this->evaluationTable){
                std::get<2>(tableEntry)=storm::utility::regions::convertNumber<CoefficientType, ConstantType>(
                        storm::utility::regions::evaluateFunction(
                                std::get<0>(tableEntry),
                                instantiatedSubs[std::get<1>(tableEntry)]
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
                    auto const& vertices=region.getVerticesOfRegion(this->choseOptimalRewardPars[std::get<1>(tableEntry)]);
                    for(auto const& vertex : vertices){
                        //extend the substitution
                        for(auto const& sub : vertex){
                            instantiatedRewardSubs[std::get<1>(tableEntry)][sub.first]=sub.second;
                        }
                        ConstantType currValue = storm::utility::regions::convertNumber<CoefficientType, ConstantType>(
                                storm::utility::regions::evaluateFunction(
                                    std::get<0>(tableEntry),
                                    instantiatedRewardSubs[std::get<1>(tableEntry)]
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
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::computeValues(storm::logic::OptimalityType const& optimalityType) {
            
            storm::modelchecker::SparseMdpPrctlModelChecker<ConstantType> modelChecker(*this->model);
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr;
            if(this->computeRewards){
                storm::logic::ReachabilityRewardFormula reachRewFormula(targetFormulaPtr);
                //write the reward values into the model
                switch(optimalityType){
                    case storm::logic::OptimalityType::Minimize:
                        for(auto& mappingTriple : this->stateRewardMapping){
                            *std::get<2>(mappingTriple)=*std::get<0>(mappingTriple);
                        }
                        for(auto& mappingTriple : this->transitionRewardMapping){
                            std::get<2>(mappingTriple)->setValue(*(std::get<0>(mappingTriple)));
                        }
                        break;
                    case storm::logic::OptimalityType::Maximize:
                        for(auto& mappingTriple : this->stateRewardMapping){
                            *std::get<2>(mappingTriple)=*std::get<1>(mappingTriple);
                        }
                        for(auto& mappingTriple : this->transitionRewardMapping){
                            std::get<2>(mappingTriple)->setValue(*(std::get<1>(mappingTriple)));
                        }
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given optimality Type is not supported.");
                }
                //perform model checking on the mdp
                resultPtr = modelChecker.computeReachabilityRewards(reachRewFormula, false, optimalityType);
            }
            else {
                storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
                //perform model checking on the mdp
                resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula, false, optimalityType);
            }
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }


#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::ApproximationModel;
#endif
    }
}