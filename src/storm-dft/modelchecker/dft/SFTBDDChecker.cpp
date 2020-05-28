#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"

namespace storm {
namespace modelchecker {

using ValueType = SFTBDDChecker::ValueType;
using Bdd = SFTBDDChecker::Bdd;

SFTBDDChecker::SFTBDDChecker(
    std::shared_ptr<storm::storage::DFT<ValueType>> dft)
    : sylvanBddManager{std::make_shared<storm::storage::SylvanBddManager>()},
      dft{std::move(dft)},
      calculatedTopLevelGate{false} {}

SFTBDDChecker::SFTBDDChecker(
    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager,
    std::shared_ptr<storm::storage::DFT<ValueType>> dft)
    : sylvanBddManager{sylvanBddManager},
      dft{std::move(dft)},
      calculatedTopLevelGate{false} {}

Bdd SFTBDDChecker::getTopLevelGateBdd() {
    if (!calculatedTopLevelGate) {
        storm::transformations::dft::SftToBddTransformator<ValueType>
            transformer{dft, sylvanBddManager};

        topLevelGateBdd = transformer.transform();
        calculatedTopLevelGate = true;
    }
    return topLevelGateBdd;
}

std::map<std::string, Bdd> SFTBDDChecker::getRelevantEventBdds(
    std::set<std::string> relevantEventNames) {
    storm::transformations::dft::SftToBddTransformator<ValueType> transformer{
        dft, sylvanBddManager};
    auto results{transformer.transform(relevantEventNames)};

    topLevelGateBdd = results[dft->getTopLevelGate()->name()];
    calculatedTopLevelGate = true;

    return results;
}

std::set<std::set<std::string>> SFTBDDChecker::getMinimalCutSets() {
    auto bdd{minsol(getTopLevelGateBdd())};

    std::set<std::set<std::string>> rval{};
    std::vector<uint32_t> buffer{};
    recursiveMCS(bdd, buffer, rval);

    return rval;
}

ValueType SFTBDDChecker::getProbabilityAtTimebound(Bdd bdd,
                                                   ValueType timebound) const {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    std::map<uint64_t, ValueType> bddToProbability{};
    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};
    return probability;
}

std::vector<ValueType> SFTBDDChecker::getProbabilitiesAtTimepoints(
    Bdd bdd, std::vector<ValueType> const &timepoints, size_t chunksize) const {
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
    std::vector<ValueType> resultProbabilities{};
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
            auto const beIndex{sylvanBddManager->getIndex(be->name())};
            // Vectorize known BETypes
            // fallback to getUnreliability() otherwise
            if (be->beType() == storm::storage::BEType::EXPONENTIAL) {
                auto const failureRate{
                    std::static_pointer_cast<
                        storm::storage::BEExponential<ValueType>>(be)
                        ->activeFailureRate()};

                // exponential distribution
                // p(T <= t) = 1 - exp(-lambda*t)
                indexToProbabilities[beIndex] =
                    1 - (-failureRate * timepointsArray).exp();
            } else {
                auto probabilities{timepointsArray};
                for (size_t i{0}; i < chunksize; ++i) {
                    probabilities(i) = be->getUnreliability(timepointsArray(i));
                }
                indexToProbabilities[beIndex] = probabilities;
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

ValueType SFTBDDChecker::recursiveProbability(
    Bdd const bdd, std::map<uint32_t, ValueType> const &indexToProbability,
    std::map<uint64_t, ValueType> &bddToProbability) const {
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

    auto const thenProbability{
        recursiveProbability(bdd.Then(), indexToProbability, bddToProbability)};
    auto const elseProbability{
        recursiveProbability(bdd.Else(), indexToProbability, bddToProbability)};

    // P(Ite(x, f1, f2)) = P(x) * P(f1) + P(!x) * P(f2)
    auto const probability{currentProbability * thenProbability +
                           (1 - currentProbability) * elseProbability};
    bddToProbability[bdd.GetBDD()] = probability;
    return probability;
}

Eigen::ArrayXd const *SFTBDDChecker::recursiveProbabilities(
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

Bdd SFTBDDChecker::without(Bdd const f, Bdd const g) {
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

Bdd SFTBDDChecker::minsol(Bdd const f) {
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

void SFTBDDChecker::recursiveMCS(
    Bdd const bdd, std::vector<uint32_t> &buffer,
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

}  // namespace modelchecker
}  // namespace storm
