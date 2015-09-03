/* 
 * File:   ApproximationModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:29 AM
 */

#include "src/modelchecker/region/ApproximationModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/vector.h"
#include "src/utility/regions.h"
#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::ApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, bool computeRewards) : computeRewards(computeRewards){
            //Start with the probabilities
            storm::storage::SparseMatrix<ConstantType> probabilityMatrix;
            std::vector<std::size_t> rowSubstitutions;// the substitution used in every row (required if rewards are computed)
            std::vector<std::size_t> matrixEntryToEvalTableMapping;// This vector will get an entry for every probability-matrix entry
            //for the corresponding matrix entry, it stores the corresponding entry in the probability evaluation table (more precisely: the position in that table).
            //We can later transform this mapping into the desired mapping with iterators
            const std::size_t constantEntryIndex=std::numeric_limits<std::size_t>::max(); //this value is stored in the matrixEntrytoEvalTableMapping for every constant matrix entry. (also used for rewards later)
            initializeProbabilities(parametricModel, probabilityMatrix, rowSubstitutions, matrixEntryToEvalTableMapping, constantEntryIndex);
            
            //Now consider rewards
            boost::optional<std::vector<ConstantType>> stateRewards;
            boost::optional<storm::storage::SparseMatrix<ConstantType>> transitionRewards;
            std::vector<std::size_t> stateRewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
            std::vector<std::size_t> transitionRewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
            if(this->computeRewards){
                initializeRewards(parametricModel, probabilityMatrix, rowSubstitutions, stateRewards, transitionRewards, stateRewardEntryToEvalTableMapping, transitionRewardEntryToEvalTableMapping, constantEntryIndex);
            }
            //Obtain other model ingredients and the approximation model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(stateRewards), std::move(transitionRewards), std::move(noChoiceLabeling));
            
            //translate the matrixEntryToEvalTableMapping into the actual probability mapping
            typename storm::storage::SparseMatrix<ConstantType>::index_type matrixRow=0;
            auto tableIndex=matrixEntryToEvalTableMapping.begin();
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                for (; matrixRow<this->model->getTransitionMatrix().getRowGroupIndices()[row+1]; ++matrixRow){
                    auto approxModelEntry = this->model->getTransitionMatrix().getRow(matrixRow).begin();
                    for(auto const& parEntry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(*tableIndex == constantEntryIndex){
                            approxModelEntry->setValue(storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parEntry.getValue())));
                        } else {
                            this->probabilityMapping.emplace_back(std::make_pair(&(std::get<2>(this->probabilityEvaluationTable[*tableIndex])), approxModelEntry));
                        }
                        ++approxModelEntry;
                        ++tableIndex;
                    }
                }
            }
            if(this->computeRewards){
                //the same for state and transition rewards
                auto approxModelStateRewardEntry = this->model->getStateRewardVector().begin();
                for (std::size_t const& tableIndex : stateRewardEntryToEvalTableMapping){
                    if(tableIndex != constantEntryIndex){
                        this->stateRewardMapping.emplace_back(std::make_tuple(&(std::get<2>(this->rewardEvaluationTable[tableIndex])), &(std::get<3>(this->rewardEvaluationTable[tableIndex])), approxModelStateRewardEntry));
                    }
                    ++approxModelStateRewardEntry;
                }
                auto approxModelTransitionRewardEntry = this->model->getTransitionRewardMatrix().begin();
                for (std::size_t const& tableIndex : transitionRewardEntryToEvalTableMapping){
                    this->transitionRewardMapping.emplace_back(std::make_tuple(&(std::get<2>(this->rewardEvaluationTable[tableIndex])), &(std::get<3>(this->rewardEvaluationTable[tableIndex])), approxModelTransitionRewardEntry));
                    ++approxModelTransitionRewardEntry;
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
                        if(this->parametricTypeComparator.isConstant(entry.getValue())){
                            matrixEntryToEvalTableMapping.emplace_back(constantEntryIndex);
                        } else {
                            ++numOfNonConstEntries;
                            std::size_t tableIndex=storm::utility::vector::findOrInsert(this->probabilityEvaluationTable, std::tuple<ParametricType, std::size_t, ConstantType>(entry.getValue(), substitutionIndex, storm::utility::zero<ConstantType>()));
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
                                                                                                               boost::optional<std::vector<ConstantType> >& stateRewards,
                                                                                                               boost::optional<storm::storage::SparseMatrix<ConstantType> >& transitionRewards,
                                                                                                               std::vector<std::size_t>& stateRewardEntryToEvalTableMapping,
                                                                                                               std::vector<std::size_t>& transitionRewardEntryToEvalTableMapping,
                                                                                                               std::size_t const& constantEntryIndex) {
            // run through the state reward vector of the parametric model.
            // Constant entries can be set directly.
            // For Parametric entries we have two cases:
            // (1) make state rewards if the reward is independent of parameters that occur in probability functions.
            // (2) make transition rewards otherwise.
            
            //stateRewards
            std::vector<ConstantType> stateRewardsAsVector(parametricModel.getNumberOfStates());
            std::size_t numOfNonConstStateRewEntries=0;
            //TransitionRewards
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(probabilityMatrix.getRowCount(), probabilityMatrix.getColumnCount(), 0, true, true, probabilityMatrix.getRowGroupCount());
            std::size_t numOfNonConstTransitonRewEntries=0;
            this->rewardSubstitutions.emplace_back(std::map<VariableType, TypeOfBound>()); //we want that the empty substitution is always the first one
            
            for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                matrixBuilder.newRowGroup(probabilityMatrix.getRowGroupIndices()[state]);
                std::set<VariableType> occurringRewVariables;
                std::set<VariableType> occurringProbVariables;
                bool makeStateReward=true;
                if(this->parametricTypeComparator.isConstant(parametricModel.getStateRewardVector()[state])){
                    stateRewardsAsVector[state]=storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parametricModel.getStateRewardVector()[state]));
                    stateRewardEntryToEvalTableMapping.emplace_back(constantEntryIndex);
                } else {
                    //reward depends on parameters. Lets find out if probability parameters occur here.
                    //If this is the case, the reward depends on the nondeterministic choices and should be given as transition rewards.
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
                        //Get the corresponding substitution and substitutionIndex
                        std::map<VariableType, TypeOfBound> rewardSubstitution;
                        for(auto const& rewardVar : occurringRewVariables){
                            rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                        }
                        std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->rewardSubstitutions, std::move(rewardSubstitution));
                        //insert table entry and dummy data in the stateRewardVector
                        std::size_t tableIndex=storm::utility::vector::findOrInsert(this->rewardEvaluationTable, std::tuple<ParametricType, std::size_t, ConstantType, ConstantType>(parametricModel.getStateRewardVector()[state], substitutionIndex, storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>()));
                        stateRewardEntryToEvalTableMapping.emplace_back(tableIndex);
                        stateRewardsAsVector[state]=storm::utility::zero<ConstantType>();
                        ++numOfNonConstStateRewEntries;
                    } else {
                        for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                            //Get the corresponding substitution and substitutionIndex
                            std::map<VariableType, TypeOfBound> rewardSubstitution;
                            for(auto const& rewardVar : occurringRewVariables){
                                typename std::map<VariableType, TypeOfBound>::iterator const substitutionIt=this->probabilitySubstitutions[rowSubstitutions[matrixRow]].find(rewardVar);
                                if(substitutionIt==this->probabilitySubstitutions[rowSubstitutions[matrixRow]].end()){
                                    rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                                } else {
                                    rewardSubstitution.insert(*substitutionIt);
                                }
                            }
                            std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->rewardSubstitutions, std::move(rewardSubstitution));
                            //insert table entries and dummy data
                            std::size_t tableIndex=storm::utility::vector::findOrInsert(this->rewardEvaluationTable, std::tuple<ParametricType, std::size_t, ConstantType, ConstantType>(parametricModel.getStateRewardVector()[state], substitutionIndex, storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>()));
                            for(auto const& matrixEntry : probabilityMatrix.getRow(matrixRow)){
                                transitionRewardEntryToEvalTableMapping.emplace_back(tableIndex);
                                matrixBuilder.addNextValue(matrixRow, matrixEntry.getColumn(), storm::utility::zero<ConstantType>());
                                ++numOfNonConstTransitonRewEntries;
                            }
                        }
                        stateRewardsAsVector[state]=storm::utility::zero<ConstantType>();
                        stateRewardEntryToEvalTableMapping.emplace_back(constantEntryIndex);
                    }
                }
            }
            stateRewards=std::move(stateRewardsAsVector);
            this->stateRewardMapping.reserve(numOfNonConstStateRewEntries);
            transitionRewards=matrixBuilder.build();
            this->transitionRewardMapping.reserve(numOfNonConstTransitonRewEntries);
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