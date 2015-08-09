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
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::SamplingModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel) : mapping(), evaluationTable(){
            // Run through the rows of the original model and obtain the set of distinct functions as well as a matrix with dummy entries
            std::set<ParametricType> functionSet;
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(parametricModel.getNumberOfStates(), parametricModel.getNumberOfStates(), parametricModel.getTransitionMatrix().getEntryCount());
            uint_fast64_t numOfNonConstEntries=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                ConstantType dummyEntry=storm::utility::one<ConstantType>();
                for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                    if(!this->parametricTypeComparator.isConstant(entry.getValue())){
                        functionSet.insert(entry.getValue());
                        ++numOfNonConstEntries;
                    }
                    matrixBuilder.addNextValue(row,entry.getColumn(), dummyEntry);
                    dummyEntry=storm::utility::zero<ConstantType>();
                }
            }
            
            //Obtain other model ingredients and the approximation model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<std::vector<ConstantType>> noStateRewards;
            boost::optional<storm::storage::SparseMatrix<ConstantType>> noTransitionRewards;
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Dtmc<ConstantType>>(matrixBuilder.build(), std::move(labeling), std::move(noStateRewards), std::move(noTransitionRewards), std::move(noChoiceLabeling));
            
            //Get the evaluation table. Note that it remains sorted due to the fact that functionSet is sorted
            this->evaluationTable.reserve(functionSet.size());
            for(auto const& func : functionSet){
                this->evaluationTable.emplace_back(func, storm::utility::zero<ConstantType>());
            }
            
            //Fill in the entries for the mapping
            this->mapping.reserve(numOfNonConstEntries);
            auto samEntry = this->model->getTransitionMatrix().begin();
            for(auto const& parEntry : parametricModel.getTransitionMatrix()){
                if(this->parametricTypeComparator.isConstant(parEntry.getValue())){
                    samEntry->setValue(storm::utility::regions::convertNumber<CoefficientType, ConstantType>(storm::utility::regions::getConstantPart<ParametricType, ConstantType>(parEntry.getValue())));
                }
                else {
                    std::pair<ParametricType, ConstantType> searchedPair(parEntry.getValue(), storm::utility::zero<ConstantType>());
                    auto const tableIt= std::lower_bound(evaluationTable.begin(), evaluationTable.end(), searchedPair);
                    STORM_LOG_THROW((*tableIt==searchedPair), storm::exceptions::UnexpectedException, "Could not find the current pair in the evaluationTable. Either the table is missing that pair or it is not sorted properly");
                    mapping.emplace_back(std::make_pair(&(tableIt->second), samEntry));
                }
                ++samEntry;
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
                        storm::utility::regions::evaluateFunction<ParametricType, ConstantType>(
                                tableEntry.first,
                                point
                            )
                        );
            }
            //write the instantiated values to the matrix according to the mapping
            for(auto& mappingPair : this->mapping){
                mappingPair.second->setValue(*(mappingPair.first));
            }
        }

        template<typename ParametricType, typename ConstantType>
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel::computeReachabilityProbabilities() {
            std::shared_ptr<storm::logic::Formula> targetFormulaPtr(new storm::logic::AtomicLabelFormula("target"));
            storm::logic::EventuallyFormula eventuallyFormula(targetFormulaPtr);
            storm::modelchecker::SparseDtmcPrctlModelChecker<ConstantType> modelChecker(*this->model);
            
            //perform model checking on the mdp
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr = modelChecker.computeEventuallyProbabilities(eventuallyFormula);
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::SamplingModel;
#endif
    }
}