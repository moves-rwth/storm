
#include "GraphConditions.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace analysis {

        template <typename ValueType>
        ConstraintCollector<ValueType>::ConstraintCollector(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
            process(dtmc);
        }

        template <typename ValueType>
        std::unordered_set<typename ConstraintType<ValueType>::val> const& ConstraintCollector<ValueType>::getWellformedConstraints() const {
            return this->wellformedConstraintSet;
        }

        template <typename ValueType>
        std::unordered_set<typename ConstraintType<ValueType>::val> const& ConstraintCollector<ValueType>::getGraphPreservingConstraints() const {
            return this->graphPreservingConstraintSet;
        }


        template <typename ValueType>
        void ConstraintCollector<ValueType>::process(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
            for(uint_fast64_t state = 0; state < dtmc.getNumberOfStates(); ++state) {
                ValueType sum = storm::utility::zero<ValueType>();
                for (auto const& transition : dtmc.getRows(state)) {
                    sum += transition.getValue();
                    if (!storm::utility::isConstant(transition.getValue())) {
                        if (transition.getValue().denominator().isConstant()) {
                            if (transition.getValue().denominatorAsNumber() > 0) {
                                wellformedConstraintSet.emplace((transition.getValue().nominator() - transition.getValue().denominator()).polynomial(), storm::CompareRelation::LEQ);
                                wellformedConstraintSet.emplace(transition.getValue().nominator().polynomial(), storm::CompareRelation::GEQ);
                            } else if (transition.getValue().denominatorAsNumber() < 0) {
                                wellformedConstraintSet.emplace((transition.getValue().nominator() - transition.getValue().denominator()).polynomial(), storm::CompareRelation::GEQ);
                                wellformedConstraintSet.emplace(transition.getValue().nominator().polynomial(), storm::CompareRelation::LEQ);
                            } else {
                                assert(false); // Should fail before.
                            }
                        } else {
                            wellformedConstraintSet.emplace(transition.getValue().denominator().polynomial(), storm::CompareRelation::NEQ);
                            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Rational functions at edges not yet supported.");
                        }
                        graphPreservingConstraintSet.emplace(transition.getValue().nominator().polynomial(), storm::CompareRelation::NEQ);
                    }
                }
                STORM_LOG_ASSERT(!storm::utility::isConstant(sum) || storm::utility::isOne(sum), "If the sum is a constant, it must be equal to 1.");
                if(!storm::utility::isConstant(sum)) {
                    wellformedConstraintSet.emplace((sum.nominator() - sum.denominator()).polynomial(), storm::CompareRelation::EQ);
                }

            }
            for(auto const& rewModelEntry : dtmc.getRewardModels()) {
                if (rewModelEntry.second.hasStateRewards()) {
                    for(auto const& stateReward : rewModelEntry.second.getStateRewardVector()) {
                        if (!storm::utility::isConstant(stateReward)) {
                            if (stateReward.denominator().isConstant()) {
                                if (stateReward.denominatorAsNumber() > 0) {
                                    wellformedConstraintSet.emplace(stateReward.nominator().polynomial(), storm::CompareRelation::GEQ);
                                } else if (stateReward.denominatorAsNumber() < 0) {
                                    wellformedConstraintSet.emplace(stateReward.nominator().polynomial(), storm::CompareRelation::LEQ);
                                } else {
                                    assert(false); // Should fail before.
                                }
                            } else {
                                wellformedConstraintSet.emplace(stateReward.denominator().polynomial(), storm::CompareRelation::NEQ);
                                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Rational functions at state rewards not yet supported.");
                            }
                        }
                    }
                }
                if (rewModelEntry.second.hasStateActionRewards()) {
                    for(auto const& saReward : rewModelEntry.second.getStateActionRewardVector()) {
                        if (!storm::utility::isConstant(saReward)) {
                            if (saReward.denominator().isConstant()) {
                                if (saReward.denominatorAsNumber() > 0) {
                                    wellformedConstraintSet.emplace(saReward.nominator().polynomial(), storm::CompareRelation::GEQ);
                                } else if (saReward.denominatorAsNumber() < 0) {
                                    wellformedConstraintSet.emplace(saReward.nominator().polynomial(), storm::CompareRelation::LEQ);
                                } else {
                                    assert(false); // Should fail before.
                                }
                            } else {
                                wellformedConstraintSet.emplace(saReward.denominator().polynomial(), storm::CompareRelation::NEQ);
                                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Rational functions at state rewards not yet supported.");
                            }
                        }
                    }
                }

            }
        }

        template <typename ValueType>
        void ConstraintCollector<ValueType>::operator()(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
            process(dtmc);
        }


        template class ConstraintCollector<storm::RationalFunction>;
    }
}