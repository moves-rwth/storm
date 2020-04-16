#pragma once

#include <map>
#include <memory>
#include <set>
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
    }

    /**
     * Exports the Bdd that represents the top level gate to a file
     * in the dot format.
     *
     * \param filename
     * The name of the file the dot graph is written to
     */
    void exportBddToDot(std::string const &filename) {
        FILE *filePointer = fopen(filename.c_str(), "w+");

        // fopen returns a nullptr on failure
        if (filePointer == nullptr) {
            STORM_LOG_ERROR("Failure to open file: " << filename);
        } else {
            topLevelGateBdd.PrintDot(filePointer);
            fclose(filePointer);
        }
    }

    /**
     * \return
     * Generated Bdd that represents the formula of the top level gate
     */
    Bdd getBdd() { return topLevelGateBdd; }

    /**
     * \return
     * A set of minimal cut sets,
     * where the basic events are identified by their name
     */
    std::set<std::set<std::string>> getMinimalCutSets() const {
        auto bdd{minsol(topLevelGateBdd)};

        std::set<std::set<std::string>> rval{};
        std::vector<uint32_t> buffer{};
        recursiveMCS(bdd, buffer, rval);

        return rval;
    }

    /**
     * \return
     * The Probability that the top level gate fails.
     *
     * \note
     * Works only with exponential distributions and no spares.
     * Otherwise the function returns an arbitrary value
     */
    double getProbabilityAtTimebound(double timebound) const {
        std::map<uint32_t, double> indexToProbability{};
        for (auto const &be : dft->getBasicElements()) {
            if (be->type() == storm::storage::DFTElementType::BE_EXP) {
                auto const failureRate{
                    std::static_pointer_cast<
                        storm::storage::BEExponential<ValueType>>(be)
                        ->activeFailureRate()};

                // exponential distribution
                // p(T <= t) = 1 - exp(-lambda*t)
                auto const failurePropability{
                    1 - std::exp(-failureRate * timebound)};

                auto const currentIndex{sylvanBddManager->getIndex(be->name())};
                indexToProbability[currentIndex] = failurePropability;
            } else {
                STORM_LOG_ERROR("Basic Element Type not supported.");
                return 1;
            }
        }

        std::map<uint64_t, double> bddToProbability{};
        auto const probability{recursiveProbability(
            topLevelGateBdd, indexToProbability, bddToProbability)};
        return probability;
    }

   private:
    /**
     * \returns
     * The probability that the bdd is true
     * given the probabilities that the variables are true.
     *
     * \param bdd
     * The bdd for which to calculate the probability
     *
     * \param indexToProbability
     * A reference to a mapping
     * that must map every variable in the bdd to a probability
     *
     * \param bddToProbability
     * A cache for common sub Bdds.
     * Must be empty or from an earlier call with a bdd that is an
     * ancestor of the current one.
     */
    double recursiveProbability(
        Bdd const bdd, std::map<uint32_t, double> const &indexToProbability,
        std::map<uint64_t, double> &bddToProbability) const {
        if (bdd.isOne()) {
            return 1;
        } else if (bdd.isZero()) {
            return 0;
        }

        auto const it{bddToProbability.find(bdd.GetBDD())};
        if (it != bddToProbability.end()) {
            return it->second;
        }

        auto const currentVar{bdd.TopVar()};
        auto const currentProbability{indexToProbability.at(currentVar)};

        auto const thenProbability{recursiveProbability(
            bdd.Then(), indexToProbability, bddToProbability)};
        auto const elseProbability{recursiveProbability(
            bdd.Else(), indexToProbability, bddToProbability)};

        // P(Ite(x, f1, f2)) = P(x) * P(f1) + P(!x) * P(f2)
        auto const probability{currentProbability * thenProbability +
                               (1 - currentProbability) * elseProbability};
        bddToProbability[bdd.GetBDD()] = probability;
        return probability;
    }

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
