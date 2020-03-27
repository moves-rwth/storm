#pragma once

#include <map>
#include <boost/optional.hpp>
//#include <boost/container/flat_map.hpp>

#include "storm/utility/macros.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace storage {
        
        template <typename PomdpType, typename BeliefValueType, typename StateType = uint64_t>
        class BeliefGrid {
        public:
            
            typedef typename PomdpType::ValueType ValueType;
            //typedef boost::container::flat_map<StateType, BeliefValueType> BeliefType
            typedef std::map<StateType, BeliefValueType> BeliefType;
            typedef uint64_t BeliefId;
            
            BeliefGrid(PomdpType const& pomdp, BeliefValueType const& precision) : pomdp(pomdp), cc(precision, false) {
                // Intentionally left empty
            }
            
            struct Triangulation {
                std::vector<BeliefId> gridPoints;
                std::vector<BeliefValueType> weights;
                uint64_t size() const {
                    return weights.size();
                }
            };
            
            BeliefType const& getGridPoint(BeliefId const& id) const {
                return gridPoints[id];
            }
            
            BeliefId getIdOfGridPoint(BeliefType const& gridPoint) const {
                auto idIt = gridPointToIdMap.find(gridPoint);
                STORM_LOG_THROW(idIt != gridPointToIdMap.end(), storm::exceptions::UnexpectedException, "Unknown grid state.");
                return idIt->second;
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
                    uintmax_t entryObservation = pomdp.getObservation(entry.first);
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
                    BeliefType const& gridPoint = getGridPoint(triangulation.gridPoints[i]);
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
                    STORM_LOG_ERROR("Belief does not match triangulated belief.");
                    return false;
                }
                return true;
            }
            
            Triangulation triangulateBelief(BeliefType belief, uint64_t resolution) {
                //TODO this can also be simplified using the sparse vector interpretation
                //TODO Enable chaching for this method?
                STORM_LOG_ASSERT(assertBelief(belief), "Input belief for triangulation is not valid.");
                
                auto nrStates = pomdp.getNumberOfStates();
                
                // This is the Freudenthal Triangulation as described in Lovejoy (a whole lotta math)
                // Variable names are based on the paper
                // TODO avoid reallocations for these vectors
                std::vector<BeliefValueType> x(nrStates);
                std::vector<BeliefValueType> v(nrStates);
                std::vector<BeliefValueType> d(nrStates);
                auto convResolution = storm::utility::convertNumber<BeliefValueType>(resolution);

                for (size_t i = 0; i < nrStates; ++i) {
                    for (auto const &probEntry : belief) {
                        if (probEntry.first >= i) {
                            x[i] += convResolution * probEntry.second;
                        }
                    }
                    v[i] = storm::utility::floor(x[i]);
                    d[i] = x[i] - v[i];
                }

                auto p = storm::utility::vector::getSortedIndices(d);

                std::vector<std::vector<BeliefValueType>> qs(nrStates, std::vector<BeliefValueType>(nrStates));
                for (size_t i = 0; i < nrStates; ++i) {
                    if (i == 0) {
                        for (size_t j = 0; j < nrStates; ++j) {
                            qs[i][j] = v[j];
                        }
                    } else {
                        for (size_t j = 0; j < nrStates; ++j) {
                            if (j == p[i - 1]) {
                                qs[i][j] = qs[i - 1][j] + storm::utility::one<BeliefValueType>();
                            } else {
                                qs[i][j] = qs[i - 1][j];
                            }
                        }
                    }
                }
                
                Triangulation result;
                // The first weight is 1-sum(other weights). We therefore process the js in reverse order
                BeliefValueType firstWeight = storm::utility::one<BeliefValueType>();
                for (size_t j = nrStates; j > 0;) {
                    --j;
                    // First create the weights. The weights vector will be reversed at the end.
                    ValueType weight;
                    if (j == 0) {
                        weight = firstWeight;
                    } else {
                        weight = d[p[j - 1]] - d[p[j]];
                        firstWeight -= weight;
                    }
                    if (!cc.isZero(weight)) {
                        result.weights.push_back(weight);
                        BeliefType gridPoint;
                        auto const& qsj = qs[j];
                        for (size_t i = 0; i < nrStates - 1; ++i) {
                            BeliefValueType gridPointEntry = qsj[i] - qsj[i + 1];
                            if (!cc.isZero(gridPointEntry)) {
                                gridPoint[i] = gridPointEntry / convResolution;
                            }
                        }
                        if (!cc.isZero(qsj[nrStates - 1])) {
                            gridPoint[nrStates - 1] = qsj[nrStates - 1] / convResolution;
                        }
                        result.gridPoints.push_back(getOrAddGridPointId(gridPoint));
                    }
                }
                std::reverse(result.weights.begin(), result.weights.end());
                
                STORM_LOG_ASSERT(assertTriangulation(belief, result), "Incorrect triangulation.");
                
                return result;
            }
            
            template<typename DistributionType>
            void addToDistribution(DistributionType& distr, StateType const& state, BeliefValueType const& value) {
                auto insertionRes = distr.emplace(state, value);
                if (!insertionRes.second) {
                    insertionRes.first->second += value;
                }
            }
            
            BeliefId getNumberOfGridPointIds() const {
                return gridPoints.size();
            }
            
            std::map<BeliefId, ValueType> expandAction(BeliefId const& gridPointId, uint64_t actionIndex, std::vector<uint64_t> const& observationResolutions) {
                
                std::map<BeliefId, ValueType> destinations; // The belief ids should be ordered
                
                BeliefType gridPoint = getGridPoint(gridPointId);
                
                // Find the probability we go to each observation
                BeliefType successorObs; // This is actually not a belief but has the same type
                for (auto const& pointEntry : gridPoint) {
                    uint64_t state = pointEntry.first;
                    for (auto const& pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                        if (!storm::utility::isZero(pomdpTransition.getValue())) {
                            auto obs = pomdp.getObservation(pomdpTransition.getColumn());
                            addToDistribution(successorObs, obs, pointEntry.second * pomdpTransition.getValue());
                        }
                    }
                }
                
                // Now for each successor observation we find and triangulate the successor belief
                for (auto const& successor : successorObs) {
                    BeliefType successorBelief;
                    for (auto const& pointEntry : gridPoint) {
                        uint64_t state = pointEntry.first;
                        for (auto const& pomdpTransition : pomdp.getTransitionMatrix().getRow(state, actionIndex)) {
                            if (pomdp.getObservation(pomdpTransition.getColumn()) == successor.first) {
                                ValueType prob = pointEntry.second * pomdpTransition.getValue() / successor.second;
                                addToDistribution(successorBelief, pomdpTransition.getColumn(), prob);
                            }
                        }
                    }
                    STORM_LOG_ASSERT(assertBelief(successorBelief), "Invalid successor belief.");
                    
                    Triangulation triangulation = triangulateBelief(successorBelief, observationResolutions[successor.first]);
                    for (size_t j = 0; j < triangulation.size(); ++j) {
                        addToDistribution(destinations, triangulation.gridPoints[j], triangulation.weights[j] * successor.second);
                    }
                }
    
                return destinations;
                
            }
            
        private:
            
            BeliefId getOrAddGridPointId(BeliefType const& gridPoint) {
                auto insertioRes = gridPointToIdMap.emplace(gridPoint, gridPoints.size());
                if (insertioRes.second) {
                    // There actually was an insertion, so add the new grid state
                    gridPoints.push_back(gridPoint);
                }
                // Return the id
                return insertioRes.first->second;
            }
            
            PomdpType const& pomdp;
            uint64_t resolution;
            
            std::vector<BeliefType> gridPoints;
            std::map<BeliefType, BeliefId> gridPointToIdMap;
            storm::utility::ConstantsComparator<ValueType> cc;
            
            
        };
    }
}