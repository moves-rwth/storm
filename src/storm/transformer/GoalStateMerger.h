#pragma once

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"


namespace storm {
    namespace transformer {
        
        /*
         * Merges the given target and sink states into a single state with a selfloop
         */
        template <typename SparseModelType>
        class GoalStateMerger {
        public:
                      
            GoalStateMerger(SparseModelType const& model);
            
            /* Computes a submodel of the specified model that only considers the states given by maybeStates as well as
             *  * one target state to which all transitions to a state selected by targetStates are redirected and
             *  * one sink state to which all transitions to a state selected by sinkStates are redirected.
             *  If there is no transition to either target or sink, the corresponding state will not be created.
             *
             *  The two given bitvectors targetStates and sinkStates are modified such that they represent the corresponding state in the obtained submodel.
             *
             *  Notes:
             *  * The resulting model will not have any labels (other then "init") and only the selected reward models.
             *  * Rewards are reduced to stateActionRewards.
             *  * The target and sink states will not get any reward
             *  * It is assumed that the given set of maybeStates can only be left via either a target or a sink state. Otherwise an exception is thrown.
             *  * It is assumed that maybeStates, targetStates, and sinkStates are all disjoint. Otherwise an exception is thrown.
             */
            std::shared_ptr<SparseModelType> mergeTargetAndSinkStates(storm::storage::BitVector const& maybeStates, storm::storage::BitVector& targetStates, storm::storage::BitVector& sinkStates, std::vector<std::string> const& selectedRewardModels = std::vector<std::string>());
            
        private:
            SparseModelType const& originalModel;
            
            /*!
             * Initializes the matrix builder for the transition matrix of the resulting model
             *
             * @param newTargetState Will be set to the index of the target state of the resulting model (Only if it has a target state)
             * @param newSinkState Will be set to the index of the sink state of the resulting model (Only if it has a sink state)
             */
            storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType> initializeTransitionMatrixBuilder(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& sinkStates, boost::optional<uint_fast64_t>& newTargetState, boost::optional<uint_fast64_t>& newSinkState);
            
            /*!
             * Builds the transition matrix of the resulting model
             */
            storm::storage::SparseMatrix<typename SparseModelType::ValueType> buildTransitionMatrix(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates, storm::storage::BitVector const& sinkStates, boost::optional<uint_fast64_t> const& newTargetState, boost::optional<uint_fast64_t> const& newSinkState, storm::storage::SparseMatrixBuilder<typename SparseModelType::ValueType>& builder);

            
        };
    }
}
