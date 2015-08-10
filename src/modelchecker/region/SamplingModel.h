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

namespace storm {
    namespace modelchecker{

        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker<ParametricType, ConstantType>::SamplingModel {
            
        public:
            
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType VariableType;
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType CoefficientType;
            
            SamplingModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel);
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
             * 
             * @param optimalityType Use MAXIMIZE to get upper bounds or MINIMIZE to get lower bounds
             */
            std::vector<ConstantType> const& computeReachabilityProbabilities();
            
            
        private:
            
            
            //Vector has one entry for every (non-constant) matrix entry.
            //pair.first points to an entry in the evaluation table,
            //pair.second is an iterator to the corresponding matrix entry
            std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> mapping;
            
            //Vector has one entry for every distinct, non-constant function that occurs somewhere in the model.
            //The second entry should contain the result when evaluating the function in the first entry.
            std::vector<std::pair<ParametricType, ConstantType>> evaluationTable;
            
            //The model with which we work
            std::shared_ptr<storm::models::sparse::Dtmc<ConstantType>> model;
            
            // comparators that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> parametricTypeComparator;
            storm::utility::ConstantsComparator<ConstantType> constantTypeComparator;
        };
        
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

