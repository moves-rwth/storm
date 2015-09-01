/* 
 * File:   SamplingModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:31 AM
 */

#include "src/modelchecker/region/SamplingModel.h"
#include "modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::SamplingModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, bool computeRewards) : probabilityMapping(), stateRewardMapping(), evaluationTable(), computeRewards(computeRewards){
            // Run through the rows of the original model and obtain the set of distinct functions as well as a matrix with dummy entries
            std::set<ParametricType> functionSet;
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(parametricModel.getNumberOfStates(), parametricModel.getNumberOfStates(), parametricModel.getTransitionMatrix().getEntryCount());
            std::size_t numOfNonConstProbEntries=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                ConstantType dummyEntry=storm::utility::one<ConstantType>();
                for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                    if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                        functionSet.insert(entry.getValue());
                        ++numOfNonConstProbEntries;
                    }
                    matrixBuilder.addNextValue(row,entry.getColumn(), dummyEntry);
                    dummyEntry=storm::utility::zero<ConstantType>();
                }
            }
            this->probabilityMapping.reserve(numOfNonConstProbEntries);
            
            //Obtain other model ingredients and the sampling model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<ConstantType>> stateRewards;
            if(this->computeRewards){
                std::size_t numOfNonConstRewEntries=0;
                std::vector<ConstantType> stateRewardsAsVector(parametricModel.getNumberOfStates());
                for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                    if(this->parametricTypeComparator.isConstant(parametricModel.getStateRewardVector()[state])){
                        stateRewardsAsVector[state] = storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parametricModel.getStateRewardVector()[state]));
                    } else {
                        stateRewardsAsVector[state] = storm::utility::zero<ConstantType>();
                        functionSet.insert(parametricModel.getStateRewardVector()[state]);
                        ++numOfNonConstRewEntries;
                    }
                }
                stateRewards=std::move(stateRewardsAsVector);
                this->stateRewardMapping.reserve(numOfNonConstRewEntries);
            }
            boost::optional<storm::storage::SparseMatrix<ConstantType>> noTransitionRewards;
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Dtmc<ConstantType>>(matrixBuilder.build(), std::move(labeling), std::move(stateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
            
            //Get the evaluation table. Note that it remains sorted due to the fact that probFunctionSet is sorted
            this->evaluationTable.reserve(functionSet.size());
            for(auto const& func : functionSet){
                this->evaluationTable.emplace_back(func, storm::utility::zero<ConstantType>());
            }
            
            //Fill in the entries for the mappings
            auto samEntry = this->model->getTransitionMatrix().begin();
            for(auto const& parEntry : parametricModel.getTransitionMatrix()){
                if(this->parametricTypeComparator.isConstant(parEntry.getValue())){
                    samEntry->setValue(storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart(parEntry.getValue())));
                }
                else {
                    std::pair<ParametricType, ConstantType> searchedPair(parEntry.getValue(), storm::utility::zero<ConstantType>());
                    auto const tableIt= std::lower_bound(evaluationTable.begin(), evaluationTable.end(), searchedPair);
                    STORM_LOG_THROW((*tableIt==searchedPair), storm::exceptions::UnexpectedException, "Could not find the current pair in the evaluationTable. Either the table is missing that pair or it is not sorted properly");
                    this->probabilityMapping.emplace_back(std::make_pair(&(tableIt->second), samEntry));
                }
                ++samEntry;
            }
            if(this->computeRewards){
                for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                    if(!this->parametricTypeComparator.isConstant(parametricModel.getStateRewardVector()[state])){
                        std::pair<ParametricType, ConstantType> searchedPair(parametricModel.getStateRewardVector()[state], storm::utility::zero<ConstantType>());
                        auto const tableIt= std::lower_bound(evaluationTable.begin(), evaluationTable.end(), searchedPair);
                        STORM_LOG_THROW((*tableIt==searchedPair), storm::exceptions::UnexpectedException, "Could not find the current pair in the evaluationTable. Either the table is missing that pair or it is not sorted properly");
                        this->stateRewardMapping.emplace_back(std::make_pair(&(tableIt->second), &(this->model->getStateRewardVector()[state])));
                    }
                }
            }
        }

        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::~SamplingModel() {
            //Intentionally left empty
        }
        
        template<typename ParametricType, typename ConstantType>
        std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::getModel() const {
            return this->model;
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::instantiate(std::map<VariableType, CoefficientType>const& point) {
            //write entries into evaluation table
            for(auto& tableEntry : this->evaluationTable){
                tableEntry.second=storm::utility::regions::convertNumber<CoefficientType, ConstantType>(
                        storm::utility::regions::evaluateFunction(tableEntry.first, point));
            }
            //write the instantiated values to the matrix according to the mappings
            for(auto& mappingPair : this->probabilityMapping){
                mappingPair.second->setValue(*(mappingPair.first));
            }
            if(this->computeRewards){
                for(auto& mappingPair : this->stateRewardMapping){
                    *mappingPair.second=*mappingPair.first;
                }
            }
        }

        template<typename ParametricType, typename ConstantType>
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::computeValues() {
            storm::modelchecker::SparseDtmcPrctlModelChecker<ConstantType> modelChecker(*this->model);
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr;
            if(this->computeRewards){
                storm::logic::ReachabilityRewardFormula reachRewFormula(targetFormulaPtr);
                //perform model checking on the dtmc
                resultPtr = modelChecker.computeReachabilityRewards(reachRewFormula);
            }
            else {
                storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
                //perform model checking on the dtmc
                resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula);
            }
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::SamplingModel;
#endif
    }
}