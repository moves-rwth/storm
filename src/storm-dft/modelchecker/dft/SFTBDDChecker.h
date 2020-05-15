#pragma once

#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/transformations/SftToBddTransformator.h"
#include "storm/adapters/eigen.h"
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
          dft{std::move(dft)},
          calculatedTopLevelGate{false} {}

    SFTBDDChecker(
        std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager,
        std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : sylvanBddManager{sylvanBddManager},
          dft{std::move(dft)},
          calculatedTopLevelGate{false} {}

    /**
     * Exports the Bdd that represents the top level gate to a file
     * in the dot format.
     *
     * \param filename
     * The name of the file the dot graph is written to
     */
    void exportBddToDot(std::string const &filename) {
        sylvanBddManager->exportBddToDot(getTopLevelGateBdd(), filename);
    }

    /**
     * \return
     * Generated Bdd that represents the formula of the top level gate
     */
    Bdd getTopLevelGateBdd() {
        if (!calculatedTopLevelGate) {
            storm::transformations::dft::SftToBddTransformator<ValueType>
                transformer{dft, sylvanBddManager};

            topLevelGateBdd = transformer.transform();
            calculatedTopLevelGate = true;
        }
        return topLevelGateBdd;
    }

    /**
     * \return
     * Generated Bdds that represent the logical formula of the given events
     */
    std::map<std::string, Bdd> getRelevantEventBdds(
        std::set<std::string> relevantEventNames) {
        storm::transformations::dft::SftToBddTransformator<ValueType>
            transformer{dft, sylvanBddManager};
        auto results{transformer.transform(relevantEventNames)};

        topLevelGateBdd = results[dft->getTopLevelGate()->name()];
        calculatedTopLevelGate = true;

        return results;
    }

    /**
     * \return
     * A set of minimal cut sets,
     * where the basic events are identified by their name
     */
    std::set<std::set<std::string>> getMinimalCutSets() {
        auto bdd{minsol(getTopLevelGateBdd())};

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
    double getProbabilityAtTimebound(double timebound) {
        return getProbabilityAtTimebound(getTopLevelGateBdd(), timebound);
    }

    /**
     * \return
     * The Probabilities that the given Event fails at the given timebound.
     *
     * \param bdd
     * The bdd that represents an event in the dft.
     * Must be from a call to some function of *this.
     *
     * \note
     * Works only with exponential distributions and no spares.
     * Otherwise the function returns an arbitrary value
     */
    double getProbabilityAtTimebound(Bdd bdd, double timebound) const {
        std::map<uint32_t, double> indexToProbability{};
        for (auto const &be : dft->getBasicElements()) {
            if (be->beType() == storm::storage::BEType::EXPONENTIAL) {
                auto const failureRate{
                    std::static_pointer_cast<
                        storm::storage::BEExponential<ValueType>>(be)
                        ->activeFailureRate()};

                // exponential distribution
                // p(T <= t) = 1 - exp(-lambda*t)
                auto const failureProbability{
                    1 - std::exp(-failureRate * timebound)};

                auto const currentIndex{sylvanBddManager->getIndex(be->name())};
                indexToProbability[currentIndex] = failureProbability;
            } else {
                STORM_LOG_ERROR("Basic Element Type " << be->typestring()
                                                      << " not supported.");
                return -1;
            }
        }

        std::map<uint64_t, double> bddToProbability{};
        auto const probability{
            recursiveProbability(bdd, indexToProbability, bddToProbability)};
        return probability;
    }

    /**
     * \return
     * The Probabilities that the top level gate fails at the given timepoints.
     *
     * \param timepoints
     * Array of timebounds to calculate the failure probabilities for.
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     *
     * \note
     * Works only with exponential distributions and no spares.
     * Otherwise the function returns an arbitrary value
     */
    std::vector<double> getProbabilitiesAtTimepoints(
        std::vector<double> const &timepoints, size_t const chunksize = 0) {
        return getProbabilitiesAtTimepoints(getTopLevelGateBdd(), timepoints,
                                            chunksize);
    }

    /**
     * \return
     * The Probabilities that the given Event fails at the given timepoints.
     *
     * \param bdd
     * The bdd that represents an event in the dft.
     * Must be from a call to some function of *this.
     *
     * \param timepoints
     * Array of timebounds to calculate the failure probabilities for.
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     *
     * \note
     * Works only with exponential distributions and no spares.
     * Otherwise the function returns an arbitrary value
     */
    std::vector<double> getProbabilitiesAtTimepoints(
        Bdd bdd, std::vector<double> const &timepoints,
        size_t chunksize = 0) const {
        if (chunksize == 0) {
            chunksize = timepoints.size();
        }

        // caches
        auto const basicElemets{dft->getBasicElements()};
        std::map<uint32_t, Eigen::ArrayXd> indexToProbabilities{};
        std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
            bddToProbabilities{};

        // The current timepoints we calculate with
        Eigen::ArrayXd timepointsArray{chunksize};

        // The Return vector
        std::vector<double> resultProbabilities{};
        resultProbabilities.reserve(timepoints.size());

        for (size_t currentIndex{}; currentIndex < timepoints.size();
             currentIndex += chunksize) {
            auto const sizeLeft{timepoints.size() - currentIndex};
            if (sizeLeft < chunksize) {
                chunksize = sizeLeft;
                timepointsArray = Eigen::ArrayXd{chunksize};
            }

            // Update current timepoints
            for (size_t i{currentIndex}; i < currentIndex + chunksize; ++i) {
                timepointsArray(i - currentIndex) = timepoints[i];
            }

            // Update the probabilities of the basic elements
            for (auto const &be : basicElemets) {
                if (be->beType() == storm::storage::BEType::EXPONENTIAL) {
                    auto const failureRate{
                        std::static_pointer_cast<
                            storm::storage::BEExponential<ValueType>>(be)
                            ->activeFailureRate()};

                    // exponential distribution
                    // p(T <= t) = 1 - exp(-lambda*t)
                    auto const beIndex{sylvanBddManager->getIndex(be->name())};
                    indexToProbabilities[beIndex] =
                        1 - (-failureRate * timepointsArray).exp();
                } else {
                    STORM_LOG_ERROR("Basic Element Type not supported.");
                    return {};
                }
            }

            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            // and points to an element in bddToProbabilities
            auto const &probabilitiesArray{*recursiveProbabilities(
                chunksize, bdd, indexToProbabilities, bddToProbabilities)};

            // Update result Probabilities
            for (size_t i{0}; i < chunksize; ++i) {
                resultProbabilities.push_back(probabilitiesArray(i));
            }
        }

        return resultProbabilities;
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
     * \returns
     * The probabilities that the bdd is true
     * given the probabilities that the variables are true.
     *
     * \param chunksize
     * The width of the Eigen Arrays
     *
     * \param bdd
     * The bdd for which to calculate the probabilities
     *
     * \param indexToProbabilities
     * A reference to a mapping
     * that must map every variable in the bdd to probabilities
     *
     * \param bddToProbabilities
     * A cache for common sub Bdds.
     * Must be empty or from an earlier call with a bdd that is an
     * ancestor of the current one.
     *
     * \note
     * Great care was made that all pointers
     * are valid elements in bddToProbabilities.
     *
     */
    Eigen::ArrayXd const *recursiveProbabilities(
        size_t const chunksize, Bdd const bdd,
        std::map<uint32_t, Eigen::ArrayXd> const &indexToProbabilities,
        std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
            &bddToProbabilities) const {
        auto const bddId{bdd.GetBDD()};
        auto const it{bddToProbabilities.find(bddId)};
        if (it != bddToProbabilities.end() && it->second.first) {
            return &it->second.second;
        }

        auto &bddToProbabilitiesElement{bddToProbabilities[bddId]};
        if (bdd.isOne()) {
            bddToProbabilitiesElement.first = true;
            bddToProbabilitiesElement.second =
                Eigen::ArrayXd::Constant(chunksize, 1);
            return &bddToProbabilitiesElement.second;
        } else if (bdd.isZero()) {
            bddToProbabilitiesElement.first = true;
            bddToProbabilitiesElement.second =
                Eigen::ArrayXd::Constant(chunksize, 0);
            return &bddToProbabilitiesElement.second;
        }

        auto const &thenProbabilities{*recursiveProbabilities(
            chunksize, bdd.Then(), indexToProbabilities, bddToProbabilities)};
        auto const &elseProbabilities{*recursiveProbabilities(
            chunksize, bdd.Else(), indexToProbabilities, bddToProbabilities)};

        auto const currentVar{bdd.TopVar()};
        auto const &currentProbabilities{indexToProbabilities.at(currentVar)};

        // P(Ite(x, f1, f2)) = P(x) * P(f1) + P(!x) * P(f2)
        bddToProbabilitiesElement.first = true;
        bddToProbabilitiesElement.second =
            currentProbabilities * thenProbabilities +
            (1 - currentProbabilities) * elseProbabilities;
        return &bddToProbabilitiesElement.second;
    }

    std::map<uint64_t, std::map<uint64_t, Bdd>> withoutCache{};
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
    Bdd without(Bdd const f, Bdd const g) {
        if (f.isZero() || g.isOne()) {
            return sylvanBddManager->getZero();
        } else if (g.isZero()) {
            return f;
        } else if (f.isOne()) {
            return sylvanBddManager->getOne();
        }

        auto const it1{withoutCache.find(f.GetBDD())};
        if (it1 != withoutCache.end()) {
            auto const &fCache{it1->second};
            auto const it2{fCache.find(g.GetBDD())};
            if (it2 != fCache.end()) {
                return it2->second;
            }
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

            auto const result{varOne.Ite(u, v)};
            withoutCache[f.GetBDD()][g.GetBDD()] = result;
            return result;
        }
    }

    std::map<uint64_t, Bdd> minsolCache{};
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
    Bdd minsol(Bdd const f) {
        if (f.isTerminal()) return f;

        auto const it{minsolCache.find(f.GetBDD())};
        if (it != minsolCache.end()) {
            return it->second;
        }

        // f = Ite(x, g, h)

        auto const g{f.Then()};
        auto const h{f.Else()};
        auto const k{minsol(g)};
        auto const u{without(k, h)};
        auto const v{minsol(h)};

        auto const currentVar{f.TopVar()};
        auto const varOne{sylvanBddManager->getPositiveLiteral(currentVar)};

        auto const result{varOne.Ite(u, v)};
        minsolCache[result.GetBDD()] = result;
        return result;
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

    bool calculatedTopLevelGate;
    Bdd topLevelGateBdd;
};

}  // namespace modelchecker
}  // namespace storm
