#ifndef EXPLICITDFTMODELBUILDER_H
#define	EXPLICITDFTMODELBUILDER_H

#include "../storage/dft/DFT.h"

#include <src/models/sparse/StateLabeling.h>
#include <src/models/sparse/StandardRewardModel.h>
#include <src/models/sparse/Model.h>
#include <src/storage/SparseMatrix.h>
#include <src/storage/BitVectorHashMap.h>
#include <boost/container/flat_set.hpp>
#include <boost/optional/optional.hpp>
#include <stack>
#include <unordered_set>

namespace storm {
    namespace builder {

        template<typename ValueType>
        class ExplicitDFTModelBuilder {

            using DFTElementPointer = std::shared_ptr<storm::storage::DFTElement<ValueType>>;
            using DFTElementCPointer = std::shared_ptr<storm::storage::DFTElement<ValueType> const>;
            using DFTGatePointer = std::shared_ptr<storm::storage::DFTGate<ValueType>>;
            using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;

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
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;
            };

            storm::storage::DFT<ValueType> const &mDft;
            storm::storage::BitVectorHashMap<DFTStatePointer> mStates;
            size_t newIndex = 0;

        public:
            ExplicitDFTModelBuilder(storm::storage::DFT<ValueType> const &dft) : mDft(dft), mStates(((mDft.stateVectorSize() / 64) + 1) * 64, std::pow(2, mDft.nrBasicElements())) {
                // stateSize is bound for size of bitvector
                // 2^nrBE is upper bound for state space
            }

            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildModel();

        private:
            bool exploreStates(std::queue<DFTStatePointer>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<uint_fast64_t>& markovianStates, std::vector<ValueType>& exitRates);

        };
    }
}

#endif	/* EXPLICITDFTMODELBUILDER_H */