#pragma once

#include <memory>

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

    SftToBddTransformator(std::shared_ptr<storm::dd::DdManager<Type>> ddManager)
        : ddManager{ddManager} {}

    Bdd transform(std::shared_ptr<storm::storage::DFT<ValueType>> dft) {
        // create Variables for the BEs
        // auto basicElements = dft->getBasicElements();
        std::vector<storm::expressions::Variable> variables{};
        for (auto const& i : dft->getBasicElements()) {
            auto const& tmpVariables{ddManager->addMetaVariable(i->name(), 1)};
            variables.insert(variables.end(), tmpVariables.begin(),
                             tmpVariables.end());
        }

        return translate(dft->getTopLevelGate());
    }

   private:
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
            return ddManager->getBddZero();
        }
    }

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
            return translate(
                std::static_pointer_cast<storm::storage::DFTVot<ValueType>>(
                    gate));
        }
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Gate not NotSupportedException");
        return ddManager->getBddZero();
    }

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

        // used only in conjunctions therefore neutral element -> 1
        Bdd outerBdd{ddManager->getBddOne};
        const Bdd zero{ddManager->getBddZero};

        // generate all permutations
        for (uint64_t combination{smallestIntWithNBitsSet(vot->threshold())};
             (combination < (1ul << children.size)) && (combination != 0);
             combination = nextBitPermutation(combination)) {
            // used only in disjunctions therefore neutral element -> 0
            Bdd innerBdd{zero};

            for (uint8_t i{}; i < static_cast<uint8_t>(children.size()); ++i) {
                if ((combination & (1ul << i)) != 0) {
                    // Rangecheck children.size() < 64
                    innerBdd = innerBdd || translate(children[i]);
                }
            }
            outerBdd = outerBdd && innerBdd;
        }

        return outerBdd;
    }

    Bdd translate(std::shared_ptr<storm::storage::DFTBE<ValueType> const> const
                      basicElement) {
        auto var = ddManager->getMetaVariable(basicElement->name());

        return ddManager->getEncoding(var, 1);
    }

    std::shared_ptr<storm::dd::DdManager<Type>> ddManager;
};

}  // namespace dft
}  // namespace transformations
}  // namespace storm
