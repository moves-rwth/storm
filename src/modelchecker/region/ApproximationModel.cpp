/* 
 * File:   ApproximationModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:29 AM
 */
#include "src/modelchecker/region/ApproximationModel.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/ModelType.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/macros.h"
#include "src/utility/region.h"
#include "src/utility/vector.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
        
            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::ApproximationModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula) : formula(formula){
                if(this->formula->isProbabilityOperatorFormula()){
                    this->computeRewards=false;
                } else if(this->formula->isRewardOperatorFormula()){
                    this->computeRewards=true;
                    STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should be unique");
                    STORM_LOG_THROW(parametricModel.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should have state rewards only");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << this->formula << ". Approximation model only supports eventually or reachability reward formulae.");
                }
                //Start with the probabilities
                storm::storage::SparseMatrix<ConstantType> probabilityMatrix;
                std::vector<typename storm::storage::SparseMatrix<ConstantType>::index_type> approxRowGroupIndices;// Indicates which rows of the probabilityMatrix belong to a given row of the parametricModel
                std::vector<std::size_t> rowSubstitutions;// the substitution used in every row (required if rewards are computed)
                std::vector<ProbTableEntry*> matrixEntryToEvalTableMapping;// This vector will get an entry for every probability-matrix entry
                //for the corresponding matrix entry, it stores the corresponding entry in the probability evaluation table.
                //We can later transform this mapping into the desired mapping with iterators
                ProbTableEntry constantProbEntry; //this value is stored in the matrixEntrytoEvalTableMapping for every constant probability matrix entry. 
                initializeProbabilities(parametricModel, probabilityMatrix, approxRowGroupIndices, rowSubstitutions, matrixEntryToEvalTableMapping, &constantProbEntry);


                //Now consider rewards
                std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ConstantType>> rewardModels;
                std::vector<RewTableEntry*> rewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
                RewTableEntry constantRewEntry; //this value is stored in the rewardEntryToEvalTableMapping for every constant reward vector entry. 
                if(this->computeRewards){
                    std::vector<ConstantType> stateActionRewards;
                    initializeRewards(parametricModel, probabilityMatrix, approxRowGroupIndices, rowSubstitutions, stateActionRewards, rewardEntryToEvalTableMapping, &constantRewEntry);
                    rewardModels.insert(std::pair<std::string, storm::models::sparse::StandardRewardModel<ConstantType>>("", storm::models::sparse::StandardRewardModel<ConstantType>(boost::optional<std::vector<ConstantType>>(), boost::optional<std::vector<ConstantType>>(std::move(stateActionRewards)))));
                }
                //Obtain other model ingredients and the approximation model itself
                storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling; 
                switch(parametricModel.getType()){
                    case storm::models::ModelType::Dtmc:
                        this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(rewardModels), std::move(noChoiceLabeling));
                        break;
                    case storm::models::ModelType::Mdp:
                        //TODO: use a two-player-game here
                        this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(rewardModels), std::move(noChoiceLabeling));
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Tried to build an Approximation model for an unsupported model type");
                }
                

                //translate the matrixEntryToEvalTableMapping into the actual probability mapping
                typename storm::storage::SparseMatrix<ConstantType>::index_type matrixRow=0;
                auto tableEntry=matrixEntryToEvalTableMapping.begin();
                for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                    for (; matrixRow<approxRowGroupIndices[row+1]; ++matrixRow){
                        auto approxModelEntry = this->model->getTransitionMatrix().getRow(matrixRow).begin();
                        for(auto const& parEntry : parametricModel.getTransitionMatrix().getRow(row)){
                            if(*tableEntry == &constantProbEntry){
                                STORM_LOG_THROW(storm::utility::isConstant(parEntry.getValue()), storm::exceptions::UnexpectedException, "Expected a constant matrix entry but got a non-constant one.");
                                approxModelEntry->setValue(storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(parEntry.getValue())));
                            } else {
                                this->probabilityMapping.emplace_back(std::make_pair(&((*tableEntry)->second), approxModelEntry));
                            }
                            ++approxModelEntry;
                            ++tableEntry;
                        }
                    }
                }
                if(this->computeRewards){
                    //the same for rewards
                    auto approxModelRewardEntry = this->model->getUniqueRewardModel()->second.getStateActionRewardVector().begin();
                    for (auto tableEntry : rewardEntryToEvalTableMapping){
                        if(tableEntry != &constantRewEntry){
                            //insert pointer to minValue, pointer to maxValue, iterator to reward vector entry
                            this->rewardMapping.emplace_back(std::make_tuple(&(tableEntry->second.first), &(tableEntry->second.second), approxModelRewardEntry));
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

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeProbabilities(ParametricSparseModelType const& parametricModel,
                                                                                                                         storm::storage::SparseMatrix<ConstantType>& probabilityMatrix,
                                                                                                                         std::vector<typename storm::storage::SparseMatrix<ConstantType>::index_type>& approxRowGroupIndices,
                                                                                                                         std::vector<std::size_t>& rowSubstitutions,
                                                                                                                         std::vector<ProbTableEntry*>& matrixEntryToEvalTableMapping,
                                                                                                                         ProbTableEntry* constantEntry) {
                STORM_LOG_DEBUG("Approximation model initialization for probabilities");
                // Run through the rows of the original model and obtain the different substitutions, the probability evaluation table,
                // a probability matrix with some dummy entries, and the mapping between the two
                //TODO: if the parametricModel is nondeterministic, we will need a matrix for a two player game. For now, we assume the two players cooperate, i.e. we get a simple MDP
                bool customRowGroups = parametricModel.getTransitionMatrix().hasNontrivialRowGrouping();
                auto curParamRowGroup = parametricModel.getTransitionMatrix().getRowGroupIndices().begin();
                storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(0, //unknown number of rows
                                                                                parametricModel.getTransitionMatrix().getColumnCount(),
                                                                                0, //Unknown number of entries
                                                                                true, //force dimensions
                                                                                true, //will have custom row grouping
                                                                                parametricModel.getNumberOfStates());
                approxRowGroupIndices.reserve(parametricModel.getTransitionMatrix().getRowCount()+1);
                std::size_t numOfNonConstEntries=0;
                typename storm::storage::SparseMatrix<ConstantType>::index_type apprModelMatrixRow=0;
                for(typename storm::storage::SparseMatrix<ParametricType>::index_type paramRow=0; paramRow < parametricModel.getTransitionMatrix().getRowCount(); ++paramRow ){
                    approxRowGroupIndices.push_back(apprModelMatrixRow);
                    if(customRowGroups && paramRow==*curParamRowGroup){
                        //Only make a new row group if the parametric model matrix has a new row group at this position
                        matrixBuilder.newRowGroup(apprModelMatrixRow);
                        ++curParamRowGroup;
                    } else if (!customRowGroups){
                        matrixBuilder.newRowGroup(apprModelMatrixRow);
                    }
                    // Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
                    std::set<VariableType> occurringVariables;
                    for(auto const& entry : parametricModel.getTransitionMatrix().getRow(paramRow)){
                        storm::utility::region::gatherOccurringVariables(entry.getValue(), occurringVariables);
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
                        for(auto const& entry : parametricModel.getTransitionMatrix().getRow(paramRow)){
                            if(storm::utility::isConstant(entry.getValue())){
                                matrixEntryToEvalTableMapping.emplace_back(constantEntry);
                            } else {
                                ++numOfNonConstEntries;
                                auto evalTableIt = this->probabilityEvaluationTable.insert(ProbTableEntry(FunctionSubstitution(entry.getValue(), substitutionIndex), storm::utility::zero<ConstantType>())).first;
                                matrixEntryToEvalTableMapping.emplace_back(&(*evalTableIt));
                            }
                            matrixBuilder.addNextValue(apprModelMatrixRow, entry.getColumn(), dummyEntry);
                            dummyEntry=storm::utility::zero<ConstantType>(); 
                        }
                        ++apprModelMatrixRow;
                    }
                }
                this->probabilityMapping.reserve(numOfNonConstEntries);
                probabilityMatrix=matrixBuilder.build();
                approxRowGroupIndices.push_back(apprModelMatrixRow);
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeRewards(ParametricSparseModelType const& parametricModel,
                                                                                                                   storm::storage::SparseMatrix<ConstantType> const& probabilityMatrix,
                                                                                                                   std::vector<typename storm::storage::SparseMatrix<ConstantType>::index_type> const& approxRowGroupIndices,
                                                                                                                   std::vector<std::size_t> const& rowSubstitutions,
                                                                                                                   std::vector<ConstantType>& stateActionRewardVector,
                                                                                                                   std::vector<RewTableEntry*>& rewardEntryToEvalTableMapping,
                                                                                                                   RewTableEntry* constantEntry){
                STORM_LOG_DEBUG("Approximation model initialization for Rewards");
                STORM_LOG_THROW(parametricModel.getType()==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Rewards are only supported for DTMCs (yet)"); // Todo : check if this code also works for rewards on mdps (we should then consider state action rewards of the original model and approxRowGroupIndices)
                // run through the state reward vector of the parametric model.
                // Constant entries can be set directly.
                // For Parametric entries we set a dummy value and insert one entry to the rewardEntryEvalTableMapping
                stateActionRewardVector.reserve(probabilityMatrix.getRowCount());
                rewardEntryToEvalTableMapping.reserve(probabilityMatrix.getRowCount());
                std::size_t numOfNonConstRewEntries=0;

                for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                    std::set<VariableType> occurringRewVariables;
                    std::set<VariableType> occurringProbVariables;
                    if(storm::utility::isConstant(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state])){
                        ConstantType reward = storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state]));
                        //Add one of these entries for every row in the row group of state
                        for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                            stateActionRewardVector.emplace_back(reward);
                            rewardEntryToEvalTableMapping.emplace_back(constantEntry);
                        }
                    } else {
                        storm::utility::region::gatherOccurringVariables(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], occurringRewVariables);
                        //For each row in the row group of state, we get the corresponding substitution and tableIndex
                        // We might find out that the reward is independent of the probability variables (and will thus be independent of nondeterministic choices)
                        // In that case, the reward function and the substitution will not change and thus we can use the same table index
                        bool rewardDependsOnProbVars=true;
                        RewTableEntry* evalTablePtr;
                        for(auto matrixRow=probabilityMatrix.getRowGroupIndices()[state]; matrixRow<probabilityMatrix.getRowGroupIndices()[state+1]; ++matrixRow){
                            if(rewardDependsOnProbVars){ //always executed in first iteration
                                rewardDependsOnProbVars=false; //Assume that independent...
                                std::map<VariableType, TypeOfBound> rewardSubstitution;
                                for(auto const& rewardVar : occurringRewVariables){
                                    typename std::map<VariableType, TypeOfBound>::iterator const substitutionIt=this->probabilitySubstitutions[rowSubstitutions[matrixRow]].find(rewardVar);
                                    if(substitutionIt==this->probabilitySubstitutions[rowSubstitutions[matrixRow]].end()){
                                        rewardSubstitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                                    } else {
                                        rewardSubstitution.insert(*substitutionIt);
                                        rewardDependsOnProbVars=true; //.. assumption wrong
                                    }
                                }
                                std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->rewardSubstitutions, std::move(rewardSubstitution));
                                auto evalTableIt = this->rewardEvaluationTable.insert(
                                        RewTableEntry(FunctionSubstitution(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], substitutionIndex),
                                        std::pair<ConstantType, ConstantType>(storm::utility::zero<ConstantType>(), storm::utility::zero<ConstantType>()))
                                    ).first;
                                evalTablePtr=&(*evalTableIt);
                            }
                            //insert table entries and dummy data
                            stateActionRewardVector.emplace_back(storm::utility::zero<ConstantType>());
                            rewardEntryToEvalTableMapping.emplace_back(evalTablePtr);
                            ++numOfNonConstRewEntries;
                        }
                    }
                }
                this->rewardMapping.reserve(numOfNonConstRewEntries);
            }



            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::~ApproximationModel() {
                //Intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<storm::models::sparse::Model<ConstantType>> const& ApproximationModel<ParametricSparseModelType, ConstantType>::getModel() const {
                return this->model;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::instantiate(const ParameterRegion<ParametricType>& region) {
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
                    tableEntry.second=storm::utility::region::convertNumber<ConstantType>(
                            storm::utility::region::evaluateFunction(
                                    tableEntry.first.getFunction(),
                                    instantiatedSubs[tableEntry.first.getSubstitution()]
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
                        auto& funcSub = tableEntry.first;
                        auto& minMax = tableEntry.second;
                        minMax.first=storm::utility::infinity<ConstantType>();
                        minMax.second=storm::utility::zero<ConstantType>();
                        //Iterate over the different combinations of lower and upper bounds and update the min and max values
                        auto const& vertices=region.getVerticesOfRegion(this->choseOptimalRewardPars[funcSub.getSubstitution()]);
                        for(auto const& vertex : vertices){
                            //extend the substitution
                            for(auto const& vertexSub : vertex){
                                instantiatedRewardSubs[funcSub.getSubstitution()][vertexSub.first]=vertexSub.second;
                            }
                            ConstantType currValue = storm::utility::region::convertNumber<ConstantType>(
                                    storm::utility::region::evaluateFunction(
                                        funcSub.getFunction(),
                                        instantiatedRewardSubs[funcSub.getSubstitution()]
                                    )
                                );
                            minMax.first=std::min(minMax.first, currValue);
                            minMax.second=std::max(minMax.second, currValue);
                        }
                    }
                    //Note: the rewards are written to the model as soon as the optimality type is known (i.e. in computeValues)
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::unique_ptr<storm::modelchecker::CheckResult> ApproximationModel<ParametricSparseModelType, ConstantType>::computeValues(storm::solver::OptimizationDirection const& approximationOpDir) {
                std::unique_ptr<storm::modelchecker::AbstractModelChecker> modelChecker;
                switch(this->getModel()->getType()){
                    case storm::models::ModelType::Mdp:
                        modelChecker = std::make_unique<storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ConstantType>>>(*this->model->template as<storm::models::sparse::Mdp<ConstantType>>());
                        break;
                    case storm::models::ModelType::S2pg:
                        //TODO: instantiate right model checker here
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Approximation with two player games not yet implemented");
                        break;
                    default: 
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Tried to build an approximation model for an unsupported model type");
                }
                std::unique_ptr<storm::modelchecker::CheckResult> resultPtr;
                //TODO: use this when two player games are available
                boost::optional<storm::solver::OptimizationDirection> formulaOpDir = storm::logic::isLowerBound(this->formula->getComparisonType()) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;

                if(this->computeRewards){
                    //write the reward values into the model
                    switch(approximationOpDir){
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
                    //perform model checking
                    boost::optional<std::string> noRewardModelName; //it should be uniquely given
                    resultPtr = modelChecker->computeReachabilityRewards(this->formula->asRewardOperatorFormula().getSubformula().asReachabilityRewardFormula(), noRewardModelName, false, approximationOpDir);
                }
                else {
                    //perform model checking
                    resultPtr = modelChecker->computeEventuallyProbabilities(this->formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula(), false, approximationOpDir);
                }
                return resultPtr;
            }


#ifdef STORM_HAVE_CARL
            template class ApproximationModel<storm::models::sparse::Dtmc<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
            template class ApproximationModel<storm::models::sparse::Mdp<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
#endif
        } //namespace region
    }
}