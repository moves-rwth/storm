#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/dft/DFT.h"
#include "storm/storage/dd/DdManager.h"
#include "storm/utility/bitoperations.h"

namespace storm {
namespace transformations {
namespace dft {

/*!
 * Transformator for DFT -> BDD.
 */
template <storm::dd::DdType Type, typename ValueType>
class SftToBddTransformator {
   public:
    using Bdd = storm::dd::Bdd<Type>;

    SftToBddTransformator(std::shared_ptr<storm::storage::DFT<ValueType>> dft,
                          std::shared_ptr<storm::dd::DdManager<Type>> ddManager)
        : dft{std::move(dft)}, ddManager{std::move(ddManager)}, variables{} {
        // create Variables for the BEs
        for (auto const& i : this->dft->getBasicElements()) {
            // Filter constantBeTrigger
            if (i->name() != "constantBeTrigger") {
                auto tmpVars{this->ddManager->addMetaVariable(i->name(), 1)};

                variables.insert(variables.end(), tmpVars.begin(),
                                 tmpVars.end());
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
    std::vector<storm::expressions::Variable> const& getDdVariables() {
        return variables;
    }

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
            return ddManager->getBddZero();
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
            auto tmpBdd{ddManager->getBddOne()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd = tmpBdd && translate(child);
            }
            return tmpBdd;
        } else if (gate->type() == storm::storage::DFTElementType::OR) {
            auto tmpBdd{ddManager->getBddZero()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd = tmpBdd || translate(child);
            }
            return tmpBdd;
        } else if (gate->type() == storm::storage::DFTElementType::VOT) {
            return translate(std::dynamic_pointer_cast<
                             storm::storage::DFTVot<ValueType> const>(gate));
        }
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Gate is not supported. Probably not a SFT.");
        return ddManager->getBddZero();
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
        Bdd outerBdd{ddManager->getBddZero()};
        const Bdd one{ddManager->getBddOne()};

        // generate all permutations
        for (uint64_t combination{smallestIntWithNBitsSet(vot->threshold())};
             (combination < (1ul << children.size())) && (combination != 0);
             combination = nextBitPermutation(combination)) {
            // used only in conjunctions therefore neutral element -> 1
            Bdd innerBdd{one};

            for (uint8_t i{}; i < static_cast<uint8_t>(children.size()); ++i) {
                if ((combination & (1ul << i)) != 0) {
                    // Rangecheck children.size() < 64
                    innerBdd = innerBdd && translate(children[i]);
                }
            }
            outerBdd = outerBdd || innerBdd;
        }

        return outerBdd;
    }

    /**
     * Translate a DFT basic element gate into a BDD.
     *
     * \note This is the rekursion anker.
     *
     */
    Bdd translate(std::shared_ptr<storm::storage::DFTBE<ValueType> const> const
                      basicElement) {
        auto var = ddManager->getMetaVariable(basicElement->name());

        return ddManager->getEncoding(var, 1);
    }

    std::shared_ptr<storm::storage::DFT<ValueType>> dft;
    std::shared_ptr<storm::dd::DdManager<Type>> ddManager;

    std::vector<storm::expressions::Variable> variables;
};

}  // namespace dft
}  // namespace transformations
}  // namespace storm
