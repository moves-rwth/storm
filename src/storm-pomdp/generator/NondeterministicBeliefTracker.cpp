
#include "storm-pomdp/generator/NondeterministicBeliefTracker.h"
#include "storm/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {

        template<typename ValueType>
        BeliefStateManager<ValueType>::BeliefStateManager(storm::models::sparse::Pomdp<ValueType> const& pomdp)
        : pomdp(pomdp)
        {
            numberActionsPerObservation = std::vector<uint64_t>(pomdp.getNrObservations(), 0);
            for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                numberActionsPerObservation[pomdp.getObservation(state)] = pomdp.getNumberOfChoices(state);
            }
        }

        template<typename ValueType>
        uint64_t BeliefStateManager<ValueType>::getActionsForObservation(uint32_t observation) const {
            return numberActionsPerObservation[observation];
        }

        template<typename ValueType>
        ValueType BeliefStateManager<ValueType>::getRisk(uint64_t state) const {
            return riskPerState.at(state);
        }

        template<typename ValueType>
        storm::models::sparse::Pomdp<ValueType> const& BeliefStateManager<ValueType>::getPomdp() const {
            return pomdp;
        }

        template<typename ValueType>
        void BeliefStateManager<ValueType>::setRiskPerState(std::vector<ValueType> const& risk) {
            riskPerState = risk;
        }

        template<typename ValueType>
        SparseBeliefState<ValueType>::SparseBeliefState(std::shared_ptr<BeliefStateManager<ValueType>> const& manager, uint64_t state)
        : manager(manager), belief()
        {
            belief[state] = storm::utility::one<ValueType>();
            risk = manager->getRisk(state);
        }

        template<typename ValueType>
        SparseBeliefState<ValueType>::SparseBeliefState(std::shared_ptr<BeliefStateManager<ValueType>> const& manager, std::map<uint64_t, ValueType> const& belief,
                                                        std::size_t hash, ValueType const& risk)
        : manager(manager), belief(belief), prestoredhash(hash), risk(risk)
        {
            // Intentionally left empty
        }

        template<typename ValueType>
        ValueType SparseBeliefState<ValueType>::get(uint64_t state) const {
            return belief.at(state);
        }

        template<typename ValueType>
        ValueType SparseBeliefState<ValueType>::getRisk() const {
            return risk;
        }

        template<typename ValueType>
        std::size_t SparseBeliefState<ValueType>::hash() const noexcept {
            return prestoredhash;
        }

        template<typename ValueType>
        bool SparseBeliefState<ValueType>::isValid() const {
            return !belief.empty();
        }

        template<typename ValueType>
        std::string SparseBeliefState<ValueType>::toString() const {
            std::stringstream sstr;
            bool first = true;
            for (auto const& beliefentry : belief) {
                if (!first) {
                    sstr << ", ";
                } else {
                    first = false;
                }
                sstr << beliefentry.first << " : " << beliefentry.second;
            }
            return sstr.str();
        }

        template<typename ValueType>
        bool operator==(SparseBeliefState<ValueType> const& lhs, SparseBeliefState<ValueType> const& rhs) {
            if (lhs.hash() != rhs.hash()) {
                return false;
            }
            if (lhs.belief.size() != rhs.belief.size()) {
                return false;
            }
            storm::utility::ConstantsComparator<ValueType> cmp(0.00001, true);
            auto lhsIt = lhs.belief.begin();
            auto rhsIt = rhs.belief.begin();
            while(lhsIt != lhs.belief.end()) {
                if (lhsIt->first != rhsIt->first || !cmp.isEqual(lhsIt->second, rhsIt->second)) {
                    return false;
                }
                ++lhsIt;
                ++rhsIt;
            }
            return true;
            //return std::equal(lhs.belief.begin(), lhs.belief.end(), rhs.belief.begin());
        }

        template<typename ValueType>
        SparseBeliefState<ValueType> SparseBeliefState<ValueType>::update(uint64_t action, uint32_t observation) const {
            std::map<uint64_t, ValueType> newBelief;
            ValueType sum = storm::utility::zero<ValueType>();
            for (auto const& beliefentry : belief) {
                assert(manager->getPomdp().getNumberOfChoices(beliefentry.first) > action);
                auto row = manager->getPomdp().getNondeterministicChoiceIndices()[beliefentry.first] + action;
                for (auto const& transition : manager->getPomdp().getTransitionMatrix().getRow(row)) {
                    if (observation != manager->getPomdp().getObservation(transition.getColumn())) {
                        continue;
                    }

                    if (newBelief.count(transition.getColumn()) == 0) {
                        newBelief[transition.getColumn()] = transition.getValue() * beliefentry.second;
                    } else {
                        newBelief[transition.getColumn()] += transition.getValue() * beliefentry.second;
                    }
                    sum += transition.getValue() * beliefentry.second;
                }
            }
            std::size_t newHash = 0;
            ValueType risk = storm::utility::zero<ValueType>();
            for(auto& entry : newBelief) {
                assert(!storm::utility::isZero(sum));
                entry.second /= sum;
                //boost::hash_combine(newHash, std::hash<ValueType>()(entry.second));
                boost::hash_combine(newHash, entry.first);
                risk += entry.second * manager->getRisk(entry.first);
            }
            return SparseBeliefState<ValueType>(manager, newBelief, newHash, risk);
        }


        template<typename ValueType, typename BeliefState>
        NondeterministicBeliefTracker<ValueType, BeliefState>::NondeterministicBeliefTracker(storm::models::sparse::Pomdp<ValueType> const& pomdp) :
        pomdp(pomdp), manager(std::make_shared<BeliefStateManager<ValueType>>(pomdp)), beliefs() {
            //
        }

        template<typename ValueType, typename BeliefState>
        bool NondeterministicBeliefTracker<ValueType, BeliefState>::reset(uint32_t observation) {
            bool hit = false;
            for (auto state : pomdp.getInitialStates()) {
                if (observation == pomdp.getObservation(state)) {
                    hit = true;
                    beliefs.emplace(manager, state);
                }
            }
            lastObservation = observation;
            return hit;
        }

        template<typename ValueType, typename BeliefState>
        bool NondeterministicBeliefTracker<ValueType, BeliefState>::track(uint64_t newObservation) {
            STORM_LOG_THROW(!beliefs.empty(), storm::exceptions::InvalidOperationException, "Cannot track without a belief (need to reset).");
            std::unordered_set<BeliefState> newBeliefs;
            for (uint64_t action = 0; action < manager->getActionsForObservation(lastObservation); ++action) {
                for (auto const& belief : beliefs) {
                    auto newBelief = belief.update(action, newObservation);
                    if (newBelief.isValid()) {
                        newBeliefs.insert(newBelief);
                    }
                }
            }
            beliefs = newBeliefs;
            lastObservation = newObservation;
            return !beliefs.empty();
        }

        template<typename ValueType, typename BeliefState>
        ValueType NondeterministicBeliefTracker<ValueType, BeliefState>::getCurrentRisk(bool max) {
            STORM_LOG_THROW(!beliefs.empty(), storm::exceptions::InvalidOperationException, "Risk is only defined for beliefs (run reset() first).");
            ValueType result = beliefs.begin()->getRisk();
            if (max) {
                for (auto const& belief : beliefs) {
                    if (belief.getRisk() > result) {
                        result = belief.getRisk();
                    }
                }
            } else {
                for (auto const& belief : beliefs) {
                    if (belief.getRisk() < result) {
                        result = belief.getRisk();
                    }
                }
            }
            return result;
        }

        template<typename ValueType, typename BeliefState>
        void NondeterministicBeliefTracker<ValueType, BeliefState>::setRisk(std::vector<ValueType> const& risk) {
            manager->setRiskPerState(risk);
        }

        template<typename ValueType, typename BeliefState>
        std::unordered_set<BeliefState> const& NondeterministicBeliefTracker<ValueType, BeliefState>::getCurrentBeliefs() const {
            return beliefs;
        }

        template<typename ValueType, typename BeliefState>
        uint32_t NondeterministicBeliefTracker<ValueType, BeliefState>::getCurrentObservation() const {
            return lastObservation;
        }

        template class SparseBeliefState<double>;
        template bool operator==(SparseBeliefState<double> const&, SparseBeliefState<double> const&);
        template class NondeterministicBeliefTracker<double, SparseBeliefState<double>>;

    }
}
