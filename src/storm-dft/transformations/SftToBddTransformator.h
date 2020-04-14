#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/utility/bitoperations.h"

namespace storm {
namespace transformations {
namespace dft {

/**
 * Transformator for DFT -> BDD.
 */
template <typename ValueType>
class SftToBddTransformator {
   public:
    using Bdd = sylvan::Bdd;

    SftToBddTransformator(
        std::shared_ptr<storm::storage::DFT<ValueType>> dft,
        std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager)
        : dft{std::move(dft)},
          sylvanBddManager{std::move(sylvanBddManager)},
          variables{} {
        // create Variables for the BEs
        for (auto const& i : this->dft->getBasicElements()) {
            // Filter constantBeTrigger
            if (i->name() != "constantBeTrigger") {
                variables.push_back(
                    this->sylvanBddManager->createVariable(i->name()));
            }
        }
    }

    /**
     * Transform a sft into a BDD
     * representing the function of the toplevel gate.
     *
     * \exception storm::exceptions::NotSupportedException
     * Either the DFT is no SFT
     * or there is a VOT gate with more then 63 children.
     */
    Bdd transform() { return translate(dft->getTopLevelGate()); }

    /**
     * Get Variables in the order they where inserted
     */
    std::vector<uint32_t> const& getDdVariables() { return variables; }

   private:
    /**
     * Translate a simple DFT element into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * Either the DFT is no SFT
     * or there is a VOT gate with more then 63 children.
     */
    Bdd translate(
        std::shared_ptr<storm::storage::DFTElement<ValueType> const> element) {
        if (element->isGate()) {
            return translate(
                std::dynamic_pointer_cast<
                    storm::storage::DFTGate<ValueType> const>(element));
        } else if (element->isBasicElement()) {
            return translate(std::dynamic_pointer_cast<
                             storm::storage::DFTBE<ValueType> const>(element));
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "Element is not supported. Probably not a SFT.");
            return sylvanBddManager->getZero();
        }
    }

    /**
     * Translate a simple DFT gate into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * Either the DFT is no SFT
     * or there is a VOT gate with more then 63 children.
     */
    Bdd translate(
        std::shared_ptr<storm::storage::DFTGate<ValueType> const> gate) {
        if (gate->type() == storm::storage::DFTElementType::AND) {
            // used only in conjunctions therefore neutral element -> 1
            auto tmpBdd{sylvanBddManager->getOne()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd &= translate(child);
            }
            return tmpBdd;
        } else if (gate->type() == storm::storage::DFTElementType::OR) {
            // used only in disjunctions therefore neutral element -> 0
            auto tmpBdd{sylvanBddManager->getZero()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd |= translate(child);
            }
            return tmpBdd;
        } else if (gate->type() == storm::storage::DFTElementType::VOT) {
            return translate(std::dynamic_pointer_cast<
                             storm::storage::DFTVot<ValueType> const>(gate));
        }
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Gate is not supported. Probably not a SFT.");
        return sylvanBddManager->getZero();
    }

    /**
     * Translate a DFT VOT gate into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * Either the DFT is no SFT
     * or there is a VOT gate with more then 63 children.
     */
    Bdd translate(
        std::shared_ptr<storm::storage::DFTVot<ValueType> const> vot) {
        auto children{vot->children()};

        /*
         * We can only support up to 63 children
         * As we permute a 64 bit integer
         * Most likely would result in a state exoplosion anyways
         */
        STORM_LOG_THROW(children.size() < 64,
                        storm::exceptions::NotSupportedException,
                        "Too many children of a VOT Gate.");

        // used only in disjunctions therefore neutral element -> 0
        Bdd outerBdd{sylvanBddManager->getZero()};
        const Bdd one{sylvanBddManager->getOne()};

        // generate all permutations
        for (uint64_t combination{smallestIntWithNBitsSet(vot->threshold())};
             (combination < (1ul << children.size())) && (combination != 0);
             combination = nextBitPermutation(combination)) {
            // used only in conjunctions therefore neutral element -> 1
            Bdd innerBdd{one};

            for (uint8_t i{}; i < static_cast<uint8_t>(children.size()); ++i) {
                if ((combination & (1ul << i)) != 0) {
                    // Rangecheck children.size() < 64
                    innerBdd &= translate(children[i]);
                }
            }
            outerBdd |= innerBdd;
        }

        return outerBdd;
    }

    /**
     * Translate a DFT basic element gate into a BDD.
     *
     * \note This is the recursion anchor.
     *
     */
    Bdd translate(std::shared_ptr<storm::storage::DFTBE<ValueType> const> const
                      basicElement) {
        return sylvanBddManager->getPositiveLiteral(basicElement->name());
    }

    std::shared_ptr<storm::storage::DFT<ValueType>> dft;
    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;

    std::vector<uint32_t> variables;
};

}  // namespace dft
}  // namespace transformations
}  // namespace storm
