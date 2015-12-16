#ifndef EXPLICITDFTMODELBUILDER_H
#define	EXPLICITDFTMODELBUILDER_H

#include "../storage/dft/DFT.h"

#include <src/models/sparse/StateLabeling.h>
#include <src/models/sparse/StandardRewardModel.h>
#include <src/storage/SparseMatrix.h>
#include <boost/container/flat_set.hpp>
#include <boost/optional/optional.hpp>
#include <stack>
#include <unordered_set>

namespace storm {
    namespace builder {

        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>, typename IndexType = uint32_t>
        class ExplicitDFTModelBuilder {
            storm::storage::DFT const &mDft;
            std::unordered_set<storm::storage::DFTState> mStates;
            size_t newIndex = 0;

            // The transition matrix.
            storm::storage::SparseMatrix<ValueType> transitionMatrix;

            // The state labeling.
            storm::models::sparse::StateLabeling stateLabeling;

            // A vector that stores a labeling for each choice.
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;

        public:
            ExplicitDFTModelBuilder(storm::storage::DFT const &dft) : mDft(dft) {

            }

            void buildCTMC();

        private:
            void exploreStates(std::queue<storm::storage::DFTState>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder);

        };
    }
}

#endif	/* EXPLICITDFTMODELBUILDER_H */