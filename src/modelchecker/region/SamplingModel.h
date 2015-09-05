/* 
 * File:   SamplingModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:31 AM
 */

#ifndef STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H
#define	STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H

#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "src/models/sparse/Dtmc.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace modelchecker{

        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel {
            
        public:
            
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType VariableType;
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType CoefficientType;
            
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
            
            void initializeProbabilities(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, storm::storage::SparseMatrix<ConstantType>& probabilityMatrix, std::vector<std::size_t>& matrixEntryToEvalTableMapping, std::size_t const& constantEntryIndex);
            void initializeRewards(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, boost::optional<std::vector<ConstantType>>& stateRewards, std::vector<std::size_t>& rewardEntryToEvalTableMapping, std::size_t const& constantEntryIndex);
            
            //Vector has one entry for every (non-constant) matrix entry.
            //pair.first points to an entry in the evaluation table,
            //pair.second is an iterator to the corresponding matrix entry
            std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> probabilityMapping;
            std::vector<std::pair<ConstantType*, typename std::vector<ConstantType>::iterator>> stateRewardMapping;
            
            //Vector has one entry for every distinct, non-constant function that occurs somewhere in the model.
            //The second entry should contain the result when evaluating the function in the first entry.
            std::vector<std::pair<ParametricType, ConstantType>> probabilityEvaluationTable;
            std::vector<std::pair<ParametricType, ConstantType>> rewardEvaluationTable;
            
            //The model with which we work
            std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> model;
            //The formula for which we will compute the values
            std::shared_ptr<storm::logic::Formula> formula;
            //A flag that denotes whether we compute probabilities or rewards
            bool computeRewards;
            
            // comparators that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> parametricTypeComparator;
            storm::utility::ConstantsComparator<ConstantType> constantTypeComparator;
        };
        
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

