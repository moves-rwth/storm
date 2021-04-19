#include "storm-pomdp/storage/BeliefManager.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/utility/macros.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace storage {

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        uint64_t BeliefManager<PomdpType, BeliefValueType, StateType>::Triangulation::size() const {
            return weights.size();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        BeliefManager<PomdpType, BeliefValueType, StateType>::FreudenthalDiff::FreudenthalDiff(StateType const &dimension, BeliefValueType &&diff) : dimension(dimension),
                                                                                                                                                     diff(std::move(diff)) {
            // Intentionally left empty
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::FreudenthalDiff::operator>(FreudenthalDiff const &other) const {
            if (diff != other.diff) {
                return diff > other.diff;
            } else {
                return dimension < other.dimension;
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::Belief_equal_to::operator()(const BeliefType &lhBelief, const BeliefType &rhBelief) const {
            return lhBelief == rhBelief;
        }

        template<>
        bool BeliefManager<storm::models::sparse::Pomdp<double>, double, uint64_t>::Belief_equal_to::operator()(const BeliefType &lhBelief, const BeliefType &rhBelief) const {
            // If the sizes are different, we don't have to look inside the belief
            if(lhBelief.size() != rhBelief.size()){
                return false;
            }
            // Assumes that beliefs are ordered
            auto lhIt = lhBelief.begin();
            auto rhIt = rhBelief.begin();
            while(lhIt != lhBelief.end() || rhIt != rhBelief.end()){
                // Iterate over the entries simultaneously, beliefs not equal if they contain either different states or different values for the same state
                if(lhIt->first != rhIt->first || std::fabs(lhIt->second - rhIt->second) > 1e-15/*std::numeric_limits<double>::epsilon()*/){
                    return false;
                }
                ++lhIt;
                ++rhIt;
            }
            return lhIt == lhBelief.end() && rhIt == rhBelief.end();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::size_t BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefHash::operator()(const BeliefType &belief) const {
            std::size_t seed = 0;
            // Assumes that beliefs are ordered
            for (auto const &entry : belief) {
                boost::hash_combine(seed, entry.first);
                boost::hash_combine(seed, entry.second);
            }
            return seed;
        }

        template<>
        std::size_t BeliefManager<storm::models::sparse::Pomdp<double>, double, uint64_t>::BeliefHash::operator()(const BeliefType &belief) const {
            std::size_t seed = 0;
            // Assumes that beliefs are ordered
            for (auto const &entry : belief) {
                boost::hash_combine(seed, entry.first);
                boost::hash_combine(seed, round(storm::utility::convertNumber<double>(entry.second) * 1e15));
            }
            return seed;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefManager(PomdpType const &pomdp, BeliefValueType const &precision, TriangulationMode const &triangulationMode)
                : pomdp(pomdp), triangulationMode(triangulationMode) {
            cc = storm::utility::ConstantsComparator<ValueType>(precision, false);
            beliefToIdMap.resize(pomdp.getNrObservations());
            initialBeliefId = computeInitialBelief();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::setRewardModel(boost::optional<std::string> rewardModelName) {
            if (rewardModelName) {
                auto const &rewardModel = pomdp.getRewardModel(rewardModelName.get());
                pomdpActionRewardVector = rewardModel.getTotalRewardVector(pomdp.getTransitionMatrix());
            } else {
                setRewardModel(pomdp.getUniqueRewardModelName());
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::unsetRewardModel() {
            pomdpActionRewardVector.clear();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId BeliefManager<PomdpType, BeliefValueType, StateType>::noId() const {
            return std::numeric_limits<BeliefId>::max();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::isEqual(BeliefId const &first, BeliefId const &second) const {
            return isEqual(getBelief(first), getBelief(second));
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::string BeliefManager<PomdpType, BeliefValueType, StateType>::toString(BeliefId const &beliefId) const {
            return toString(getBelief(beliefId));
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::string BeliefManager<PomdpType, BeliefValueType, StateType>::toString(Triangulation const &t) const {
            std::stringstream str;
            str << "(\n";
            for (uint64_t i = 0; i < t.size(); ++i) {
                str << "\t" << t.weights[i] << " * \t" << toString(getBelief(t.gridPoints[i])) << "\n";
            }
            str << ")\n";
            return str.str();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType
        BeliefManager<PomdpType, BeliefValueType, StateType>::getWeightedSum(BeliefId const &beliefId, std::vector<ValueType> const &summands) {
            ValueType result = storm::utility::zero<ValueType>();
            for (auto const &entry : getBelief(beliefId)) {
                result += storm::utility::convertNumber<ValueType>(entry.second) * storm::utility::convertNumber<ValueType>(summands.at(entry.first));
            }
            return result;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId const &BeliefManager<PomdpType, BeliefValueType, StateType>::getInitialBelief() const {
            return initialBeliefId;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType
        BeliefManager<PomdpType, BeliefValueType, StateType>::getBeliefActionReward(BeliefId const &beliefId, uint64_t const &localActionIndex) const {
            auto const &belief = getBelief(beliefId);
            STORM_LOG_ASSERT(!pomdpActionRewardVector.empty(), "Requested a reward although no reward model was specified.");
            auto result = storm::utility::zero<ValueType>();
            auto const &choiceIndices = pomdp.getTransitionMatrix().getRowGroupIndices();
            for (auto const &entry : belief) {
                uint64_t choiceIndex = choiceIndices[entry.first] + localActionIndex;
                STORM_LOG_ASSERT(choiceIndex < choiceIndices[entry.first + 1], "Invalid local action index.");
                STORM_LOG_ASSERT(choiceIndex < pomdpActionRewardVector.size(), "Invalid choice index.");
                result += entry.second * pomdpActionRewardVector[choiceIndex];
            }
            return result;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        uint32_t BeliefManager<PomdpType, BeliefValueType, StateType>::getBeliefObservation(BeliefId beliefId) {
            return getBeliefObservation(getBelief(beliefId));
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        uint64_t BeliefManager<PomdpType, BeliefValueType, StateType>::getBeliefNumberOfChoices(BeliefId beliefId) {
            auto const &belief = getBelief(beliefId);
            return pomdp.getNumberOfChoices(belief.begin()->first);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::Triangulation
        BeliefManager<PomdpType, BeliefValueType, StateType>::triangulateBelief(BeliefId beliefId, BeliefValueType resolution) {
            return triangulateBelief(getBelief(beliefId), resolution);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        template<typename DistributionType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::addToDistribution(DistributionType &distr, StateType const &state, BeliefValueType const &value) {
            auto insertionRes = distr.emplace(state, value);
            if (!insertionRes.second) {
                insertionRes.first->second += value;
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        template<typename DistributionType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::adjustDistribution(DistributionType &distr) {
            if(distr.size() == 1 && cc.isEqual(distr.begin()->second, storm::utility::one<ValueType>())){
                // If the distribution consists of only one entry and its value is sufficiently close to 1, make it exactly 1 to avoid numerical problems
                distr.begin()->second = storm::utility::one<ValueType>();
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::joinSupport(BeliefId const &beliefId, BeliefSupportType &support) {
            auto const &belief = getBelief(beliefId);
            for (auto const &entry : belief) {
                support.insert(entry.first);
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId BeliefManager<PomdpType, BeliefValueType, StateType>::getNumberOfBeliefIds() const {
            return beliefs.size();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::vector<std::pair<typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId, typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType>>
        BeliefManager<PomdpType, BeliefValueType, StateType>::expandAndTriangulate(BeliefId const &beliefId, uint64_t actionIndex,
                                                                                   std::vector<BeliefValueType> const &observationResolutions) {
            return expandInternal(beliefId, actionIndex, observationResolutions);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::vector<std::pair<typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId, typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType>>
        BeliefManager<PomdpType, BeliefValueType, StateType>::expandAndClip(BeliefId const &beliefId, uint64_t actionIndex,
                                                                            std::vector<uint64_t> const &observationResolutions) {
            return expandInternal(beliefId, actionIndex, boost::none, observationResolutions);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::vector<std::pair<typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId, typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType>>
        BeliefManager<PomdpType, BeliefValueType, StateType>::expand(BeliefId const &beliefId, uint64_t actionIndex) {
            return expandInternal(beliefId, actionIndex);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefType const &BeliefManager<PomdpType, BeliefValueType, StateType>::getBelief(BeliefId const &id) const {
            STORM_LOG_ASSERT(id != noId(), "Tried to get a non-existent belief.");
            STORM_LOG_ASSERT(id < getNumberOfBeliefIds(), "Belief index " << id << " is out of range.");
            return beliefs[id];
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId BeliefManager<PomdpType, BeliefValueType, StateType>::getId(BeliefType const &belief) const {
            uint32_t obs = getBeliefObservation(belief);
            STORM_LOG_ASSERT(obs < beliefToIdMap.size(), "Belief has unknown observation.");
            auto idIt = beliefToIdMap[obs].find(belief);
            STORM_LOG_ASSERT(idIt != beliefToIdMap[obs].end(), "Unknown Belief.");
            return idIt->second;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::string BeliefManager<PomdpType, BeliefValueType, StateType>::toString(BeliefType const &belief) const {
            std::setprecision(std::numeric_limits<double>::max_digits10);
            std::stringstream str;
            str << "{ ";
            bool first = true;
            for (auto const &entry : belief) {
                if (first) {
                    first = false;
                } else {
                    str << ", ";
                }
                str << entry.first << ": " << entry.second;
            }
            str << " }";
            return str.str();
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::isEqual(BeliefType const &first, BeliefType const &second) const {
            if (first.size() != second.size()) {
                return false;
            }
            auto secondIt = second.begin();
            for (auto const &firstEntry : first) {
                if (firstEntry.first != secondIt->first) {
                    return false;
                }
                if (!cc.isEqual(firstEntry.second, secondIt->second)) {
                    return false;
                }
                ++secondIt;
            }
            return true;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::assertBelief(BeliefType const &belief) const {
            BeliefValueType sum = storm::utility::zero<ValueType>();
            boost::optional<uint32_t> observation;
            for (auto const &entry : belief) {
                if (entry.first >= pomdp.getNumberOfStates()) {
                    STORM_LOG_ERROR("Belief does refer to non-existing pomdp state " << entry.first << ".");
                    return false;
                }
                uint64_t entryObservation = pomdp.getObservation(entry.first);
                if (observation) {
                    if (observation.get() != entryObservation) {
                        STORM_LOG_ERROR("Beliefsupport contains different observations.");
                        return false;
                    }
                } else {
                    observation = entryObservation;
                }
                // Don't use cc for these checks, because computations with zero are usually fine
                if (storm::utility::isZero(entry.second)) {
                    // We assume that beliefs only consider their support.
                    STORM_LOG_ERROR("Zero belief probability.");
                    return false;
                }
                if (entry.second < storm::utility::zero<BeliefValueType>()) {
                    STORM_LOG_ERROR("Negative belief probability.");
                    return false;
                }
                if (cc.isLess(storm::utility::one<BeliefValueType>(), entry.second)) {
                    STORM_LOG_ERROR("Belief probability greater than one.");
                    return false;
                }
                sum += entry.second;
            }
            if (!cc.isOne(sum)) {
                STORM_LOG_ERROR("Belief does not sum up to one. (" << sum << " instead).");
                return false;
            }
            return true;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        bool BeliefManager<PomdpType, BeliefValueType, StateType>::assertTriangulation(BeliefType const &belief, Triangulation const &triangulation) const {
            if (triangulation.weights.size() != triangulation.gridPoints.size()) {
                STORM_LOG_ERROR("Number of weights and points in triangulation does not match.");
                return false;
            }
            if (triangulation.size() == 0) {
                STORM_LOG_ERROR("Empty triangulation.");
                return false;
            }
            BeliefType triangulatedBelief;
            BeliefValueType weightSum = storm::utility::zero<BeliefValueType>();
            for (uint64_t i = 0; i < triangulation.weights.size(); ++i) {
                if (cc.isZero(triangulation.weights[i])) {
                    STORM_LOG_ERROR("Zero weight in triangulation.");
                    return false;
                }
                if (cc.isLess(triangulation.weights[i], storm::utility::zero<BeliefValueType>())) {
                    STORM_LOG_ERROR("Negative weight in triangulation.");
                    return false;
                }
                if (cc.isLess(storm::utility::one<BeliefValueType>(), triangulation.weights[i])) {
                    STORM_LOG_ERROR("Weight greater than one in triangulation.");
                }
                weightSum += triangulation.weights[i];
                BeliefType const &gridPoint = getBelief(triangulation.gridPoints[i]);
                for (auto const &pointEntry : gridPoint) {
                    BeliefValueType &triangulatedValue = triangulatedBelief.emplace(pointEntry.first, storm::utility::zero<ValueType>()).first->second;
                    triangulatedValue += triangulation.weights[i] * pointEntry.second;
                }
            }
            if (!cc.isOne(weightSum)) {
                STORM_LOG_ERROR("Triangulation weights do not sum up to one.");
                return false;
            }
            if (!assertBelief(triangulatedBelief)) {
                STORM_LOG_ERROR("Triangulated belief is not a belief.");
            }
            if (!isEqual(belief, triangulatedBelief)) {
                STORM_LOG_ERROR("Belief:\n\t" << toString(belief) << "\ndoes not match triangulated belief:\n\t" << toString(triangulatedBelief) << ".");
                return false;
            }
            return true;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        uint32_t BeliefManager<PomdpType, BeliefValueType, StateType>::getBeliefObservation(BeliefType belief) const {
            STORM_LOG_ASSERT(assertBelief(belief), "Invalid belief.");
            return pomdp.getObservation(belief.begin()->first);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        void
        BeliefManager<PomdpType, BeliefValueType, StateType>::triangulateBeliefFreudenthal(BeliefType const &belief, BeliefValueType const &resolution, Triangulation &result) {
            STORM_LOG_ASSERT(resolution != 0, "Invalid resolution: 0");
            STORM_LOG_ASSERT(storm::utility::isInteger(resolution), "Expected an integer resolution");
            StateType numEntries = belief.size();
            // This is the Freudenthal Triangulation as described in Lovejoy (a whole lotta math)
            // Probabilities will be triangulated to values in 0/N, 1/N, 2/N, ..., N/N
            // Variable names are mostly based on the paper
            // However, we speed this up a little by exploiting that belief states usually have sparse support (i.e. numEntries is much smaller than pomdp.getNumberOfStates()).
            // Initialize diffs and the first row of the 'qs' matrix (aka v)
            std::set<FreudenthalDiff, std::greater<FreudenthalDiff>> sorted_diffs; // d (and p?) in the paper
            std::vector<BeliefValueType> qsRow; // Row of the 'qs' matrix from the paper (initially corresponds to v
            qsRow.reserve(numEntries);
            std::vector<StateType> toOriginalIndicesMap; // Maps 'local' indices to the original pomdp state indices
            toOriginalIndicesMap.reserve(numEntries);
            BeliefValueType x = resolution;
            for (auto const &entry : belief) {
                qsRow.push_back(storm::utility::floor(x)); // v
                sorted_diffs.emplace(toOriginalIndicesMap.size(), x - qsRow.back()); // x-v
                toOriginalIndicesMap.push_back(entry.first);
                x -= entry.second * resolution;
            }
            // Insert a dummy 0 column in the qs matrix so the loops below are a bit simpler
            qsRow.push_back(storm::utility::zero<BeliefValueType>());

            result.weights.reserve(numEntries);
            result.gridPoints.reserve(numEntries);
            auto currentSortedDiff = sorted_diffs.begin();
            auto previousSortedDiff = sorted_diffs.end();
            --previousSortedDiff;
            for (StateType i = 0; i < numEntries; ++i) {
                // Compute the weight for the grid points
                BeliefValueType weight = previousSortedDiff->diff - currentSortedDiff->diff;
                if (i == 0) {
                    // The first weight is a bit different
                    weight += storm::utility::one<ValueType>();
                } else {
                    // 'compute' the next row of the qs matrix
                    qsRow[previousSortedDiff->dimension] += storm::utility::one<BeliefValueType>();
                }
                if (!cc.isZero(weight)) {
                    result.weights.push_back(weight);
                    // Compute the grid point
                    BeliefType gridPoint;
                    for (StateType j = 0; j < numEntries; ++j) {
                        BeliefValueType gridPointEntry = qsRow[j] - qsRow[j + 1];
                        if (!cc.isZero(gridPointEntry)) {
                            gridPoint[toOriginalIndicesMap[j]] = gridPointEntry / resolution;
                        }
                    }
                    result.gridPoints.push_back(getOrAddBeliefId(gridPoint));
                }
                previousSortedDiff = currentSortedDiff++;
            }
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        void BeliefManager<PomdpType, BeliefValueType, StateType>::triangulateBeliefDynamic(BeliefType const &belief, BeliefValueType const &resolution, Triangulation &result) {
            // Find the best resolution for this belief, i.e., N such that the largest distance between one of the belief values to a value in {i/N | 0 ≤ i ≤ N} is minimal
            STORM_LOG_ASSERT(storm::utility::isInteger(resolution), "Expected an integer resolution");
            BeliefValueType finalResolution = resolution;
            uint64_t finalResolutionMisses = belief.size() + 1;
            // We don't need to check resolutions that are smaller than the maximal resolution divided by 2 (as we already checked multiples of these)
            for (BeliefValueType currResolution = resolution; currResolution > resolution / 2; --currResolution) {
                uint64_t currResMisses = 0;
                bool continueWithNextResolution = false;
                for (auto const &belEntry : belief) {
                    BeliefValueType product = belEntry.second * currResolution;
                    if (!cc.isZero(product - storm::utility::round(product))) {
                        ++currResMisses;
                        if (currResMisses >= finalResolutionMisses) {
                            // This resolution is not better than a previous resolution
                            continueWithNextResolution = true;
                            break;
                        }
                    }
                }
                if (!continueWithNextResolution) {
                    STORM_LOG_ASSERT(currResMisses < finalResolutionMisses, "Distance for this resolution should not be larger than a previously checked one.");
                    finalResolution = currResolution;
                    finalResolutionMisses = currResMisses;
                    if (currResMisses == 0) {
                        break;
                    }
                }
            }

            STORM_LOG_TRACE("Picking resolution " << finalResolution << " for belief " << toString(belief));

            // do standard freudenthal with the found resolution
            triangulateBeliefFreudenthal(belief, finalResolution, result);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::Triangulation
        BeliefManager<PomdpType, BeliefValueType, StateType>::triangulateBelief(BeliefType const &belief, BeliefValueType const &resolution) {
            STORM_LOG_ASSERT(assertBelief(belief), "Input belief for triangulation is not valid.");
            Triangulation result;
            // Quickly triangulate Dirac beliefs
            if (belief.size() == 1u) {
                result.weights.push_back(storm::utility::one<BeliefValueType>());
                result.gridPoints.push_back(getOrAddBeliefId(belief));
            } else {
                auto ceiledResolution = storm::utility::ceil<BeliefValueType>(resolution);
                switch (triangulationMode) {
                    case TriangulationMode::Static:
                        triangulateBeliefFreudenthal(belief, ceiledResolution, result);
                        break;
                    case TriangulationMode::Dynamic:
                        triangulateBeliefDynamic(belief, ceiledResolution, result);
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "Invalid triangulation mode.");
                }
            }
            STORM_LOG_ASSERT(assertTriangulation(belief, result), "Incorrect triangulation: " << toString(result));
            return result;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        std::vector<std::pair<typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId, typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType>>
        BeliefManager<PomdpType, BeliefValueType, StateType>::expandInternal(BeliefId const &beliefId, uint64_t actionIndex,
                                                                             boost::optional<std::vector<BeliefValueType>> const &observationTriangulationResolutions,
                                                                             boost::optional<std::vector<uint64_t>> const &observationGridClippingResolutions) {
            std::vector<std::pair<BeliefId, ValueType>> destinations;

            BeliefType belief = getBelief(beliefId);

            // Find the probability we go to each observation
            BeliefType successorObs; // This is actually not a belief but has the same type
            for (auto const &pointEntry : belief) {
                uint64_t state = pointEntry.first;
                for (auto const &pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                    if (!storm::utility::isZero(pomdpTransition.getValue())) {
                        auto obs = pomdp.getObservation(pomdpTransition.getColumn());
                        addToDistribution(successorObs, obs, pointEntry.second * pomdpTransition.getValue());
                    }
                }
            }
            adjustDistribution(successorObs);

            // Now for each successor observation we find and potentially triangulate the successor belief
            for (auto const &successor : successorObs) {
                BeliefType successorBelief;
                for (auto const &pointEntry : belief) {
                    uint64_t state = pointEntry.first;
                    for (auto const &pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                        if (pomdp.getObservation(pomdpTransition.getColumn()) == successor.first) {
                            ValueType prob = pointEntry.second * pomdpTransition.getValue() / successor.second;
                            addToDistribution(successorBelief, pomdpTransition.getColumn(), prob);
                        }
                    }
                }
                adjustDistribution(successorBelief);
                STORM_LOG_ASSERT(assertBelief(successorBelief), "Invalid successor belief.");

                // Insert the destination. We know that destinations have to be disjoint since they have different observations
                if (observationTriangulationResolutions) {
                    Triangulation triangulation = triangulateBelief(successorBelief, observationTriangulationResolutions.get()[successor.first]);
                    for (size_t j = 0; j < triangulation.size(); ++j) {
                        // Here we additionally assume that triangulation.gridPoints does not contain the same point multiple times
                        destinations.emplace_back(triangulation.gridPoints[j], triangulation.weights[j] * successor.second);
                    }
                } else if(observationGridClippingResolutions){
                    BeliefClipping clipping = clipBeliefToGrid(successorBelief, observationGridClippingResolutions.get()[successor.first]);
                    if(clipping.isClippable) {
                        destinations.emplace_back(clipping.targetBelief, (storm::utility::one<ValueType>() - clipping.delta) * successor.second);
                    } else {
                        // Belief on Grid
                        destinations.emplace_back(getOrAddBeliefId(successorBelief), successor.second);
                    }
                } else {
                    destinations.emplace_back(getOrAddBeliefId(successorBelief), successor.second);
                }
            }

            return destinations;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType BeliefManager<PomdpType, BeliefValueType, StateType>::computeDifference1norm(BeliefId const &belief1, BeliefId const &belief2){
            return computeDifference1normInternal(getBelief(belief1), getBelief(belief2));
        }

       template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::ValueType BeliefManager<PomdpType, BeliefValueType, StateType>::computeDifference1normInternal(BeliefType const &belief1, BeliefType const &belief2){
            auto sum = storm::utility::zero<ValueType>();

            auto lhIt = belief1.begin();
            auto rhIt = belief2.begin();
            while(lhIt != belief1.end() || rhIt != belief2.end()){
                // Iterate over the entries simultaneously
                if(lhIt == belief1.end()){
                    sum += rhIt->second;
                    ++rhIt;
                } else if(rhIt == belief2.end()){
                    sum += lhIt->second;
                    ++lhIt;
                } else if(lhIt->first == rhIt->first){
                    // Both iterators are on the same state, take the difference and add it to the sum
                    if(lhIt->second > rhIt->second) {
                        sum += lhIt->second - rhIt->second;
                    } else if(rhIt->second > lhIt->second){
                        sum += rhIt->second - lhIt->second;
                    }
                    ++lhIt;
                    ++rhIt;
                } else if(lhIt->first > rhIt->first){
                    // Iterator 1 skipped a state which belief 2 contains
                    sum += rhIt->second;
                    ++rhIt;
                } else {
                    // Iterator 2 skipped a state which belief 1 contains
                    sum += lhIt->second;
                    ++lhIt;
                }
            }
            return sum;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefClipping
        BeliefManager<PomdpType, BeliefValueType, StateType>::clipBeliefToGrid(BeliefId const &beliefId, uint64_t resolution){
            auto res = clipBeliefToGrid(getBelief(beliefId), resolution);
            res.startingBelief = beliefId;
            return res;
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefClipping
        BeliefManager<PomdpType, BeliefValueType, StateType>::clipBeliefToGrid(BeliefType const &belief, uint64_t resolution){
            uint32_t obs = getBeliefObservation(belief);
            STORM_LOG_ASSERT(obs < beliefToIdMap.size(), "Belief has unknown observation.");
            if(!lpSolver){
                auto lpSolverFactory = storm::utility::solver::LpSolverFactory<ValueType>();
                lpSolver = lpSolverFactory.create("POMDP LP Solver");
            } else {
                lpSolver->pop();
            }
            lpSolver->push();

            std::vector<ValueType> helper(belief.size(), ValueType(0));
            helper[0] = storm::utility::convertNumber<ValueType>(resolution);
            bool done = false;
            std::vector<storm::expressions::Expression> decisionVariables;

            std::vector<BeliefType> gridCandidates;
            while (!done) {
                BeliefType candidate;
                auto belIter = belief.begin();
                for (size_t i = 0; i < belief.size() - 1; ++i) {
                    if(!cc.isEqual(helper[i] - helper[i + 1], storm::utility::zero<ValueType>())) {
                        candidate[belIter->first] = (helper[i] - helper[i + 1]) / storm::utility::convertNumber<ValueType>(resolution);
                    }
                    belIter++;
                }
                if(!cc.isEqual(helper[belief.size() - 1], storm::utility::zero<ValueType>())){
                    candidate[belIter->first] = helper[belief.size() - 1] / storm::utility::convertNumber<ValueType>(resolution);
                }
                if(isEqual(candidate, belief)){
                    // TODO Do something for successors which are already on the grid
                    //STORM_PRINT("Belief on grid" << std::endl)
                    return BeliefClipping{false, noId(), noId(), storm::utility::zero<BeliefValueType>(), {}};
                } else {
                    //STORM_PRINT("Add candidate " << toString(candidate) << std::endl)
                    gridCandidates.push_back(candidate);

                    // Add variables a_j, D_j
                    auto decisionVar = lpSolver->addBinaryVariable("a_" + std::to_string(gridCandidates.size() - 1));
                    decisionVariables.push_back(storm::expressions::Expression(decisionVar));
                    // Add variables for the DELTA values, their overall sum is to be minimized
                    auto bigDelta = lpSolver->addBoundedContinuousVariable("D_" + std::to_string(gridCandidates.size() - 1), storm::utility::zero<ValueType>(), storm::utility::one<ValueType>(),
                                                                           storm::utility::one<ValueType>());
                    std::vector<storm::expressions::Expression> deltas;
                    uint64_t i = 0;
                    for (auto const &state : belief) {
                        auto localDelta = lpSolver->addBoundedContinuousVariable("d_" + std::to_string(i) + "_" + std::to_string(gridCandidates.size() - 1),
                                                                                 storm::utility::zero<ValueType>(),
                                                                                 storm::utility::one<ValueType>());
                        deltas.push_back(storm::expressions::Expression(localDelta));
                        lpSolver->update();
                        // Add the constraint to describe the transformation between the state values in the beliefs
                        // b(s_i) - d_i_j
                        storm::expressions::Expression leftSide = lpSolver->getConstant(state.second) - localDelta;
                        storm::expressions::Expression targetValue = lpSolver->getConstant(candidate[i]);
                        if (candidate.find(state.first) != candidate.end()) {
                            targetValue = lpSolver->getConstant(candidate.at(state.first));
                        } else {
                            targetValue = lpSolver->getConstant(storm::utility::zero<ValueType>());
                        }

                        // b_j(s_i) * (1 - D_j) + (1-a_j) * (b(s_i) - b_j(s_i))
                        storm::expressions::Expression rightSide =
                                targetValue * (lpSolver->getConstant(storm::utility::one<ValueType>()) - storm::expressions::Expression(bigDelta))
                                + (lpSolver->getConstant(storm::utility::one<ValueType>()) - storm::expressions::Expression(decisionVar)) *
                                  (lpSolver->getConstant(state.second) - targetValue);
                        // Add equality
                        lpSolver->addConstraint("state_eq_" + std::to_string(i) + "_" + std::to_string(gridCandidates.size() - 1), leftSide == rightSide);
                        ++i;
                    }
                    // Link decision and D_j
                    lpSolver->addConstraint("dec_" + std::to_string(gridCandidates.size() - 1),
                                            storm::expressions::Expression(bigDelta) <= storm::expressions::Expression(decisionVar));
                    // Link D_j and d_i_j
                    lpSolver->addConstraint("delta_" + std::to_string(gridCandidates.size() - 1), storm::expressions::Expression(bigDelta) == storm::expressions::sum(deltas));
                    // Exclude D_j = 0 (self-loop)
                    lpSolver->addConstraint("not_zero_" + std::to_string(gridCandidates.size() - 1), storm::expressions::Expression(bigDelta) > lpSolver->getConstant(storm::utility::zero<ValueType>()));
                    // Exclude D_j = 1. We do not include this constraint as we minimize and can later check if the optimal value is one
                    // lpSolver->addConstraint("not_one_" + std::to_string(gridCandidates.size()-1), storm::expressions::Expression(bigDelta) < lpSolver->getConstant(storm::utility::one<ValueType>()));
                }
                if (helper.back() == storm::utility::convertNumber<ValueType>(resolution)) {
                    // If the last entry of helper is the gridResolution, we have enumerated all necessary distributions
                    done = true;
                } else {
                    // Update helper by finding the index to increment
                    auto helperIt = helper.end()-1;
                    while (*helperIt == *(helperIt-1)) {
                        --helperIt;
                    }
                    STORM_LOG_ASSERT(helperIt != helper.begin(), "Error in grid clipping - index wrong");
                    // Increment the value at the index
                    *helperIt += 1;
                    // Reset all indices greater than the changed one to 0
                    ++helperIt;
                    while (helperIt != helper.end()) {
                        *helperIt = 0;
                        ++helperIt;
                    }

                }
            }

            // Only one target belief should be chosen
            lpSolver->addConstraint("choice", storm::expressions::sum(decisionVariables) == lpSolver->getConstant(storm::utility::one<ValueType>()));

            lpSolver->optimize();
            // Get the optimal belief for clipping
            BeliefId targetBelief = noId();
            // Not a belief but has the same type
            BeliefType deltaValues;
            auto optDelta = storm::utility::zero<BeliefValueType>();
            if(lpSolver->isOptimal()){
                optDelta = lpSolver->getObjectiveValue();
                if(optDelta == storm::utility::one<ValueType>()){
                    STORM_LOG_WARN("Huh!!!!!" << std::endl);
                    // If we get an optimal value of 1, we cannot clip the belief as by definition this would correspond to a division by 0.
                    return BeliefClipping{false, noId(), noId(), storm::utility::zero<BeliefValueType>(), {}};
                }
                for(uint64_t dist = 0; dist < gridCandidates.size(); ++dist){
                    if(lpSolver->getBinaryValue(lpSolver->getManager().getVariable("a_" + std::to_string(dist)))){
                        targetBelief = getOrAddBeliefId(gridCandidates[dist]);
                        //Collect delta values
                        uint64_t i = 0;
                        for (auto const &state : belief) {
                            auto val = lpSolver->getContinuousValue(lpSolver->getManager().getVariable("d_" + std::to_string(i) + "_" + std::to_string(dist)));
                            if(cc.isLess(storm::utility::zero<ValueType>(),val)){
                                deltaValues.emplace(state.first, val);
                            }
                            ++i;
                        }
                        break;
                    }
                }

                if(cc.isEqual(optDelta, storm::utility::zero<ValueType>())){
                    // If we get an optimal value of 0, the LP solver considers two beliefs to be equal, possibly due to numerical instability
                    // For a sound result, we consider the state to be not be clippable
                    STORM_LOG_WARN("LP solver returned an optimal value of 0. This should definitely not happen when using a grid");
                    STORM_LOG_WARN("Origin" << toString(belief));
                    STORM_LOG_WARN("Target [Bel " << targetBelief << "] " << toString(targetBelief));
                    return BeliefClipping{false, noId(), noId(), storm::utility::zero<BeliefValueType>(), {}};
                }
                //STORM_LOG_ASSERT(cc.isEqual(optDelta, lpSolver->getContinuousValue(lpSolver->getManager().getVariable("D_" + std::to_string(targetBelief)))), "Objective value " << optDelta << " is not equal to the Delta " << lpSolver->getContinuousValue(lpSolver->getManager().getVariable("D_" + std::to_string(targetBelief))) << " for the target state");
            }
            return BeliefClipping{lpSolver->isOptimal(), noId(), targetBelief, optDelta, deltaValues};
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefClipping
        BeliefManager<PomdpType, BeliefValueType, StateType>::clipBelief(BeliefId const &beliefId, ValueType threshold, boost::optional<std::vector<BeliefId>> const &targets){
            //TODO compare performance if a blacklist is used instead of target list (whitelist)
            uint32_t obs = getBeliefObservation(beliefId);
            STORM_LOG_ASSERT(obs < beliefToIdMap.size(), "Belief has unknown observation.");
            if(beliefToIdMap[obs].size() < 2){
                STORM_LOG_DEBUG("Belief " << beliefId << " cannot be clipped - only one belief with observation " << obs);
                return BeliefClipping{false, beliefId, noId(), storm::utility::zero<BeliefValueType>()};
            }
            if(!lpSolver){
                auto lpSolverFactory = storm::utility::solver::LpSolverFactory<ValueType>();
                lpSolver = lpSolverFactory.create("POMDP LP Solver");
            } else {
                lpSolver->pop();
            }
            lpSolver->push();
            uint64_t i = 0;
            // Iterate over all possible candidates TODO optimize this
            std::vector<storm::expressions::Expression> decisionVariables;
            std::vector<BeliefId> consideredCandidates;
            STORM_LOG_DEBUG("Clip Belief with ID " << beliefId << " (" << toString(beliefId) << ")");
            for(auto const &candidate : beliefToIdMap[obs]) {
                if (candidate.second != beliefId) {
                    if(targets){
                        // if a target list is set and the candidate is not on it, skip
                        if(std::find(targets.get().begin(), targets.get().end(), candidate.second) == targets.get().end()){
                            //STORM_LOG_DEBUG("Belief with ID " << candidate.second << " not on target list!!!");
                            continue;
                        }
                    }
                    if (!std::includes(getBelief(beliefId).begin(), getBelief(beliefId).end(), candidate.first.begin(), candidate.first.end(),
                                      [](const std::pair<StateType, BeliefValueType> b1, const std::pair<StateType, BeliefValueType> b2){
                                            return b1.first < b2.first;
                                        })
                    ){
                        // Check if the support of the candidate is a subset of the belief's support
                        STORM_LOG_DEBUG("Belief with ID " << candidate.second << " has unsuitable support for clipping");
                    } else {
                        // Check if reductions to zero would exceed the threshold
                        // First create a map of all 0 values in the candidate which are set in the belief
                        std::map<StateType, BeliefValueType> compHelper;
                        std::set_difference(getBelief(beliefId).begin(), getBelief(beliefId).end(), candidate.first.begin(), candidate.first.end(), std::inserter(compHelper, compHelper.end()),
                                            [](const std::pair<StateType, BeliefValueType> b1, const std::pair<StateType, BeliefValueType> b2) {
                                                return b1.first < b2.first;
                                            });
                        // Check if the sum of all values in the newly created map exceeds the threshold
                        if (std::accumulate(compHelper.begin(), compHelper.end(), storm::utility::zero<ValueType>(),
                                            [](const ValueType acc, const std::pair<StateType, BeliefValueType> belief) { return acc + belief.second; }) > threshold) {
                            STORM_LOG_DEBUG("Belief with ID " << candidate.second << " exceeds clipping threshold");
                        } else {
                            STORM_LOG_DEBUG("Add constraints for Belief with ID " << candidate.second << " " << toString(candidate.second));
                            consideredCandidates.push_back(candidate.second);
                            // Add variables a_j, D_j
                            auto decisionVar = lpSolver->addBinaryVariable("a_" + std::to_string(candidate.second));
                            decisionVariables.push_back(storm::expressions::Expression(decisionVar));
                            // Add variables for the DELTA values, their overall sum is to be minimized
                            auto bigDelta = lpSolver->addBoundedContinuousVariable("D_" + std::to_string(candidate.second), storm::utility::zero<ValueType>(), threshold,
                                                                                   storm::utility::one<ValueType>());
                            std::vector<storm::expressions::Expression> deltas;
                            i = 0;
                            for (auto const &state : getBelief(beliefId)) {
                                auto localDelta = lpSolver->addBoundedContinuousVariable("d_" + std::to_string(i) + "_" + std::to_string(candidate.second),
                                                                                         storm::utility::zero<ValueType>(),
                                                                                         threshold);
                                deltas.push_back(storm::expressions::Expression(localDelta));
                                lpSolver->update();
                                // Add the constraint to describe the transformation between the state values in the beliefs
                                // b(s_i) - d_i_j
                                storm::expressions::Expression leftSide = lpSolver->getConstant(state.second) - localDelta;
                                storm::expressions::Expression targetValue;
                                try {
                                    targetValue = lpSolver->getConstant(candidate.first.at(state.first));
                                } catch (const std::out_of_range &) {
                                    targetValue = lpSolver->getConstant(storm::utility::zero<ValueType>());
                                }
                                // b_j(s_i) * (1 - D_j) + (1-a_j) * (b(s_i) - b_j(s_i))
                                storm::expressions::Expression rightSide =
                                        targetValue * (lpSolver->getConstant(storm::utility::one<ValueType>()) - storm::expressions::Expression(bigDelta))
                                        + (lpSolver->getConstant(storm::utility::one<ValueType>()) - storm::expressions::Expression(decisionVar)) *
                                          (lpSolver->getConstant(state.second) - targetValue);
                                // Add equality
                                lpSolver->addConstraint("state_eq_" + std::to_string(i) + "_" + std::to_string(candidate.second), leftSide == rightSide);
                                ++i;
                            }
                            // Link decision and D_j
                            lpSolver->addConstraint("dec_" + std::to_string(candidate.second),
                                                    storm::expressions::Expression(bigDelta) <= storm::expressions::Expression(decisionVar));
                            // Link D_j and d_i_j
                            lpSolver->addConstraint("delta_" + std::to_string(candidate.second), storm::expressions::Expression(bigDelta) == storm::expressions::sum(deltas));
                            // Exclude D_j = 0. This can only occur due to duplicate beliefs or numerical inaccuracy
                            lpSolver->addConstraint("not_zero_" + std::to_string(candidate.second), storm::expressions::Expression(bigDelta) > lpSolver->getConstant(storm::utility::zero<ValueType>()));
                            // Exclude D_j = 1. We do not include this constraint as we minimize and can later check if the optimal value is one
                            // lpSolver->addConstraint("not_one_" + std::to_string(candidate.second), storm::expressions::Expression(bigDelta) < lpSolver->getConstant(storm::utility::one<ValueType>()));
                        }
                    }
                }
            }
            // If no valid candidate was found, we cannot clip the belief
            if(decisionVariables.empty()){
                STORM_LOG_DEBUG("Belief " << beliefId << " cannot be clipped - no candidate with valid support");
                return BeliefClipping{false, beliefId, noId(), storm::utility::zero<BeliefValueType>(), {}};
            }
            // Only one target belief should be chosen
            lpSolver->addConstraint("choice", storm::expressions::sum(decisionVariables) == lpSolver->getConstant(storm::utility::one<ValueType>()));

            lpSolver->optimize();
            // Get the optimal belief fo clipping
            BeliefId targetBelief = noId();
            auto optDelta = storm::utility::zero<BeliefValueType>();
            // Not a belief but has the same type
            BeliefType deltaValues;
            if(lpSolver->isOptimal()){
                optDelta = lpSolver->getObjectiveValue();
                if(optDelta == storm::utility::one<ValueType>()){
                    // If we get an optimal value of 1, we cannot clip the belief as by definition this would correspond to a division by 0.
                    return BeliefClipping{false, beliefId, noId(), storm::utility::zero<BeliefValueType>(), {}};
                }
                for(auto const &candidate : consideredCandidates) {
                    if(lpSolver->getBinaryValue(lpSolver->getManager().getVariable("a_" + std::to_string(candidate)))){
                        targetBelief = candidate;
                        uint64_t stateIndex = 0;
                        for (auto const &state : getBelief(beliefId)) {
                            auto val = lpSolver->getContinuousValue(lpSolver->getManager().getVariable("d_" + std::to_string(stateIndex) + "_" + std::to_string(candidate)));
                            if(cc.isLess(storm::utility::zero<ValueType>(),val)){
                                deltaValues.emplace(state.first, val);
                            }
                            ++stateIndex;
                        }
                        break;
                    }
                }
                if(optDelta == storm::utility::zero<ValueType>()){
                    // If we get an optimal value of 0, the LP solver considers two beliefs to be equal, possibly due to numerical instability
                    // For a sound result, we consider the state to not be clippable
                    STORM_LOG_WARN("LP solver returned an optimal value of 0. Consider increasing the solver's precision");
                    STORM_LOG_WARN("Origin [Bel " << beliefId << "] " << toString(beliefId));
                    STORM_LOG_WARN("Target [Bel " << targetBelief << "] " << toString(targetBelief));
                    return BeliefClipping{false, beliefId, noId(), storm::utility::zero<BeliefValueType>(), {}};
                }
                STORM_LOG_ASSERT(cc.isEqual(optDelta, lpSolver->getContinuousValue(lpSolver->getManager().getVariable("D_" + std::to_string(targetBelief)))), "Objective value " << optDelta << " is not equal to the Delta " << lpSolver->getContinuousValue(lpSolver->getManager().getVariable("D_" + std::to_string(targetBelief))) << " for the target state");
            }
            return BeliefClipping{lpSolver->isOptimal(), beliefId, targetBelief, optDelta, deltaValues};
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId BeliefManager<PomdpType, BeliefValueType, StateType>::computeInitialBelief() {
            STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() < 2,
                             "POMDP contains more than one initial state");
            STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() == 1,
                             "POMDP does not contain an initial state");
            BeliefType belief;
            belief[*pomdp.getInitialStates().begin()] = storm::utility::one<ValueType>();

            STORM_LOG_ASSERT(assertBelief(belief), "Invalid initial belief.");
            return getOrAddBeliefId(belief);
        }

        template<typename PomdpType, typename BeliefValueType, typename StateType>
        typename BeliefManager<PomdpType, BeliefValueType, StateType>::BeliefId BeliefManager<PomdpType, BeliefValueType, StateType>::getOrAddBeliefId(BeliefType const &belief) {
            uint32_t obs = getBeliefObservation(belief);
            STORM_LOG_ASSERT(obs < beliefToIdMap.size(), "Belief has unknown observation.");
            auto insertioRes = beliefToIdMap[obs].emplace(belief, beliefs.size());
            if (insertioRes.second) {
                // There actually was an insertion, so add the new belief
                STORM_LOG_TRACE("Add Belief " << beliefs.size() << " " << toString(belief));
                beliefs.push_back(belief);
            }
            // Return the id
            return insertioRes.first->second;
        }

        template class BeliefManager<storm::models::sparse::Pomdp<double>>;

        template class BeliefManager<storm::models::sparse::Pomdp<storm::RationalNumber>>;
    }
}