#include <tuple>

#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/transformations/SftToBddTransformator.h"
#include "storm/adapters/eigen.h"

namespace storm {
namespace modelchecker {

using ValueType = SFTBDDChecker::ValueType;
using Bdd = SFTBDDChecker::Bdd;

namespace {

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
ValueType recursiveProbability(
    Bdd const bdd, std::map<uint32_t, ValueType> const &indexToProbability,
    std::map<uint64_t, ValueType> &bddToProbability) {
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

/**
 * \returns
 * The birnbaum importance factor of the given variable
 *
 * \param variableIndex
 * The index of the variable the birnbaum factor should be calculated
 *
 * \param bdd
 * The bdd for which to calculate the factor
 *
 * \param indexToProbability
 * A reference to a mapping
 * that must map every variable in the bdd to a probability
 *
 * \param bddToProbability
 * A cache for common sub Bdds.
 * Must be empty or from an earlier call with a bdd that is an
 * ancestor of the current one.
 *
 * \param bddToBirnbaumFactor
 * A cache for common sub Bdds.
 * Must be empty or from an earlier call with a bdd that is an
 * ancestor of the current one.
 */
ValueType recursiveBirnbaumFactor(
    uint32_t const variableIndex, Bdd const bdd,
    std::map<uint32_t, ValueType> const &indexToProbability,
    std::map<uint64_t, ValueType> &bddToProbability,
    std::map<uint64_t, ValueType> &bddToBirnbaumFactor) {
    if (bdd.isTerminal()) {
        return 0;
    }

    auto const it{bddToBirnbaumFactor.find(bdd.GetBDD())};
    if (it != bddToBirnbaumFactor.end()) {
        return it->second;
    }

    auto const currentVar{bdd.TopVar()};
    auto const currentProbability{indexToProbability.at(currentVar)};

    ValueType birnbaumFactor{0};

    if (currentVar > variableIndex) {
        return 0;
    } else if (currentVar == variableIndex) {
        auto const thenProbability{recursiveProbability(
            bdd.Then(), indexToProbability, bddToProbability)};
        auto const elseProbability{recursiveProbability(
            bdd.Else(), indexToProbability, bddToProbability)};
        birnbaumFactor = thenProbability - elseProbability;
    } else if (currentVar < variableIndex) {
        auto const thenBirnbaumFactor{recursiveBirnbaumFactor(
            variableIndex, bdd.Then(), indexToProbability, bddToProbability,
            bddToBirnbaumFactor)};
        auto const elseBirnbaumFactor{recursiveBirnbaumFactor(
            variableIndex, bdd.Else(), indexToProbability, bddToProbability,
            bddToBirnbaumFactor)};

        birnbaumFactor = currentProbability * thenBirnbaumFactor +
                         (1 - currentProbability) * elseBirnbaumFactor;
    }

    bddToBirnbaumFactor[bdd.GetBDD()] = birnbaumFactor;
    return birnbaumFactor;
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
        &bddToProbabilities) {
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

/**
 * \returns
 * The birnbaum importance factors of the given variable
 *
 * \param chunksize
 * The width of the Eigen Arrays
 *
 * \param bdd
 * The bdd for which to calculate the factors
 *
 * \param variableIndex
 * The index of the variable the birnbaum factors should be calculated
 *
 * \param indexToProbability
 * A reference to a mapping
 * that must map every variable in the bdd to a probabilities
 *
 * \param bddToProbability
 * A cache for common sub Bdds.
 * Must be empty or from an earlier call with a bdd that is an
 * ancestor of the current one.
 *
 * \param bddToBirnbaumFactor
 * A cache for common sub Bdds.
 * Must be empty or from an earlier call with a bdd that is an
 * ancestor of the current one.
 */
Eigen::ArrayXd const *recursiveBirnbaumFactors(
    size_t const chunksize, uint32_t const variableIndex, Bdd const bdd,
    std::map<uint32_t, Eigen::ArrayXd> const &indexToProbabilities,
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        &bddToProbabilities,
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        &bddToBirnbaumFactors) {
    auto const bddId{bdd.GetBDD()};
    auto const it{bddToBirnbaumFactors.find(bddId)};
    if (it != bddToBirnbaumFactors.end() && it->second.first) {
        return &it->second.second;
    }

    auto &bddToBirnbaumFactorsElement{bddToBirnbaumFactors[bddId]};
    if (bdd.isTerminal() || bdd.TopVar() > variableIndex) {
        // return vector 0;
        bddToBirnbaumFactorsElement.second =
            Eigen::ArrayXd::Constant(chunksize, 0);
        return &bddToBirnbaumFactorsElement.second;
    }

    auto const currentVar{bdd.TopVar()};
    auto const &currentProbabilities{indexToProbabilities.at(currentVar)};

    if (currentVar == variableIndex) {
        auto const &thenProbabilities{*recursiveProbabilities(
            chunksize, bdd.Then(), indexToProbabilities, bddToProbabilities)};
        auto const &elseProbabilities{*recursiveProbabilities(
            chunksize, bdd.Else(), indexToProbabilities, bddToProbabilities)};

        bddToBirnbaumFactorsElement.first = true;
        bddToBirnbaumFactorsElement.second =
            thenProbabilities - elseProbabilities;
        return &bddToBirnbaumFactorsElement.second;
    }

    // currentVar < variableIndex
    auto const &thenBirnbaumFactors{*recursiveBirnbaumFactors(
        chunksize, variableIndex, bdd.Then(), indexToProbabilities,
        bddToProbabilities, bddToBirnbaumFactors)};
    auto const &elseBirnbaumFactors{*recursiveBirnbaumFactors(
        chunksize, variableIndex, bdd.Else(), indexToProbabilities,
        bddToProbabilities, bddToBirnbaumFactors)};

    bddToBirnbaumFactorsElement.first = true;
    bddToBirnbaumFactorsElement.second =
        currentProbabilities * thenBirnbaumFactors +
        (1 - currentProbabilities) * elseBirnbaumFactors;
    return &bddToBirnbaumFactorsElement.second;
}
}  // namespace

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

template <typename T>
void SFTBDDChecker::chunkCalculationTemplate(
    T func, std::vector<ValueType> const &timepoints, size_t chunksize) const {
    if (chunksize == 0) {
        chunksize = timepoints.size();
    }

    // caches
    auto const basicElemets{dft->getBasicElements()};
    std::map<uint32_t, Eigen::ArrayXd> indexToProbabilities{};

    // The current timepoints we calculate with
    Eigen::ArrayXd timepointsArray{chunksize};

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

        func(chunksize, timepointsArray, indexToProbabilities);
    }
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
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::vector<ValueType> resultProbabilities{};
    resultProbabilities.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            // and points to an element in bddToProbabilities
            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultProbabilities.push_back(probabilitiesArray(i));
            }
        },
        timepoints, chunksize);

    return resultProbabilities;
}

ValueType SFTBDDChecker::getBirnbaumFactorAtTimebound(std::string const &beName,
                                                      ValueType timebound) {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    auto const bdd{getTopLevelGateBdd()};
    auto const index{sylvanBddManager->getIndex(beName)};
    std::map<uint64_t, ValueType> bddToProbability{};
    std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
    auto const birnbaumFactor{recursiveBirnbaumFactor(
        index, bdd, indexToProbability, bddToProbability, bddToBirnbaumFactor)};
    return birnbaumFactor;
}

std::vector<ValueType> SFTBDDChecker::getAllBirnbaumFactorsAtTimebound(
    ValueType timebound) {
    auto const bdd{getTopLevelGateBdd()};

    std::vector<ValueType> birnbaumFactors{};
    birnbaumFactors.reserve(dft->getBasicElements().size());

    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }
    std::map<uint64_t, ValueType> bddToProbability{};

    for (auto const &be : dft->getBasicElements()) {
        auto const index{sylvanBddManager->getIndex(be->name())};
        std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
        auto const birnbaumFactor{
            recursiveBirnbaumFactor(index, bdd, indexToProbability,
                                    bddToProbability, bddToBirnbaumFactor)};
        birnbaumFactors.push_back(birnbaumFactor);
    }
    return birnbaumFactors;
}

std::vector<ValueType> SFTBDDChecker::getBirnbaumFactorsAtTimepoints(
    std::string const &beName, std::vector<ValueType> const &timepoints,
    size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<ValueType> resultVector{};
    resultVector.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd caches
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }
            for (auto &i : bddToBirnbaumFactors) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            auto const index{sylvanBddManager->getIndex(beName)};
            auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                currentChunksize, index, bdd, indexToProbabilities,
                bddToProbabilities, bddToBirnbaumFactors)};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultVector.push_back(birnbaumFactorsArray(i));
            }
        },
        timepoints, chunksize);

    return resultVector;
}

std::vector<std::vector<ValueType>>
SFTBDDChecker::getAllBirnbaumFactorsAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    auto const basicElemets{dft->getBasicElements()};

    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<std::vector<ValueType>> resultVector{};
    resultVector.resize(dft->getBasicElements().size());
    for (auto &i : resultVector) {
        i.reserve(timepoints.size());
    }

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            for (size_t basicElemetIndex{0};
                 basicElemetIndex < basicElemets.size(); ++basicElemetIndex) {
                auto const &be{basicElemets[basicElemetIndex]};
                // Invalidate bdd cache
                for (auto &i : bddToBirnbaumFactors) {
                    i.second.first = false;
                }

                // Great care was made so that the pointer returned is always
                // valid and points to an element in bddToProbabilities
                auto const index{sylvanBddManager->getIndex(be->name())};
                auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                    currentChunksize, index, bdd, indexToProbabilities,
                    bddToProbabilities, bddToBirnbaumFactors)};

                // Update result Probabilities
                for (size_t i{0}; i < currentChunksize; ++i) {
                    resultVector[basicElemetIndex].push_back(
                        birnbaumFactorsArray(i));
                }
            }
        },
        timepoints, chunksize);

    return resultVector;
}

ValueType SFTBDDChecker::getCIFAtTimebound(std::string const &beName,
                                           ValueType timebound) {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    auto const bdd{getTopLevelGateBdd()};
    auto const index{sylvanBddManager->getIndex(beName)};
    std::map<uint64_t, ValueType> bddToProbability{};
    std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};
    auto const birnbaumFactor{recursiveBirnbaumFactor(
        index, bdd, indexToProbability, bddToProbability, bddToBirnbaumFactor)};
    auto const CIF{(indexToProbability[index] / probability) * birnbaumFactor};
    return CIF;
}

std::vector<ValueType> SFTBDDChecker::getAllCIFsAtTimebound(
    ValueType timebound) {
    auto const bdd{getTopLevelGateBdd()};

    std::vector<ValueType> resultVector{};
    resultVector.reserve(dft->getBasicElements().size());

    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }
    std::map<uint64_t, ValueType> bddToProbability{};

    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};

    for (auto const &be : dft->getBasicElements()) {
        auto const index{sylvanBddManager->getIndex(be->name())};
        std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
        auto const birnbaumFactor{
            recursiveBirnbaumFactor(index, bdd, indexToProbability,
                                    bddToProbability, bddToBirnbaumFactor)};
        auto const CIF{(indexToProbability[index] / probability) *
                       birnbaumFactor};
        resultVector.push_back(CIF);
    }
    return resultVector;
}

std::vector<ValueType> SFTBDDChecker::getCIFsAtTimepoints(
    std::string const &beName, std::vector<ValueType> const &timepoints,
    size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<ValueType> resultVector{};
    resultVector.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd caches
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }
            for (auto &i : bddToBirnbaumFactors) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            auto const index{sylvanBddManager->getIndex(beName)};
            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};
            auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                currentChunksize, index, bdd, indexToProbabilities,
                bddToProbabilities, bddToBirnbaumFactors)};

            auto const CIFArray{
                (indexToProbabilities.at(index) / probabilitiesArray) *
                birnbaumFactorsArray};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultVector.push_back(CIFArray(i));
            }
        },
        timepoints, chunksize);

    return resultVector;
}

std::vector<std::vector<ValueType>> SFTBDDChecker::getAllCIFsAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    auto const basicElemets{dft->getBasicElements()};

    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<std::vector<ValueType>> resultVector{};
    resultVector.resize(dft->getBasicElements().size());
    for (auto &i : resultVector) {
        i.reserve(timepoints.size());
    }

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};

            for (size_t basicElemetIndex{0};
                 basicElemetIndex < basicElemets.size(); ++basicElemetIndex) {
                auto const &be{basicElemets[basicElemetIndex]};
                // Invalidate bdd cache
                for (auto &i : bddToBirnbaumFactors) {
                    i.second.first = false;
                }

                // Great care was made so that the pointer returned is always
                // valid and points to an element in bddToProbabilities
                auto const index{sylvanBddManager->getIndex(be->name())};
                auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                    currentChunksize, index, bdd, indexToProbabilities,
                    bddToProbabilities, bddToBirnbaumFactors)};

                auto const CIFArray{
                    (indexToProbabilities.at(index) / probabilitiesArray) *
                    birnbaumFactorsArray};

                // Update result Probabilities
                for (size_t i{0}; i < currentChunksize; ++i) {
                    resultVector[basicElemetIndex].push_back(CIFArray(i));
                }
            }
        },
        timepoints, chunksize);

    return resultVector;
}

ValueType SFTBDDChecker::getDIFAtTimebound(std::string const &beName,
                                           ValueType timebound) {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    auto const bdd{getTopLevelGateBdd()};
    auto const index{sylvanBddManager->getIndex(beName)};
    std::map<uint64_t, ValueType> bddToProbability{};
    std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};
    auto const birnbaumFactor{recursiveBirnbaumFactor(
        index, bdd, indexToProbability, bddToProbability, bddToBirnbaumFactor)};
    auto const &beProbability{indexToProbability[index]};
    auto const DIF{beProbability +
                   (beProbability * (1 - beProbability) * birnbaumFactor) /
                       probability};
    return DIF;
}

std::vector<ValueType> SFTBDDChecker::getAllDIFsAtTimebound(
    ValueType timebound) {
    auto const bdd{getTopLevelGateBdd()};

    std::vector<ValueType> resultVector{};
    resultVector.reserve(dft->getBasicElements().size());

    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }
    std::map<uint64_t, ValueType> bddToProbability{};

    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};

    for (auto const &be : dft->getBasicElements()) {
        auto const index{sylvanBddManager->getIndex(be->name())};
        std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
        auto const birnbaumFactor{
            recursiveBirnbaumFactor(index, bdd, indexToProbability,
                                    bddToProbability, bddToBirnbaumFactor)};
        auto const &beProbability{indexToProbability[index]};
        auto const DIF{beProbability +
                       (beProbability * (1 - beProbability) * birnbaumFactor) /
                           probability};
        resultVector.push_back(DIF);
    }
    return resultVector;
}

std::vector<ValueType> SFTBDDChecker::getDIFsAtTimepoints(
    std::string const &beName, std::vector<ValueType> const &timepoints,
    size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<ValueType> resultVector{};
    resultVector.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd caches
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }
            for (auto &i : bddToBirnbaumFactors) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            auto const index{sylvanBddManager->getIndex(beName)};
            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};
            auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                currentChunksize, index, bdd, indexToProbabilities,
                bddToProbabilities, bddToBirnbaumFactors)};

            auto const &beProbabilitiesArray{indexToProbabilities.at(index)};
            auto const DIFArray{
                beProbabilitiesArray +
                (beProbabilitiesArray * (1 - beProbabilitiesArray) *
                 birnbaumFactorsArray) /
                    probabilitiesArray};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultVector.push_back(DIFArray(i));
            }
        },
        timepoints, chunksize);

    return resultVector;
}

std::vector<std::vector<ValueType>> SFTBDDChecker::getAllDIFsAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    auto const basicElemets{dft->getBasicElements()};

    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<std::vector<ValueType>> resultVector{};
    resultVector.resize(dft->getBasicElements().size());
    for (auto &i : resultVector) {
        i.reserve(timepoints.size());
    }

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};

            for (size_t basicElemetIndex{0};
                 basicElemetIndex < basicElemets.size(); ++basicElemetIndex) {
                auto const &be{basicElemets[basicElemetIndex]};
                // Invalidate bdd cache
                for (auto &i : bddToBirnbaumFactors) {
                    i.second.first = false;
                }

                // Great care was made so that the pointer returned is always
                // valid and points to an element in bddToProbabilities
                auto const index{sylvanBddManager->getIndex(be->name())};
                auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                    currentChunksize, index, bdd, indexToProbabilities,
                    bddToProbabilities, bddToBirnbaumFactors)};

                auto const &beProbabilitiesArray{
                    indexToProbabilities.at(index)};
                auto const DIFArray{
                    beProbabilitiesArray +
                    (beProbabilitiesArray * (1 - beProbabilitiesArray) *
                     birnbaumFactorsArray) /
                        probabilitiesArray};

                // Update result Probabilities
                for (size_t i{0}; i < currentChunksize; ++i) {
                    resultVector[basicElemetIndex].push_back(DIFArray(i));
                }
            }
        },
        timepoints, chunksize);

    return resultVector;
}

ValueType SFTBDDChecker::getRAWAtTimebound(std::string const &beName,
                                           ValueType timebound) {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    auto const bdd{getTopLevelGateBdd()};
    auto const index{sylvanBddManager->getIndex(beName)};
    std::map<uint64_t, ValueType> bddToProbability{};
    std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};
    auto const birnbaumFactor{recursiveBirnbaumFactor(
        index, bdd, indexToProbability, bddToProbability, bddToBirnbaumFactor)};
    auto const &beProbability{indexToProbability[index]};
    auto const RAW{1 + ((1 - beProbability) * birnbaumFactor) / probability};
    return RAW;
}

std::vector<ValueType> SFTBDDChecker::getAllRAWsAtTimebound(
    ValueType timebound) {
    auto const bdd{getTopLevelGateBdd()};

    std::vector<ValueType> resultVector{};
    resultVector.reserve(dft->getBasicElements().size());

    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }
    std::map<uint64_t, ValueType> bddToProbability{};

    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};

    for (auto const &be : dft->getBasicElements()) {
        auto const index{sylvanBddManager->getIndex(be->name())};
        std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
        auto const birnbaumFactor{
            recursiveBirnbaumFactor(index, bdd, indexToProbability,
                                    bddToProbability, bddToBirnbaumFactor)};
        auto const &beProbability{indexToProbability[index]};
        auto const RAW{1 +
                       ((1 - beProbability) * birnbaumFactor) / probability};
        resultVector.push_back(RAW);
    }
    return resultVector;
}

std::vector<ValueType> SFTBDDChecker::getRAWsAtTimepoints(
    std::string const &beName, std::vector<ValueType> const &timepoints,
    size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<ValueType> resultVector{};
    resultVector.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd caches
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }
            for (auto &i : bddToBirnbaumFactors) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            auto const index{sylvanBddManager->getIndex(beName)};
            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};
            auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                currentChunksize, index, bdd, indexToProbabilities,
                bddToProbabilities, bddToBirnbaumFactors)};

            auto const &beProbabilitiesArray{indexToProbabilities.at(index)};
            auto const RAWArray{
                1 + ((1 - beProbabilitiesArray) * birnbaumFactorsArray) /
                        probabilitiesArray};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultVector.push_back(RAWArray(i));
            }
        },
        timepoints, chunksize);

    return resultVector;
}

std::vector<std::vector<ValueType>> SFTBDDChecker::getAllRAWsAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    auto const basicElemets{dft->getBasicElements()};

    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<std::vector<ValueType>> resultVector{};
    resultVector.resize(dft->getBasicElements().size());
    for (auto &i : resultVector) {
        i.reserve(timepoints.size());
    }

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};

            for (size_t basicElemetIndex{0};
                 basicElemetIndex < basicElemets.size(); ++basicElemetIndex) {
                auto const &be{basicElemets[basicElemetIndex]};
                // Invalidate bdd cache
                for (auto &i : bddToBirnbaumFactors) {
                    i.second.first = false;
                }

                // Great care was made so that the pointer returned is always
                // valid and points to an element in bddToProbabilities
                auto const index{sylvanBddManager->getIndex(be->name())};
                auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                    currentChunksize, index, bdd, indexToProbabilities,
                    bddToProbabilities, bddToBirnbaumFactors)};

                auto const &beProbabilitiesArray{
                    indexToProbabilities.at(index)};
                auto const RAWArray{
                    1 + ((1 - beProbabilitiesArray) * birnbaumFactorsArray) /
                            probabilitiesArray};

                // Update result Probabilities
                for (size_t i{0}; i < currentChunksize; ++i) {
                    resultVector[basicElemetIndex].push_back(RAWArray(i));
                }
            }
        },
        timepoints, chunksize);

    return resultVector;
}

ValueType SFTBDDChecker::getRRWAtTimebound(std::string const &beName,
                                           ValueType timebound) {
    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }

    auto const bdd{getTopLevelGateBdd()};
    auto const index{sylvanBddManager->getIndex(beName)};
    std::map<uint64_t, ValueType> bddToProbability{};
    std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};
    auto const birnbaumFactor{recursiveBirnbaumFactor(
        index, bdd, indexToProbability, bddToProbability, bddToBirnbaumFactor)};
    auto const &beProbability{indexToProbability[index]};
    auto const RRW{probability /
                   (probability - beProbability * birnbaumFactor)};
    return RRW;
}

std::vector<ValueType> SFTBDDChecker::getAllRRWsAtTimebound(
    ValueType timebound) {
    auto const bdd{getTopLevelGateBdd()};

    std::vector<ValueType> resultVector{};
    resultVector.reserve(dft->getBasicElements().size());

    std::map<uint32_t, ValueType> indexToProbability{};
    for (auto const &be : dft->getBasicElements()) {
        auto const currentIndex{sylvanBddManager->getIndex(be->name())};
        indexToProbability[currentIndex] = be->getUnreliability(timebound);
    }
    std::map<uint64_t, ValueType> bddToProbability{};

    auto const probability{
        recursiveProbability(bdd, indexToProbability, bddToProbability)};

    for (auto const &be : dft->getBasicElements()) {
        auto const index{sylvanBddManager->getIndex(be->name())};
        std::map<uint64_t, ValueType> bddToBirnbaumFactor{};
        auto const birnbaumFactor{
            recursiveBirnbaumFactor(index, bdd, indexToProbability,
                                    bddToProbability, bddToBirnbaumFactor)};
        auto const &beProbability{indexToProbability[index]};
        auto const RRW{probability /
                       (probability - beProbability * birnbaumFactor)};
        resultVector.push_back(RRW);
    }
    return resultVector;
}

std::vector<ValueType> SFTBDDChecker::getRRWsAtTimepoints(
    std::string const &beName, std::vector<ValueType> const &timepoints,
    size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<ValueType> resultVector{};
    resultVector.reserve(timepoints.size());

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd caches
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }
            for (auto &i : bddToBirnbaumFactors) {
                i.second.first = false;
            }

            // Great care was made so that the pointer returned is always valid
            auto const index{sylvanBddManager->getIndex(beName)};
            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};
            auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                currentChunksize, index, bdd, indexToProbabilities,
                bddToProbabilities, bddToBirnbaumFactors)};

            auto const &beProbabilitiesArray{indexToProbabilities.at(index)};
            auto const RRWArray{probabilitiesArray /
                                (probabilitiesArray -
                                 beProbabilitiesArray * birnbaumFactorsArray)};

            // Update result Probabilities
            for (size_t i{0}; i < currentChunksize; ++i) {
                resultVector.push_back(RRWArray(i));
            }
        },
        timepoints, chunksize);

    return resultVector;
}

std::vector<std::vector<ValueType>> SFTBDDChecker::getAllRRWsAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t chunksize) {
    auto const bdd{getTopLevelGateBdd()};
    auto const basicElemets{dft->getBasicElements()};

    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToProbabilities{};
    std::unordered_map<uint64_t, std::pair<bool, Eigen::ArrayXd>>
        bddToBirnbaumFactors{};
    std::vector<std::vector<ValueType>> resultVector{};
    resultVector.resize(dft->getBasicElements().size());
    for (auto &i : resultVector) {
        i.reserve(timepoints.size());
    }

    chunkCalculationTemplate(
        [&](auto const currentChunksize, auto const &timepointsArray,
            auto const &indexToProbabilities) {
            // Invalidate bdd cache
            for (auto &i : bddToProbabilities) {
                i.second.first = false;
            }

            auto const &probabilitiesArray{*recursiveProbabilities(
                currentChunksize, bdd, indexToProbabilities,
                bddToProbabilities)};

            for (size_t basicElemetIndex{0};
                 basicElemetIndex < basicElemets.size(); ++basicElemetIndex) {
                auto const &be{basicElemets[basicElemetIndex]};
                // Invalidate bdd cache
                for (auto &i : bddToBirnbaumFactors) {
                    i.second.first = false;
                }

                // Great care was made so that the pointer returned is always
                // valid and points to an element in bddToProbabilities
                auto const index{sylvanBddManager->getIndex(be->name())};
                auto const &birnbaumFactorsArray{*recursiveBirnbaumFactors(
                    currentChunksize, index, bdd, indexToProbabilities,
                    bddToProbabilities, bddToBirnbaumFactors)};

                auto const &beProbabilitiesArray{
                    indexToProbabilities.at(index)};
                auto const RRWArray{
                    probabilitiesArray /
                    (probabilitiesArray -
                     beProbabilitiesArray * birnbaumFactorsArray)};

                // Update result Probabilities
                for (size_t i{0}; i < currentChunksize; ++i) {
                    resultVector[basicElemetIndex].push_back(RRWArray(i));
                }
            }
        },
        timepoints, chunksize);

    return resultVector;
}

Bdd SFTBDDChecker::without(Bdd const f, Bdd const g) {
    if (f.isZero() || g.isOne()) {
        return sylvanBddManager->getZero();
    } else if (g.isZero()) {
        return f;
    } else if (f.isOne()) {
        return sylvanBddManager->getOne();
    }

    auto const inputPair{std::make_pair(f.GetBDD(), g.GetBDD())};
    auto const it{withoutCache.find(inputPair)};
    if (it != withoutCache.end()) {
        return it->second;
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
        withoutCache[inputPair] = result;
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
    minsolCache[f.GetBDD()] = result;
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
