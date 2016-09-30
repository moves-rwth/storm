#ifndef EXPLICITDFTMODELBUILDERAPPROX_H
#define	EXPLICITDFTMODELBUILDERAPPROX_H

#include <src/models/sparse/StateLabeling.h>
#include <src/models/sparse/StandardRewardModel.h>
#include <src/models/sparse/Model.h>
#include <src/storage/SparseMatrix.h>
#include "src/storage/sparse/StateStorage.h"
#include <src/storage/dft/DFT.h>
#include <src/storage/dft/SymmetricUnits.h>
#include <boost/container/flat_set.hpp>
#include <boost/optional/optional.hpp>
#include <stack>
#include <unordered_set>
#include <limits>

namespace storm {
    namespace builder {

        /*!
         * Build a Markov chain from DFT.
         */
        template<typename ValueType, typename StateType = uint32_t>
        class ExplicitDFTModelBuilderApprox {

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
                boost::optional<std::vector<boost::container::flat_set<StateType>>> choiceLabeling;

                // A flag indicating if the model is deterministic.
                bool deterministicModel;
            };

        public:
            // A structure holding the labeling options.
            struct LabelOptions {
                bool buildFailLabel = true;
                bool buildFailSafeLabel = false;
                std::set<std::string> beLabels = {};
            };

            /*!
             * Constructor.
             *
             * @param dft DFT.
             * @param symmetries Symmetries in the dft.
             * @param enableDC Flag indicating if dont care propagation should be used.
             */
            ExplicitDFTModelBuilderApprox(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTIndependentSymmetries const& symmetries, bool enableDC);

            /*!
             * Build model from DFT.
             *
             * @param labelOpts          Options for labeling.
             * @param approximationError Error allowed for approximation.
             */
            void buildModel(LabelOptions const& labelOpts, double approximationError = 0.0);

            /*!
             * Get the built model.
             *
             * @return The model built from the DFT.
             */
            std::shared_ptr<storm::models::sparse::Model<ValueType>> getModel();

            /*!
             * Get the built approximation model for either the lower or upper bound.
             *
             * @param lowerBound If true, the lower bound model is returned, else the upper bound model
             *
             * @return The model built from the DFT.
             */
            std::shared_ptr<storm::models::sparse::Model<ValueType>> getModelApproximation(bool lowerBound = true);

        private:

            /*!
             * Add a state to the explored states (if not already there). It also handles pseudo states.
             *
             * @param state The state to add.
             *
             * @return Id of state.
             */
            StateType getOrAddStateIndex(DFTStatePointer const& state);

            /*!
             * Set if the given state is markovian.
             *
             * @param id Id of the state.
             * @param markovian Flag indicating if the state is markovian.
             */
            void setMarkovian(StateType id, bool markovian);

            /*!
             * Set a mapping from a state id to its new id.
             *
             * @param id Id of the state.
             * @param mappedId New id to use.
             */
            void setRemapping(StateType id, StateType mappedId);

            /**
             * Change matrix to reflect the lower approximation bound.
             *
             * @param matrix Matrix to change. The change are reflected here.
             */
            void changeMatrixLowerBound(storm::storage::SparseMatrix<ValueType> & matrix) const;

            /*!
             * Change matrix to reflect the upper approximation bound.
             *
             * @param matrix Matrix to change. The change are reflected here.
             */
            void changeMatrixUpperBound(storm::storage::SparseMatrix<ValueType> & matrix) const;

            /*!
             * Create the model model from the model components.
             *
             * @param copy If true, all elements of the model component are copied (used for approximation). If false
             *             they are moved to reduce the memory overhead.
             *
             * @return The model built from the model components.
             */
            std::shared_ptr<storm::models::sparse::Model<ValueType>> createModel(bool copy);


            // Initial size of the bitvector.
            const size_t INITIAL_BITVECTOR_SIZE = 20000;
            // Offset used for pseudo states.
            const StateType OFFSET_PSEUDO_STATE = std::numeric_limits<StateType>::max() / 2;

            // Dft
            storm::storage::DFT<ValueType> const& dft;

            // General information for state generation
            // TODO Matthias: use const reference
            std::shared_ptr<storm::storage::DFTStateGenerationInfo> stateGenerationInfo;

            // Current id for new state
            size_t newIndex = 0;

            // Mapping from pseudo states to (id of concrete state, bitvector)
            std::vector<std::pair<StateType, storm::storage::BitVector>> mPseudoStatesMapping;

            //TODO Matthias: make changeable
            const bool mergeFailedStates = true;

            // Id of failed state
            size_t failedStateId = 0;

            // Id of initial state
            size_t initialStateIndex = 0;

            // Flag indication if dont care propagation should be used.
            bool enableDC = true;

            // Structure for the components of the model.
            ModelComponents modelComponents;

            // Internal information about the states that were explored.
            storm::storage::sparse::StateStorage<StateType> stateStorage;

            // A set of states that still need to be explored.
            std::deque<DFTStatePointer> statesToExplore;

            // A mapping from state indices to the row groups in which they actually reside
            // TODO Matthias: avoid hack with fixed int type
            std::vector<uint_fast64_t> stateRemapping;

            // Holds all skipped states which were not yet expanded. More concrete it is a mapping from matrix indices
            // to the corresponding skipped state.
            std::unordered_map<StateType, DFTStatePointer> skippedStates;
        };
    }
}

#endif	/* EXPLICITDFTMODELBUILDERAPPROX_H */
