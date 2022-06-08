#pragma once

#include "storm/api/verification.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm-pomdp/analysis/FormulaInformation.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {

            /**
             * Struct for storing precomputed values bounding the actual values on the POMDP
             */
            template<typename ValueType>
            struct TrivialPomdpValueBounds {
                // Vectors containing upper and lower bound values for the POMDP states
                std::vector<std::vector<ValueType>> lower;
                std::vector<std::vector<ValueType>> upper;
                std::vector<ValueType> parametric;
                /**
                 * Picks the precomputed lower bound for a given scheduler index and state of the POMDP
                 * @param scheduler_id the scheduler ID
                 * @param state the state ID
                 * @return the lower bound value
                 */
                ValueType getLowerBound(uint64_t scheduler_id, uint64_t const& state);
                /**
                 * Picks the precomputed upper bound for a given scheduler index and state of the POMDP
                 * @param scheduler_id the scheduler ID
                 * @param state the state ID
                 * @return the smallest upper bound value
                 */
                ValueType getUpperBound(uint64_t scheduler_id, uint64_t const& state);

                /**
                 * Picks the largest precomputed lower bound for a given state of the POMDP
                 * @param state the state ID
                 * @return the largest lower bound value
                 */
                ValueType getHighestLowerBound(uint64_t const& state);
                /**
                 * Picks the smallest precomputed upper bound for a given state of the POMDP
                 * @param state the state ID
                 * @return the smallest upper bound value
                 */
                ValueType getSmallestUpperBound(uint64_t const& state);

                ValueType getParametricBound(uint64_t const& state);

            };

            /**
             * Struct to store the extreme bound values needed for the reward correction values when clipping is used
             */
            template<typename ValueType>
            struct ExtremePOMDPValueBound{
                bool min;
                std::vector<ValueType> values;
                storm::storage::BitVector isInfinite;
                /**
                 * Get the extreme bound value for a given state
                 * @param state the state ID
                 * @return the bound value
                 */
                ValueType getValueForState(uint64_t const& state);
            };

            template <typename ValueType>
            class TrivialPomdpValueBoundsModelChecker {
            public:
                typedef TrivialPomdpValueBounds<ValueType> ValueBounds;
                typedef ExtremePOMDPValueBound<ValueType> ExtremeValueBound;
                TrivialPomdpValueBoundsModelChecker(storm::models::sparse::Pomdp<ValueType> const& pomdp);
                
                ValueBounds getValueBounds(storm::logic::Formula const& formula);
                
                ValueBounds getValueBounds(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info);

                ExtremeValueBound getExtremeValueBound(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info);

            private:
                storm::models::sparse::Pomdp<ValueType> const& pomdp;

                std::vector<ValueType> getChoiceValues(std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards);

                std::vector<ValueType> computeValuesForGuessedScheduler(std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards, storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> underlyingMdp, ValueType const& scoreThreshold, bool relativeScore);

                std::vector<ValueType> computeValuesForRandomFMPolicy(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, uint64_t memoryBound);

                std::vector<ValueType> computeValuesForRandomMemorylessPolicy(storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> underlyingMdp);
            };
        }
    }
}