#pragma once

#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "storm-dft/storage/SylvanBddManager.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/storage/PairHash.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for the SFTBDDChecker
 *
 */
class SFTBDDChecker {
   public:
    using ValueType = double;
    using Bdd = sylvan::Bdd;

    SFTBDDChecker(std::shared_ptr<storm::storage::DFT<ValueType>> dft);

    SFTBDDChecker(
        std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager,
        std::shared_ptr<storm::storage::DFT<ValueType>> dft);

    /**
     * \return
     * The internal sylvanBddManager
     */
    std::shared_ptr<storm::storage::SylvanBddManager> getSylvanBddManager() {
        return sylvanBddManager;
    }

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
    Bdd getTopLevelGateBdd();

    /**
     * \return
     * Generated Bdds that represent the logical formula of the given events
     */
    std::map<std::string, Bdd> getRelevantEventBdds(
        std::set<std::string> relevantEventNames);

    /**
     * \return
     * A set of minimal cut sets,
     * where the basic events are identified by their name
     */
    std::vector<std::vector<std::string>> getMinimalCutSets();

    /**
     * \return
     * A set of minimal cut sets,
     * where the basic events are identified by their index
     * in the bdd manager
     */
    std::vector<std::vector<uint32_t>> getMinimalCutSetsAsIndices();

    /**
     * \return
     * The Probability that the top level gate fails.
     *
     * \note
     * Works only with exponential distributions and no spares.
     * Otherwise the function returns an arbitrary value
     */
    ValueType getProbabilityAtTimebound(ValueType timebound) {
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
    ValueType getProbabilityAtTimebound(Bdd bdd, ValueType timebound) const;

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
    std::vector<ValueType> getProbabilitiesAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t const chunksize = 0) {
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
    std::vector<ValueType> getProbabilitiesAtTimepoints(
        Bdd bdd, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0) const;

    /**
     * \return
     * The birnbaum importance factor of the given basic event
     * at the given timebound
     */
    ValueType getBirnbaumFactorAtTimebound(std::string const &beName,
                                           ValueType timebound);

    /**
     * \return
     * The birnbaum importance factor of all basic events
     * at the given timebound
     *
     * \note
     * Sorted after the order of dft->getBasicElements.
     * Faster than looping over getBirnbaumFactorAtTimebound.
     */
    std::vector<ValueType> getAllBirnbaumFactorsAtTimebound(
        ValueType timebound);

    /**
     * \return
     * The birnbaum importance factors of the given basic event
     *
     * \param timepoints
     * Array of timebounds to calculate the factors for.
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<ValueType> getBirnbaumFactorsAtTimepoints(
        std::string const &beName, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0);

    /**
     * \return
     * The birnbaum importance factors of the given basic event
     *
     * \param timepoints
     * Array of timebounds to calculate the factors for.
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<std::vector<ValueType>> getAllBirnbaumFactorsAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t chunksize = 0);

    /**
     * \return
     * The Critical importance factor of the given basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     */
    ValueType getCIFAtTimebound(std::string const &beName, ValueType timebound);

    /**
     * \return
     * The Critical importance factor of all basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \note
     * Sorted after the order of dft->getBasicElements.
     * Faster than looping over getBirnbaumFactorAtTimebound.
     */
    std::vector<ValueType> getAllCIFsAtTimebound(ValueType timebound);

    /**
     * \return
     * The Critical importance factor of the given basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<ValueType> getCIFsAtTimepoints(
        std::string const &beName, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0);

    /**
     * \return
     * The Critical importance factor of all basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<std::vector<ValueType>> getAllCIFsAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t chunksize = 0);

    /**
     * \return
     * The Diagnostic importance factor of the given basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     */
    ValueType getDIFAtTimebound(std::string const &beName, ValueType timebound);

    /**
     * \return
     * The Diagnostic importance factor of all basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \note
     * Sorted after the order of dft->getBasicElements.
     * Faster than looping over getBirnbaumFactorAtTimebound.
     */
    std::vector<ValueType> getAllDIFsAtTimebound(ValueType timebound);

    /**
     * \return
     * The Diagnostic importance factor of the given basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<ValueType> getDIFsAtTimepoints(
        std::string const &beName, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0);

    /**
     * \return
     * The Diagnostic importance factor of all basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<std::vector<ValueType>> getAllDIFsAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t chunksize = 0);

    /**
     * \return
     * The risk achievement worth of the given basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     */
    ValueType getRAWAtTimebound(std::string const &beName, ValueType timebound);

    /**
     * \return
     * The risk achievement worth of all basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \note
     * Sorted after the order of dft->getBasicElements.
     * Faster than looping over getBirnbaumFactorAtTimebound.
     */
    std::vector<ValueType> getAllRAWsAtTimebound(ValueType timebound);

    /**
     * \return
     * The risk achievement worth of the given basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<ValueType> getRAWsAtTimepoints(
        std::string const &beName, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0);

    /**
     * \return
     * The risk achievement worth of all basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<std::vector<ValueType>> getAllRAWsAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t chunksize = 0);

    /**
     * \return
     * The risk reduction worth of the given basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     */
    ValueType getRRWAtTimebound(std::string const &beName, ValueType timebound);

    /**
     * \return
     * The risk reduction worth of all basic event
     * at the given timebound as defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \note
     * Sorted after the order of dft->getBasicElements.
     * Faster than looping over getBirnbaumFactorAtTimebound.
     */
    std::vector<ValueType> getAllRRWsAtTimebound(ValueType timebound);

    /**
     * \return
     * The risk reduction worth of the given basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<ValueType> getRRWsAtTimepoints(
        std::string const &beName, std::vector<ValueType> const &timepoints,
        size_t chunksize = 0);

    /**
     * \return
     * The risk reduction worth of all basic event
     * defined in
     * 10.1016/S0951-8320(01)00004-7
     *
     * \param chunksize
     * Splits the timepoints array into chunksize chunks.
     * A value of 0 represents to calculate the whole array at once.
     */
    std::vector<std::vector<ValueType>> getAllRRWsAtTimepoints(
        std::vector<ValueType> const &timepoints, size_t chunksize = 0);

   private:
    std::unordered_map<std::pair<uint64_t, uint64_t>, Bdd,
                       std::hash<std::pair<uint_fast64_t, uint_fast64_t>>>
        withoutCache;
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
    Bdd without(Bdd const f, Bdd const g);

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
    Bdd minsol(Bdd const f);

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
                      std::vector<std::vector<uint32_t>> &minimalCutSets) const;

    template <typename FuncType>
    void chunkCalculationTemplate(FuncType func,
                                  std::vector<ValueType> const &timepoints,
                                  size_t chunksize) const;

    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;

    bool calculatedTopLevelGate;
    Bdd topLevelGateBdd;
};

}  // namespace modelchecker
}  // namespace storm
