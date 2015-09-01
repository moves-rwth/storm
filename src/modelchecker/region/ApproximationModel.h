/* 
 * File:   ApproximationModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:29 AM
 */

#ifndef STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H
#define	STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H

#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "src/models/sparse/Mdp.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ApproximationModel{

        public:
            
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType VariableType;
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType CoefficientType;
            
            
            ApproximationModel(storm::models::sparse::Dtmc<ParametricType> const& parametricModel, bool computeRewards);
            virtual ~ApproximationModel();

            /*!
             * returns the underlying model
             */
            std::shared_ptr<storm::models::sparse::Mdp<ConstantType>> const& getModel() const;
            
            /*!
             * Instantiates the underlying model according to the given region
             */
            void instantiate(ParameterRegion const& region);
            
            /*!
             * Returns the approximated reachability probabilities for every state.
             * Undefined behavior if model has not been instantiated first!
             * @param optimalityType Use MAXIMIZE to get upper bounds or MINIMIZE to get lower bounds
             */
            std::vector<ConstantType> const& computeValues(storm::logic::OptimalityType const& optimalityType);

                
        private:
            enum class TypeOfBound { 
                LOWER,
                UPPER,
                CHOSEOPTIMAL
            }; 
           
            //Vector has one entry for every (non-constant) matrix entry.
            //pair.first points to an entry in the evaluation table,
            //pair.second is an iterator to the corresponding matrix entry
            std::vector<std::pair<ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> probabilityMapping;
            //similar for the rewards. But now the first entry points to a minimal and the second one to a maximal value.
            //The third entry points to the state reward vector and the transitionRewardMatrix, respectively.
            std::vector<std::tuple<ConstantType*, ConstantType*, ConstantType*>> stateRewardMapping;
            std::vector<std::tuple<ConstantType*, ConstantType*, typename storm::storage::SparseMatrix<ConstantType>::iterator>> transitionRewardMapping;
            
            //Vector has one entry for
            //(every distinct, non-constant function that occurs somewhere in the model) x (the required combinations of lower and upper bounds of the region)
            //The second entry represents a substitution as an index in the substitutions vector
            //The third entry should contain the result when evaluating the function in the first entry, regarding the substitution given by the second entry.
            std::vector<std::tuple<ParametricType, std::size_t, ConstantType>> evaluationTable;
            std::vector<std::tuple<ParametricType, std::size_t, ConstantType, ConstantType>> rewardEvaluationTable;
            
            //Vector has one entry for every required substitution (=replacement of parameters with lower/upper bounds of region)
            std::vector<std::map<VariableType, TypeOfBound>> substitutions;
            //Same for the different substitutions for the reward functions.
            //In addition, we store the parameters for which the correct substitution 
            //depends on the region and whether to minimize/maximize
            std::vector<std::map<VariableType, TypeOfBound>> rewardSubstitutions;
            std::vector<std::set<VariableType>> choseOptimalRewardPars;
            
            //The Model with which we work
            std::shared_ptr<storm::models::sparse::Mdp<ConstantType>> model;
            
            //A flag that denotes whether we compute probabilities or rewards
            bool computeRewards;
            
            // comparators that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> parametricTypeComparator;
            storm::utility::ConstantsComparator<ConstantType> constantTypeComparator;
                    
        };
    }
}


#endif	/* STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H */

