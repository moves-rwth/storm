#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <map>
#include <boost/optional.hpp>


#include "storm/storage/BitVector.h"
#include "storm-pomdp/storage/BeliefManager.h"
#include "storm-pomdp/modelchecker/TrivialPomdpValueBoundsModelChecker.h"

namespace storm {
    
    namespace builder {
        template<typename PomdpType, typename BeliefValueType = typename PomdpType::ValueType>
        class BeliefMdpExplorer {
        public:
            typedef typename PomdpType::ValueType ValueType;
            typedef storm::storage::BeliefManager<PomdpType, BeliefValueType> BeliefManagerType;
            typedef typename BeliefManagerType::BeliefId BeliefId;
            typedef uint64_t MdpStateType;

            struct SuccessorObservationInformation {
                SuccessorObservationInformation(ValueType const &obsProb, ValueType const &maxProb, uint64_t const &count);
                void join(SuccessorObservationInformation other);
                ValueType observationProbability; /// The probability we move to the corresponding observation.
                ValueType maxProbabilityToSuccessorWithObs; /// The maximal probability to move to a successor with the corresponding observation.
                uint64_t successorWithObsCount; /// The number of successor beliefstates with this observation
                typename BeliefManagerType::BeliefSupportType support;
            };

            enum class Status {
                Uninitialized,
                Exploring,
                ModelFinished,
                ModelChecked
            };

            BeliefMdpExplorer(std::shared_ptr<BeliefManagerType> beliefManager, storm::pomdp::modelchecker::TrivialPomdpValueBounds<ValueType> const &pomdpValueBounds);

            BeliefMdpExplorer(BeliefMdpExplorer &&other) = default;

            BeliefManagerType const &getBeliefManager() const;

            void startNewExploration(boost::optional<ValueType> extraTargetStateValue = boost::none, boost::optional<ValueType> extraBottomStateValue = boost::none);

            /*!
             * Restarts the exploration to allow re-exploring each state.
             * After calling this, the "currently explored" MDP has the same number of states and choices as the "old" one, but the choices are still empty
             * This method inserts the initial state of the MDP in the exploration queue.
             * While re-exploring, the reference to the old MDP remains valid.
             */
            void restartExploration();

            bool hasUnexploredState() const;

            BeliefId exploreNextState();

            void addTransitionsToExtraStates(uint64_t const &localActionIndex, ValueType const &targetStateValue = storm::utility::zero<ValueType>(),
                                             ValueType const &bottomStateValue = storm::utility::zero<ValueType>());

            void addSelfloopTransition(uint64_t const &localActionIndex = 0, ValueType const &value = storm::utility::one<ValueType>());

            /*!
             * Adds the next transition to the given successor belief
             * @param localActionIndex
             * @param transitionTarget
             * @param value
             * @param ignoreNewBeliefs If true, beliefs that were not found before are not inserted, i.e. we might not insert the transition.
             * @return true iff a transition was actually inserted. False can only happen if ignoreNewBeliefs is true.
             */
            bool addTransitionToBelief(uint64_t const &localActionIndex, BeliefId const &transitionTarget, ValueType const &value, bool ignoreNewBeliefs);

            void computeRewardAtCurrentState(uint64_t const &localActionIndex, ValueType extraReward = storm::utility::zero<ValueType>());

            void setCurrentStateIsTarget();

            void setCurrentStateIsTruncated();

            void setCurrentChoiceIsDelayed(uint64_t const &localActionIndex);

            bool currentStateHasOldBehavior() const;

            bool getCurrentStateWasTruncated() const;

            /*!
             * Retrieves whether the current state can be reached under an optimal scheduler
             * This requires a previous call of computeOptimalChoicesAndReachableMdpStates.
             */
            bool stateIsOptimalSchedulerReachable(MdpStateType mdpState) const;

            /*!
             * Retrieves whether the given action at the current state was optimal in the most recent check.
             * This requires a previous call of computeOptimalChoicesAndReachableMdpStates.
             */
            bool actionIsOptimal(uint64_t const &globalActionIndex) const;

            /*!
             * Retrieves whether the current state can be reached under a scheduler that was optimal in the most recent check.
             * This requires (i) a previous call of computeOptimalChoicesAndReachableMdpStates and (ii) that the current state has old behavior.
             */
            bool currentStateIsOptimalSchedulerReachable() const;

            /*!
             * Retrieves whether the given action at the current state was optimal in the most recent check.
             * This requires (i) a previous call of computeOptimalChoicesAndReachableMdpStates and (ii) that the current state has old behavior.
             */
            bool actionAtCurrentStateWasOptimal(uint64_t const &localActionIndex) const;

            bool getCurrentStateActionExplorationWasDelayed(uint64_t const &localActionIndex) const;

            /*!
             * Inserts transitions and rewards at the given action as in the MDP of the previous exploration.
             * Does NOT set whether the state is truncated and/or target.
             * Will add "old" states that have not been considered before into the exploration queue
             * @param localActionIndex
             */
            void restoreOldBehaviorAtCurrentState(uint64_t const &localActionIndex);

            void finishExploration();

            void dropUnexploredStates();

            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> getExploredMdp() const;

            MdpStateType getCurrentNumberOfMdpStates() const;

            MdpStateType getCurrentNumberOfMdpChoices() const;

            MdpStateType getStartOfCurrentRowGroup() const;

            ValueType getLowerValueBoundAtCurrentState() const;

            ValueType getUpperValueBoundAtCurrentState() const;

            ValueType computeLowerValueBoundAtBelief(BeliefId const &beliefId) const;

            ValueType computeUpperValueBoundAtBelief(BeliefId const &beliefId) const;

            void computeValuesOfExploredMdp(storm::solver::OptimizationDirection const &dir);

            bool hasComputedValues() const;

            std::vector<ValueType> const &getValuesOfExploredMdp() const;

            ValueType const &getComputedValueAtInitialState() const;

            MdpStateType getBeliefId(MdpStateType exploredMdpState) const;

            void gatherSuccessorObservationInformationAtCurrentState(uint64_t localActionIndex, std::map<uint32_t, SuccessorObservationInformation> &gatheredSuccessorObservations);

            void gatherSuccessorObservationInformationAtMdpChoice(uint64_t mdpChoice, std::map<uint32_t, SuccessorObservationInformation> &gatheredSuccessorObservations);

            bool currentStateHasSuccessorObservationInObservationSet(uint64_t localActionIndex, storm::storage::BitVector const &observationSet);

            void takeCurrentValuesAsUpperBounds();

            void takeCurrentValuesAsLowerBounds();

            /*!
             *
             * Computes the set of states that are reachable via a path that is consistent with an optimal MDP scheduler.
             * States that are only reachable via target states will not be in this set.
             * @param ancillaryChoicesEpsilon if the difference of a 1-step value of a choice is only epsilon away from the optimal value, the choice will be included.
             * @param relative if set, we consider the relative difference to detect ancillaryChoices
             */
            void computeOptimalChoicesAndReachableMdpStates(ValueType const &ancillaryChoicesEpsilon, bool relativeDifference);
            
        private:
            MdpStateType noState() const;

            std::shared_ptr<storm::logic::Formula const> createStandardProperty(storm::solver::OptimizationDirection const &dir, bool computeRewards);

            storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> createStandardCheckTask(std::shared_ptr<storm::logic::Formula const> &property);

            MdpStateType getCurrentMdpState() const;

            MdpStateType getCurrentBeliefId() const;

            void internalAddTransition(uint64_t const &row, MdpStateType const &column, ValueType const &value);

            void internalAddRowGroupIndex();

            MdpStateType getExploredMdpState(BeliefId const &beliefId) const;

            void insertValueHints(ValueType const &lowerBound, ValueType const &upperBound);

            MdpStateType getOrAddMdpState(BeliefId const &beliefId);
            
            // Belief state related information
            std::shared_ptr<BeliefManagerType> beliefManager;
            std::vector<BeliefId> mdpStateToBeliefIdMap;
            std::map<BeliefId, MdpStateType> beliefIdToMdpStateMap;
            storm::storage::BitVector exploredBeliefIds;
            
            // Exploration information
            std::deque<uint64_t> mdpStatesToExplore;
            std::vector<std::map<MdpStateType, ValueType>> exploredMdpTransitions;
            std::vector<MdpStateType> exploredChoiceIndices;
            std::vector<ValueType> mdpActionRewards;
            uint64_t currentMdpState;
            
            // Special states and choices during exploration
            boost::optional<MdpStateType> extraTargetState;
            boost::optional<MdpStateType> extraBottomState;
            storm::storage::BitVector targetStates;
            storm::storage::BitVector truncatedStates;
            MdpStateType initialMdpState;
            storm::storage::BitVector delayedExplorationChoices;

            // Final Mdp
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> exploredMdp;
            
            // Value and scheduler related information
            storm::pomdp::modelchecker::TrivialPomdpValueBounds<ValueType> pomdpValueBounds;
            std::vector<ValueType> lowerValueBounds;
            std::vector<ValueType> upperValueBounds;
            std::vector<ValueType> values; // Contains an estimate during building and the actual result after a check has performed
            boost::optional<storm::storage::BitVector> optimalChoices;
            boost::optional<storm::storage::BitVector> optimalChoicesReachableMdpStates;
            
            // The current status of this explorer
            Status status;
        };
    }
}