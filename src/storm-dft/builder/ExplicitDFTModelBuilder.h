#pragma  once

#include <boost/container/flat_set.hpp>
#include <boost/optional/optional.hpp>
#include <stack>
#include <unordered_set>


#include "storm/models/sparse/StateLabeling.h"
#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/BitVectorHashMap.h"

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/storage/dft/SymmetricUnits.h"





namespace storm {
    namespace builder {

        template<typename ValueType>
        class ExplicitDFTModelBuilder {

            using DFTElementPointer = std::shared_ptr<storm::storage::DFTElement<ValueType>>;
            using DFTElementCPointer = std::shared_ptr<storm::storage::DFTElement<ValueType> const>;
            using DFTGatePointer = std::shared_ptr<storm::storage::DFTGate<ValueType>>;
            using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;
            using DFTRestrictionPointer = std::shared_ptr<storm::storage::DFTRestriction<ValueType>>;


            // A structure holding the individual components of a model.
            struct ModelComponents {
                ModelComponents();

                // The transition matrix.
                storm::storage::SparseMatrix<ValueType> transitionMatrix;

                // The state labeling.
                storm::models::sparse::StateLabeling stateLabeling;

                // The Markovian states.
                storm::storage::BitVector markovianStates;

                // The exit rates.
                std::vector<ValueType> exitRates;

                // A vector that stores a labeling for each choice.
                boost::optional<storm::models::sparse::ChoiceLabeling> choiceLabeling;
            };
            
            const size_t INITIAL_BUCKETSIZE = 20000;
            const uint_fast64_t OFFSET_PSEUDO_STATE = UINT_FAST64_MAX / 2;
            
            storm::storage::DFT<ValueType> const& mDft;
            std::shared_ptr<storm::storage::DFTStateGenerationInfo> mStateGenerationInfo;
            storm::storage::BitVectorHashMap<uint_fast64_t> mStates;
            std::vector<std::pair<uint_fast64_t, storm::storage::BitVector>> mPseudoStatesMapping; // vector of (id to concrete state, bitvector)
            size_t newIndex = 0;
            bool mergeFailedStates = false;
            bool enableDC = true;
            size_t failedIndex = 0;
            size_t initialStateIndex = 0;

        public:
            struct LabelOptions {
                bool buildFailLabel = true;
                bool buildFailSafeLabel = false;
                std::set<std::string> beLabels = {};
            };
            
            ExplicitDFTModelBuilder(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTIndependentSymmetries const& symmetries, bool enableDC);

            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildModel(LabelOptions const& labelOpts);

        private:
            std::pair<uint_fast64_t, bool> exploreStates(DFTStatePointer const& state, size_t& rowOffset, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<uint_fast64_t>& markovianStates, std::vector<ValueType>& exitRates);
            
            /*!
             * Adds a state to the explored states and handles pseudo states.
             *
             * @param state The state to add.
             * @return Id of added state.
             */
            uint_fast64_t addState(DFTStatePointer const& state);
            
            /*!
             * Check if state needs an exploration and remember pseudo states for later creation.
             *
             * @param state State which might need exploration.
             * @return Pair of flag indicating whether the state needs exploration now and the state id if the state already
             * exists.
             */
            std::pair<bool, uint_fast64_t> checkForExploration(DFTStatePointer const& state);

        };
    }
}
