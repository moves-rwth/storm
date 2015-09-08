/* 
 * File:   SamplingModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:31 AM
 */

#include "src/modelchecker/region/SamplingModel.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/vector.h"
#include "src/utility/regions.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "models/sparse/StandardRewardModel.h"

namespace storm {
    namespace modelchecker {
        
        
        template<typename ParametricSparseModelType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::SamplingModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, std::shared_ptr<storm::logic::Formula> formula) : formula(formula){
            if(this->formula->isEventuallyFormula()){
                this->computeRewards=false;
            } else if(this->formula->isReachabilityRewardFormula()){
                this->computeRewards=true;
                STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should be unique");
                STORM_LOG_THROW(parametricModel.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should have state rewards only");
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << this->formula << ". Sampling model only supports eventually or reachability reward formulae.");
            }
            //Start with the probabilities
            storm::storage::SparseMatrix<ConstantType> probabilityMatrix;
            std::vector<TableEntry*> matrixEntryToEvalTableMapping;// This vector will get an entry for every probability matrix entry.
            // For the corresponding matrix entry, it stores the corresponding entry in the probability evaluation table. 
            TableEntry constantEntry; //this value is stored in the matrixEntrytoEvalTableMapping for every constant matrix entry. (also used for rewards later)
            initializeProbabilities(parametricModel, probabilityMatrix, matrixEntryToEvalTableMapping, &constantEntry);
            
            //Now consider rewards
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ConstantType>> rewardModels;
            std::vector<TableEntry*> rewardEntryToEvalTableMapping; //does a similar thing as matrixEntryToEvalTableMapping
            if(this->computeRewards){
                boost::optional<std::vector<ConstantType>> stateRewards;
                initializeRewards(parametricModel, stateRewards, rewardEntryToEvalTableMapping, &constantEntry);
                rewardModels.insert(std::pair<std::string, storm::models::sparse::StandardRewardModel<ConstantType>>("", storm::models::sparse::StandardRewardModel<ConstantType>(std::move(stateRewards))));
            }
            
            //Obtain other model ingredients and the sampling model itself
            storm::models::sparse::StateLabeling labeling(parametricModel.getStateLabeling());
            boost::optional<storm::storage::SparseMatrix<ConstantType>> noTransitionRewards;
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> noChoiceLabeling;  
            this->model=std::make_shared<storm::models::sparse::Dtmc<ConstantType>>(std::move(probabilityMatrix), std::move(labeling), std::move(rewardModels), std::move(noChoiceLabeling));
                
            //translate the matrixEntryToEvalTableMapping into the actual probability mapping
            auto sampleModelEntry = this->model->getTransitionMatrix().begin();
            auto parModelEntry = parametricModel.getTransitionMatrix().begin();
            for(auto tableEntry : matrixEntryToEvalTableMapping){
                STORM_LOG_THROW(sampleModelEntry->getColumn()==parModelEntry->getColumn(), storm::exceptions::UnexpectedException, "The entries of the given parametric model and the constructed sampling model do not match");
                if(tableEntry == &constantEntry){
                    sampleModelEntry->setValue(storm::utility::regions::convertNumber<ConstantType>(storm::utility::regions::getConstantPart(parModelEntry->getValue())));
                } else {
                    this->probabilityMapping.emplace_back(std::make_pair(&(tableEntry->second), sampleModelEntry));
                }
                ++sampleModelEntry;
                ++parModelEntry;
            }
            //also do this for the rewards
            if(this->computeRewards){    
                auto sampleModelStateRewardEntry = this->model->getUniqueRewardModel()->second.getStateRewardVector().begin();
                for(auto tableEntry : rewardEntryToEvalTableMapping){
                    if(tableEntry != &constantEntry){
                        this->stateRewardMapping.emplace_back(std::make_pair(&(tableEntry->second), sampleModelStateRewardEntry));
                    }
                    ++sampleModelStateRewardEntry;
                }
            }
        }
        
        template<typename ParametricSparseModelType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::initializeProbabilities(storm::models::sparse::Dtmc<ParametricType> const& parametricModel,
                                                                                                                storm::storage::SparseMatrix<ConstantType>& probabilityMatrix,
                                                                                                                std::vector<TableEntry*>& matrixEntryToEvalTableMapping,
                                                                                                                TableEntry* constantEntry) {
            // Run through the rows of the original model and obtain the probability evaluation table, a matrix with dummy entries and the mapping between the two.
            storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(parametricModel.getNumberOfStates(), parametricModel.getNumberOfStates(), parametricModel.getTransitionMatrix().getEntryCount());
            matrixEntryToEvalTableMapping.reserve(parametricModel.getTransitionMatrix().getEntryCount());
            std::size_t numOfNonConstEntries=0;
            for(typename storm::storage::SparseMatrix<ParametricType>::index_type row=0; row < parametricModel.getTransitionMatrix().getRowCount(); ++row ){
                ConstantType dummyEntry=storm::utility::one<ConstantType>();
                for(auto const& entry : parametricModel.getTransitionMatrix().getRow(row)){
                    if(storm::utility::isConstant(entry.getValue())){
                        matrixEntryToEvalTableMapping.emplace_back(constantEntry);
                    } else {
                        ++numOfNonConstEntries;
                        auto evalTableIt = this->probabilityEvaluationTable.insert(TableEntry(entry.getValue(), storm::utility::zero<ConstantType>())).first;
                        matrixEntryToEvalTableMapping.emplace_back(&(*evalTableIt));
                    }
                    matrixBuilder.addNextValue(row,entry.getColumn(), dummyEntry);
                    dummyEntry=storm::utility::zero<ConstantType>();
                }
            }
            this->probabilityMapping.reserve(numOfNonConstEntries);
            probabilityMatrix=matrixBuilder.build();
        }
        
        template<typename ParametricSparseModelType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::initializeRewards(storm::models::sparse::Dtmc<ParametricType> const& parametricModel,
                                                                                                          boost::optional<std::vector<ConstantType>>& stateRewards,
                                                                                                          std::vector<TableEntry*>& rewardEntryToEvalTableMapping,
                                                                                                          TableEntry* constantEntry) {
            // run through the state reward vector of the parametric model. Constant entries can be set directly. Parametric entries are inserted into the table
            std::vector<ConstantType> stateRewardsAsVector(parametricModel.getNumberOfStates());
            rewardEntryToEvalTableMapping.reserve(parametricModel.getNumberOfStates());
            std::size_t numOfNonConstEntries=0;
            for(std::size_t state=0; state<parametricModel.getNumberOfStates(); ++state){
                if(storm::utility::isConstant(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state])){
                    stateRewardsAsVector[state] = storm::utility::regions::convertNumber<ConstantType>(storm::utility::regions::getConstantPart(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state]));
                    rewardEntryToEvalTableMapping.emplace_back(constantEntry);
                } else {
                    ++numOfNonConstEntries;
                    auto evalTableIt = this->probabilityEvaluationTable.insert(TableEntry(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], storm::utility::zero<ConstantType>())).first;
                    rewardEntryToEvalTableMapping.emplace_back(&(*evalTableIt));
                }
            }
            this->stateRewardMapping.reserve(numOfNonConstEntries);
            stateRewards=std::move(stateRewardsAsVector);
        }

        template<typename ParametricSparseModelType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::~SamplingModel() {
            //Intentionally left empty
        }
        
        template<typename ParametricSparseModelType, typename ConstantType>
        std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> const& SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::getModel() const {
            return this->model;
        }

        template<typename ParametricSparseModelType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::instantiate(std::map<VariableType, CoefficientType>const& point) {
            //write entries into evaluation tables
            for(auto& tableEntry : this->probabilityEvaluationTable){
                tableEntry.second=storm::utility::regions::convertNumber<ConstantType>(
                        storm::utility::regions::evaluateFunction(tableEntry.first, point));
            }
            for(auto& tableEntry : this->rewardEvaluationTable){
                tableEntry.second=storm::utility::regions::convertNumber<ConstantType>(
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

        template<typename ParametricSparseModelType, typename ConstantType>
        std::vector<ConstantType> const& SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel::computeValues() {
            storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ConstantType>> modelChecker(*this->model);
            std::unique_ptr<storm::modelchecker::CheckResult> resultPtr;
            //perform model checking on the dtmc
            if(this->computeRewards){
                resultPtr = modelChecker.computeReachabilityRewards(this->formula->asReachabilityRewardFormula());
            }
            else {
                resultPtr = modelChecker.computeEventuallyProbabilities(this->formula->asEventuallyFormula());
            }
            return resultPtr->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
#endif
    }
}