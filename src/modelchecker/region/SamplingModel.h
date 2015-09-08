/* 
 * File:   SamplingModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:31 AM
 */

#ifndef STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H
#define	STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H

#include <unordered_map>

#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "src/models/sparse/Dtmc.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace modelchecker{

        template<typename ParametricSparseModelType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        
        template<typename ParametricSparseModelType, typename ConstantType>
        class SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::SamplingModel {
            
        public:
            
            typedef typename ParametricSparseModelType::ValueType ParametricType;
            typedef typename SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::VariableType VariableType;
            typedef typename SparseDtmcRegionModelChecker<ParametricSparseModelType, ConstantType>::CoefficientType CoefficientType;
            
            SamplingModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, std::shared_ptr<storm::logic::Formula> formula);
            virtual ~SamplingModel();
            
            /*!
             * returns the underlying model
             */
            std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> const& getModel() const;
            
            /*!
             * Instantiates the underlying model according to the given point
             */
            void instantiate(std::map<VariableType, CoefficientType>const& point);
            
            /*!
             * Returns the reachability probabilities for every state according to the current instantiation.
             * Undefined behavior if model has not been instantiated first!
             */
            std::vector<ConstantType> const& computeValues();
            
            
        private:
            
            typedef typename std::unordered_map<ParametricType, ConstantType>::value_type TableEntry;
            
            void initializeProbabilities(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, storm::storage::SparseMatrix<ConstantType>& probabilityMatrix, std::vector<TableEntry*>& matrixEntryToEvalTableMapping, TableEntry* constantEntry);
            void initializeRewards(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, boost::optional<std::vector<ConstantType>>& stateRewards, std::vector<TableEntry*>& rewardEntryToEvalTableMapping, TableEntry* constantEntry);
            
            //The model with which we work
            std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> model;
            //The formula for which we will compute the values
            std::shared_ptr<storm::logic::Formula> formula;
            //A flag that denotes whether we compute probabilities or rewards
            bool computeRewards;

            // We store one (unique) entry for every occurring function.
            // Whenever a sampling point is given, we can then evaluate the functions
            // and store the result to the target value of this map
            std::unordered_map<ParametricType, ConstantType> probabilityEvaluationTable;
            std::unordered_map<ParametricType, ConstantType> rewardEvaluationTable;

            //This Vector connects the probability evaluation table with the probability matrix of the model.
            //Vector has one entry for every (non-constant) matrix entry.
            //pair.first points to an entry in the evaluation table,
            //pair.second is an iterator to the corresponding matrix entry
            std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> probabilityMapping;
            std::vector<std::pair<ConstantType*, typename std::vector<ConstantType>::iterator>> stateRewardMapping;
            
        };
        
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

