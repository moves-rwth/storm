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
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::ApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel) : mapping(), evaluationTable(), substitutions() {
            // Run through the rows of the original model and obtain
            // (1) the different substitutions (this->substitutions) and the substitution used for every row
            std::vector<std::size_t> rowSubstitutions;
            // (2) the set of distinct pairs <Function, Substitution> 
            std::set<std::pair<ParametricType, std::size_t>> distinctFuncSubs;
            // (3) the MDP matrix with some dummy entries
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(0, parametricModel.getNumberOfStates(), 0, true, true, parametricModel.getNumberOfStates());
            typename storm::storage::SparseMatrix<ConstantType>::index_type approxModelRow=0;
            uint_fast64_t numOfNonConstEntries=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                matrixBuilder.newRowGroup(approxModelRow);
                // For (1): Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
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
                            auto pair=distinctFuncSubs.insert(std::make_pair(entry.getValue(), substitutionIndex));
                            ++numOfNonConstEntries;
                        }
                        matrixBuilder.addNextValue(approxModelRow, entry.getColumn(), dummyEntry);
                        dummyEntry=storm::utility::zero<ConstantType>(); 
                    }
                    ++approxModelRow;
                }
            }
            
            //Obtain other model ingredients and the approximation model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<ConstantType>> noStateRewards;
            boost::optional<storm::storage::SparseMatrix<ConstantType>> noTransitionRewards;
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Mdp<ConstantType>>(matrixBuilder.build(), std::move(labeling), std::move(noStateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
            
            //Get the evaluation table. Note that it remains sorted due to the fact that distinctFuncSubs is sorted
            this->evaluationTable.reserve(distinctFuncSubs.size());
            for(auto const& funcSub : distinctFuncSubs){
                this->evaluationTable.emplace_back(funcSub.first, funcSub.second, storm::utility::zero<ConstantType>());
            }
            
            //Fill in the entries for the mapping
            this->mapping.reserve(numOfNonConstEntries);
            approxModelRow=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                for (; approxModelRow<this->model->getTransitionMatrix().getRowGroupIndices()[row+1]; ++approxModelRow){
                    auto appEntry = this->model->getTransitionMatrix().getRow(approxModelRow).begin();
                    for(auto const& parEntry : parametricModel.getTransitionMatrix().getRow(row)){
                        if(this->parametricTypeComparator.isConstant(parEntry.getValue())){
                            appEntry->setValue(storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parEntry.getValue())));
                        }
                        else {
                            std::tuple<ParametricType, std::size_t, ConstantType> searchedTuple(parEntry.getValue(), rowSubstitutions[approxModelRow], storm::utility::zero<ConstantType>());
                            auto const tableIt= std::find(evaluationTable.begin(), evaluationTable.end(), searchedTuple);
                            //auto const tableIt= std::lower_bound(evaluationTable.begin(), evaluationTable.end(), searchedTuple);
                            //auto const& tableIt= std::lower_bound(evaluationTable.begin(), evaluationTable.end(), searchedTuple);
                            if(tableIt==evaluationTable.end()){
                                std::cout << "did not found tuple in the table: " << parEntry.getValue() << " substitution " << rowSubstitutions[approxModelRow] << " in parametric model at row " << row << " column " << parEntry.getColumn() << " approximation Row " << approxModelRow << std::endl;
                            }
                            STORM_LOG_THROW((*tableIt==searchedTuple),storm::exceptions::UnexpectedException, "Could not find the current tuple in the evaluationTable. Either the table is missing that tuple or it is not sorted properly");
                            mapping.emplace_back(std::make_pair(&(std::get<2>(*tableIt)), appEntry));
                        }
                        ++appEntry;
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
            for(auto& mappingPair : this->mapping){
                mappingPair.second->setValue(*(mappingPair.first));
            }
        }

        template<typename ParametricType, typename ConstantType>
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel::computeReachabilityProbabilities(storm::logic::OptimalityType const& optimalityType) {
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
            storm::modelchecker::SparseMdpPrctlModelChecker<ConstantType> modelChecker(*this->model);
            
            //perform model checking on the mdp
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula, false, optimalityType);
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }



#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::ApproximationModel;
#endif
    }
}