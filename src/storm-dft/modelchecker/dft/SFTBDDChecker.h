#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/transformations/SftToBddTransformator.h"
#include "storm/storage/dd/DdManager.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for the SFTBDDChecker
 *
 */
template <typename ValueType>
class SFTBDDChecker {
   public:
    using Bdd = sylvan::Bdd;

    SFTBDDChecker(std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : sylvanBddManager{std::make_shared<
              storm::storage::SylvanBddManager>()},
          dft{std::move(dft)} {
        storm::transformations::dft::SftToBddTransformator<ValueType>
            transformer{this->dft, this->sylvanBddManager};

        topLevelGateBdd = transformer.transform();

        STORM_LOG_ERROR("MCS: " << getMinimalCutSets());
    }

    /**
     * \return
     * Generated Bdd that represents the formula of the top level gate
     */
    Bdd translate() { return topLevelGateBdd; }

    std::set<std::set<std::string>> getMinimalCutSets() {
        auto bdd{minsol(topLevelGateBdd)};

        std::set<std::set<std::string>> rval{};
        std::vector<uint32_t> buffer{};
        recursiveMCS(bdd, buffer, rval);

        return rval;
    }

   private:
    /**
     * The without operator as defined by Rauzy93
     * https://doi.org/10.1016/0951-8320(93)90060-C
     *
     * \node
     * f and g must be monotonic
     *
     * \return
     * f without paths that are included in a path in g
     */
    Bdd without(Bdd const f, Bdd const g) const {
        if (f.isZero() || g.isOne()) {
            return sylvanBddManager->getZero();
        } else if (g.isZero()) {
            return f;
        } else if (f.isOne()) {
            return sylvanBddManager->getOne();
        }

        // f = Ite(x, f1, f2)
        // g = Ite(y, g1, g2)

        if (f.TopVar() < g.TopVar()) {
            auto const f1{f.Then()};
            auto const f2{f.Else()};

            auto const currentVar{f.TopVar()};
            auto const varOne{sylvanBddManager->getPositiveLiteral(currentVar)};

            auto const u{without(f1, g)};
            auto const v{without(f2, g)};

            return varOne.Ite(u, v);
        } else if (f.TopVar() > g.TopVar()) {
            auto const g2{g.Else()};

            return without(f, g2);
        } else {
            auto const f1{f.Then()};
            auto const f2{f.Else()};
            auto const g1{g.Then()};
            auto const g2{g.Else()};

            auto const currentVar{f.TopVar()};
            auto const varOne{sylvanBddManager->getPositiveLiteral(currentVar)};

            auto const u{without(f1, g1)};
            auto const v{without(f2, g2)};

            return varOne.Ite(u, v);
        }
    }

    /**
     * The minsol algorithm as defined by Rauzy93
     * https://doi.org/10.1016/0951-8320(93)90060-C
     *
     * \node
     * f must be monotonic
     *
     * \return
     * A bdd encoding the minmal solutions of f
     */
    Bdd minsol(Bdd const f) const {
        if (f.isTerminal()) return f;

        // f = Ite(x, g, h)

        auto const g{f.Then()};
        auto const h{f.Else()};
        auto const k{minsol(g)};
        auto const u{without(k, h)};
        auto const v{minsol(h)};

        auto const currentVar{f.TopVar()};
        auto const varOne{sylvanBddManager->getPositiveLiteral(currentVar)};
        return varOne.Ite(u, v);
    }

    /**
     * recursivly traverses the given BDD and returns the minimalCutSets
     *
     * \param bdd
     * The current bdd
     *
     * \param buffer
     * Reference to a vector that is used as a stack.
     * Temporarily stores the positive variables encountered.
     *
     * \param minimalCutSets
     * Reference to a set of minimal cut sets.
     * Will be populated by the function.
     */
    void recursiveMCS(Bdd const bdd, std::vector<uint32_t> &buffer,
                      std::set<std::set<std::string>> &minimalCutSets) const {
        if (bdd.isOne()) {
            std::set<std::string> minimalCutSet{};
            for (auto const &var : buffer) {
                minimalCutSet.insert(sylvanBddManager->getName(var));
            }
            minimalCutSets.insert(std::move(minimalCutSet));
        } else if (!bdd.isZero()) {
            auto const currentVar{bdd.TopVar()};

            buffer.push_back(currentVar);
            recursiveMCS(bdd.Then(), buffer, minimalCutSets);
            buffer.pop_back();

            recursiveMCS(bdd.Else(), buffer, minimalCutSets);
        }
    }

    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;

    Bdd topLevelGateBdd;
};

}  // namespace modelchecker
}  // namespace storm
