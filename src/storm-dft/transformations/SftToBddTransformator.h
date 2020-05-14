#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/exceptions/NotSupportedException.h"
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
     * The given DFT is not a SFT
     */
    Bdd transform() {
        calculateRelevantEvents = false;
        return translate(std::static_pointer_cast<
                         storm::storage::DFTElement<ValueType> const>(
            dft->getTopLevelGate()));
    }

    /**
     * Transform a sft into a BDDs
     * representing the functions of the given Events
     * if they are reachable from the toplevel Event.
     *
     * \exception storm::exceptions::NotSupportedException
     * The given DFT is not a SFT
     *
     * \param relevantEventNames
     * The Names of the Events that should be transformed into Bdd's
     *
     */
    std::map<std::string, Bdd> transform(
        std::set<std::string> const& relevantEventNames) {
        this->relevantEventNames = relevantEventNames;
        relevantEventBdds = {};
        calculateRelevantEvents = true;
        relevantEventBdds[dft->getTopLevelGate()->name()] =
            translate(std::static_pointer_cast<
                      storm::storage::DFTElement<ValueType> const>(
                dft->getTopLevelGate()));

        return relevantEventBdds;
    }

    /**
     * Get Variables in the order they where inserted
     */
    std::vector<uint32_t> const& getDdVariables() { return variables; }

   private:
    bool calculateRelevantEvents{false};
    std::set<std::string> relevantEventNames{};
    std::map<std::string, Bdd> relevantEventBdds{};

    /**
     * Translate a simple DFT element into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * The given DFT is not a SFT
     */
    Bdd translate(
        std::shared_ptr<storm::storage::DFTElement<ValueType> const> element) {
        if (calculateRelevantEvents) {
            auto const it{relevantEventBdds.find(element->name())};
            if (it != relevantEventBdds.end()) {
                return it->second;
            }
        }

        Bdd rBdd;
        if (element->isGate()) {
            rBdd =
                translate(std::dynamic_pointer_cast<
                          storm::storage::DFTGate<ValueType> const>(element));
        } else if (element->isBasicElement()) {
            rBdd = translate(std::dynamic_pointer_cast<
                             storm::storage::DFTBE<ValueType> const>(element));
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "Element of type \""
                                << element->typestring()
                                << "\" is not supported. Probably not a SFT.");
            rBdd = sylvanBddManager->getZero();
        }

        if (calculateRelevantEvents) {
            auto const it{relevantEventNames.find(element->name())};
            if (it != relevantEventNames.end()) {
                // rBdd can't be in relevantEventBdds
                // as we would've returned
                // at the start of the function
                relevantEventBdds[element->name()] = rBdd;
            }
        }

        return rBdd;
    }

    /**
     * Translate a simple DFT gate into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * The given DFT is not a SFT
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
                        "Gate of type \""
                            << gate->typestring()
                            << "\" is not supported. Probably not a SFT.");
        return sylvanBddManager->getZero();
    }

    /**
     * Translate a DFT VOT gate into a BDD.
     *
     * \exception storm::exceptions::NotSupportedException
     * The given DFT is not a SFT
     */
    Bdd translate(
        std::shared_ptr<storm::storage::DFTVot<ValueType> const> vot) {
        std::vector<Bdd> bdds;
        bdds.reserve(vot->children().size());

        for (auto const& child : vot->children()) {
            bdds.push_back(translate(child));
        }

        auto const rval{translateVot(0, vot->threshold(), bdds)};
        return rval;
    }

    /**
     * Helper function to translate a Vot gate into a BDD
     *
     * Performs a shannon decomposition
     *
     * \param currentIndex
     * The index of the child we currently look at.
     *
     * \param threshold
     * How many children still need to be chosen.
     *
     * \param bdds
     * A reference to all children of the vot gate as Bdds.
     */
    Bdd translateVot(size_t const currentIndex, size_t const threshold,
                     std::vector<Bdd> const& bdds) const {
        if (threshold == 0) {
            return sylvanBddManager->getOne();
        } else if (currentIndex >= bdds.size()) {
            return sylvanBddManager->getZero();
        }

        auto const notChosenBdd{
            translateVot(currentIndex + 1, threshold, bdds)};
        auto const chosenBdd{
            translateVot(currentIndex + 1, threshold - 1, bdds)};

        return bdds[currentIndex].Ite(chosenBdd, notChosenBdd);
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
