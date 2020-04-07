#pragma once

#include <map>
#include <boost/optional.hpp>
//#include <boost/container/flat_map.hpp>

#include "storm/utility/macros.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace storage {
        
        template <typename PomdpType, typename BeliefValueType = typename PomdpType::ValueType, typename StateType = uint64_t>
        class BeliefManager {
        public:
            
            typedef typename PomdpType::ValueType ValueType;
            //typedef boost::container::flat_map<StateType, BeliefValueType> BeliefType
            typedef std::map<StateType, BeliefValueType> BeliefType;
            typedef uint64_t BeliefId;
            
            BeliefManager(PomdpType const& pomdp, BeliefValueType const& precision) : pomdp(pomdp), cc(precision, false) {
                initialBeliefId = computeInitialBelief();
            }
            
            void setRewardModel(boost::optional<std::string> rewardModelName = boost::none) {
                if (rewardModelName) {
                    auto const& rewardModel = pomdp.getRewardModel(rewardModelName.get());
                    pomdpActionRewardVector = rewardModel.getTotalRewardVector(pomdp.getTransitionMatrix());
                } else {
                    setRewardModel(pomdp.getUniqueRewardModelName());
                }
            }
            
            void unsetRewardModel() {
                pomdpActionRewardVector.clear();
            }
            
            struct Triangulation {
                std::vector<BeliefId> gridPoints;
                std::vector<BeliefValueType> weights;
                uint64_t size() const {
                    return weights.size();
                }
            };
            
            BeliefId noId() const {
                return std::numeric_limits<BeliefId>::max();
            }
            
            bool isEqual(BeliefId const& first, BeliefId const& second) const {
                return isEqual(getBelief(first), getBelief(second));
            }

            std::string toString(BeliefId const& beliefId) const {
                return toString(getBelief(beliefId));
            }

            
            std::string toString(Triangulation const& t) const {
                std::stringstream str;
                str << "(\n";
                for (uint64_t i = 0; i < t.size(); ++i) {
                    str << "\t" << t.weights[i] << " * \t" << toString(getBelief(t.gridPoints[i])) << "\n";
                }
                str <<")\n";
                return str.str();
            }
            
            template <typename SummandsType>
            ValueType getWeightedSum(BeliefId const& beliefId, SummandsType const& summands) {
                ValueType result = storm::utility::zero<ValueType>();
                for (auto const& entry : getBelief(beliefId)) {
                    result += storm::utility::convertNumber<ValueType>(entry.second) * storm::utility::convertNumber<ValueType>(summands.at(entry.first));
                }
                return result;
            }
            
            
            BeliefId const& getInitialBelief() const {
                return initialBeliefId;
            }
            
            ValueType getBeliefActionReward(BeliefId const& beliefId, uint64_t const& localActionIndex) const {
                auto const& belief = getBelief(beliefId);
                STORM_LOG_ASSERT(!pomdpActionRewardVector.empty(), "Requested a reward although no reward model was specified.");
                auto result = storm::utility::zero<ValueType>();
                auto const& choiceIndices = pomdp.getTransitionMatrix().getRowGroupIndices();
                for (auto const &entry : belief) {
                    uint64_t choiceIndex = choiceIndices[entry.first] + localActionIndex;
                    STORM_LOG_ASSERT(choiceIndex < choiceIndices[entry.first + 1], "Invalid local action index.");
                    STORM_LOG_ASSERT(choiceIndex < pomdpActionRewardVector.size(), "Invalid choice index.");
                    result += entry.second * pomdpActionRewardVector[choiceIndex];
                }
                return result;
            }
            
            uint32_t getBeliefObservation(BeliefId beliefId) {
                return getBeliefObservation(getBelief(beliefId));
            }
            
            uint64_t getBeliefNumberOfChoices(BeliefId beliefId) {
                auto belief = getBelief(beliefId);
                return pomdp.getNumberOfChoices(belief.begin()->first);
            }
            
            Triangulation triangulateBelief(BeliefId beliefId, uint64_t resolution) {
                return triangulateBelief(getBelief(beliefId), resolution);
            }
            
            template<typename DistributionType>
            void addToDistribution(DistributionType& distr, StateType const& state, BeliefValueType const& value) {
                auto insertionRes = distr.emplace(state, value);
                if (!insertionRes.second) {
                    insertionRes.first->second += value;
                }
            }
            
            BeliefId getNumberOfBeliefIds() const {
                return beliefs.size();
            }
            
            std::vector<std::pair<BeliefId, ValueType>> expandAndTriangulate(BeliefId const& beliefId, uint64_t actionIndex, std::vector<uint64_t> const& observationResolutions) {
                return expandInternal(beliefId, actionIndex, observationResolutions);
            }
            
            std::vector<std::pair<BeliefId, ValueType>> expand(BeliefId const& beliefId, uint64_t actionIndex) {
                return expandInternal(beliefId, actionIndex);
            }
            
        private:
            
            BeliefType const& getBelief(BeliefId const& id) const {
                STORM_LOG_ASSERT(id != noId(), "Tried to get a non-existend belief.");
                STORM_LOG_ASSERT(id < getNumberOfBeliefIds(), "Belief index " << id << " is out of range.");
                return beliefs[id];
            }
            
            BeliefId getId(BeliefType const& belief) const {
                auto idIt = beliefToIdMap.find(belief);
                STORM_LOG_THROW(idIt != beliefToIdMap.end(), storm::exceptions::UnexpectedException, "Unknown Belief.");
                return idIt->second;
            }
            
            std::string toString(BeliefType const& belief) const {
                std::stringstream str;
                str << "{ ";
                bool first = true;
                for (auto const& entry : belief) {
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
            
            bool isEqual(BeliefType const& first, BeliefType const& second) const {
                if (first.size() != second.size()) {
                    return false;
                }
                auto secondIt = second.begin();
                for (auto const& firstEntry : first) {
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
            
            bool assertBelief(BeliefType const& belief) const {
                BeliefValueType sum = storm::utility::zero<ValueType>();
                boost::optional<uint32_t> observation;
                for (auto const& entry : belief) {
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
                    if (cc.isZero(entry.second)) {
                        // We assume that beliefs only consider their support.
                        STORM_LOG_ERROR("Zero belief probability.");
                        return false;
                    }
                    if (cc.isLess(entry.second, storm::utility::zero<BeliefValueType>())) {
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
                    STORM_LOG_ERROR("Belief does not sum up to one.");
                    return false;
                }
                return true;
            }
            
            bool assertTriangulation(BeliefType const& belief, Triangulation const& triangulation) const {
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
                    BeliefType const& gridPoint = getBelief(triangulation.gridPoints[i]);
                    for (auto const& pointEntry : gridPoint) {
                        BeliefValueType& triangulatedValue = triangulatedBelief.emplace(pointEntry.first, storm::utility::zero<ValueType>()).first->second;
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
            
            uint32_t getBeliefObservation(BeliefType belief) {
                STORM_LOG_ASSERT(assertBelief(belief), "Invalid belief.");
                return pomdp.getObservation(belief.begin()->first);
            }
            
            struct FreudenthalData {
                FreudenthalData(StateType const& pomdpState, StateType const& dimension, BeliefValueType const& x) : pomdpState(pomdpState), dimension(dimension), value(storm::utility::floor(x)), diff(x-value) { };
                StateType pomdpState;
                StateType dimension; // i
                BeliefValueType value; // v[i] in the Lovejoy paper
                BeliefValueType diff; // d[i] in the Lovejoy paper
            };
            struct FreudenthalDataComparator {
                bool operator()(FreudenthalData const& first, FreudenthalData const& second) const {
                    if (first.diff != second.diff) {
                        return first.diff > second.diff;
                    } else {
                        return first.dimension < second.dimension;
                    }
                }
            };
            
            Triangulation triangulateBelief(BeliefType belief, uint64_t resolution) {
                //TODO Enable chaching for this method?
                STORM_LOG_ASSERT(assertBelief(belief), "Input belief for triangulation is not valid.");
                
                auto convResolution = storm::utility::convertNumber<BeliefValueType>(resolution);
                
                // This is the Freudenthal Triangulation as described in Lovejoy (a whole lotta math)
                // Variable names are based on the paper
                // However, we speed this up a little by exploiting that belief states usually have sparse support.
                // TODO: for the sorting, it probably suffices to have a map from diffs to dimensions. The other Freudenthaldata could then also be stored in vectors, which would be a bit more like the original algorithm
                
                // Initialize some data
                std::vector<typename std::set<FreudenthalData, FreudenthalDataComparator>::iterator> dataIterators;
                dataIterators.reserve(belief.size());
                // Initialize first row of 'qs' matrix
                std::vector<BeliefValueType> qsRow;
                qsRow.reserve(dataIterators.size());
                std::set<FreudenthalData, FreudenthalDataComparator> freudenthalData;
                BeliefValueType x = convResolution;
                for (auto const& entry : belief) {
                    auto insertionIt = freudenthalData.emplace(entry.first, dataIterators.size(), x).first;
                    dataIterators.push_back(insertionIt);
                    qsRow.push_back(dataIterators.back()->value);
                    x -= entry.second * convResolution;
                }
                qsRow.push_back(storm::utility::zero<BeliefValueType>());
                assert(!freudenthalData.empty());
                
                Triangulation result;
                result.weights.reserve(freudenthalData.size());
                result.gridPoints.reserve(freudenthalData.size());
                
                // Insert first grid point
                // TODO: this special treatment is actually not necessary.
                BeliefValueType firstWeight = storm::utility::one<ValueType>() - freudenthalData.begin()->diff + freudenthalData.rbegin()->diff;
                if (!cc.isZero(firstWeight)) {
                    result.weights.push_back(firstWeight);
                    BeliefType gridPoint;
                    for (StateType j = 0; j < dataIterators.size(); ++j) {
                        BeliefValueType gridPointEntry = qsRow[j] - qsRow[j + 1];
                        if (!cc.isZero(gridPointEntry)) {
                            gridPoint[dataIterators[j]->pomdpState] = gridPointEntry / convResolution;
                        }
                    }
                    result.gridPoints.push_back(getOrAddBeliefId(gridPoint));
                }
                
                if (freudenthalData.size() > 1) {
                    // Insert remaining grid points
                    auto currentSortedEntry = freudenthalData.begin();
                    auto previousSortedEntry = currentSortedEntry++;
                    for (StateType i = 1; i < dataIterators.size(); ++i) {
                        // 'compute' the next row of the qs matrix
                        qsRow[previousSortedEntry->dimension] += storm::utility::one<BeliefValueType>();
                        
                        BeliefValueType weight = previousSortedEntry->diff - currentSortedEntry->diff;
                        if (!cc.isZero(weight)) {
                            result.weights.push_back(weight);
                            
                            BeliefType gridPoint;
                            for (StateType j = 0; j < dataIterators.size(); ++j) {
                                BeliefValueType gridPointEntry = qsRow[j] - qsRow[j + 1];
                                if (!cc.isZero(gridPointEntry)) {
                                    gridPoint[dataIterators[j]->pomdpState] = gridPointEntry / convResolution;
                                }
                            }
                            result.gridPoints.push_back(getOrAddBeliefId(gridPoint));
                        }
                        ++previousSortedEntry;
                        ++currentSortedEntry;
                    }
                }
                
                STORM_LOG_ASSERT(assertTriangulation(belief, result), "Incorrect triangulation: " << toString(result));
                return result;
            }
            
            std::vector<std::pair<BeliefId, ValueType>> expandInternal(BeliefId const& beliefId, uint64_t actionIndex, boost::optional<std::vector<uint64_t>> const& observationTriangulationResolutions = boost::none) {
                std::vector<std::pair<BeliefId, ValueType>> destinations;
                // TODO: Output as vector?
                
                BeliefType belief = getBelief(beliefId);
                
                // Find the probability we go to each observation
                BeliefType successorObs; // This is actually not a belief but has the same type
                for (auto const& pointEntry : belief) {
                    uint64_t state = pointEntry.first;
                    for (auto const& pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                        if (!storm::utility::isZero(pomdpTransition.getValue())) {
                            auto obs = pomdp.getObservation(pomdpTransition.getColumn());
                            addToDistribution(successorObs, obs, pointEntry.second * pomdpTransition.getValue());
                        }
                    }
                }
                
                // Now for each successor observation we find and potentially triangulate the successor belief
                for (auto const& successor : successorObs) {
                    BeliefType successorBelief;
                    for (auto const& pointEntry : belief) {
                        uint64_t state = pointEntry.first;
                        for (auto const& pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                            if (pomdp.getObservation(pomdpTransition.getColumn()) == successor.first) {
                                ValueType prob = pointEntry.second * pomdpTransition.getValue() / successor.second;
                                addToDistribution(successorBelief, pomdpTransition.getColumn(), prob);
                            }
                        }
                    }
                    STORM_LOG_ASSERT(assertBelief(successorBelief), "Invalid successor belief.");
                    
                    // Insert the destination. We know that destinations have to be disjoined since they have different observations
                    if (observationTriangulationResolutions) {
                        Triangulation triangulation = triangulateBelief(successorBelief, observationTriangulationResolutions.get()[successor.first]);
                        for (size_t j = 0; j < triangulation.size(); ++j) {
                            // Here we additionally assume that triangulation.gridPoints does not contain the same point multiple times
                            destinations.emplace_back(triangulation.gridPoints[j], triangulation.weights[j] * successor.second);
                        }
                    } else {
                        destinations.emplace_back(getOrAddBeliefId(successorBelief), successor.second);
                    }
                }
    
                return destinations;
                
            }
            
            BeliefId computeInitialBelief() {
                STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() < 2,
                                 "POMDP contains more than one initial state");
                STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() == 1,
                                 "POMDP does not contain an initial state");
                BeliefType belief;
                belief[*pomdp.getInitialStates().begin()] = storm::utility::one<ValueType>();
                
                STORM_LOG_ASSERT(assertBelief(belief), "Invalid initial belief.");
                return getOrAddBeliefId(belief);
            }
            
            BeliefId getOrAddBeliefId(BeliefType const& belief) {
                auto insertioRes = beliefToIdMap.emplace(belief, beliefs.size());
                if (insertioRes.second) {
                    // There actually was an insertion, so add the new belief
                    beliefs.push_back(belief);
                }
                // Return the id
                return insertioRes.first->second;
            }
            
            PomdpType const& pomdp;
            std::vector<ValueType> pomdpActionRewardVector;
            
            std::vector<BeliefType> beliefs;
            std::map<BeliefType, BeliefId> beliefToIdMap;
            BeliefId initialBeliefId;
            
            storm::utility::ConstantsComparator<ValueType> cc;
            
            
        };
    }
}