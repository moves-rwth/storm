#pragma once
#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace generator {
        template<typename ValueType>
        class BeliefStateManager {
        public:
            BeliefStateManager(storm::models::sparse::Pomdp<ValueType> const& pomdp);
            storm::models::sparse::Pomdp<ValueType> const& getPomdp() const;
            uint64_t getActionsForObservation(uint32_t observation) const;
            ValueType getRisk(uint64_t) const;
            void setRiskPerState(std::vector<ValueType> const& risk);
            uint64_t getFreshId();
        private:
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            std::vector<ValueType> riskPerState;
            std::vector<uint64_t> numberActionsPerObservation;
            uint64_t beliefIdCounter = 0;
        };

        template<typename ValueType>
        class SparseBeliefState;
        template<typename ValueType>
        bool operator==(SparseBeliefState<ValueType> const& lhs, SparseBeliefState<ValueType> const& rhs);
        template<typename ValueType>
        class SparseBeliefState {
        public:
            SparseBeliefState(std::shared_ptr<BeliefStateManager<ValueType>> const& manager, uint64_t state);
            SparseBeliefState update(uint64_t action, uint32_t  observation) const;
            void update(uint32_t newObservation, std::unordered_set<SparseBeliefState>& previousBeliefs) const;
            std::size_t hash() const noexcept;
            ValueType get(uint64_t state) const;
            ValueType getRisk() const;
            std::string toString() const;
            bool isValid() const;

            friend bool operator==<>(SparseBeliefState<ValueType> const& lhs, SparseBeliefState<ValueType> const& rhs);
        private:
            void updateHelper(std::vector<std::map<uint64_t, ValueType>> const& partialBeliefs, std::vector<ValueType> const& sums, typename std::map<uint64_t, ValueType>::const_iterator nextStateIt, uint32_t newObservation, std::unordered_set<SparseBeliefState<ValueType>>& previousBeliefs) const;
            SparseBeliefState(std::shared_ptr<BeliefStateManager<ValueType>> const& manager, std::map<uint64_t, ValueType> const& belief, std::size_t newHash,  ValueType const& risk, uint64_t prevId);
            std::shared_ptr<BeliefStateManager<ValueType>> manager;

            std::map<uint64_t, ValueType> belief; // map is ordered for unique hashing.
            std::size_t prestoredhash = 0;
            ValueType risk;
            uint64_t id;
            uint64_t prevId;

        };


        template<typename ValueType>
        class ObservationDenseBeliefState {
        public:
            ObservationDenseBeliefState(std::shared_ptr<BeliefStateManager<ValueType>> const& manager, uint64_t state);
            ObservationDenseBeliefState update(uint64_t action, uint32_t observation) const;
        private:
            std::shared_ptr<BeliefStateManager<ValueType>> manager;
            std::unordered_map<uint64_t, ValueType> belief;

            void normalize();
        };

        template<typename ValueType, typename BeliefState>
        class NondeterministicBeliefTracker {
        public:
            NondeterministicBeliefTracker(storm::models::sparse::Pomdp<ValueType> const& pomdp);
            bool reset(uint32_t observation);
            bool track(uint64_t newObservation);
            std::unordered_set<BeliefState> const& getCurrentBeliefs() const;
            uint32_t getCurrentObservation() const;
            ValueType getCurrentRisk(bool max=true);
            void setRisk(std::vector<ValueType> const& risk);

        private:

            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            std::shared_ptr<BeliefStateManager<ValueType>> manager;
            std::unordered_set<BeliefState> beliefs;
            uint32_t lastObservation;
        };
    }
}

//
namespace std {
    template<typename T>
    struct hash<storm::generator::SparseBeliefState<T>> {
        std::size_t operator()(storm::generator::SparseBeliefState<T> const& s) const noexcept {
            return s.hash();
        }
    };
}
