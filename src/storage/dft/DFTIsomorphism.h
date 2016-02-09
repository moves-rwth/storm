#pragma once

#include <vector>
#include <unordered_map>
#include "DFTElementType.h"
#include "DFTElements.h"

namespace storm {
namespace storage {


    struct GateGroupToHash {
        static constexpr uint_fast64_t fivebitmask = (1 << 6) - 1;
        static constexpr uint_fast64_t eightbitmaks = (1 << 8) - 1;

        /**
         * Hash function, which ensures that the colours are sorted according to their rank.
         */
        uint_fast64_t operator()(DFTElementType type, size_t nrChildren, size_t nrParents, size_t nrPDEPs, size_t rank) const {
            // Sets first bit to 1
            uint_fast64_t groupHash = static_cast<uint_fast64_t>(1) << 63;
            //Assumes 5 bits for the rank,
            groupHash |= (static_cast<uint_fast64_t>(rank) & fivebitmask) << (62 - 5);
            // 8 bits for the nrChildren,
            groupHash |= (static_cast<uint_fast64_t>(nrChildren) & eightbitmaks) << (62 - 5 - 8);
            // 5 bits for nrParents,
            groupHash |= (static_cast<uint_fast64_t>(nrParents) & fivebitmask) << (62 - 5 - 8 - 5);
            // 5 bits for nrPDEPs,
            groupHash |= (static_cast<uint_fast64_t>(nrPDEPs) & fivebitmask) << (62 - 5 - 8 - 5 - 5);
            // 5 bits for the type
            groupHash |= (static_cast<uint_fast64_t>(type) & fivebitmask) << (62 - 5 - 8 - 5 - 5 - 5);
            return groupHash;
        }

    };

    /**
     *
     */
    template<typename ValueType>
    class BijectionCandidates {
        std::unordered_map<size_t, std::vector<size_t>> groupCandidates;
        std::unordered_map<std::pair<ValueType, ValueType>, std::vector<size_t>> beCandidates;

    };




    /**
     * Saves isomorphism between subtrees
     */
    template<typename ValueType>
    class DFTIsomorphismCheck {
        /// Coloured nodes as provided by the input: left hand side
        BijectionCandidates<ValueType> const& bleft;
        /// Coloured nodes as provided by the input: right hand side.
        BijectionCandidates<ValueType> const& bright;
        /// Whether the colourings are compatible
        bool candidatesCompatible = true;
        /// Current bijection
        std::map<size_t, size_t> bijection;
        /// Current permutations of right hand side groups which lead to the homomorphism.
        /// Contains only colours with more than one member.
        BijectionCandidates<ValueType> currentPermutations;

    public:
        DFTIsomorphismCheck(BijectionCandidates<ValueType> const& left, BijectionCandidates<ValueType> const& right) : bleft(left), bright(right)
        {
            checkCompatibility();
        }

        /**
         * Checks whether the candidates are compatible, that is, checks the colours and the number of members for each colour.
         * @return True iff compatible, ie if the preliminary check allows for a isomorphism.
         */
        bool compatible() {
            return candidatesCompatible;
        }


        /**
         * Returns the isomorphism
         * Can only be called after the findIsomorphism procedure returned that an isomorphism has found.
         * @see findIsomorphism
         */
        std::vector<std::pair<size_t, size_t>> getIsomorphism() const {

        }

        /**
         * Check whether an isomorphism exists.
         *
         * @return true iff an isomorphism exists.
         */
        bool findIsomorphism() {
            if(!candidatesCompatible) return false;
            constructInitialBijection();
            while(!check()) {
                // continue our search
                if(!findNextBijection()) {
                    // No further bijections to check, no is
                    return false;
                }
            }
            return true;
        }

    protected:

        void constructInitialBijection() {
            assert(candidatesCompatible);
            // We first construct the currentPermutations, which helps to determine the current state of the check.
            for(auto const& colour : bright.beCandidates) {
                if(colour.second.size()>1) {
                    currentPermutations.beCandidates.insert(colour);
                }
            }

            for(auto const& colour : bright.groupCandidates) {
                if(colour.second.size()>1) {
                    currentPermutations.groupCandidates.insert(colour);
                }
            }

            // Constructing the initial homomorphism based on the right hand side instead of the current permutations allows us
            // to treat trivial and non-trivial classes in one go.
            for(auto const& colour : bleft.beCandidates) {
                zipVectorsIntoMap(colour.second, bright.beCandidates.find(colour.first)->second, bijection);
            }

            for(auto const& colour : bleft.groupCandidates) {
                zipVectorsIntoMap(colour.second, bright.groupCandidates.find(colour.first)->second, bijection);
            }

        }

        bool findNextBijection() {
            return false;
        }


        /**
         *
         */
        bool check() {

        }

    private:
        /**
         * Returns true if the colours are compatible.
         */
        bool checkCompatibility() {
            if(bleft.groupCandidates.size() != bright.groupCandidates.size()) {
                candidatesCompatible = false;
            }
            else if(bleft.beCandidates.size() != bright.beCandidates.size()) {
                candidatesCompatible = false;
            }
            else {
                for (auto const &gc : bleft.groupCandidates) {
                    if (bright.groupCandidates.count(gc.first) == 0) {
                        candidatesCompatible = false;
                        break;
                    }
                }
                if(candidatesCompatible) {
                    for(auto const& bc : bleft.groupCandidates) {
                        if(bright.beCandidates.count(bc.first) == 0) {
                            candidatesCompatible = false;
                            break;
                        }
                    }
                }
            }
        }

        /**
         * Local helper function for the creation of bijections, should be hidden from api.
         */
        void zipVectorsIntoMap(std::vector<size_t> const& a, std::vector<size_t> const& b, std::map<size_t, size_t>& map) const {
            // Assert should pass due to compatibility check
            assert(a.size() == b.size());
            auto it = b.cbegin();
            for(size_t lIndex : a) {
                assert(map.count(lIndex) == 0);
                map[lIndex] = *it;
                ++it;
            }
        }


        //std::vector<std::pair<size_t, size_t>> computeNextCandidate(){

        //}
    };


} // namespace storm::dft
} // namespace storm
