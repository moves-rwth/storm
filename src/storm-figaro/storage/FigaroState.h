#pragma once

#include <sstream>
#include <memory>

#include "storm/storage/BitVector.h"

#include "storm-dft/storage/dft/DFTElementState.h"
#include "storm-dft/builder/DftExplorationHeuristic.h"

namespace storm {
    namespace figaro {
        namespace storage {

//            class FigaroStateGenerationInfo;

            template<typename ValueType>
            class FigaroState {
                friend struct std::hash<FigaroState>;

            private:
                // Status is bitvector where each element has two bits with the meaning according to DFTElementState
                storm::storage::BitVector mStatus;
                size_t mId;
//                FailableElements failableElements;
//                std::vector<size_t> mUsedRepresentants;
//                size_t indexRelevant;
                bool mPseudoState;
                bool mValid = true;
//                const DFT<ValueType> &mDft;
//                const FigaroStateGenerationInfo &mStateGenerationInfo;
            public:
                /**
                 * Construct the initial state.
                 *
                 * @param dft                 DFT
                 * @param stateGenerationInfo General information for state generation
                 * @param id                  State id
                 */
//                FigaroState(DFT<ValueType> const &dft, FigaroStateGenerationInfo const &stateGenerationInfo, size_t id);

                /**
                 * Construct temporary pseudo state. The actual state is constructed later.
                 *
                 * @param status              BitVector representing the status of the state.
                 * @param dft                 DFT
                 * @param stateGenerationInfo General information for state generation
                 * @param id                  Pseudo state id
                 */
//                FigaroState(storm::storage::BitVector const &status, DFT<ValueType> const &dft,
//                         FigaroStateGenerationInfo const &stateGenerationInfo, size_t id);
                /**
                 * Construct state by bitVector and state id.
                 *
                 * @param status              BitVector representing the status of the state.
                 * @param id                  Pseudo state id
                 */
                FigaroState(storm::storage::BitVector const &status, size_t id);

                /**
                 * Construct concerete state from pseudo state by using the underlying bitvector.
                 */
//                void construct();

//                std::shared_ptr<FigaroState<ValueType>> copy() const;
//
//                DFTElementState getElementState(size_t id) const;
//
//                static DFTElementState getElementState(storm::storage::BitVector const &state,
//                                                       FigaroStateGenerationInfo const &stateGenerationInfo, size_t id);
                size_t getId() const;

                void setId(size_t id);
//
//
//                bool hasFailed(size_t id) const;
//
//                static bool hasFailed(storm::storage::BitVector const &state, size_t indexId);
//
//                void markAsInvalid() {
//                    mValid = false;
//                }
//                bool isInvalid() const {
//                    return !mValid;
//                }
//                bool isPseudoState() const {
//                    return mPseudoState;
//                }
                storm::storage::BitVector const &status() const {
                    return mStatus;
                }
//                friend bool operator==(FigaroState const &a, FigaroState const &b) {
//                    return a.mStatus == b.mStatus;
//                }

//            private:
//
//
//                int getElementStateInt(size_t id) const;
//
//                static int getElementStateInt(storm::storage::BitVector const &state,
//                                              FigaroStateGenerationInfo const &stateGenerationInfo, size_t id);

            };

        }
    }
}

    namespace std {
        template<typename ValueType>
        struct hash<storm::figaro::storage::FigaroState<ValueType>> {
            size_t operator()(storm::figaro::storage::FigaroState<ValueType> const &s) const {
                return hash<storm::storage::BitVector>()(s.mStatus);
            }
        };
    }

